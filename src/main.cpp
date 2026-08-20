#include "MainWindow.h"
#include "EditorSwitchDialog.h"
#include "ProjectScannerService.h"
#include "ConfigManager.h"
#include "EditorEntry.h"
#include "SettingsManager.h"
#include "StyleManager.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QCommandLineParser>

static int handleOpenProject(const QString& projectPath) {
    QFileInfo fi(projectPath);
    if (!fi.exists() || !fi.isFile()) {
        QMessageBox::critical(nullptr, "Error", "Project file not found:\n" + projectPath);
        return 1;
    }

    QString absoluteProjectPath = fi.absoluteFilePath();

    QList<EditorEntry> candidates;
    QList<EditorEntry> allEditors = ConfigManager::loadEntries();

    for (const auto& editor : allEditors) {
        QString editorProjectsFolder = QDir::cleanPath(ProjectScannerService::getProjectsFolderPath(editor.path)) + "/";
        if (absoluteProjectPath.startsWith(editorProjectsFolder)) {
            candidates.append(editor);
        }
    }

    if (candidates.isEmpty()) {
        QMessageBox::critical(nullptr, "No Editor Found",
            "No registered Unreal Editor found for this project.\n"
            "Project: " + absoluteProjectPath);
        return 1;
    }

    EditorEntry chosen;
    if (candidates.size() == 1) {
        chosen = candidates.first();
    } else {
        EditorSwitchDialog dialog(fi.completeBaseName(), candidates);
        if (dialog.exec() != QDialog::Accepted) {
            return 0;
        }
        chosen = dialog.selectedEditor();
    }

    QString editorBinary = chosen.path + "/Engine/Binaries/Linux/UnrealEditor";
    if (!QFile::exists(editorBinary)) {
        QMessageBox::critical(nullptr, "Error",
            "UnrealEditor binary not found at:\n" + editorBinary);
        return 1;
    }

    QStringList args;
    args << QString("-project=%1").arg(absoluteProjectPath);

    QProcess::startDetached(editorBinary, args);
    return 0;
}

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "wayland;xcb");

    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Unrealium-Launcher");

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("open-project", "Open a project file", "project-path"));
    parser.addOption(QCommandLineOption("daemon", "Run as background scanner service (no UI)"));
    parser.process(a);

    if (parser.isSet("open-project")) {
        QString projectPath = parser.value("open-project");
        return handleOpenProject(projectPath);
    }

    AppSettings settings = SettingsManager::load();
    StyleManager::apply(settings.fontScale, settings.uiScale);

    if (parser.isSet("daemon")) {
        return a.exec();
    }

    MainWindow w;
    w.show();
    return a.exec();
}