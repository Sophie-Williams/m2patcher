#include "patcherwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString g_xmlfilelist_filename;
    QStringList argumente = QApplication::arguments();
    qDebug() << argumente;

    g_xmlfilelist_filename = "filelist.xml";
    qDebug() << "Argumente: " + QString::number(argc);
    if(argc > 1) {
        g_xmlfilelist_filename = argumente.at(2);
    }

    PatcherWindow w;
    w.setFileListName(g_xmlfilelist_filename);
	w.show();

    return a.exec();
}
