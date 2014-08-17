#ifndef HTTPHANDLER_HPP
#define HTTPHANDLER_HPP

#include <QObject>
#include <QDebug>
#include <QHttp>

class HttpHandler : public QObject
{
    Q_OBJECT
public:
    explicit HttpHandler(QObject *parent = 0);
    void submitComputerDetails(void);

signals:
    
public slots:
    void stateChanged(int state);
    void responseHeaderReceived ( const QHttpResponseHeader & resp );
    void requestFinished ( int id, bool error );

private:
    QHttp *http;

};

#endif // HTTPHANDLER_HPP
