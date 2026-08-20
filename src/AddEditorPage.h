#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QList>

#include "EngineDiscovery.h"

class AddEditorPage : public QWidget {
    Q_OBJECT

public:
    explicit AddEditorPage(QWidget* parent = nullptr);
    void rescanDiscovered();

signals:
    void editorAdded();
    void closed(); // user wants to leave the page (e.g. after adding or cancel)

private slots:
    void browseFolder();
    void onNameChanged(const QString& text);
    void addEditor();
    void adoptDiscovered(const QString& path);
    void cancelAndClose();

private:
    QString m_selectedRootPath;
    QLineEdit* m_pathLineEdit   = nullptr;
    QLineEdit* m_nameLineEdit   = nullptr;
    QLineEdit* m_launchArgsEdit = nullptr;
    QPushButton* m_addButton    = nullptr;
    QPushButton* m_browseButton = nullptr;
    QPushButton* m_rescanButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    QWidget*     m_discoveredHost = nullptr;

    void rebuildDiscoveredList();
    void setSelectedPath(const QString& path);
    bool writeEngine(const QString& name, const QString& path, const QString& launchArgs);
};