#ifndef DOWNLOADMANAGER_HPP
#define DOWNLOADMANAGER_HPP

#include <QObject>
#include <QFile>
#include <QDir>
#include <QMultiMap>
#include <QList>
#include <QApplication>
#include <types.hpp>
#include <downloader.hpp>
#include <xmlparser.hpp>

class DownloadManager : public QThread
{
    Q_OBJECT
public:
	DownloadManager();

    bool downloadFile(QString folder, QString file);
    // void getFileList(void);
    SDownloadFile* getActiveDownloadFile(void);

    void setDownloadHost(QString, int port = 80, QString protocol = "http"); // Host, Port, Protocol
    void setDownloadRootfolder(QString); // Folder

    //void startDownload(void);
    void getFileListFromXml(QFile*);

    QMultiMap<QString, QStringList>* getFileStartList(void);
    void setXmlfilelistFilename(QString name);

	QString getTopNumber();

	void run();

protected:
    bool createTempDir(void);
    bool isTempDirExisting(void);
    void recursiveXmlWalker(QDomElement root, QString path="");
    FileStatus isFileOk(SDownloadFile *file);    

public:
    QDir tmpDir;
    QDir rootPath;

protected:
    SDownloadFile fileList;

    DownloadHost downloadHost;

    QMultiMap<QString, QDomElement> *filesToDownload;
    QMultiMap<QString, QStringList> *filesToStart;

    Downloader *activeDownload;

    QString xmlfilelist_filename;

	QString toplistNumber;

signals:
    void checkFile(QFile &);
    void updateProgress(qint64, qint64);
    void reportDownloadStatus(SDownloadFile *, FileStatus);
	void downloadingComplete(SDownloadFile*);
    void writeLog(QString);
	void xmlParsed();
	void topListLink(QString);

    void finished();
    void error(QString err);

public slots:
    void replyFileStatus(SDownloadFile &, FileStatus);
    void dlPrg(qint64,qint64);
    void downloadComplete();
    void writeFileStatusLog(QString, QString);
    void destruct();

    void startDownload(void);
    void getFileList(void);

    void process();

protected slots:
    void nextDownload();
    
};

#endif // DOWNLOADMANAGER_HPP
