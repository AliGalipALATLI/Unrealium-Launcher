#include "SettingsManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
    QString getSettingsPath() {
        QString configDir = QDir::homePath() + "/.config/Unrealium-Launcher";
        QDir dir(configDir);
        if (!dir.exists()) dir.mkpath(".");
        return configDir + "/settings.json";
    }
}

AppSettings SettingsManager::load() {
    AppSettings s;
    QFile file(getSettingsPath());
    if (!file.open(QIODevice::ReadOnly)) return s;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return s;

    QJsonObject obj = doc.object();
    s.language  = obj.value("language").toString(s.language);
    s.fontScale = obj.value("fontScale").toInt(s.fontScale);
    s.uiScale   = obj.value("uiScale").toInt(s.uiScale);

    if (s.fontScale < 70)  s.fontScale = 70;
    if (s.fontScale > 150) s.fontScale = 150;
    if (s.uiScale < 70)    s.uiScale = 70;
    if (s.uiScale > 150)   s.uiScale = 150;

    return s;
}

void SettingsManager::save(const AppSettings& settings) {
    QJsonObject obj;
    obj["language"]  = settings.language;
    obj["fontScale"] = settings.fontScale;
    obj["uiScale"]   = settings.uiScale;

    QFile file(getSettingsPath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(obj).toJson());
    }
}