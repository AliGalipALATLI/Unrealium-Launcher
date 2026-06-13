#include "EditorCardWidget.h"
#include "ConfigManager.h"
#include "DesktopEntryWriter.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QLineEdit>
#include <QDir>
#include <QPixmap>
#include <QCoreApplication>
#include <QIcon>
#include <QPainter>
#include <QEvent>

EditorCardWidget::EditorCardWidget(const EditorEntry& entry, QWidget* parent)
    : QWidget(parent), m_entry(entry) {
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    QString appDir = QCoreApplication::applicationDirPath();

    // Create an inner content frame so the card visuals and contents are together
    QWidget* content = new QWidget(this);
    content->setStyleSheet("background: #2b2b2b; border: 1px solid #3a3a3a; border-radius: 8px;");
    QHBoxLayout* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(12, 8, 12, 8);

    m_nameLabel = new QLabel(entry.name, content);
    // Allow clicking the label to start inline edit
    m_nameLabel->setCursor(Qt::IBeamCursor);
    m_nameLabel->installEventFilter(this);

    // Create name editor but keep it hidden until editing
    m_nameEdit = new QLineEdit(entry.name, content);
    m_nameEdit->setVisible(false);
    m_nameEdit->setStyleSheet("background: #2d2d2d; color: #ffffff; border: 1px solid #444; padding: 4px;");
    connect(m_nameEdit, &QLineEdit::editingFinished, this, &EditorCardWidget::commitNameEdit);
    m_nameLabel->setStyleSheet("color: #ffffff; padding-left: 8px;");

    // Delete button inside the card (on the right)
    // Use QToolButton which is more suitable for icon-only buttons inside compact widgets
    m_deleteButton = new QToolButton(content);
    QPixmap delPix;
    // Try multiple candidate paths so icon loads regardless of working dir or binary location
    const QStringList candidates = {
        appDir + "/assets/TRA.png",
        appDir + "/../assets/TRA.png",
        QString("./assets/TRA.png"),
        QString("assets/TRA.png"),
        QString("build/assets/TRA.png")
    };
    for (const QString &p : candidates) {
        if (delPix.load(p)) break;
    }
    if (!delPix.isNull()) {
        m_deleteButton->setIcon(QIcon(delPix));
        m_deleteButton->setIconSize(QSize(20, 20));
    } else {
        // Create a simple fallback trash icon (white) so the button is visibly an icon
        QPixmap fallback(20,20);
        fallback.fill(Qt::transparent);
        QPainter p(&fallback);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(Qt::white));
        // Draw a simple bin: rectangle body and small lid
        p.drawRect(5,7,10,10);
        p.drawRect(4,5,12,2);
        p.end();
        m_deleteButton->setIcon(QIcon(fallback));
        m_deleteButton->setIconSize(QSize(20,20));
    }
    m_deleteButton->setFixedSize(40, 40);
    m_deleteButton->setCursor(Qt::PointingHandCursor);
    m_deleteButton->setStyleSheet("border: none; background: transparent; color: #ffffff;");

    contentLayout->addWidget(m_nameLabel, 1);
    contentLayout->addWidget(m_nameEdit, 1);

    // Favorite button
    m_favoriteButton = new QToolButton(content);
    m_favoriteButton->setFixedSize(40, 40);
    m_favoriteButton->setCursor(Qt::PointingHandCursor);
    m_favoriteButton->setStyleSheet("border: none; background: transparent; color: white; font-size: 20px;");
    
    // Load icons with multiple candidate paths
    auto loadIcon = [&](const QString& filename, QPixmap& pix) {
        const QStringList candidates = {
            appDir + "/assets/" + filename,
            appDir + "/../assets/" + filename,
            QString("./assets/") + filename,
            QString("assets/") + filename,
            QString("build/assets/") + filename
        };
        for (const QString &p : candidates) {
            if (pix.load(p)) return true;
        }
        return false;
    };

    bool emptyLoaded = loadIcon("empty.png", m_emptyPix);
    bool fullLoaded = loadIcon("full.png", m_fullPix);

#ifdef QT_DEBUG
    if (!emptyLoaded) qDebug("EditorCardWidget: failed to load empty.png");
    if (!fullLoaded) qDebug("EditorCardWidget: failed to load full.png");
#endif

    m_favoriteButton->installEventFilter(this);
    connect(m_favoriteButton, &QToolButton::clicked, this, &EditorCardWidget::toggleFavorite);
    updateFavoriteIcon(false);

    contentLayout->addWidget(m_favoriteButton, 0, Qt::AlignVCenter);
    contentLayout->addWidget(m_deleteButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(content);

    // Make the card a bit wider and taller so delete button is clearly inside the card
    setMinimumWidth(260);
    setMinimumHeight(56);
    this->setContentsMargins(0,0,0,0);

    // Debug: log whether TRA.png loaded
#ifdef QT_DEBUG
    if (!delPix.isNull()) {
        qDebug("EditorCardWidget: loaded TRA.png icon");
    } else {
        qDebug("EditorCardWidget: TRA.png not loaded, using text fallback");
    }
#endif

    connect(m_deleteButton, &QToolButton::clicked, this, &EditorCardWidget::showDeleteMenu);
}

void EditorCardWidget::showDeleteMenu() {
    QMenu menu(this);
    
    QAction* deleteAppAction = menu.addAction("Delete App");
    QAction* deleteFilesAction = menu.addAction("Delete Files");

    // Use cursor position for menu placement to ensure it appears where the user clicked
    QAction* selectedAction = menu.exec(QCursor::pos());

    if (selectedAction == deleteAppAction) {
        DesktopEntryWriter::remove(m_entry.name);
        ConfigManager::removeEntry(m_entry.name);
        emit deletionRequested();
    } else if (selectedAction == deleteFilesAction) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Warning",
            QString("This will permanently delete all engine source files at:\n%1\nThis action cannot be undone.").arg(m_entry.path),
            QMessageBox::Ok | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Ok) {
            DesktopEntryWriter::remove(m_entry.name);
            QDir dir(m_entry.path);
            if (dir.exists()) {
                dir.removeRecursively();
            }
            ConfigManager::removeEntry(m_entry.name);
            emit deletionRequested();
        }
    }
}

void EditorCardWidget::toggleFavorite() {
    m_entry.isFavorite = !m_entry.isFavorite;
    ConfigManager::saveEntry(m_entry);
    updateFavoriteIcon(true); // Keep full icon if clicked
    emit favoriteChanged();
}

void EditorCardWidget::updateFavoriteIcon(bool hovered) {
    if (m_entry.isFavorite) {
        if (!m_fullPix.isNull()) {
            m_favoriteButton->setIcon(QIcon(m_fullPix));
            m_favoriteButton->setIconSize(QSize(20, 20));
            m_favoriteButton->setText("");
        } else {
            m_favoriteButton->setText("★");
            m_favoriteButton->setIcon(QIcon());
        }
    } else {
        if (hovered) {
            if (!m_fullPix.isNull()) {
                m_favoriteButton->setIcon(QIcon(m_fullPix));
                m_favoriteButton->setIconSize(QSize(20, 20));
                m_favoriteButton->setText("");
            } else {
                m_favoriteButton->setText("★");
                m_favoriteButton->setIcon(QIcon());
            }
        } else {
            if (!m_emptyPix.isNull()) {
                m_favoriteButton->setIcon(QIcon(m_emptyPix));
                m_favoriteButton->setIconSize(QSize(20, 20));
                m_favoriteButton->setText("");
            } else {
                m_favoriteButton->setText("☆");
                m_favoriteButton->setIcon(QIcon());
            }
        }
    }
}

bool EditorCardWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_favoriteButton) {
        if (event->type() == QEvent::Enter) {
            updateFavoriteIcon(true);
        } else if (event->type() == QEvent::Leave) {
            updateFavoriteIcon(false);
        }
    } else if (watched == m_nameLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            // Start inline editing
            startEditingName();
            return true;
        }
    }
    return false;
}

void EditorCardWidget::startEditingName() {
    if (!m_nameEdit) return;
    m_nameEdit->setText(m_entry.name);
    m_nameLabel->setVisible(false);
    m_nameEdit->setVisible(true);
    m_nameEdit->setFocus(Qt::FocusReason::MouseFocusReason);
    m_nameEdit->selectAll();
}

void EditorCardWidget::commitNameEdit() {
    if (!m_nameEdit) return;
    QString newName = m_nameEdit->text().trimmed();
    QString oldName = m_entry.name;
    // Hide editor and show label regardless; we'll revert label if needed
    m_nameEdit->setVisible(false);
    m_nameLabel->setVisible(true);

    if (newName.isEmpty() || newName == oldName) {
        // No changes
        m_nameLabel->setText(oldName);
        return;
    }

    // Attempt to write new .desktop entry for the new name. If write fails, warn and revert.
    bool ok = DesktopEntryWriter::write(newName, m_entry.path, m_entry.launchArgs);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Failed to create desktop entry for the new name. Name not changed.");
        m_nameLabel->setText(oldName);
        return;
    }

    // Remove old desktop entry
    DesktopEntryWriter::remove(oldName);

    // Update internal model and persist via ConfigManager
    EditorEntry newEntry = m_entry;
    newEntry.name = newName;
    ConfigManager::renameEntry(oldName, newEntry);

    m_entry = newEntry;
    m_nameLabel->setText(m_entry.name);
    emit nameChanged(oldName, m_entry);
}
