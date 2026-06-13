#pragma once
#include <QString>

struct EditorEntry {
    QString name;
    QString path;
    QString launchArgs;
    bool isFavorite = false;
};
