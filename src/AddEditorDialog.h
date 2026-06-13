#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class AddEditorDialog : public QDialog {
    Q_OBJECT

public:
    AddEditorDialog(QWidget *parent = nullptr);

private slots:
    void browseFolder();
    void onNameChanged(const QString &text);
    void addEditor();

private:
    QString selectedRootPath;
    QLineEdit *nameLineEdit;
    QLineEdit *launchArgsLineEdit;
    QPushButton *addButton;
    QPushButton *browseButton;
};
