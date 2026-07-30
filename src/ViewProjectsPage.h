#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLayout>
#include "ConfigManager.h"

class ViewProjectsPage : public QWidget {
    Q_OBJECT
public:
    explicit ViewProjectsPage(QWidget* parent = nullptr);
    void loadProjects();

signals:
    void backRequested();

private:
    QGridLayout* m_gridLayout;
    int m_columnCount = 5;
    void clearLayout(QLayout* layout);
};
