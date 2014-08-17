#ifndef PATCHERWINDOW_HPP
#define PATCHERWINDOW_HPP

#include <QWidget>
#include <QDebug>
#include <QtGui>
#include <QThread>
#include <downloadmanager.hpp>
#include <iostream>

#include <boost/thread.hpp>

#include "vote/toplist.h"

#define _CRT_SECURE_NO_WARNINGS

namespace Ui {
class PatcherWindow;
}

class PatcherWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit PatcherWindow(QWidget *parent = 0);
    ~PatcherWindow();
    void setFileListName(QString);

    QString xmlfilelist_filename;

	QString getTopListHost();
	QString getTopListLink();
    
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QThread *dlmT;

	QString listHost;
	QString listLink;

	boost::thread vote;

private slots:
    void on_btnExit_clicked();
    void on_btnStart_clicked();
    void updateProgress(qint8); // percent
    void updateProgress(qint64, qint64); // recv, sum
    void progressDownloadStatus(SDownloadFile*,FileStatus); // percent
	void downloadingComplete(SDownloadFile*);
    void writeLog(QString);
	void xmlParsed();
	void setTopListLink(QString);

    void on_btnStartPatch_clicked();

    void on_btnConfig_clicked();

signals:
    void startDlmDownload();
    void getDlmFileList();

private:
    Ui::PatcherWindow *ui;
    QPoint dragPosition;

};

#endif // PATCHERWINDOW_HPP
