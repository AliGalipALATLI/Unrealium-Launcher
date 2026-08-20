#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <optional>

struct NodePin {
    QString name;
    QString type;   // e.g. "Vector", "Float", "Bool", "Exec"
    QString description;
};

struct NodeEntry {
    QString id; // filename without extension
    QString name;
    QVector<QString> keywords;
    QString description;
    QVector<NodePin> inputs;
    QVector<NodePin> outputs;
};

class NodeManager {
public:
    // Load all nodes from the given directory (relative to application dir or repo)
    static QVector<NodeEntry> loadNodes(const QString& dirPath);
    // Find by id
    static std::optional<NodeEntry> loadNodeById(const QString& filePath);
    // Filter nodes by query using the project's search logic (title -> keywords -> description)
    static QVector<NodeEntry> filterNodes(const QVector<NodeEntry>& nodes, const QString& q, int maxResults = 50);
    // Finds the nodes directory by checking various locations
    static QString getNodesPath();
    // Finds a generic data path
    static QString getDataPath(const QString& relativePath);
};
