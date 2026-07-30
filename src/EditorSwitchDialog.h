#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "EditorEntry.h"

class EditorSwitchDialog : public QDialog {
    Q_OBJECT

public:
    EditorSwitchDialog(const QString& projectName, const QList<EditorEntry>& editors, QWidget* parent = nullptr);
    EditorEntry selectedEditor() const;

private slots:
    void onSelectionChanged();
    void onAccepted();

private:
    QListWidget* m_listWidget;
    QPushButton* m_okButton;
    QList<EditorEntry> m_editors;
    int m_selectedIndex = -1;
};
