#-------------------------------------------------
#
# Project created by QtCreator 2013-05-29T15:23:32
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = M2Patcher_V2
TEMPLATE = app

win32-msvc* {
#  CONFIG += embed_manifest_exe
#  QMAKE_LFLAGS_WINDOWS += $$quote( /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\" )
}

SOURCES += main.cpp \
            patcherwindow.cpp \
            downloadmanager.cpp \
            downloader.cpp \
            xmlparser.cpp \
            hashgenerator.cpp \
       #     hashworker.cpp \
            httphandler.cpp \
            vote\toplist.cpp \
            vote\md5.cpp \
       #     vote\CTrie.cpp \
            vote\CSocket.cpp \
            vote\CAddress.cpp \
            vote\dbc\deathbycaptcha.c \
       #     vote\dbc\client.c \
            vote\dbc\cJSON.c \
            vote\dbc\base64.c

HEADERS  += patcherwindow.hpp \
            downloadmanager.hpp \
            downloader.hpp \
            xmlparser.hpp \
            hashgenerator.hpp \
            types.hpp \
       #     hashworker.hpp \
            httphandler.hpp \
            vote\toplist.h \
            vote\md5.h \
            vote\CTrie.h \
            vote\CSocket.h \
            vote\CAddress.h \
            vote\dbc\deathbycaptcha.h \
            vote\dbc\cJSON.h \
            vote\dbc\base64.h

FORMS    += patcherwindow.ui

RESOURCES += launcher.qrc

RC_FILE += launcher.rc
