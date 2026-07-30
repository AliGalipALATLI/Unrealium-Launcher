#include "DesktopEntryWriter.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

static bool isValidEnvVarName(const QString& s) {
    if (s.isEmpty()) return false;
    if (!s[0].isLetter() && s[0] != '_') return false;
    for (int i = 1; i < s.size(); ++i) {
        if (!s[i].isLetterOrNumber() && s[i] != '_') return false;
    }
    return true;
}

bool DesktopEntryWriter::write(const QString& name, const QString& engineRootPath, const QString& launchArgs) {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (desktopPath.isEmpty()) {
        desktopPath = QDir::homePath() + "/.local/share/applications";
    }
    QDir dir(desktopPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            desktopPath = QDir::homePath() + "/.local/share/applications";
            dir.setPath(desktopPath);
            if (!dir.exists()) {
                if (!dir.mkpath(".")) {
                    return false;
                }
            }
        }
    }

    QString sanitizedName = name.toLower().replace(" ", "-");
    QString fileName = QString("unreal-%1.desktop").arg(sanitizedName);
    QString filePath = dir.absoluteFilePath(fileName);

    // Parse launchArgs into env vars (VAR=value) vs regular arguments
    QStringList envVars;
    QStringList cmdArgs;
    if (!launchArgs.isEmpty()) {
        QStringList tokens = launchArgs.split(' ', Qt::SkipEmptyParts);
        for (const QString& token : tokens) {
            int eqPos = token.indexOf('=');
            if (eqPos > 0 && isValidEnvVarName(token.left(eqPos))) {
                envVars << token;
            } else {
                cmdArgs << token;
            }
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "[Desktop Entry]\n";
    out << "Type=Application\n";
    out << "Name=" << name << "\n";

    if (!envVars.isEmpty()) {
        QString wrapperDir = QDir::homePath() + "/.local/share/Unrealium-Launcher/wrappers";
        QDir().mkpath(wrapperDir);
        QString wrapperPath = wrapperDir + "/unreal-" + sanitizedName + ".sh";

        QFile wrapperFile(wrapperPath);
        if (!wrapperFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream wout(&wrapperFile);
        wout << "#!/bin/bash\n";
        wout << "cd \"" << engineRootPath << "\"\n";
        wout << "exec env";
        for (const QString& ev : envVars) {
            wout << " " << ev;
        }
        wout << " ./Engine/Binaries/Linux/UnrealEditor";
        for (const QString& arg : cmdArgs) {
            wout << " " << arg;
        }
        wout << "\n";
        wrapperFile.close();
        wrapperFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                                   QFile::ReadGroup | QFile::ExeGroup |
                                   QFile::ReadOther | QFile::ExeOther);
        out << "Exec=" << wrapperPath << "\n";
    } else {
        out << "Exec=" << engineRootPath << "/Engine/Binaries/Linux/UnrealEditor";
        if (!cmdArgs.isEmpty()) {
            out << " " << cmdArgs.join(" ");
        }
        out << "\n";
    }

    QString iconPath = QDir::homePath() + "/.local/share/icons/UE.png";
    if (!QFile::exists(iconPath)) {
        iconPath = engineRootPath + "/Engine/Content/Slate/Testing/PerfCapture.png";
    }
    if (QFile::exists(iconPath)) {
        out << "Icon=" << iconPath << "\n";
    }
    out << "Terminal=false\n";
    out << "Categories=Development;\n";

    file.close();
    QFile::Permissions perms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                               QFile::ReadGroup | QFile::ExeGroup |
                               QFile::ReadOther | QFile::ExeOther;
    file.setPermissions(perms);

    return true;
}

void DesktopEntryWriter::remove(const QString& name) {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (desktopPath.isEmpty()) desktopPath = QDir::homePath() + "/.local/share/applications";
    QDir dir(desktopPath);
    QString sanitizedName = name.toLower().replace(" ", "-");
    QString fileName = QString("unreal-%1.desktop").arg(sanitizedName);
    QString filePath = dir.absoluteFilePath(fileName);
    
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }

    QString wrapperPath = QDir::homePath() + "/.local/share/Unrealium-Launcher/wrappers/unreal-" + sanitizedName + ".sh";
    QFile wrapperFile(wrapperPath);
    if (wrapperFile.exists()) {
        wrapperFile.remove();
    }
}

bool DesktopEntryWriter::writeProjectEntry(const QString& projectName, const QString& projectFilePath, const QString& launcherBinary) {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (desktopPath.isEmpty()) {
        desktopPath = QDir::homePath() + "/.local/share/applications";
    }
    QDir dir(desktopPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString sanitizedName = projectName.toLower().replace(" ", "-");
    QString fileName = QString("ueproject-%1.desktop").arg(sanitizedName);
    QString filePath = dir.absoluteFilePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "[Desktop Entry]\n";
    out << "Type=Application\n";
    out << "Name=" << projectName << "\n";
    out << "Comment=Open " << projectName << " in Unreal Editor\n";

    QString escapedProjectPath = QString("\"%1\"").arg(projectFilePath);
    out << "Exec=" << launcherBinary << " --open-project " << escapedProjectPath << "\n";

    QString iconPath = QDir::homePath() + "/.local/share/icons/UE.png";
    if (QFile::exists(iconPath)) {
        out << "Icon=" << iconPath << "\n";
    }
    out << "Terminal=false\n";
    out << "Categories=Development;\n";

    file.close();
    QFile::Permissions perms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                               QFile::ReadGroup | QFile::ExeGroup |
                               QFile::ReadOther | QFile::ExeOther;
    file.setPermissions(perms);

    return true;
}

void DesktopEntryWriter::removeProjectEntry(const QString& projectName) {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (desktopPath.isEmpty()) desktopPath = QDir::homePath() + "/.local/share/applications";
    QDir dir(desktopPath);
    QString sanitizedName = projectName.toLower().replace(" ", "-");
    QString fileName = QString("ueproject-%1.desktop").arg(sanitizedName);
    QString filePath = dir.absoluteFilePath(fileName);

    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }
}
