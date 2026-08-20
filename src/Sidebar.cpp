#include "Sidebar.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>
#include <QStyle>
#include <cmath>

Sidebar::Sidebar(QWidget* parent) : QWidget(parent) {
    setObjectName("sidebar");
    setFixedWidth(240);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(14, 18, 14, 14);
    root->setSpacing(10);

    // --- Logo (top) ---
    m_logoLabel = new QLabel(this);
    m_logoLabel->setAlignment(Qt::AlignCenter);
    QPixmap logoPixmap(":/assets/UE.png");
    if (logoPixmap.isNull()) {
        logoPixmap = QPixmap("assets/UE.png");
    }
    if (!logoPixmap.isNull()) {
        const QPixmap scaled = logoPixmap.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_logoLabel->setPixmap(scaled);
    }
    m_logoLabel->setMinimumHeight(110);
    root->addWidget(m_logoLabel, 0, Qt::AlignHCenter);

    root->addSpacing(6);

    // --- Add Editor (filled blue button) ---
    m_addEditorBtn = new QPushButton("Add Editor", this);
    m_addEditorBtn->setObjectName("addEditorBtn");
    m_addEditorBtn->setCursor(Qt::PointingHandCursor);
    m_addEditorBtn->setMinimumHeight(40);
    root->addWidget(m_addEditorBtn);

    connect(m_addEditorBtn, &QPushButton::clicked, this, &Sidebar::addEditorRequested);

    root->addSpacing(14);

    auto makeNavCard = [&](QPushButton* btn, QWidget* parent) -> QFrame* {
        auto* card = new QFrame(parent);
        card->setObjectName("navCard");
        auto* lay = new QVBoxLayout(card);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(0);
        lay->addWidget(btn);
        return card;
    };

    m_newsBtn     = makeNavBtn("News",     this);
    m_editorsBtn  = makeNavBtn("Editors",  this);
    m_projectsBtn = makeNavBtn("Projects", this);
    m_nodesBtn    = makeNavBtn("Nodes",    this);

    m_newsCard     = makeNavCard(m_newsBtn,     this);
    m_editorsCard  = makeNavCard(m_editorsBtn,  this);
    m_projectsCard = makeNavCard(m_projectsBtn, this);
    m_nodesCard    = makeNavCard(m_nodesBtn,    this);

    root->addWidget(m_newsCard);
    root->addWidget(m_editorsCard);
    root->addWidget(m_projectsCard);
    root->addWidget(m_nodesCard);

    root->addStretch(1);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    m_navGroup->addButton(m_newsBtn,     News);
    m_navGroup->addButton(m_editorsBtn,  Editors);
    m_navGroup->addButton(m_projectsBtn, Projects);
    m_navGroup->addButton(m_nodesBtn,    Nodes);

    connect(m_navGroup, &QButtonGroup::idClicked, this, [this](int id){
        emit pageRequested(static_cast<Page>(id));
    });
}

QPushButton* Sidebar::makeNavBtn(const QString& text, QWidget* parent) {
    auto* btn = new QPushButton(text, parent);
    btn->setObjectName("navBtn");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFlat(true);
    btn->setMinimumHeight(38);
    return btn;
}

void Sidebar::setActivePage(Page page) {
    m_activePage = page;
    QAbstractButton* btn = m_navGroup->button(static_cast<int>(page));
    if (btn) btn->setChecked(true);

    auto applyActive = [](QFrame* card, bool active){
        card->setProperty("active", active);
        card->style()->unpolish(card);
        card->style()->polish(card);
    };
    applyActive(m_newsCard,     page == News);
    applyActive(m_editorsCard,  page == Editors);
    applyActive(m_projectsCard, page == Projects);
    applyActive(m_nodesCard,    page == Nodes);
}

void Sidebar::setUiScale(int percent) {
    if (percent < 70)  percent = 70;
    if (percent > 150) percent = 150;
    m_uiScale = percent;

    const double scale = percent / 100.0;
    setFixedWidth(static_cast<int>(std::lround(240.0 * scale)));
}