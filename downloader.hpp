#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <QObject>
#include <QDebug>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QFile>
#include <QTime>
#include <QDir>
#include <types.hpp>
#include <hashgenerator.hpp>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0); // explicit keywoard is used to avoid implicit type conversion.
    void setDownloadFile(QDomElement dlFile);
    void setDownloadFile(SDownloadFile& dlFile);
    void setDownloadHost(DownloadHost *dlHost);
    void setRootFolder(QDir rootPath);
    void setDownloadUrl(QString);
    DownloadHost *getDownloadHost(void);
    SDownloadFile* getCurrentDownloadFile(void);

    bool startDownload();

    QTime getDownloadTime();
    QNetworkReply *getCurrentDownload();

protected:
	QNetworkAccessManager           manager;
    QNetworkReply                   *currentDownload;
    QFile                           currentFile;
    QTime                           downloadTime;
    QDir                            rootPath;

    QString                         downloadUrl;

    SDownloadFile                   *dlFile;
    DownloadHost                    *dlHost;

public slots:
     void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
     void downloadFinished();

private slots:
     void downloadReadyRead();
     void downloadError(QNetworkReply::NetworkError);

signals:
    void finished();
    void error(QNetworkReply::NetworkError);
    void fileDownloadStatus(QString status, QString filename);
    void dlPrg(qint64 bytesReceived, qint64 bytesTotal);
    
};

#endif // DOWNLOADER_HPP
