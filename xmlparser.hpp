#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <QObject>
#include <QDebug>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QMultiMap>
#include <QFile>
#include <QMessageBox>
#include <types.hpp>

class XmlParser : public QObject
{
    Q_OBJECT
public:
    explicit    XmlParser(QObject *parent = 0, QFile *xmlFile = NULL, QString rootElement = XMLFILELIST_DOCTYPE);
    ~XmlParser(void);

    //QDomNode    getFileList(QDomNode rootElement);
    QDomNode    getFileList();
    QDomNode    getFolder(QString foldername, QDomNode rootElement);

    QMultiMap<QString, QDomNode>   getFilesFromFolder(QDomNode *folder);
    QMultiMap<QString, QDomNode>   getSubfolders(QDomNode *folder);

    QDomElement getRootElement();

    bool        loadNodeList(void);

protected:
    QString         fileListRootElement;
    QFile           *xmlFile;
    QDomDocument    fileListDocument;
    QDomElement     fileListRootDomElement;

    QMultiMap<QString, QDomNode>   getSubentriesFromNodeByTagName(QDomNode *node, QString tagName);

signals:
    
public slots:
    
};

#endif // XMLPARSER_HPP
