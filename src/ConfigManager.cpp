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

    QList<EditorEntry> parseDesktopFiles() {
        QList<EditorEntry> entries;
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
        QDir dir(desktopPath);
        if (!dir.exists()) return entries;

        // Look for .desktop files that begin with "unreal-". Use name filters
        // and also accept any .desktop files in case the naming differs.
        QStringList filters;
        filters << "unreal-*.desktop" << "*.desktop";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);

        for (const QFileInfo& fileInfo : files) {
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString name;
                QString path;
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    if (line.startsWith("Name=")) {
                        name = line.mid(5).trimmed();
                    } else if (line.startsWith("Exec=")) {
                        // Exec lines can contain arguments or be surrounded by quotes.
                        QString exec = line.mid(5).trimmed();
                        // Remove trailing arguments: split at first space unless path is quoted
                        if (exec.startsWith('"')) {
                            // "..." possibly with args after
                            int endQuote = exec.indexOf('"', 1);
                            if (endQuote > 0) {
                                path = exec.mid(1, endQuote - 1);
                            } else {
                                path = exec;
                            }
                        } else {
                            int sp = exec.indexOf(' ');
                            QString candidate = (sp > 0) ? exec.left(sp) : exec;
                            // If Exec points to the UnrealEditor binary, strip the binary name
                            const QString suffix = "/Engine/Binaries/Linux/UnrealEditor";
                            if (candidate.endsWith(suffix)) {
                                path = candidate.left(candidate.length() - suffix.length());
                            } else {
                                path = candidate;
                            }
                        }
                    }
                }
                if (!name.isEmpty() && !path.isEmpty()) {
                    // Ensure the path is a directory and not the binary itself
                    QDir maybeDir(path);
                    if (!maybeDir.exists() && QFile::exists(path)) {
                        // If path itself is a file, take its directory
                        maybeDir = QDir(QFileInfo(path).absolutePath());
                    }
                    if (maybeDir.exists()) {
                        entries.append({name, maybeDir.absolutePath()});
                    }
                }
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
