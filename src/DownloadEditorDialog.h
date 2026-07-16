#pragma once
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class DownloadEditorDialog : public QDialog {
    Q_OBJECT

public:
    explicit DownloadEditorDialog(QWidget *parent = nullptr);

private slots:
    void onDownloadClicked();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onReadyRead();

private:
    QComboBox *versionComboBox;
    QPushButton *downloadButton;
    QPushButton *cancelButton;
    
    QLabel *statusLabel;
    QProgressBar *progressBar;
    
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    QFile *downloadFile;
};
