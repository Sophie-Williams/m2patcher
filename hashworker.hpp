#ifndef HASHWORKER_HPP
#define HASHWORKER_HPP

#include <QObject>
#include <types.hpp>
#include <QCryptographicHash>
#include <QMutex>
#include <QDebug>

class HashWorker : public QObject
{
    Q_OBJECT
public:
    HashWorker(QObject *parent = 0);
    void setContent(SDownloadFile &dlFile, QCryptographicHash::Algorithm hashType);
    ~HashWorker();

	QMutex mut;
	QByteArray hashBuffer;

public slots:
    void process();

signals:
    void finished();
    void error(QString err);
    void outputHash(QByteArray hash);
    
private:
    SDownloadFile dlFile;
    QCryptographicHash::Algorithm hashType;
};

#endif // HASHWORKER_HPP
