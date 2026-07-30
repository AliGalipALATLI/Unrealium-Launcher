#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include "ConfigManager.h"

class ProjectCardWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProjectCardWidget(const KnownProject& project, QWidget* parent = nullptr);

signals:
    void deletionRequested();
    void favoriteChanged();
    void nameChanged();

private slots:
    void toggleFavorite();
    void deleteProject();
    void startEditingName();
    void commitNameEdit();

private:
    KnownProject m_project;
    QLabel* m_nameLabel;
    QLineEdit* m_nameEdit = nullptr;
    QToolButton* m_deleteButton;
    QToolButton* m_favoriteButton;
    QPixmap m_emptyPix;
    QPixmap m_fullPix;
    void updateFavoriteIcon(bool hovered = false);
    static bool renameProjectOnDisk(const QString& oldPath, const QString& oldName, const QString& newName);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};
