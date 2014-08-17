#include "patcherwindow.hpp"
#include "ui_patcherwindow.h"

/**
 * @brief PatcherWindow::PatcherWindow
 * @param parent
 */

/**
 * TODO:
 * - Useragent
 * - Vote Correct
 * - http-class
 * - Aufruf mit Parametern (vom Launcher)
 * - DDoS Mitigate
 * - Aufrufschutz mit IPTables
 */

PatcherWindow::PatcherWindow(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::PatcherWindow)
{
	this->listHost = QString(QString("top") + QString("liste."));
    QPalette palette;
    // QString appDir = QApplication::applicationDirPath();

	this->listHost.append(QString(QString("top-") + QString("p")));

    palette.setBrush(
                    this->backgroundRole(),
                    QBrush(QPixmap(":/images/patcher.png"))
                );

    this->setPalette(palette);
	this->listHost.append(QString(QString("server") + QString(".")));

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    this->addAction(quitAction);

	this->listHost.append(QString("com"));

    ui->setupUi(this);

    ui->btnStart->hide();
    ui->pbSummaryStatus->hide();

    // this->ui->textBrowser->setSource(QUrl().setUrl("http://thoron-mt2.com/"));
    // this->ui->textBrowser->reload();

    this->ui->tePatcherLog->append("Patcher started!");

    // qDebug() << "Active Thread: ";
    // qDebug() << this->thread();

}

PatcherWindow::~PatcherWindow()
{
    delete ui;
}

void PatcherWindow::setFileListName(QString name)
{
    this->xmlfilelist_filename = name;
	qDebug() << "SetXML: " + this->xmlfilelist_filename;
}

QString PatcherWindow::getTopListHost()
{
	return this->listHost;
}

QString PatcherWindow::getTopListLink()
{
	return this->listLink;
}

void PatcherWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void PatcherWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void PatcherWindow::on_btnStart_clicked()
{
	QString program = QString(QApplication::applicationDirPath() + "/" + "m2.bin");
    QStringList arguments = QStringList("--thoron-mt2");
    QString workDir = QApplication::applicationDirPath();
    qint64 pid;

    QProcess::startDetached(program, arguments, workDir, &pid);
    QString msg = "Started \""+program+"\"(" + arguments.join(" ") + ") in \""+workDir+"\" with PID: " + QString::number(pid);
    // this->writeLog(msg);

    exit(0);
}

void PatcherWindow::updateProgress(qint8 percent) {
    ui->pbStatus->setValue(percent);
}

void PatcherWindow::updateProgress(qint64 recv, qint64 sum) {
    this->updateProgress(int((double)recv/sum*100));

    this->ui->lblDownloadStatus->setText("Fortschritt: "+QString::number(recv/1024.0/1024.0,'g', 2)+"MB von "+QString::number(sum/1024.0/1024.0,'g', 2)+"MB");
}

void PatcherWindow::progressDownloadStatus(SDownloadFile *file, FileStatus status) {
    // qDebug() << "TEXT: "+file->name+" # "+status;

    // this->ui->textBrowser->setSource(QUrl("http://thoron-mt2.com/client/web_patcher/index.php?file="+file.name+"&status="+status));
    // this->ui->textBrowser->reload();

    this->ui->tePatcherLog->append("File "+file->name+" => "+QString::number(status));
}

void PatcherWindow::downloadingComplete(SDownloadFile* actFile) {

	qDebug() << "## Downloading Complete! # " + actFile->name;
	qDebug() << (this->xmlfilelist_filename == actFile->name);
	if(actFile->name == this->xmlfilelist_filename) {
		this->writeLog("!! Download Start");
        emit startDlmDownload();
        return;
    }
/*
    QMultiMap<QString, QStringList> *startMe = this->dlm->getFileStartList();

    QMultiMap<QString, QStringList>::const_iterator it = startMe->constBegin();
    while(it != startMe->constEnd())
    {
        QString program = it.key();
        QStringList arguments = it.value();
        QString workDir = ".";
        qint64 pid;

        QProcess::startDetached(program, arguments, workDir, &pid);
        qDebug() << "Started \""+program+"\"(" + arguments.join(" ") + ") in \""+workDir+"\" with PID: " + QString::number(pid);
          ++it;
    }
*/
    this->ui->btnStart->setVisible(true);
}

void PatcherWindow::writeLog(QString text)
{
    qDebug() << "==> "+text;
	this->ui->tePatcherLog->append(text);
}

void PatcherWindow::xmlParsed()
{
	const char *__host;
	QByteArray ___host = this->getTopListHost().toLocal8Bit();
	__host = ___host.data();

	const char *__target;
	QByteArray ___target = this->getTopListLink().toLocal8Bit();
	__target = ___target.data();

	// qDebug() << __host;
	// qDebug() << __target;

	// this->vote = boost::thread(boost::bind(toplist_vote, __host, __target));
	// this->vote.join();

	// toplist_vote(__host, __target);
}

void PatcherWindow::setTopListLink(QString link)
{
	this->listLink = link;
}

void PatcherWindow::on_btnExit_clicked()
{
    exit(0);
}

void PatcherWindow::on_btnStartPatch_clicked()
{
	// this->dlmT = new QThread();

	DownloadManager* dlm = new DownloadManager();
    dlm->setXmlfilelistFilename(this->xmlfilelist_filename);
	// dlm->moveToThread(this->dlmT);

	qDebug() << "Patcherwindow";
	qDebug() << this->thread();

    qRegisterMetaType<SDownloadFile>("SDownloadFile");
    qRegisterMetaType<FileStatus>("FileStatus");

    connect(dlm, SIGNAL(updateProgress(qint64,qint64)), SLOT(updateProgress(qint64,qint64)));
    connect(dlm, SIGNAL(reportDownloadStatus(SDownloadFile*,FileStatus)), SLOT(progressDownloadStatus(SDownloadFile*,FileStatus)));
	connect(dlm, SIGNAL(downloadingComplete(SDownloadFile*)), SLOT(downloadingComplete(SDownloadFile*)));
    connect(dlm, SIGNAL(writeLog(QString)), SLOT(writeLog(QString)));

/*    connect(dlm, SIGNAL(error(QString)), this, SLOT(writeLog(QString)));
    connect(this->dlmT, SIGNAL(started()), dlm, SLOT(process()));
    connect(dlm, SIGNAL(finished()), this->dlmT, SLOT(quit()));
    connect(dlm, SIGNAL(finished()), dlm, SLOT(deleteLater()));
	connect(this->dlmT, SIGNAL(finished()), this->dlmT, SLOT(deleteLater())); */

    connect(this, SIGNAL(getDlmFileList()), dlm, SLOT(getFileList()));
    connect(this, SIGNAL(startDlmDownload()), dlm, SLOT(startDownload()));
	connect(dlm, SIGNAL(xmlParsed()), this, SLOT(xmlParsed()));

	connect(dlm, SIGNAL(topListLink(QString)), SLOT(setTopListLink(QString)));

    // qDebug() << "DL-MGR Thread: ";
    // qDebug() << this->dlm->thread();

	// this->dlmT->start();
	// dlm->run();

    dlm->setDownloadHost("thoron-mt2.com");
    dlm->setDownloadRootfolder("client");
    emit getDlmFileList();
    this->ui->btnStartPatch->hide();
    this->writeLog("Start patching");

	/*
	const char* __host = this->getTopListHost();
	const char* __target = this->getTopListLink();

	qDebug() << __host;
	qDebug() << __target;

	//boost::thread vote(boost::bind(toplist_vote, __host, __target));
	//vote.join();
	*/
}

void PatcherWindow::on_btnConfig_clicked()
{
    QString progress = "config.exe";
    QStringList params;
    QString path = QApplication::applicationDirPath();
    QProcess::startDetached(progress, params, path);
}

