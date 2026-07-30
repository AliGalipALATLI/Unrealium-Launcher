#include "ProjectScannerService.h"
#include "DesktopEntryWriter.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QProcess>

ProjectScannerService::ProjectScannerService(QObject* parent)
    : QObject(parent) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ProjectScannerService::doScan);
    m_knownProjects = ConfigManager::loadKnownProjects();
}

void ProjectScannerService::start() {
    m_timer->start(10000);
    doScan();
}

void ProjectScannerService::stop() {
    m_timer->stop();
}

void ProjectScannerService::scanNow() {
    doScan();
}

QString ProjectScannerService::findLauncherBinary() {
    QString binPath = QCoreApplication::applicationFilePath();
    if (QFile::exists(binPath)) {
        return binPath;
    }
    QString localBin = QDir::homePath() + "/.local/bin/Unrealium-Launcher";
    if (QFile::exists(localBin)) {
        return localBin;
    }
    return "Unrealium-Launcher";
}

QString ProjectScannerService::getProjectsFolderPath(const QString& editorPath) {
    QDir editorDir(editorPath);
    QString parentPath = QDir::cleanPath(editorDir.absolutePath() + "/..");
    return parentPath + "/Unreal Projects";
}

QMap<QString, QList<EditorEntry>> ProjectScannerService::buildEditorProjectsFolderMap() {
    QMap<QString, QList<EditorEntry>> map;
    QList<EditorEntry> editors = ConfigManager::loadEntries();

    for (const auto& editor : editors) {
        QString projectsFolder = getProjectsFolderPath(editor.path);
        map[projectsFolder].append(editor);
    }
    return map;
}

void ProjectScannerService::doScan() {
    m_knownProjects = ConfigManager::loadKnownProjects();

    QMap<QString, QList<EditorEntry>> folderMap = buildEditorProjectsFolderMap();

    for (auto it = folderMap.begin(); it != folderMap.end(); ++it) {
        scanProjectsFolder(it.key(), it.value());
    }

    cleanupDeletedProjects();
}

void ProjectScannerService::scanProjectsFolder(const QString& projectsFolder, const QList<EditorEntry>& editors) {
    QDir projectsDir(projectsFolder);
    if (!projectsDir.exists()) {
        return;
    }

    QStringList nameFilters;
    nameFilters << "*.uproject";
    QFileInfoList projectFiles = projectsDir.entryInfoList(nameFilters, QDir::Files);

    QStringList subDirs = projectsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& subDir : subDirs) {
        QDir subDirObj(projectsDir.absoluteFilePath(subDir));
        QFileInfoList subFiles = subDirObj.entryInfoList(nameFilters, QDir::Files);
        projectFiles.append(subFiles);
    }

    QString launcherBin = findLauncherBinary();

    for (const QFileInfo& fileInfo : projectFiles) {
        QString projectPath = fileInfo.absoluteFilePath();
        QString projectName = fileInfo.completeBaseName();

        if (m_knownProjects.contains(projectPath)) {
            KnownProject& known = m_knownProjects[projectPath];
            QStringList editorNames;
            for (const auto& e : editors) {
                if (!editorNames.contains(e.name)) {
                    editorNames.append(e.name);
                }
            }
            if (known.editorNames != editorNames) {
                known.editorNames = editorNames;
                ConfigManager::saveKnownProject(known);
            }
            continue;
        }

        KnownProject project;
        project.projectPath = projectPath;
        project.projectName = projectName;
        project.projectsFolder = projectsFolder;
        for (const auto& e : editors) {
            if (!project.editorNames.contains(e.name)) {
                project.editorNames.append(e.name);
            }
        }

        bool ok = DesktopEntryWriter::writeProjectEntry(projectName, projectPath, launcherBin);
        if (ok) {
            QString sanitizedName = projectName.toLower().replace(" ", "-");
            project.desktopFile = QString("ueproject-%1.desktop").arg(sanitizedName);
        }

        m_knownProjects[projectPath] = project;
        ConfigManager::saveKnownProject(project);
    }
}

void ProjectScannerService::cleanupDeletedProjects() {
    QStringList toRemove;
    for (auto it = m_knownProjects.begin(); it != m_knownProjects.end(); ++it) {
        if (!QFile::exists(it->projectPath)) {
            DesktopEntryWriter::removeProjectEntry(it->projectName);
            toRemove.append(it.key());
        }
    }
    for (const QString& key : toRemove) {
        m_knownProjects.remove(key);
        ConfigManager::removeKnownProject(key);
    }
}
