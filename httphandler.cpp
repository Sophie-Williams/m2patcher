#include "httphandler.hpp"

HttpHandler::HttpHandler(QObject *parent) :
    QObject(parent)
{
    http = new QHttp(this);
   connect(http, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
   connect(http, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(responseHeaderReceived(QHttpResponseHeader)));
   connect(http, SIGNAL(requestFinished(int,bool)), this, SLOT(requestFinished(int,bool)));
}

void HttpHandler::stateChanged(int state)   {
    switch(state)   {
    case 0:
        qDebug() << "Unconnected";
        break;
    case 1:
        qDebug() << "Host Lookup";
        break;
    case 2:
        qDebug() << "Connecting";
        break;
    case 3:
        qDebug() << "Sending";
        break;
    case 4:
        qDebug() << "Reading";
        break;
    case 5:
        qDebug() << "Connect";
        break;
    case 6:
        qDebug() << "Closing";
        break;
    }
}

void HttpHandler::responseHeaderReceived(const QHttpResponseHeader &resp)   {
    qDebug() << "Size : " << resp.contentLength();
    qDebug() << "Type : " << resp.contentType();
    qDebug() << "Status Code : " << resp.statusCode();
}

void HttpHandler::requestFinished(int id, bool error)   {
    qDebug() << "Request Id : " << id;
    if(error)   {
        qDebug() << "Error";
    }   else    {
        qDebug() << http->readAll();
    }
}
