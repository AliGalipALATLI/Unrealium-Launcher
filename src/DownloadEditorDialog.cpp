#include "DownloadEditorDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QDir>
#include <QFileInfo>

DownloadEditorDialog::DownloadEditorDialog(QWidget *parent) : QDialog(parent), networkManager(new QNetworkAccessManager(this)), currentReply(nullptr), downloadFile(nullptr) {
    setWindowTitle("Download Unreal Engine");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *infoLabel = new QLabel("Select an Unreal Engine version to download.", this);
    mainLayout->addWidget(infoLabel);

    versionComboBox = new QComboBox(this);
    
    // Populate the versions
    struct UEVersion {
        QString name;
        QString url;
    };

    QList<UEVersion> versions = {
        {"Unreal Engine 5.8.0 Preview 1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/43/a9/1660-68b1-4b2e-aa71-22851fb19444"},
        {"Unreal Engine 5.7.4", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/40/f8/1da3-a635-4dd3-90f8-b0fa66fa6951"},
        {"Unreal Engine 5.7.3", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/63/5c/71ed-9bc9-436d-8b4b-af858cea7d7c"},
        {"Unreal Engine 5.7.2", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/ad/79/6508-f5d7-45e6-ab2c-4d0acebdaa2f"},
        {"Unreal Engine 5.7.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/6a/83/2664-1399-4364-a400-6dd312ddbc29"},
        {"Unreal Engine 5.7.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/cb/1a/fca2-7fdc-4278-8992-8984da0b4f7c"},
        {"Unreal Engine 5.6.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/a6/74/6941-c1f8-4517-9935-eb628e64deca"},
        {"Unreal Engine 5.6.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/5c/13/a883-e89a-4e0c-bc2d-b3068924487e"},
        {"Unreal Engine 5.5.4", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/9f/02/31b6-fe92-4ed1-a45c-3e378aadb63b"},
        {"Unreal Engine 5.5.3", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/69/2d/1e89-81d6-4332-a266-a9f07b5a2178"},
        {"Unreal Engine 5.5.2", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/dc/53/656c-52af-49a7-a767-e2b15f1d2a54"},
        {"Unreal Engine 5.5.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/3f/3f/0beb-810f-4632-af59-e0cde2571944"},
        {"Unreal Engine 5.5.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/cf/01/58c6-6d9f-48b2-8dff-e5f2e0762b1f"},
        {"Unreal Engine 5.4.4", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/1d/67/9995-2ead-4b8c-9004-2091a21b05c8"},
        {"Unreal Engine 5.4.3", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/0e/d7/ac48-36a1-4589-be0f-eb3c77bf2caf"},
        {"Unreal Engine 5.4.2", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/62/fe/1df0-e04e-4749-909a-e799b6acdf49"},
        {"Unreal Engine 5.4.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/ba/26/dde3-4b7d-4309-a978-8099a533cd39"},
        {"Unreal Engine 5.4.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/f3/1c/8f9f-61cc-4655-abfc-c064ee867a2c"},
        {"Unreal Engine 5.3.2", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/c0/1d/f01f-73a9-4db4-9e1b-bde8090b673f"},
        {"Unreal Engine 5.3.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/15/9e/942d-9917-483a-af1d-82a1af8c0a24"},
        {"Unreal Engine 5.3.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/71/b2/cf1a-7eff-4b77-b198-456ff056389f"},
        {"Unreal Engine 5.2.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/bb/23/fb99-7e96-4f4b-8a0f-678684655c9d"},
        {"Unreal Engine 5.2.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/be/1f/cac4-78f9-42fa-a7e5-67f86c3cc89d"},
        {"Unreal Engine 5.1.1", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/b8/d5/90e7-70cd-450d-8ea8-1141aa91ed40"},
        {"Unreal Engine 5.1.0", "https://ucs-blob-store.s3-accelerate.amazonaws.com/blobs/b8/1c/231c-63ec-4d57-b0dc-ab856cfccb6a"}
    };

    for (const auto& v : versions) {
        versionComboBox->addItem(v.name, v.url);
    }
    
    mainLayout->addWidget(versionComboBox);

    statusLabel = new QLabel("", this);
    statusLabel->setVisible(false);
    mainLayout->addWidget(statusLabel);

    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    cancelButton = new QPushButton("Cancel", this);
    downloadButton = new QPushButton("Download", this);
    
    // Give download button a primary style
    downloadButton->setStyleSheet("background-color: #0078D7; color: white; padding: 6px 12px; border: none; border-radius: 4px;");

    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(downloadButton);

    mainLayout->addLayout(buttonsLayout);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(downloadButton, &QPushButton::clicked, this, &DownloadEditorDialog::onDownloadClicked);
}

void DownloadEditorDialog::onDownloadClicked() {
    if (currentReply) {
        return; // Already downloading
    }

    QString urlString = versionComboBox->currentData().toString();
    if (urlString.isEmpty()) return;

    QUrl url(urlString);
    
    // Create a generic filename from the version text
    QString selectedText = versionComboBox->currentText();
    QString filename = selectedText.replace(" ", "_") + ".zip";
    
    downloadFile = new QFile(filename, this);
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Could not create file for writing: " + filename);
        delete downloadFile;
        downloadFile = nullptr;
        return;
    }

    QNetworkRequest request(url);
    currentReply = networkManager->get(request);

    connect(currentReply, &QNetworkReply::downloadProgress, this, &DownloadEditorDialog::onDownloadProgress);
    connect(currentReply, &QNetworkReply::finished, this, &DownloadEditorDialog::onDownloadFinished);
    connect(currentReply, &QNetworkReply::readyRead, this, &DownloadEditorDialog::onReadyRead);

    // Update UI for download state
    versionComboBox->setEnabled(false);
    downloadButton->setEnabled(false);
    cancelButton->setText("Cancel Download");
    
    // Disconnect previous reject connection and connect cancel to abort
    disconnect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        if (currentReply) {
            currentReply->abort();
        } else {
            reject();
        }
    });

    statusLabel->setText("Downloading " + filename + "...");
    statusLabel->setVisible(true);
    progressBar->setValue(0);
    progressBar->setVisible(true);
}

void DownloadEditorDialog::onReadyRead() {
    if (currentReply && downloadFile) {
        downloadFile->write(currentReply->readAll());
    }
}

void DownloadEditorDialog::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    if (bytesTotal > 0) {
        progressBar->setMaximum(bytesTotal);
        progressBar->setValue(bytesReceived);
        
        double mbReceived = bytesReceived / (1024.0 * 1024.0);
        double mbTotal = bytesTotal / (1024.0 * 1024.0);
        statusLabel->setText(QString("Downloading: %1 MB / %2 MB").arg(mbReceived, 0, 'f', 1).arg(mbTotal, 0, 'f', 1));
    }
}

void DownloadEditorDialog::onDownloadFinished() {
    if (downloadFile) {
        downloadFile->close();
    }

    if (currentReply->error() != QNetworkReply::NoError) {
        // If aborted or network error
        if (currentReply->error() == QNetworkReply::OperationCanceledError) {
            statusLabel->setText("Download cancelled.");
            if (downloadFile) downloadFile->remove(); // delete partial file
        } else {
            QMessageBox::critical(this, "Download Error", currentReply->errorString());
            statusLabel->setText("Download failed.");
        }
    } else {
        progressBar->setValue(progressBar->maximum());
        statusLabel->setText("Download complete!");
        QMessageBox::information(this, "Success", "Download completed successfully.\n\nExtract the ZIP file and use 'Add Editor' to link it.");
        accept();
        return;
    }

    // Reset UI if failed or cancelled
    currentReply->deleteLater();
    currentReply = nullptr;
    if (downloadFile) {
        delete downloadFile;
        downloadFile = nullptr;
    }
    
    versionComboBox->setEnabled(true);
    downloadButton->setEnabled(true);
    cancelButton->setText("Close");
    disconnect(cancelButton, &QPushButton::clicked, 0, 0);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}
