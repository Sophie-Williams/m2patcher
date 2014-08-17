#ifndef HASHGENERATOR_HPP
#define HASHGENERATOR_HPP

#include <QObject>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <types.hpp>
#include <boost/thread.hpp>

#include <hashworker.hpp>
#include <QThread>
#include <QMutex>

class HashGenerator : public QObject
{
    Q_OBJECT
public:

    enum HashType {
        SHA1,
        MD5
    };

    explicit HashGenerator(QObject *parent = 0);
    // QByteArray generate(QFile* file, QCryptographicHash::Algorithm hashType = QCryptographicHash::Sha1);
    QByteArray generate(SDownloadFile &dlFile, QCryptographicHash::Algorithm hashType = QCryptographicHash::Sha1);

protected:
    //QByteArray hash(QFile* file, QByteArray* target, QCryptographicHash::Algorithm hashType);
    void hash();

    QFile* fileRef;
    QCryptographicHash::Algorithm hashType;

    QByteArray hashString;
    boost::timed_mutex mutex;
    
signals:
    
public slots:
    void setHash(QByteArray);
    void errorString(QString);
    
};

#endif // HASHGENERATOR_HPP
