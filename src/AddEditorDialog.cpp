#include "AddEditorDialog.h"
#include "DesktopEntryWriter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "ConfigManager.h"

AddEditorDialog::AddEditorDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Add Unreal Editor");
    setMinimumWidth(350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Browse row
    QHBoxLayout *browseLayout = new QHBoxLayout();
    QLabel *pathLabel = new QLabel("Engine Path:", this);
    browseButton = new QPushButton("Browse", this);
    browseLayout->addWidget(pathLabel);
    browseLayout->addWidget(browseButton);

    // Name row
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Name:", this);
    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setPlaceholderText("e.g. UE5.6");
    nameLineEdit->setEnabled(false); // Enable after selecting valid folder
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameLineEdit);

    // Launch args row
    QHBoxLayout *launchArgsLayout = new QHBoxLayout();
    QLabel *launchArgsLabel = new QLabel("Launch Args:", this);
    launchArgsLineEdit = new QLineEdit(this);
    launchArgsLineEdit->setPlaceholderText("e.g. -game -project=/path/to/proj.uproject");
    launchArgsLayout->addWidget(launchArgsLabel);
    launchArgsLayout->addWidget(launchArgsLineEdit);

    // Add button
    addButton = new QPushButton("Add", this);
    addButton->setEnabled(false); // Enable when name is not empty

    mainLayout->addLayout(browseLayout);
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(launchArgsLayout);
    mainLayout->addWidget(addButton, 0, Qt::AlignCenter);

    connect(browseButton, &QPushButton::clicked, this, &AddEditorDialog::browseFolder);
    connect(nameLineEdit, &QLineEdit::textChanged, this, &AddEditorDialog::onNameChanged);
    connect(addButton, &QPushButton::clicked, this, &AddEditorDialog::addEditor);
}

void AddEditorDialog::browseFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Engine Root Folder",
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        QString editorPath = dir + "/Engine/Binaries/Linux/UnrealEditor";
        if (QFile::exists(editorPath)) {
            selectedRootPath = dir;
            nameLineEdit->setEnabled(true);
            nameLineEdit->setFocus();
            QDir d(dir);
            nameLineEdit->setText(d.dirName());
        } else {
            QMessageBox::warning(this, "Invalid Folder", "Could not find UnrealEditor in the selected folder.\nPlease select the correct Unreal Engine root folder.");
            nameLineEdit->setEnabled(false);
            nameLineEdit->clear();
            selectedRootPath.clear();
        }
    }
}

void AddEditorDialog::onNameChanged(const QString &text) {
    addButton->setEnabled(!text.trimmed().isEmpty() && !selectedRootPath.isEmpty());
}

void AddEditorDialog::addEditor() {
    QString name = nameLineEdit->text().trimmed();
    QString launchArgs = launchArgsLineEdit->text().trimmed();
    if (DesktopEntryWriter::write(name, selectedRootPath, launchArgs)) {
        // Save to config as well so View Editors shows the entry immediately
        ConfigManager::saveEntry({name, selectedRootPath, launchArgs});
        QMessageBox::information(this, "Success", "Desktop entry created successfully.");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create desktop entry.");
    }
}
