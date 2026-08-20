#pragma once
#include <QString>

struct AppSettings {
    QString language = QStringLiteral("English");
    int fontScale = 100;
    int uiScale = 100;
};

class SettingsManager {
public:
    static AppSettings load();
    static void save(const AppSettings& settings);
};