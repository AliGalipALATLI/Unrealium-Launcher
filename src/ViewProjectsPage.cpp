#include "ViewProjectsPage.h"
#include "ProjectCardWidget.h"
#include "ConfigManager.h"
#include <QVBoxLayout>
#include <QScrollArea>

ViewProjectsPage::ViewProjectsPage(QWidget* parent) : QWidget(parent) {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 24);
    outer->setSpacing(0);

    auto* header = new QLabel("Projects", this);
    header->setObjectName("pageTitle");
    outer->addWidget(header);

    auto* sub = new QLabel("Discovered .uproject files associated with registered editors.", this);
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