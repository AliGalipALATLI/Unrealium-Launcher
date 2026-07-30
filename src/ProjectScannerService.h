#pragma once
#include <QObject>
#include <QTimer>
#include <QMap>
#include <QStringList>
#include "EditorEntry.h"
#include "ConfigManager.h"

class ProjectScannerService : public QObject {
    Q_OBJECT

public:
    explicit ProjectScannerService(QObject* parent = nullptr);
    void start();
    void stop();
    void scanNow();

    static QString findLauncherBinary();
    static QString getProjectsFolderPath(const QString& editorPath);

private slots:
    void doScan();

private:
    QTimer* m_timer;
    QMap<QString, KnownProject> m_knownProjects;

    QMap<QString, QList<EditorEntry>> buildEditorProjectsFolderMap();
    void scanProjectsFolder(const QString& projectsFolder, const QList<EditorEntry>& editors);
    void cleanupDeletedProjects();
};
