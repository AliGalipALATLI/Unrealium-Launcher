#include "NodeManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

QVector<NodeEntry> NodeManager::loadNodes(const QString& dirPath) {
    QVector<NodeEntry> out;
    QDir dir(dirPath);
    if (!dir.exists()) {
        qDebug() << "Node directory does not exist:" << dirPath;
        return out;
    }

    QStringList filters{"*.json"};
    QFileInfoList list = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
    for (const QFileInfo& fi : list) {
        QString path = fi.absoluteFilePath();
        auto opt = loadNodeById(path);
        if (opt.has_value()) {
            out.push_back(opt.value());
        }
    }
    return out;
}

std::optional<NodeEntry> NodeManager::loadNodeById(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open node file:" << filePath;
        return std::nullopt;
    }
    QByteArray data = f.readAll();
    f.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qDebug() << "Failed to parse JSON for node:" << filePath << err.errorString();
        return std::nullopt;
    }
    QJsonObject obj = doc.object();
    NodeEntry e;
    e.id = QFileInfo(filePath).baseName();
    e.name = obj.value("name").toString();
    e.description = obj.value("description").toString();
    QJsonValue kw = obj.value("keywords");
    if (kw.isArray()) {
        QJsonArray arr = kw.toArray();
        for (const QJsonValue& v : arr) {
            e.keywords.push_back(v.toString());
        }
    }
    auto parsePins = [&](const QString& key, QVector<NodePin>& target) {
        QJsonValue v = obj.value(key);
        if (!v.isArray()) return;
        QJsonArray arr = v.toArray();
        for (const QJsonValue& item : arr) {
            if (!item.isObject()) continue;
            QJsonObject po = item.toObject();
            NodePin p;
            p.name = po.value("name").toString();
            p.type = po.value("type").toString();
            p.description = po.value("description").toString();
            target.push_back(p);
        }
    };
    parsePins("inputs", e.inputs);
    parsePins("outputs", e.outputs);
    return e;
}

QVector<NodeEntry> NodeManager::filterNodes(const QVector<NodeEntry>& nodes, const QString& q, int maxResults) {
    QVector<NodeEntry> out;
    if (q.trimmed().isEmpty()) return out;
    QString ql = q.toLower();
    // Step 1: title
    for (const auto &n : nodes) {
        if (n.name.toLower().contains(ql)) out.push_back(n);
        if (out.size() >= maxResults) return out;
    }
    // Step 2: keywords
    for (const auto &n : nodes) {
        if (out.size() >= maxResults) break;
        bool already = false;
        for (const auto &e : out) if (e.id == n.id) { already = true; break; }
        if (already) continue;
        for (const auto &k : n.keywords) {
            if (k.toLower().contains(ql)) { out.push_back(n); break; }
        }
    }
    // Step 3: description
    for (const auto &n : nodes) {
        if (out.size() >= maxResults) break;
        bool already = false;
        for (const auto &e : out) if (e.id == n.id) { already = true; break; }
        if (already) continue;
        if (n.description.toLower().contains(ql)) out.push_back(n);
    }
    return out;
}

#include <QStandardPaths>
#include <QCoreApplication>

QString NodeManager::getNodesPath() {
    // 1. Check relative to app dir (for build/dev)
    QString appDir = QCoreApplication::applicationDirPath();
    QString path1 = appDir + "/assistant/nodes";
    if (QDir(path1).exists()) return path1;

    // 2. Check in ~/.local/share/Unrealium-Launcher/assistant/nodes (for installation)
    QString shareDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path2 = shareDir + "/assistant/nodes";
    if (QDir(path2).exists()) return path2;
    
    // 3. Fallback to just "assistant/nodes" in CWD
    if (QDir("assistant/nodes").exists()) return "assistant/nodes";

    return path1; // Fallback
}

QString NodeManager::getDataPath(const QString& relativePath) {
    // 1. Check relative to app dir (for build/dev)
    QString appDir = QCoreApplication::applicationDirPath();
    QString path1 = appDir + "/" + relativePath;
    if (QFile::exists(path1)) return path1;

    // 2. Check in ~/.local/share/Unrealium-Launcher/ (for installation)
    QString shareDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path2 = shareDir + "/" + relativePath;
    if (QFile::exists(path2)) return path2;
    
    // 3. Fallback to CWD
    if (QFile::exists(relativePath)) return relativePath;

    return path1; // Fallback
}
