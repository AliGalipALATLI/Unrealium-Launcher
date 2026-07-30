#pragma once
#include <QString>

class DesktopEntryWriter {
public:
    static bool write(const QString& name, const QString& engineRootPath, const QString& launchArgs = "");
    static bool writeProjectEntry(const QString& projectName, const QString& projectFilePath, const QString& launcherBinary);
    static void remove(const QString& name);
    static void removeProjectEntry(const QString& projectName);
};
