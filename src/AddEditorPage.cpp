#include "AddEditorPage.h"
#include "ConfigManager.h"
#include "DesktopEntryWriter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFrame>
#include <QScrollArea>

namespace {
    constexpr const char* kLastDirKey = "addEditor/lastDir";

    QString lastDir() {
        QString d = QDir::homePath();
        QFile f(QDir::homePath() + "/.config/Unrealium-Launcher/last_dir");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString v = QString::fromUtf8(f.readAll()).trimmed();
            if (!v.isEmpty() && QDir(v).exists()) d = v;
        }
        return d;
    }
    void saveLastDir(const QString& d) {
        QFile f(QDir::homePath() + "/.config/Unrealium-Launcher/last_dir");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            f.write(d.toUtf8());
        }
    }
}

AddEditorPage::AddEditorPage(QWidget* parent) : QWidget(parent) {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 24);
    outer->setSpacing(0);

    // ---- Header row ----
    auto* header = new QLabel("Add Editor", this);
    header->setObjectName("pageTitle");
    outer->addWidget(header);

    auto* sub = new QLabel("Browse an Unreal Engine root folder, or pick from auto-discovered installations below.",
                            this);
    sub->setObjectName("pageSubtitle");
    sub->setWordWrap(true);
    outer->addWidget(sub);

    outer->addSpacing(18);

    // ---- Manual add card ----
    auto* manualCard = new QFrame(this);
    manualCard->setObjectName("addEditorCard");
    auto* manualLayout = new QVBoxLayout(manualCard);
    manualLayout->setContentsMargins(18, 16, 18, 16);
    manualLayout->setSpacing(10);

    auto* manualHeader = new QLabel("Manual", manualCard);
    manualHeader->setObjectName("cardHeader");
    manualLayout->addWidget(manualHeader);

    // Path row
    auto* pathRow = new QHBoxLayout();
    auto* pathLabel = new QLabel("Engine Path:", manualCard);
    pathRow->addWidget(pathLabel);
    m_pathLineEdit = new QLineEdit(manualCard);
    m_pathLineEdit->setObjectName("addEditorPath");
    m_pathLineEdit->setPlaceholderText("Select a folder containing Engine/Binaries/Linux/UnrealEditor");
    m_pathLineEdit->setReadOnly(true);
    pathRow->addWidget(m_pathLineEdit, 1);
    m_browseButton = new QPushButton("Browse", manualCard);
    m_browseButton->setObjectName("browseBtn");
    m_browseButton->setCursor(Qt::PointingHandCursor);
    pathRow->addWidget(m_browseButton);
    manualLayout->addLayout(pathRow);

    // Name row
    auto* nameRow = new QHBoxLayout();
    auto* nameLabel = new QLabel("Name:", manualCard);
    nameRow->addWidget(nameLabel);
    m_nameLineEdit = new QLineEdit(manualCard);
    m_nameLineEdit->setObjectName("addEditorName");
    m_nameLineEdit->setPlaceholderText("e.g. Unreal Engine 5.6");
    m_nameLineEdit->setEnabled(false);
    nameRow->addWidget(m_nameLineEdit, 1);
    manualLayout->addLayout(nameRow);

    // Launch args row
    auto* argsRow = new QHBoxLayout();
    auto* argsLabel = new QLabel("Launch Args:", manualCard);
    argsRow->addWidget(argsLabel);
    m_launchArgsEdit = new QLineEdit(manualCard);
    m_launchArgsEdit->setObjectName("addEditorArgs");
    m_launchArgsEdit->setPlaceholderText("e.g. -game -project=/path/to/proj.uproject");
    argsRow->addWidget(m_launchArgsEdit, 1);
    manualLayout->addLayout(argsRow);

    // Actions row
    auto* actionsRow = new QHBoxLayout();
    actionsRow->addStretch(1);
    m_addButton = new QPushButton("Add Editor", manualCard);
    m_addButton->setObjectName("primaryBtn");
    m_addButton->setCursor(Qt::PointingHandCursor);
    m_addButton->setEnabled(false);
    actionsRow->addWidget(m_addButton);
    manualLayout->addLayout(actionsRow);

    outer->addWidget(manualCard);

    outer->addSpacing(18);

    // ---- Discovered engines card ----
    auto* discCard = new QFrame(this);
    discCard->setObjectName("addEditorCard");
    auto* discLayout = new QVBoxLayout(discCard);
    discLayout->setContentsMargins(18, 16, 18, 16);
    discLayout->setSpacing(10);

    auto* discHeaderRow = new QHBoxLayout();
    auto* discHeader = new QLabel("Discovered Engines", discCard);
    discHeader->setObjectName("cardHeader");
    discHeaderRow->addWidget(discHeader);
    discHeaderRow->addStretch(1);
    m_rescanButton = new QPushButton("Rescan", discCard);
    m_rescanButton->setObjectName("secondaryBtn");
    m_rescanButton->setCursor(Qt::PointingHandCursor);
    discHeaderRow->addWidget(m_rescanButton);
    discLayout->addLayout(discHeaderRow);

    m_discoveredHost = new QWidget(discCard);
    auto* hostLayout = new QVBoxLayout(m_discoveredHost);
    hostLayout->setContentsMargins(0, 0, 0, 0);
    hostLayout->setSpacing(8);
    discLayout->addWidget(m_discoveredHost);

    outer->addWidget(discCard);

    outer->addStretch(1);

    // Cancel / Close row
    auto* bottomRow = new QHBoxLayout();
    bottomRow->addStretch(1);
    m_cancelButton = new QPushButton("Back to Editors", this);
    m_cancelButton->setObjectName("secondaryBtn");
    m_cancelButton->setCursor(Qt::PointingHandCursor);
    bottomRow->addWidget(m_cancelButton);
    outer->addLayout(bottomRow);

    // Wire up
    connect(m_browseButton, &QPushButton::clicked, this, &AddEditorPage::browseFolder);
    connect(m_nameLineEdit, &QLineEdit::textChanged, this, &AddEditorPage::onNameChanged);
    connect(m_addButton, &QPushButton::clicked, this, &AddEditorPage::addEditor);
    connect(m_rescanButton, &QPushButton::clicked, this, &AddEditorPage::rescanDiscovered);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddEditorPage::cancelAndClose);

    rescanDiscovered();
}

void AddEditorPage::rescanDiscovered() {
    rebuildDiscoveredList();
}

void AddEditorPage::browseFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Engine Root Folder",
                                                    lastDir(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty()) return;

    QString editorPath = dir + "/Engine/Binaries/Linux/UnrealEditor";
    if (QFile::exists(editorPath)) {
        setSelectedPath(dir);
        saveLastDir(dir);
    } else {
        QMessageBox::warning(this, "Invalid Folder",
                             "Could not find UnrealEditor in the selected folder.\n"
                             "Please select the Unreal Engine root folder.");
    }
}

void AddEditorPage::setSelectedPath(const QString& path) {
    m_selectedRootPath = path;
    m_pathLineEdit->setText(path);
    m_nameLineEdit->setEnabled(true);

    if (m_nameLineEdit->text().trimmed().isEmpty()) {
        DiscoveredEngine c = EngineDiscovery::classify(path);
        if (!c.needsName) m_nameLineEdit->setText(c.suggestedName);
        else m_nameLineEdit->setText(QDir(path).dirName());
        m_nameLineEdit->setFocus();
        m_nameLineEdit->selectAll();
    }
}

void AddEditorPage::onNameChanged(const QString& text) {
    m_addButton->setEnabled(!text.trimmed().isEmpty() && !m_selectedRootPath.isEmpty());
}

bool AddEditorPage::writeEngine(const QString& name, const QString& path, const QString& launchArgs) {
    if (!DesktopEntryWriter::write(name, path, launchArgs)) {
        QMessageBox::warning(this, "Error", "Failed to create desktop entry.");
        return false;
    }
    ConfigManager::saveEntry({ name, path, launchArgs, false });
    return true;
}

void AddEditorPage::addEditor() {
    const QString name = m_nameLineEdit->text().trimmed();
    const QString args = m_launchArgsEdit->text().trimmed();
    if (name.isEmpty() || m_selectedRootPath.isEmpty()) return;

    if (writeEngine(name, m_selectedRootPath, args)) {
        emit editorAdded();
        // Reset form so the user can add another if they want
        m_pathLineEdit->clear();
        m_nameLineEdit->clear();
        m_launchArgsEdit->clear();
        m_nameLineEdit->setEnabled(false);
        m_selectedRootPath.clear();
        m_addButton->setEnabled(false);
        // Drop the just-added engine from the discovered list so it doesn't show up again
        rebuildDiscoveredList();
    }
}

void AddEditorPage::adoptDiscovered(const QString& path) {
    setSelectedPath(path);
}

void AddEditorPage::cancelAndClose() {
    emit closed();
}

void AddEditorPage::rebuildDiscoveredList() {
    // Clear existing
    QLayout* lay = m_discoveredHost->layout();
    while (QLayoutItem* it = lay->takeAt(0)) {
        if (auto* w = it->widget()) w->deleteLater();
        delete it;
    }

    const auto engines = EngineDiscovery::scan();
    if (engines.isEmpty()) {
        auto* empty = new QLabel("No Unreal Engine installations found on this system.", m_discoveredHost);
        empty->setObjectName("discoveredEmpty");
        lay->addWidget(empty);
        return;
    }

    QList<EditorEntry> registered = ConfigManager::loadEntries();
    auto isRegistered = [&](const QString& path){
        for (const auto& e : registered) {
            if (QDir(e.path).absolutePath() == QDir(path).absolutePath()) return true;
        }
        return false;
    };

    for (const auto& eng : engines) {
        if (isRegistered(eng.path)) continue;

        auto* row = new QFrame(m_discoveredHost);
        row->setObjectName("discoveredRow");
        auto* rowLay = new QHBoxLayout(row);
        rowLay->setContentsMargins(12, 10, 12, 10);
        rowLay->setSpacing(10);

        auto* info = new QVBoxLayout();
        info->setSpacing(2);
        auto* nameLbl = new QLabel(eng.needsName ? "[Needs Name]" : eng.suggestedName, row);
        nameLbl->setObjectName("discoveredName");
        info->addWidget(nameLbl);
        auto* pathLbl = new QLabel(eng.path, row);
        pathLbl->setObjectName("discoveredPath");
        pathLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
        info->addWidget(pathLbl);
        rowLay->addLayout(info, 1);

        auto* useBtn = new QPushButton("Use", row);
        useBtn->setObjectName("primaryBtn");
        useBtn->setCursor(Qt::PointingHandCursor);
        connect(useBtn, &QPushButton::clicked, this, [this, path = eng.path]{
            adoptDiscovered(path);
        });
        rowLay->addWidget(useBtn);

        lay->addWidget(row);
    }

    // After rebuilding, if everything was already registered, show a hint
    bool anyUnregistered = false;
    QLayout* l = m_discoveredHost->layout();
    for (int i = 0; i < l->count(); ++i) {
        if (auto* w = qobject_cast<QFrame*>(l->itemAt(i)->widget())) {
            if (w->objectName() == QLatin1String("discoveredRow")) {
                anyUnregistered = true; break;
            }
        }
    }
    if (!anyUnregistered) {
        auto* hint = new QLabel("All detected engines are already registered.", m_discoveredHost);
        hint->setObjectName("discoveredEmpty");
        lay->addWidget(hint);
    }
}