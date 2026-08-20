#include "MainWindow.h"
#include "NewsPage.h"
#include "NodesPage.h"
#include "AddEditorPage.h"
#include "SettingsDialog.h"
#include "SettingsManager.h"
#include "StyleManager.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QFrame>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Unrealium Launcher");
    setMinimumSize(1000, 640);
    resize(1100, 680);

    m_settings = SettingsManager::load();

    auto* central = new QWidget(this);
    auto* root    = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_sidebar = new Sidebar(central);
    root->addWidget(m_sidebar);

    m_contentWrap = new QWidget(central);
    m_contentWrap->setObjectName("contentWrap");
    auto* contentLay = new QVBoxLayout(m_contentWrap);
    contentLay->setContentsMargins(0, 0, 0, 0);
    contentLay->setSpacing(0);

    auto* topBar = new QWidget(m_contentWrap);
    topBar->setObjectName("contentTopBar");
    topBar->setFixedHeight(44);
    auto* topBarLay = new QHBoxLayout(topBar);
    topBarLay->setContentsMargins(14, 8, 14, 8);
    topBarLay->setSpacing(0);

    m_settingsBtn = new QPushButton(QStringLiteral("\u2699"), topBar);
    m_settingsBtn->setObjectName("settingsBtn");
    m_settingsBtn->setCursor(Qt::PointingHandCursor);
    m_settingsBtn->setToolTip(tr("Settings"));
    topBarLay->addWidget(m_settingsBtn);
    topBarLay->addStretch(1);
    contentLay->addWidget(topBar);

    m_content = new QStackedWidget(m_contentWrap);
    m_content->setObjectName("contentStack");

    m_opacityEffect = new QGraphicsOpacityEffect(m_content);
    m_opacityEffect->setOpacity(1.0);
    m_content->setGraphicsEffect(m_opacityEffect);

    m_fadeAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeAnim->setDuration(180);

    contentLay->addWidget(m_content, 1);
    root->addWidget(m_contentWrap, 1);

    setCentralWidget(central);

    buildContent();
    applyScales();

    connect(m_sidebar, &Sidebar::addEditorRequested, this, [this]{
        switchToPage(Sidebar::AddEditor);
    });
    connect(m_sidebar, &Sidebar::pageRequested, this, &MainWindow::switchToPage);

    connect(m_addEditorPage, &AddEditorPage::editorAdded, this, [this]{
        m_editorsPage->loadEditors();
        m_addEditorPage->rescanDiscovered();
    });

    connect(m_addEditorPage, &AddEditorPage::closed, this, [this]{
        switchToPage(Sidebar::Editors);
    });

    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::openSettings);

    m_sidebar->setActivePage(Sidebar::News);
    m_content->setCurrentWidget(m_newsPage);
}

void MainWindow::buildContent() {
    m_newsPage      = new NewsPage(m_content);
    m_editorsPage   = new ViewEditorsPage(m_content);
    m_projectsPage  = new ViewProjectsPage(m_content);
    m_nodesPage     = new NodesPage(m_content);
    m_addEditorPage = new AddEditorPage(m_content);

    m_content->addWidget(m_newsPage);       // 0
    m_content->addWidget(m_editorsPage);    // 1
    m_content->addWidget(m_projectsPage);   // 2
    m_content->addWidget(m_nodesPage);      // 3
    m_content->addWidget(m_addEditorPage);  // 4
}

void MainWindow::applyScales() {
    m_sidebar->setUiScale(m_settings.uiScale);

    const double scale = m_settings.uiScale / 100.0;
    const int barH  = qRound(44.0 * scale);
    const int btnSz = qRound(28.0 * scale);
    const int m     = qRound(14.0 * scale);

    auto* topBar = m_settingsBtn->parentWidget();
    topBar->setFixedHeight(barH);

    auto* lay = qobject_cast<QHBoxLayout*>(topBar->layout());
    if (lay) lay->setContentsMargins(m, qRound(8.0 * scale), m, qRound(8.0 * scale));

    m_settingsBtn->setFixedSize(btnSz, btnSz);
}

void MainWindow::openSettings() {
    SettingsDialog dlg(m_settings, this);
    connect(&dlg, &SettingsDialog::settingsApplied, this, [this](const AppSettings& s){
        m_settings = s;
        SettingsManager::save(m_settings);
        StyleManager::apply(m_settings.fontScale, m_settings.uiScale);
        applyScales();
    });
    dlg.exec();
}

void MainWindow::onEditorAdded() {
    m_editorsPage->loadEditors();
}

void MainWindow::switchToPage(Sidebar::Page page) {
    QWidget* target = nullptr;
    switch (page) {
        case Sidebar::News:      target = m_newsPage;      break;
        case Sidebar::Editors:   target = m_editorsPage;   m_editorsPage->loadEditors();   break;
        case Sidebar::Projects:  target = m_projectsPage;  m_projectsPage->loadProjects(); break;
        case Sidebar::Nodes:     target = m_nodesPage;     break;
        case Sidebar::AddEditor: target = m_addEditorPage; m_addEditorPage->rescanDiscovered(); break;
    }
    if (!target) return;
    if (m_content->currentWidget() == target) return;

    m_sidebar->setActivePage(page);
    fadeTo(target);
}

void MainWindow::fadeTo(QWidget* target) {
    m_fadeAnim->stop();
    disconnect(m_fadeAnim, nullptr, this, nullptr);

    connect(m_fadeAnim, &QPropertyAnimation::finished, this, [this, target](){
        if (qFuzzyCompare(m_opacityEffect->opacity(), 1.0)) return;

        m_content->setCurrentWidget(target);

        m_fadeAnim->setStartValue(0.0);
        m_fadeAnim->setEndValue(1.0);
        m_fadeAnim->start();
    });

    m_fadeAnim->setStartValue(1.0);
    m_fadeAnim->setEndValue(0.0);
    m_fadeAnim->start();
}