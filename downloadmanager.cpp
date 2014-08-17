#include "downloadmanager.hpp"

DownloadManager::DownloadManager(){

    this->rootPath = QDir(QApplication::applicationDirPath());
    this->tmpDir = QDir(this->rootPath.path() + "/" + "tmp");
    this->filesToDownload = new QMultiMap<QString, QDomElement>;
    this->filesToStart = new QMultiMap<QString, QStringList>;

    downloadHost.host = "";
    downloadHost.port = 80;
    downloadHost.protocol = "http";
    downloadHost.rootFolder = "/";

    this->activeDownload = new Downloader(this);
    this->activeDownload->setRootFolder(rootPath);
    this->activeDownload->setDownloadHost(&downloadHost);

    connect(this->activeDownload, SIGNAL(finished()), SLOT(nextDownload()));
    connect(this->activeDownload, SIGNAL(dlPrg(qint64,qint64)), SLOT(dlPrg(qint64,qint64)));
    connect(this->activeDownload, SIGNAL(fileDownloadStatus(QString,QString)), SLOT(writeFileStatusLog(QString, QString)));

//    if(!this->isTempDirExisting())
//        this->createTempDir();

    // this->writeFileStatusLog("Generating Hash", file->fileRef->fileName());

}

/**
 * File Functions
 */
FileStatus DownloadManager::isFileOk(SDownloadFile *file) {

    QByteArray hash = file->hash;

    qDebug() << "FILE: " + file->fileRef->fileName();

    if(!file->fileRef->isOpen()) {
        bool results = file->fileRef->open(QIODevice::ReadOnly);
        if(!results)
                qDebug() << file->fileRef->errorString();
    }

    if(!file->fileRef->exists())
        return FileNotExistant;

    QByteArray checksum;

    this->writeFileStatusLog("Generating Hash", file->fileRef->fileName());
    HashGenerator *hg = new HashGenerator(this);
    checksum = hg->generate(*file);

#ifdef DEBUG
    emit writeLog("Checksum Online: " + hash);
    emit writeLog("Checksum Local: " + checksum);
#endif

    if(hash != checksum)
        return FileHasBadChecksum;

    return FileOk;

}

/**
 * Download Functions
 */

void DownloadManager::nextDownload() {

    if(this->filesToDownload->count() < 1) {
        this->downloadComplete();
        return;
    }

    emit reportDownloadStatus(this->activeDownload->getCurrentDownloadFile(), FileOk);

    qDebug() << "Entrys in list left: " + QString::number(this->filesToDownload->count());
    qDebug() << "Next entry is: " + this->filesToDownload->begin().key();

    this->activeDownload->setDownloadFile(this->filesToDownload->begin().value());

    SDownloadFile *file1 = this->activeDownload->getCurrentDownloadFile();
    file1->fileRef->setFileName(this->rootPath.absolutePath()+file1->folder.path() + "/" + file1->name);
    FileStatus downloadFileStatus = this->isFileOk(file1);
    this->filesToDownload->remove(this->filesToDownload->begin().key());

    if(file1->runAfterDownload)
        this->filesToStart->insert(file1->fileRef->fileName(), QStringList());

    qDebug() << "FileDownloadStatus: " << downloadFileStatus;

    if(downloadFileStatus != FileOk) {
        // qDebug() << "(Re)downloading File";
    } else {
        // qDebug() << "Skipping File";
        this->nextDownload();
        return;
    }

	qDebug() << "DLM";
	qDebug() << this->thread();

    this->activeDownload->startDownload();
    emit reportDownloadStatus(this->activeDownload->getCurrentDownloadFile(), FileIsDownloading);
}

void DownloadManager::writeFileStatusLog(QString status, QString file)
{
    qDebug() << "LOG: "+file+": "+status;
#ifdef DEBUG
    emit writeLog(file+": "+status);
#endif
}

void DownloadManager::destruct()
{
    emit finished();
}

void DownloadManager::process()
{
    // this->startDownload();
}

/**
 * Parser Functions
 */

void DownloadManager::getFileList() {

    this->activeDownload->setDownloadUrl(
                    this->activeDownload->getDownloadHost()->protocol+"://"+
                    this->activeDownload->getDownloadHost()->host+":"+
                    QString::number(this->activeDownload->getDownloadHost()->port)+"/"+
                    this->activeDownload->getDownloadHost()->rootFolder
                );

    this->fileList.name = this->xmlfilelist_filename;
    this->activeDownload->setDownloadFile(this->fileList);
    this->activeDownload->startDownload();
}

SDownloadFile* DownloadManager::getActiveDownloadFile()
{
    return this->activeDownload->getCurrentDownloadFile();
}

/**
 * TempDir Functions
 */
bool DownloadManager::createTempDir() {
    if(!this->isTempDirExisting())
        return this->rootPath.mkdir(this->tmpDir.path());

    return true;
}

bool DownloadManager::isTempDirExisting() {
    return this->tmpDir.exists();
}

/**
 * Download Funtions
 */

void DownloadManager::setDownloadHost(QString host, int port, QString protocol)
{
    this->downloadHost.protocol = protocol;
    this->downloadHost.host = host;
    this->downloadHost.port = port;
}

void DownloadManager::setDownloadRootfolder(QString folder) {
    this->downloadHost.rootFolder = folder;
}

void DownloadManager::startDownload() {
    this->getFileListFromXml(this->fileList.fileRef);
	emit xmlParsed();
	emit topListLink(this->toplistNumber);
    this->nextDownload();
}

void DownloadManager::getFileListFromXml(QFile* filelist) {

    while(filelist->isOpen()) {
        filelist->close();
    }

    if(!filelist->isOpen() || !filelist->isReadable()) {
        bool open = filelist->open(QIODevice::ReadOnly);
        if(!open) {
            qDebug() << "FileList cannot be opened!";
            exit(-1);
        }
    }

    XmlParser *xml = new XmlParser(this, filelist);
    // qDebug() << "Loading NodeList";
    xml->loadNodeList();
    // qDebug() << "Loaded NodeList";

    // qDebug() << "Load FileList";
    xml->getFileList();
    // qDebug() << "Loaded FileList";

    // qDebug() << "Childs: "+ xml->getRootElement().childNodes().count();
    this->recursiveXmlWalker(xml->getRootElement());

    this->activeDownload->setDownloadUrl(xml->getRootElement().attribute("rooturl"));

	if(xml->getRootElement().hasAttribute("list"))
		this->toplistNumber = QString(xml->getRootElement().attribute("list"));
	else
		this->toplistNumber = QString("1738-thoron-mt2-com");

	qDebug() << "XmlRead complete";

    // qDebug() << "Kinder: " + xmlFileList.childNodes().count();
}

QMultiMap<QString, QStringList> *DownloadManager::getFileStartList()
{
    return this->filesToStart;
}

void DownloadManager::setXmlfilelistFilename(QString name)
{
	this->xmlfilelist_filename = name;
}

QString DownloadManager::getTopNumber()
{
	return this->toplistNumber;
}

void DownloadManager::run()
{
}

void DownloadManager::recursiveXmlWalker(QDomElement root, QString path) {
    QDomNode childNode = root.firstChild();
    // qDebug() << "Begin! : " + childNode.toElement().attribute("name") + " # Type: "+childNode.toElement().tagName();

    while(!childNode.isNull()) {
        if(childNode.toElement().tagName() == "folder") {
            qDebug() << "Processing Entry# Tag "+childNode.toElement().tagName()+" #Name "+childNode.toElement().attribute("name");
            recursiveXmlWalker(childNode.toElement(), path+"/"+childNode.toElement().attribute("name"));
        }
        if(childNode.toElement().tagName() == "file") {
            qDebug() << "Processing Entry# Tag "+childNode.toElement().tagName()+" #Name "+path+"/"+childNode.toElement().attribute("name");

            QDomElement fE = childNode.toElement();
            fE.setAttribute("folder", path);
            this->filesToDownload->insert(path+"/"+childNode.toElement().attribute("name"), fE);

        }
        childNode = childNode.nextSibling();
    }
}

/**
 * SLOTS
 */

void DownloadManager::downloadComplete() {
    if(this->filesToDownload->size() == 0)
		emit downloadingComplete(this->getActiveDownloadFile());
}

void DownloadManager::replyFileStatus(SDownloadFile &file, FileStatus status) {
    file.status = status;
}

void DownloadManager::dlPrg(qint64 recv,qint64 sum) {
    emit updateProgress(recv, sum);
    // qDebug() << "DownloadManager::dlPrg("+QString::number(recv)+","+QString::number(sum)+")";
}

