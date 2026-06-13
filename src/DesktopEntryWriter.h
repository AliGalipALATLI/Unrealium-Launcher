#pragma once
#include <QString>

class DesktopEntryWriter {
public:
    static bool write(const QString& name, const QString& engineRootPath, const QString& launchArgs = "");
    static void remove(const QString& name);
};
