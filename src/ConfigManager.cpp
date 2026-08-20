#include "ConfigManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>

namespace {
    QString getConfigPath() {
        QString configDir = QDir::homePath() + "/.config/Unrealium-Launcher";
        QDir dir(configDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        return configDir + "/editors.json";
    }

    QString stripEngineArgs(const QString& execLine) {
        QString exec = execLine.trimmed();
        QString path;
        if (exec.startsWith('"')) {
            int endQuote = exec.indexOf('"', 1);
            if (endQuote > 0) path = exec.mid(1, endQuote - 1);
            else path = exec;
        } else {
            int sp = exec.indexOf(' ');
            path = (sp > 0) ? exec.left(sp) : exec;
        }
        const QString suffix = "/Engine/Binaries/Linux/UnrealEditor";
        if (path.endsWith(suffix)) {
            path = path.left(path.length() - suffix.length());
        }
        return path;
    }

    EditorEntry readUnrealDesktop(const QFileInfo& fileInfo) {
        EditorEntry entry;
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return entry;

        QTextStream in(&file);
        QString name, execLine;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if      (line.startsWith("Name="))  name     = line.mid(5).trimmed();
            else if (line.startsWith("Exec="))  execLine = line.mid(5).trimmed();
        }
        if (name.isEmpty() || execLine.isEmpty()) return entry;

        QString path = stripEngineArgs(execLine);

        // Reject anything that doesn't actually point to UnrealEditor.
        const QString suffix  = "/Engine/Binaries/Linux/UnrealEditor";
        const QString binary  = path + suffix;
        if (!QFile::exists(binary)) return entry;

        if (!QDir(path).exists()) {
            if (QFile::exists(path)) path = QFileInfo(path).absolutePath();
        }
        if (!QDir(path).exists()) return entry;

        entry.name = name;
        entry.path = QDir(path).absolutePath();
        return entry;
    }

    // Only consider files we ourselves wrote (prefix "unreal-") AND that point to
    // an existing UnrealEditor binary. Everything else is ignored.
    QList<EditorEntry> parseDesktopFiles() {
        QList<EditorEntry> entries;
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
        QDir dir(desktopPath);
        if (!dir.exists()) return entries;

        const QStringList filters = { "unreal-*.desktop" };
        const QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);

        for (const QFileInfo& fi : files) {
            EditorEntry e = readUnrealDesktop(fi);
            if (!e.name.isEmpty() && !e.path.isEmpty()) {
                entries.append(e);
            }
        }
        return entries;
    }

    void writeJson(const QList<EditorEntry>& entries) {
        QJsonArray array;
        for (const auto& entry : entries) {
            QJsonObject obj;
            obj["name"] = entry.name;
            obj["path"] = entry.path;
            obj["launchArgs"] = entry.launchArgs;
            obj["isFavorite"] = entry.isFavorite;
            array.append(obj);
        }
        QJsonDocument doc(array);
        QFile file(getConfigPath());
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
        }
    }
}

QList<EditorEntry> ConfigManager::loadEntries() {
    QFile file(getConfigPath());
    if (!file.exists()) {
        QList<EditorEntry> entries = parseDesktopFiles();
        writeJson(entries);
        return entries;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return QList<EditorEntry>();
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QList<EditorEntry> entries;
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (int i = 0; i < array.size(); ++i) {
            QJsonObject obj = array[i].toObject();
            EditorEntry entry;
            entry.name = obj["name"].toString();
            entry.path = obj["path"].toString();
            entry.launchArgs = obj["launchArgs"].toString();
            entry.isFavorite = obj["isFavorite"].toBool(false);
            entries.append(entry);
        }
    }
    return entries;
}

void ConfigManager::saveEntry(const EditorEntry& entry) {
    QList<EditorEntry> entries = loadEntries();
    bool found = false;
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].name == entry.name) {
            // Update all relevant fields so changes (including isFavorite)
            // are persisted when toggled from the UI.
            bool wasFavorite = entries[i].isFavorite;
            entries[i].path = entry.path;
            entries[i].launchArgs = entry.launchArgs;
            entries[i].isFavorite = entry.isFavorite;
            // If newly favorited, move to front. If unfavorited, move to end.
            if (!wasFavorite && entry.isFavorite) {
                EditorEntry tmp = entries.takeAt(i);
                entries.insert(0, tmp);
            } else if (wasFavorite && !entry.isFavorite) {
                EditorEntry tmp = entries.takeAt(i);
                entries.append(tmp);
            }
            found = true;
            break;
        }
    }
    if (!found) {
        if (entry.isFavorite) {
            entries.insert(0, entry);
        } else {
            entries.append(entry);
        }
    }
    writeJson(entries);
}

void ConfigManager::removeEntry(const QString& name) {
    QList<EditorEntry> entries = loadEntries();
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].name == name) {
            entries.removeAt(i);
            break;
        }
    }
    writeJson(entries);
}

void ConfigManager::renameEntry(const QString& oldName, const EditorEntry& newEntry) {
    QList<EditorEntry> entries = loadEntries();
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].name == oldName) {
            // Replace entry at the same position to preserve ordering
            entries[i] = newEntry;
            writeJson(entries);
            return;
        }
    }
    // If not found, append as new
    entries.append(newEntry);
    writeJson(entries);
}

static QString getProjectsConfigPath() {
    QString configDir = QDir::homePath() + "/.config/Unrealium-Launcher";
    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return configDir + "/projects.json";
}

QMap<QString, KnownProject> ConfigManager::loadKnownProjects() {
    QMap<QString, KnownProject> projects;
    QFile file(getProjectsConfigPath());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return projects;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return projects;

    QJsonObject root = doc.object();
    QJsonObject known = root["knownProjects"].toObject();

    for (auto it = known.begin(); it != known.end(); ++it) {
        QJsonObject obj = it.value().toObject();
        KnownProject p;
        p.projectPath = obj["projectPath"].toString();
        p.projectName = obj["projectName"].toString();
        p.projectsFolder = obj["projectsFolder"].toString();
        p.desktopFile = obj["desktopFile"].toString();
        p.isFavorite = obj["isFavorite"].toBool(false);
        QJsonArray editorsArr = obj["editorNames"].toArray();
        for (int i = 0; i < editorsArr.size(); ++i) {
            p.editorNames.append(editorsArr[i].toString());
        }
        projects[it.key()] = p;
    }
    return projects;
}

void ConfigManager::saveKnownProject(const KnownProject& project) {
    QMap<QString, KnownProject> projects = loadKnownProjects();
    projects[project.projectPath] = project;

    QJsonObject root;
    QJsonObject known;
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        QJsonObject obj;
        obj["projectPath"] = it->projectPath;
        obj["projectName"] = it->projectName;
        obj["projectsFolder"] = it->projectsFolder;
        obj["desktopFile"] = it->desktopFile;
        obj["isFavorite"] = it->isFavorite;
        QJsonArray editorsArr;
        for (const QString& name : it->editorNames) {
            editorsArr.append(name);
        }
        obj["editorNames"] = editorsArr;
        known[it.key()] = obj;
    }
    root["knownProjects"] = known;

    QFile file(getProjectsConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

void ConfigManager::removeKnownProject(const QString& projectPath) {
    QMap<QString, KnownProject> projects = loadKnownProjects();
    projects.remove(projectPath);

    QJsonObject root;
    QJsonObject known;
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        QJsonObject obj;
        obj["projectPath"] = it->projectPath;
        obj["projectName"] = it->projectName;
        obj["projectsFolder"] = it->projectsFolder;
        obj["desktopFile"] = it->desktopFile;
        obj["isFavorite"] = it->isFavorite;
        QJsonArray editorsArr;
        for (const QString& name : it->editorNames) {
            editorsArr.append(name);
        }
        obj["editorNames"] = editorsArr;
        known[it.key()] = obj;
    }
    root["knownProjects"] = known;

    QFile file(getProjectsConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

void ConfigManager::saveAllKnownProjects(const QMap<QString, KnownProject>& projects) {
    QJsonObject root;
    QJsonObject known;
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        QJsonObject obj;
        obj["projectPath"] = it->projectPath;
        obj["projectName"] = it->projectName;
        obj["projectsFolder"] = it->projectsFolder;
        obj["desktopFile"] = it->desktopFile;
        obj["isFavorite"] = it->isFavorite;
        QJsonArray editorsArr;
        for (const QString& name : it->editorNames) {
            editorsArr.append(name);
        }
        obj["editorNames"] = editorsArr;
        known[it.key()] = obj;
    }
    root["knownProjects"] = known;

    QFile file(getProjectsConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}
