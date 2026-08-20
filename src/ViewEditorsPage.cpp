#include "ViewEditorsPage.h"
#include "EditorCardWidget.h"
#include "ConfigManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCoreApplication>
#include <QSplitter>

ViewEditorsPage::ViewEditorsPage(QWidget* parent) : QWidget(parent) {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 24);
    outer->setSpacing(0);

    auto* header = new QLabel("Editors", this);
    header->setObjectName("pageTitle");
    outer->addWidget(header);

    auto* sub = new QLabel("Registered Unreal Engine installations on this system.", this);
    sub->setObjectName("pageSubtitle");
    outer->addWidget(sub);

    outer->addSpacing(18);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* scrollWidget = new QWidget();
    m_gridLayout = new QGridLayout(scrollWidget);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setSpacing(16);

    for (int i = 0; i < 5; ++i) {
        m_gridLayout->setColumnStretch(i, 1);
    }

    scrollWidget->setLayout(m_gridLayout);
    scrollArea->setWidget(scrollWidget);

    outer->addWidget(scrollArea, 1);
}

void ViewEditorsPage::clearLayout(QLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void ViewEditorsPage::loadEditors() {
    clearLayout(m_gridLayout);

    QList<EditorEntry> entries = ConfigManager::loadEntries();

    int row = 0;
    int col = 0;

    for (const auto& entry : entries) {
        QWidget* gridParent = m_gridLayout->parentWidget();
        EditorCardWidget* card = new EditorCardWidget(entry, gridParent);
        connect(card, &EditorCardWidget::deletionRequested, this, &ViewEditorsPage::loadEditors);
        connect(card, &EditorCardWidget::favoriteChanged, this, &ViewEditorsPage::loadEditors);
        connect(card, &EditorCardWidget::nameChanged, this, &ViewEditorsPage::loadEditors);

        m_gridLayout->addWidget(card, row, col);
        col++;
        if (col >= 5) {
            col = 0;
            row++;
        }
    }

    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_gridLayout->addItem(spacer, row + 1, 0, 1, 5);
}