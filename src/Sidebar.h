#pragma once
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>

class QVBoxLayout;

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget* parent = nullptr);

    enum Page { News = 0, Editors = 1, Projects = 2, Nodes = 3, AddEditor = 4 };
    Q_ENUM(Page)

    void setActivePage(Page page);
    Page activePage() const { return m_activePage; }

    void setUiScale(int percent);

signals:
    void addEditorRequested();
    void pageRequested(Page page);

private:
    QLabel* m_logoLabel = nullptr;
    QPushButton* m_addEditorBtn = nullptr;
    QPushButton* m_newsBtn = nullptr;
    QPushButton* m_editorsBtn = nullptr;
    QPushButton* m_projectsBtn = nullptr;
    QPushButton* m_nodesBtn = nullptr;
    QFrame*       m_newsCard     = nullptr;
    QFrame*       m_editorsCard  = nullptr;
    QFrame*       m_projectsCard = nullptr;
    QFrame*       m_nodesCard    = nullptr;
    QButtonGroup* m_navGroup = nullptr;
    Page m_activePage = News;
    int  m_uiScale = 100;

    QPushButton* makeNavBtn(const QString& text, QWidget* parent);
};