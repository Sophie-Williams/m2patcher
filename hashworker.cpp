#include "hashworker.hpp"

HashWorker::HashWorker(QObject *parent) :
    QObject(parent)
{

}

void HashWorker::setContent(SDownloadFile &dlFile, QCryptographicHash::Algorithm hashType)
{
    this->dlFile = dlFile;
    this->hashType = hashType;
    this->hashBuffer = hashBuffer;
}

HashWorker::~HashWorker() {

}

void HashWorker::process()
{
    QFile* file = this->dlFile.fileRef;

    QCryptographicHash crypto(this->hashType);

    if(!file->isOpen())
        file->open(QFile::ReadOnly);

    while(!file->atEnd()){
        crypto.addData(file->read(4096));
    }

    this->hashBuffer = crypto.result().toHex();

	this->mut.unlock();
	qDebug() << "Unlock with hash: " << this->hashBuffer.data();

	emit outputHash(this->hashBuffer);
    emit finished();
}
