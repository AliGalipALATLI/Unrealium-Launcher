#pragma once
#include <QList>
#include <QMap>
#include "EditorEntry.h"

struct KnownProject {
    QString projectPath;
    QString projectName;
    QString projectsFolder;
    QStringList editorNames;
    QString desktopFile;
    bool isFavorite = false;
};

class ConfigManager {
public:
    static QList<EditorEntry> loadEntries();
    static void saveEntry(const EditorEntry& entry);
    static void removeEntry(const QString& name);
    static void renameEntry(const QString& oldName, const EditorEntry& newEntry);

    static QMap<QString, KnownProject> loadKnownProjects();
    static void saveKnownProject(const KnownProject& project);
    static void removeKnownProject(const QString& projectPath);
    static void saveAllKnownProjects(const QMap<QString, KnownProject>& projects);
};
