#include "xmlparser.hpp"

XmlParser::XmlParser(QObject *parent, QFile *xmlFile, QString rootElement) :
    QObject(parent)
{
    if(xmlFile == NULL) {
        qDebug() << "XML Parser file cannot be NULL";
        return;
    }

    this->xmlFile               = xmlFile;
    this->fileListRootElement   = rootElement;

    this->fileListDocument = QDomDocument(XMLFILELIST_DOCTYPE);
}

XmlParser::~XmlParser() {
    if(this->xmlFile->isOpen())
        this->xmlFile->close();
}

bool XmlParser::loadNodeList() {
    // this->xmlFile

    if(!this->xmlFile->isOpen()) {
        bool result = this->xmlFile->open(QIODevice::ReadOnly);
        if(!result) {
            qDebug() << "FileOpenError: " + this->xmlFile->errorString();
            return false;
        }
    }

    if(this->xmlFile->size() < 1) {
        this->xmlFile->close();
        qDebug() << "File is empty!";
        return false;
    }

    qDebug() << "Filelist File: " + this->xmlFile->fileName();


    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!this->fileListDocument.setContent(this->xmlFile, false, &errorStr, &errorLine, &errorColumn)) {
        this->xmlFile->close();
        qDebug()  << "Error: Parse error at line " << errorLine << ","
                  << "column " << errorColumn << ": "
                  << errorStr;
        return false;
    }
/*
    if(!this->fileListDocument.setContent(this->xmlFile)) {
        this->xmlFile->close();
        qDebug() << "Cannot parse XmlContent";
        return false;
    }
*/
    qDebug() << "File is completely parsed";

    this->fileListRootDomElement = this->fileListDocument.documentElement();
    return true;
}

QDomNode XmlParser::getFileList() {

    if(this->fileListRootDomElement.childNodes().count() < 1)
        this->loadNodeList();

    QDomNodeList childs = this->fileListRootDomElement.childNodes();
    QDomNode output;

    qDebug() << "FileList has "+QString::number(childs.size())+" entries";

    if(childs.size() < 1) {
        qDebug() << "Filelist ist fehlerhaft!";
        exit(-1);
    }
/*
    QDomNode childNode = this->fileListRootDomElement.firstChild();

    while(!childNode.isNull()) {
        if(childNode.nodeType() == QDomNode::TextNode && childNode.toElement().tagName() == "file") {
            qDebug() << "Processing Entry# Tag "+childNode.toElement().tagName()+" #Name "+childNode.toElement().attribute("name");
            output.appendChild(childNode);
        }
        childNode.nextSibling();
    }
*/
    return output;
}

QDomNode XmlParser::getFolder(QString foldername, QDomNode rootElement) {
    if(!(rootElement.childNodes().count() < 1))
        rootElement = this->fileListRootDomElement;

    QDomNode childNode = rootElement.firstChild();
    while(!childNode.isNull()) {
        if(
                childNode.toElement().tagName() == "folder" &&
                childNode.toElement().attribute("name") == foldername
        ) {
           return childNode;
        }
        childNode.nextSibling();
    }

    return childNode;
}

QMultiMap<QString, QDomNode> XmlParser::getSubentriesFromNodeByTagName(QDomNode *node, QString tagName) {
    QDomNode childNode = node->firstChild();
    QMultiMap<QString, QDomNode> childList;

    while(!childNode.isNull()) {
        if(childNode.toElement().tagName() == tagName) {
            childList.insert(childNode.toElement().attribute("name"), childNode);
        }
        childNode.nextSibling();
    }

    return childList;
}

QMultiMap<QString, QDomNode> XmlParser::getFilesFromFolder(QDomNode *folder) {
    return this->getSubentriesFromNodeByTagName(folder, "folder");
}

QMultiMap<QString, QDomNode> XmlParser::getSubfolders(QDomNode *folder) {
    return this->getSubentriesFromNodeByTagName(folder, "file");
}

QDomElement XmlParser::getRootElement()
{
    return this->fileListRootDomElement;
}
