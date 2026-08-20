#pragma once
#include <QString>
#include <QList>
#include <QRegularExpression>

struct DiscoveredEngine {
    QString path;            // Engine root, e.g. /home/user/UE_5.6
    QString suggestedName;   // e.g. "Unreal Engine 5.6" (if parseable)
    bool    needsName;       // true when the folder name doesn't match known patterns
};

class EngineDiscovery {
public:
    // Scan well-known directories for Unreal Engine installations
    // (anything containing Engine/Binaries/Linux/UnrealEditor).
    static QList<DiscoveredEngine> scan();

    // Try to derive a friendly display name from a folder name.
    // Returns an empty suggestedName (and sets needsName=true) if no pattern matches.
    static DiscoveredEngine classify(const QString& enginePath);

private:
    static const QRegularExpression& patternLinux();
    static const QRegularExpression& patternUE();
    static const QRegularExpression& patternUEHyphen();
};