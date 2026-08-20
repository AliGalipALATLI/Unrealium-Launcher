#include "NewsPage.h"
#include <QLabel>
#include <QFrame>
#include <QDateTime>

static QFrame* makeNewsCard(const QString& tag,
                            const QString& title,
                            const QString& body,
                            QWidget* parent) {
    auto* card = new QFrame(parent);
    card->setObjectName("newsCard");
    card->setFrameShape(QFrame::NoFrame);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(16, 14, 16, 14);
    lay->setSpacing(6);

    auto* tagLabel = new QLabel(tag.toUpper(), card);
    tagLabel->setObjectName("newsTag");
    lay->addWidget(tagLabel);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("newsTitle");
    titleLabel->setWordWrap(true);
    lay->addWidget(titleLabel);

    auto* bodyLabel = new QLabel(body, card);
    bodyLabel->setObjectName("newsBody");
    bodyLabel->setWordWrap(true);
    lay->addWidget(bodyLabel);

    return card;
}

NewsPage::NewsPage(QWidget* parent) : QWidget(parent) {
    setObjectName("newsPage");

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 24);
    outer->setSpacing(0);

    auto* header = new QLabel("News", this);
    header->setObjectName("pageTitle");
    outer->addWidget(header);

    auto* subheader = new QLabel("Latest updates from Unreal Engine & Unrealium Launcher", this);
    subheader->setObjectName("pageSubtitle");
    outer->addWidget(subheader);
    outer->addSpacing(18);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setObjectName("newsScroll");

    auto* container = new QWidget(scroll);
    m_listLayout = new QVBoxLayout(container);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(12);
    m_listLayout->addStretch(1);

    scroll->setWidget(container);
    outer->addWidget(scroll, 1);

    // --- Sample news cards (placeholder content) ---
    QDateTime now = QDateTime::currentDateTime();

    m_listLayout->insertWidget(m_listLayout->count() - 1,
        makeNewsCard("Unrealium Launcher",
            QString("Unrealium Launcher %1").arg(now.toString("yyyy.MM.dd")),
            "Sidebar navigation redesign is now live. Switch between News, Editors, Projects and Nodes with a single click.",
            container));

    m_listLayout->insertWidget(m_listLayout->count() - 1,
        makeNewsCard("Unreal Engine",
            "Linux workflow improvements in UE 5.6",
            "Epic Games continues to improve native Linux support for the Unreal Editor, including better Wayland scaling and reduced Vulkan validation overhead.",
            container));

    m_listLayout->insertWidget(m_listLayout->count() - 1,
        makeNewsCard("Blueprint Nodes",
            "Searchable node assistant",
            "Quickly look up common Blueprint nodes by typing what you need. Results are now displayed as individual cards for a cleaner overview.",
            container));

    m_listLayout->insertWidget(m_listLayout->count() - 1,
        makeNewsCard("Community",
            "Share your .desktop entries",
            "Drop your custom launch wrappers and engine configurations into ~/.local/share/applications/ — they will be picked up automatically on next launch.",
            container));
}
