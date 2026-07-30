#include "ProjectCardWidget.h"
#include "DesktopEntryWriter.h"
#include "ConfigManager.h"
#include "ProjectScannerService.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QEvent>
#include <QCoreApplication>

ProjectCardWidget::ProjectCardWidget(const KnownProject& project, QWidget* parent)
    : QWidget(parent), m_project(project) {

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    QString appDir = QCoreApplication::applicationDirPath();

    QWidget* content = new QWidget(this);
    content->setStyleSheet("background: #2b2b2b; border: 1px solid #3a3a3a; border-radius: 8px;");
    QHBoxLayout* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(12, 8, 12, 8);

    m_nameLabel = new QLabel(project.projectName, content);
    m_nameLabel->setCursor(Qt::IBeamCursor);
    m_nameLabel->installEventFilter(this);
    m_nameLabel->setStyleSheet("color: #ffffff; padding-left: 8px;");

    m_nameEdit = new QLineEdit(project.projectName, content);
    m_nameEdit->setVisible(false);
    m_nameEdit->setStyleSheet("background: #2d2d2d; color: #ffffff; border: 1px solid #444; padding: 4px;");
    connect(m_nameEdit, &QLineEdit::editingFinished, this, &ProjectCardWidget::commitNameEdit);

    m_deleteButton = new QToolButton(content);
    QPixmap delPix;
    const QStringList candidates = {
        appDir + "/assets/TRA.png",
        appDir + "/../assets/TRA.png",
        QString("./assets/TRA.png"),
        QString("assets/TRA.png"),
        QString("build/assets/TRA.png")
    };
    for (const QString& p : candidates) {
        if (delPix.load(p)) break;
    }
    if (!delPix.isNull()) {
        m_deleteButton->setIcon(QIcon(delPix));
        m_deleteButton->setIconSize(QSize(20, 20));
    } else {
        QPixmap fallback(20,20);
        fallback.fill(Qt::transparent);
        QPainter p(&fallback);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(Qt::white));
        p.drawRect(5,7,10,10);
        p.drawRect(4,5,12,2);
        p.end();
        m_deleteButton->setIcon(QIcon(fallback));
        m_deleteButton->setIconSize(QSize(20,20));
    }
    m_deleteButton->setFixedSize(40, 40);
    m_deleteButton->setCursor(Qt::PointingHandCursor);
    m_deleteButton->setStyleSheet("border: none; background: transparent; color: #ffffff;");
    connect(m_deleteButton, &QToolButton::clicked, this, &ProjectCardWidget::deleteProject);

    contentLayout->addWidget(m_nameLabel, 1);
    contentLayout->addWidget(m_nameEdit, 1);

    m_favoriteButton = new QToolButton(content);
    m_favoriteButton->setFixedSize(40, 40);
    m_favoriteButton->setCursor(Qt::PointingHandCursor);
    m_favoriteButton->setStyleSheet("border: none; background: transparent; color: white; font-size: 20px;");
    m_favoriteButton->installEventFilter(this);
    connect(m_favoriteButton, &QToolButton::clicked, this, &ProjectCardWidget::toggleFavorite);

    auto loadIcon = [&](const QString& filename, QPixmap& pix) {
        const QStringList iconCandidates = {
            appDir + "/assets/" + filename,
            appDir + "/../assets/" + filename,
            QString("./assets/") + filename,
            QString("assets/") + filename,
            QString("build/assets/") + filename
        };
        for (const QString& p : iconCandidates) {
            if (pix.load(p)) return true;
        }
        return false;
    };
    loadIcon("empty.png", m_emptyPix);
    loadIcon("full.png", m_fullPix);

    updateFavoriteIcon(false);

    contentLayout->addWidget(m_favoriteButton, 0, Qt::AlignVCenter);
    contentLayout->addWidget(m_deleteButton, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(content);

    setMinimumWidth(260);
    setMinimumHeight(56);
    this->setContentsMargins(0,0,0,0);
}

void ProjectCardWidget::toggleFavorite() {
    m_project.isFavorite = !m_project.isFavorite;
    ConfigManager::saveKnownProject(m_project);
    updateFavoriteIcon(true);
    emit favoriteChanged();
}

void ProjectCardWidget::updateFavoriteIcon(bool hovered) {
    if (m_project.isFavorite) {
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

bool ProjectCardWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_favoriteButton) {
        if (event->type() == QEvent::Enter) {
            updateFavoriteIcon(true);
        } else if (event->type() == QEvent::Leave) {
            updateFavoriteIcon(false);
        }
    } else if (watched == m_nameLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            startEditingName();
            return true;
        }
    }
    return false;
}

void ProjectCardWidget::deleteProject() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "Delete Project",
        QString("This will permanently delete the project folder:\n%1\n\nThis action cannot be undone.").arg(m_project.projectPath),
        QMessageBox::Ok | QMessageBox::Cancel);

    if (reply == QMessageBox::Ok) {
        QDir projectDir(QFileInfo(m_project.projectPath).absolutePath());
        if (projectDir.exists()) {
            projectDir.removeRecursively();
        }
        DesktopEntryWriter::removeProjectEntry(m_project.projectName);
        ConfigManager::removeKnownProject(m_project.projectPath);
        emit deletionRequested();
    }
}

void ProjectCardWidget::startEditingName() {
    if (!m_nameEdit) return;
    m_nameEdit->setText(m_project.projectName);
    m_nameLabel->setVisible(false);
    m_nameEdit->setVisible(true);
    m_nameEdit->setFocus(Qt::FocusReason::MouseFocusReason);
    m_nameEdit->selectAll();
}

bool ProjectCardWidget::renameProjectOnDisk(const QString& oldPath, const QString& oldName, const QString& newName) {
    QFileInfo fi(oldPath);
    QDir parentDir = fi.dir();
    QString oldDirPath = parentDir.absolutePath();

    QString newDirPath = parentDir.absolutePath() + "/" + newName;
    if (QDir(newDirPath).exists()) {
        return false;
    }

    QFile uprojectFile(oldPath);
    if (!uprojectFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = uprojectFile.readAll();
    uprojectFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject root = doc.object();
        root["ProjectName"] = newName;
        doc.setObject(root);
        data = doc.toJson();
    }

    QString oldUprojectPath = oldDirPath + "/" + oldName + ".uproject";
    QString newUprojectPath = oldDirPath + "/" + newName + ".uproject";

    if (!QDir().rename(oldDirPath, newDirPath)) {
        return false;
    }

    if (!QFile::rename(newDirPath + "/" + oldName + ".uproject", newUprojectPath)) {
        QDir().rename(newDirPath, oldDirPath);
        return false;
    }

    QFile newFile(newUprojectPath);
    if (!newFile.open(QIODevice::WriteOnly)) {
        QDir().rename(newDirPath, oldDirPath);
        QFile::rename(newDirPath + "/" + newName + ".uproject", oldUprojectPath);
        return false;
    }
    newFile.write(data);
    newFile.close();

    return true;
}

void ProjectCardWidget::commitNameEdit() {
    if (!m_nameEdit) return;
    QString newName = m_nameEdit->text().trimmed();
    QString oldName = m_project.projectName;

    m_nameEdit->setVisible(false);
    m_nameLabel->setVisible(true);

    if (newName.isEmpty() || newName == oldName) {
        m_nameLabel->setText(oldName);
        return;
    }

    if (!renameProjectOnDisk(m_project.projectPath, oldName, newName)) {
        QMessageBox::warning(this, "Error", "Failed to rename project.\nThe folder name might already exist.");
        m_nameLabel->setText(oldName);
        return;
    }

    QString newPath = QFileInfo(m_project.projectPath).absolutePath() + "/" + newName + "/" + newName + ".uproject";

    DesktopEntryWriter::removeProjectEntry(oldName);

    QString launcherBin = ProjectScannerService::findLauncherBinary();
    DesktopEntryWriter::writeProjectEntry(newName, newPath, launcherBin);

    ConfigManager::removeKnownProject(m_project.projectPath);

    m_project.projectName = newName;
    m_project.projectPath = newPath;
    m_project.isFavorite = false;
    ConfigManager::saveKnownProject(m_project);

    m_nameLabel->setText(newName);
    emit nameChanged();
}
