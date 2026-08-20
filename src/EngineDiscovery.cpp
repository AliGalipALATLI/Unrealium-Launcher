#include "EngineDiscovery.h"

#include <QDir>
#include <QFileInfo>
#include <QFile>

namespace {
    QString unEditorBinary(const QString& engineRoot) {
        return engineRoot + "/Engine/Binaries/Linux/UnrealEditor";
    }

    bool looksLikeUE(const QString& path) {
        return QFile::exists(unEditorBinary(path));
    }

    bool scanDir(const QString& dir, QList<DiscoveredEngine>& out) {
        QDir d(dir);
        if (!d.exists()) return false;

        const QStringList entries = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& name : entries) {
            const QString candidate = d.absoluteFilePath(name);
            if (looksLikeUE(candidate)) {
                out.append(EngineDiscovery::classify(candidate));
            } else {
                // Recurse one level deep (handles /home/user/UnrealEngine/UE_5.6 etc.)
                QDir sub(candidate);
                const QStringList subEntries = sub.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                for (const QString& subName : subEntries) {
                    const QString subCandidate = sub.absoluteFilePath(subName);
                    if (looksLikeUE(subCandidate)) {
                        out.append(EngineDiscovery::classify(subCandidate));
                    }
                }
            }
        }
        return true;
    }
}

DiscoveredEngine EngineDiscovery::classify(const QString& enginePath) {
    DiscoveredEngine r;
    r.path = enginePath;

    const QString folderName = QDir(enginePath).dirName();

    auto m = patternLinux().match(folderName);
    if (m.hasMatch()) {
        r.suggestedName = QStringLiteral("Unreal Engine %1").arg(m.captured(1));
        r.needsName = false;
        return r;
    }
    m = patternUE().match(folderName);
    if (m.hasMatch()) {
        r.suggestedName = QStringLiteral("Unreal Engine %1").arg(m.captured(1));
        r.needsName = false;
        return r;
    }
    m = patternUEHyphen().match(folderName);
    if (m.hasMatch()) {
        r.suggestedName = QStringLiteral("Unreal Engine %1").arg(m.captured(1));
        r.needsName = false;
        return r;
    }

    r.suggestedName.clear();
    r.needsName = true;
    return r;
}

QList<DiscoveredEngine> EngineDiscovery::scan() {
    QList<DiscoveredEngine> result;
    const QString home = QDir::homePath();

    // Each location is scanned independently; duplicates are filtered at the end.
    const QStringList roots = {
        home,
        home + "/UnrealEngine",
        home + "/UnrealEngineEngine",
        home + "/.local/share/UnrealEngine",
    };

    for (const QString& root : roots) {
        scanDir(root, result);
    }

    // Deduplicate by path, keep the first occurrence
    QList<DiscoveredEngine> dedup;
    for (const auto& e : result) {
        bool found = false;
        for (const auto& d : dedup) {
            if (d.path == e.path) { found = true; break; }
        }
        if (!found) dedup.append(e);
    }
    return dedup;
}

const QRegularExpression& EngineDiscovery::patternLinux() {
    static const QRegularExpression re(QStringLiteral("^Linux_Unreal_Engine_(\\d+\\.\\d+(?:\\.\\d+)?)$"));
    return re;
}

const QRegularExpression& EngineDiscovery::patternUE() {
    static const QRegularExpression re(QStringLiteral("^UE[_ ]?(\\d+\\.\\d+(?:\\.\\d+)?)$"));
    return re;
}

const QRegularExpression& EngineDiscovery::patternUEHyphen() {
    static const QRegularExpression re(QStringLiteral("^UnrealEngine-(\\d+\\.\\d+(?:\\.\\d+)?)$"));
    return re;
}