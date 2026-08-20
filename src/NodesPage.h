#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QFrame>
#include <QVector>

#include "NodeManager.h"

class QScrollArea;
class QVBoxLayout;
class QLabel;
class QMouseEvent;

class NodeResultCard : public QFrame {
    Q_OBJECT
public:
    explicit NodeResultCard(QWidget* parent = nullptr);

    QString nodeId;
    void setSelected(bool sel);

protected:
    void mousePressEvent(QMouseEvent* e) override;

signals:
    void clicked();

private:
    QLabel* m_bodyLabel    = nullptr;
    QFrame* m_divider      = nullptr;
};

class NodesPage : public QWidget {
    Q_OBJECT
public:
    explicit NodesPage(QWidget* parent = nullptr);

private slots:
    void onSearchChanged(const QString& text);

private:
    QLineEdit* m_searchEdit = nullptr;

    QScrollArea* m_scroll = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
    QVector<NodeEntry> m_nodes;
    QString m_selectedId;

    void clearResults();
    void appendResultCard(const NodeEntry& node);
    void selectNode(const QString& id);
};
