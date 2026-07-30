#include "EditorSwitchDialog.h"
#include <QHBoxLayout>
#include <QMessageBox>

EditorSwitchDialog::EditorSwitchDialog(const QString& projectName, const QList<EditorEntry>& editors, QWidget* parent)
    : QDialog(parent), m_editors(editors) {
    setWindowTitle("Select Editor - " + projectName);
    setMinimumWidth(400);
    setMinimumHeight(300);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* infoLabel = new QLabel(
        QString("Multiple editors found for \"%1\".\nSelect which editor to open with:").arg(projectName),
        this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    m_listWidget = new QListWidget(this);
    for (const auto& editor : m_editors) {
        QListWidgetItem* item = new QListWidgetItem(editor.name, m_listWidget);
        item->setData(Qt::UserRole, editor.path);
        item->setToolTip(editor.path);
    }
    layout->addWidget(m_listWidget);

    m_okButton = new QPushButton("Open", this);
    m_okButton->setEnabled(false);
    layout->addWidget(m_okButton);

    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &EditorSwitchDialog::onSelectionChanged);
    connect(m_okButton, &QPushButton::clicked, this, &EditorSwitchDialog::onAccepted);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*) {
        onAccepted();
    });
}

EditorEntry EditorSwitchDialog::selectedEditor() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < m_editors.size()) {
        return m_editors[m_selectedIndex];
    }
    return {};
}

void EditorSwitchDialog::onSelectionChanged() {
    auto items = m_listWidget->selectedItems();
    if (!items.isEmpty()) {
        m_selectedIndex = m_listWidget->row(items.first());
        m_okButton->setEnabled(true);
    } else {
        m_selectedIndex = -1;
        m_okButton->setEnabled(false);
    }
}

void EditorSwitchDialog::onAccepted() {
    if (m_selectedIndex < 0) {
        QMessageBox::warning(this, "No Selection", "Please select an editor to continue.");
        return;
    }
    accept();
}
