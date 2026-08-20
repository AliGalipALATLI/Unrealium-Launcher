#pragma once
#include "Sidebar.h"
#include "SettingsManager.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "ViewEditorsPage.h"
#include "ViewProjectsPage.h"

class NewsPage;
class NodesPage;
class AddEditorPage;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void switchToPage(Sidebar::Page page);
    void onEditorAdded();
    void openSettings();

private:
    void buildContent();
    void applyScales();
    void fadeTo(QWidget* target);

    Sidebar*        m_sidebar     = nullptr;
    QWidget*        m_contentWrap = nullptr;
    QStackedWidget* m_content     = nullptr;

    NewsPage*        m_newsPage      = nullptr;
    ViewEditorsPage* m_editorsPage   = nullptr;
    ViewProjectsPage* m_projectsPage = nullptr;
    NodesPage*       m_nodesPage     = nullptr;
    AddEditorPage*   m_addEditorPage = nullptr;

    QGraphicsOpacityEffect* m_opacityEffect = nullptr;
    QPropertyAnimation*     m_fadeAnim      = nullptr;

    QPushButton*    m_settingsBtn  = nullptr;
    AppSettings     m_settings;
};