#include "downloader.hpp"

Downloader::Downloader(QObject *parent) :
    QObject(parent), dlFile(new SDownloadFile())
{
	qDebug() << "DL-C";
	qDebug() << this->thread();
}

void Downloader::setRootFolder(QDir rootPath) {
    this->rootPath = rootPath;
}

void Downloader::setDownloadFile(QDomElement dlFile) {
    SDownloadFile file;
    file.alias = dlFile.attribute("alias");
    file.downloadForce = dlFile.attribute("force") == "1" ? true : false;
    file.folder = QDir(dlFile.attribute("folder"));
    file.hash = dlFile.attribute("hash").toStdString().c_str();
    file.name = dlFile.attribute("name");
    file.runAfterDownload = dlFile.attribute("run") == "1" ? true : false;
    file.size = atoi(dlFile.attribute("size").toStdString().c_str());

    this->setDownloadFile(file);
}

void Downloader::setDownloadFile(SDownloadFile& dlFile) {
    //this->dlFile = dlFile;
    //memcpy(&this->dlFile, &dlFile, sizeof(this->dlFile));

    if(dlFile.alias.size() > 0)
        this->dlFile->alias = dlFile.alias;

    this->dlFile->downloadForce = dlFile.downloadForce;
    this->dlFile->fileRef = dlFile.fileRef;
    this->dlFile->folder = dlFile.folder;
    this->dlFile->hash = dlFile.hash;
    this->dlFile->name = dlFile.name;
    this->dlFile->runAfterDownload = dlFile.runAfterDownload;
    this->dlFile->size = dlFile.size;
    this->dlFile->status = dlFile.status;

    qDebug() << "Next File is: " + this->rootPath.absolutePath() + "/" + this->dlFile->folder.path() + "/" + this->dlFile->name;

    //this->dlFile(dlFile);
}

void Downloader::setDownloadHost(DownloadHost *dlHost) {
    this->dlHost = dlHost;
}

QTime Downloader::getDownloadTime() {
    return downloadTime;
}

DownloadHost* Downloader::getDownloadHost() {
    return this->dlHost;
}

SDownloadFile* Downloader::getCurrentDownloadFile() {
    return this->dlFile;
}

QNetworkReply *Downloader::getCurrentDownload() {
    return currentDownload;
}

void Downloader::setDownloadUrl(QString dlUrl) {
    this->downloadUrl = dlUrl;
}

bool Downloader::startDownload() {

	qDebug() << "DL-DL";
	qDebug() << this->thread();

    qDebug() << "BEGIN LoadFile ["+ this->dlFile->folder.path() + "/" + this->dlFile->name + "]";

    if(this->dlFile->fileRef->fileName() == "")
        this->dlFile->fileRef->setFileName(this->rootPath.absolutePath()+"/"+this->dlFile->folder.path() + "/" + this->dlFile->name);

    // qDebug() << "File: " + this->dlFile->fileRef->fileName();

    QString folder(QFileInfo(*this->dlFile->fileRef).absolutePath());
    // qDebug() << QString("Folder \""+folder+"\"");

    if(!this->rootPath.exists(folder)) {
        bool created = this->rootPath.mkdir(folder);
        if(!created) {
            qDebug() << QString("Folder \""+folder+"\" cannot be created!");
            return false;
        } else {
            // qDebug() << QString("Folder \""+folder+"\" was created!");
        }
    }

    if(!this->dlFile->fileRef->isOpen() || this->dlFile->fileRef->openMode() != QIODevice::WriteOnly) {
        if(this->dlFile->fileRef->isOpen())
            this->dlFile->fileRef->close();

        this->dlFile->fileRef->open(QIODevice::WriteOnly);
    }

    if(!this->dlFile->fileRef->isOpen())
        qDebug() << "Error while opening \""+this->dlFile->fileRef->fileName()+"\"! # "+this->dlFile->fileRef->errorString();
    else
        qDebug() << "File is Open";

    QUrl downloadUrl;
    downloadUrl.setUrl(this->downloadUrl+"/"+this->dlFile->folder.path()+"/"+this->dlFile->name);
    // qDebug() << "Pre URL: " + this->downloadUrl;
    downloadUrl.setPort(this->dlHost->port);

    qDebug() << "Download URL: " << downloadUrl.toString();

    QNetworkRequest request(downloadUrl);
    request.setRawHeader("User-Agent", "Patcher V2.0.0 by Sammy8806");

    qDebug() << "Request is set";

	// QNetworkAccessManager manager;
	currentDownload = manager.get(request);

    qDebug() << "Request is get";

    emit fileDownloadStatus("Start", this->dlFile->fileRef->fileName());
    qDebug() << "Download-Status reported";

    connect(currentDownload,SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload,SIGNAL(finished()), SLOT(downloadFinished()));
    connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)), SLOT(downloadError(QNetworkReply::NetworkError)));
    connect(currentDownload,SIGNAL(readyRead()), SLOT(downloadReadyRead()));

    downloadTime.start();
    qDebug() << "Download-Time started";

    this->dlFile->status = FileIsDownloading;
    qDebug() << "Status is set to \"FileIsDownloading\"";

    qDebug() << "COMPLETE LoadFile [" + this->dlFile->name + "]" << endl;

    return true;
}

void Downloader::downloadFinished() {
    /*if(!this->dlFile->fileRef) {
        qDebug() << "FileRef is corrupt. Exit!";
        exit(-1);
    }*/

    if(this->dlFile->fileRef->isOpen()) {
        this->dlFile->fileRef->close();
    }
    qDebug() << "File \""+this->dlFile->name+"\" is closed";

    this->dlFile->status = FileHasDownloaded;

    // QString hash = hash_murmur3(filename.toStdString().c_str());
    // QString hash = hash_sha1(filename.toStdString().c_str());
    emit fileDownloadStatus("Generating Hash", this->dlFile->fileRef->fileName());
    HashGenerator *hg = new HashGenerator(this);
    QString hash = hg->generate(*this->dlFile);
    emit fileDownloadStatus("Checking Hash", this->dlFile->fileRef->fileName());

    qDebug() << "File: " << this->dlFile->fileRef->fileName() << "\nHash: " << hash;

    emit fileDownloadStatus("OK", this->dlFile->fileRef->fileName());
    emit finished();
    qDebug() << this->dlFile->fileRef->fileName() + " finished";
}

void Downloader::downloadReadyRead() {

    if(!this->dlFile->fileRef->isWritable()) {
        qDebug() << "File is not writeable! # "+this->dlFile->fileRef->errorString();
        return;
    }

    this->dlFile->fileRef->write(currentDownload->readAll());
    this->dlFile->fileRef->flush();
}

void Downloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    emit dlPrg(bytesReceived, bytesTotal);
    // qDebug() << "Recv: "+QString::number(bytesReceived)+" # Total: "+QString::number(bytesTotal);
}

void Downloader::downloadError(QNetworkReply::NetworkError) {

}
