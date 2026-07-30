#include "MainWindow.h"
#include "EditorSwitchDialog.h"
#include "ProjectScannerService.h"
#include "ConfigManager.h"
#include "EditorEntry.h"
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

static void applyStyleSheet() {
    qApp->setStyleSheet(R"(
        QWidget        { background: #1e1e1e; color: #ffffff; }
        QPushButton    { background: #2d2d2d; border: 1px solid #444;
                         border-radius: 6px; padding: 8px 18px; color: #ffffff; }
        QPushButton:hover   { background: #3a3a3a; }
        QPushButton:pressed { background: #252525; }
        QLineEdit      { background: #2d2d2d; border: 1px solid #444;
                         border-radius: 4px; padding: 4px 8px; color: #ffffff; }
        QLabel         { color: #ffffff; }
        QDialog        { background: #1e1e1e; color: #ffffff; }
        QMenu          { background: #2d2d2d; color: #ffffff; border: 1px solid #444; }
        QMenu::item:selected { background: #3a3a3a; }
        QScrollArea    { border: none; }
        QListWidget    { background: #2d2d2d; color: #ffffff; border: 1px solid #444;
                         border-radius: 4px; padding: 4px; }
        QListWidget::item:selected { background: #3a3a3a; }
    )");
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

    applyStyleSheet();

    ProjectScannerService scanner;

    if (parser.isSet("daemon")) {
        scanner.start();
        return a.exec();
    }

    scanner.start();

    MainWindow w;
    w.show();
    return a.exec();
}
