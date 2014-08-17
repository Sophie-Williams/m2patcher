#include "hashgenerator.hpp"

#include <QtConcurrentRun>
#include <QFutureWatcher>

HashGenerator::HashGenerator(QObject *parent) :
    QObject(parent)
{

}

void HashGenerator::hash() {

    QFile* file = this->fileRef;

    QCryptographicHash crypto(this->hashType);

    if(!file->isOpen() && file->isReadable())
        file->open(QFile::ReadOnly);

    while(!file->atEnd()){
        crypto.addData(file->read(8192));
    }

    //boost::unique_lock<boost::timed_mutex> lock(this->mutex, boost::try_to_lock);
    //if (!lock.owns_lock())
    //    lock.timed_lock(boost::get_system_time() + boost::posix_time::seconds(1));

    // QByteArray hashed = crypto.result().toHex();
    // memcpy(this->hashString, (void*)hashed, sizeof(this->hashString));
    this->hashString = crypto.result().toHex();

    //boost::timed_mutex *m = lock.release();
    // m->unlock();
}

void HashGenerator::setHash(QByteArray hash)
{
    qDebug() << "### Hash: "+hash;
    this->hashString = hash;
}

void HashGenerator::errorString(QString err)
{
    qDebug() << "### ERR: "+err;
}

QByteArray HashGenerator::generate(SDownloadFile &dlFile, QCryptographicHash::Algorithm hashType) {
    // const char* filename = QString(dlFile.folder.absolutePath() + "/" + dlFile.fileRef->fileName()).toStdString().c_str();
    qDebug() << "File to hash: "+dlFile.fileRef->fileName();
    this->fileRef = dlFile.fileRef;
    this->hashType = hashType;

    // boost::thread generator(&HashGenerator::hash, this);
    // generator.join();

	this->hash();

	return this->hashString;
}
