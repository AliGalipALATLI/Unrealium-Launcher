#include "ViewProjectsPage.h"
#include "ProjectCardWidget.h"
#include "ConfigManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QPixmap>
#include <QIcon>
#include <algorithm>
#include "NodeManager.h"

ViewProjectsPage::ViewProjectsPage(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton* backButton = new QPushButton(this);
    QPixmap pixmap;
    bool iconLoaded = false;
    QString iconPath = NodeManager::getDataPath("assets/AR.png");
    if (pixmap.load(iconPath)) {
        backButton->setIcon(QIcon(pixmap));
        iconLoaded = true;
    }
    backButton->setFixedSize(44, 44);
    if (iconLoaded) {
        backButton->setIconSize(QSize(28, 28));
        backButton->setStyleSheet("border: none; background: transparent; color: #ffffff; padding-left: 6px;");
    } else {
        backButton->setText("\u2190");
        backButton->setStyleSheet("color: #ffffff; font-size: 22px; font-weight: bold; border: none; background: transparent; padding-left: 6px;");
    }
    connect(backButton, &QPushButton::clicked, this, &ViewProjectsPage::backRequested);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->addLayout(topLayout);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollWidget = new QWidget();
    m_gridLayout = new QGridLayout(scrollWidget);
    m_gridLayout->setContentsMargins(16, 16, 16, 16);
    m_gridLayout->setSpacing(16);

    for (int i = 0; i < 5; ++i) {
        m_gridLayout->setColumnStretch(i, 1);
    }

    scrollWidget->setLayout(m_gridLayout);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
    outer->addLayout(mainLayout);
}

void ViewProjectsPage::clearLayout(QLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void ViewProjectsPage::loadProjects() {
    clearLayout(m_gridLayout);

    QMap<QString, KnownProject> projects = ConfigManager::loadKnownProjects();

    QList<KnownProject> sorted = projects.values();
    std::stable_partition(sorted.begin(), sorted.end(), [](const KnownProject& p) {
        return p.isFavorite;
    });

    int row = 0;
    int col = 0;

    for (const auto& project : sorted) {
        QWidget* gridParent = m_gridLayout->parentWidget();
        ProjectCardWidget* card = new ProjectCardWidget(project, gridParent);
        connect(card, &ProjectCardWidget::deletionRequested, this, &ViewProjectsPage::loadProjects);
        connect(card, &ProjectCardWidget::favoriteChanged, this, &ViewProjectsPage::loadProjects);
        connect(card, &ProjectCardWidget::nameChanged, this, &ViewProjectsPage::loadProjects);

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
