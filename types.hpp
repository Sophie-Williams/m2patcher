#ifndef TYPES_HPP
#define TYPES_HPP

#include <QDir>
#include <QFile>
#include <QDomElement>

#define XMLFILELIST_DOCTYPE   "LauncherList"
#define XMLFILELIST_FILE_NAME   "files_2013_iv.xml"

// #define DEBUG

enum FileStatus {
    FileOk = 0,
    FileNotExistant = 1,
    FileHasBadChecksum = 2,
    FileHasUnknownError = 3,
    FileHasDownloaded = 4,
    FileIsDownloading = 5
} ;

struct SDownloadFile {
    QString         name;
    QString         alias;
    QDir            folder;
    QByteArray      hash;
    double          size;
    FileStatus      status;
    bool            downloadForce;
    bool            runAfterDownload;
    QFile           *fileRef;

    SDownloadFile() : size(0), downloadForce(false), runAfterDownload(false), status(FileNotExistant), name(""), alias(""), fileRef( new QFile()) {}
};

struct DownloadHost {
    QString     protocol;
    QString     host;
    quint16     port;
    QString     rootFolder;
};

#ifndef DEBUG
#define QT_NO_DEBUG_OUTPUT
#endif

#endif // TYPES_HPP
