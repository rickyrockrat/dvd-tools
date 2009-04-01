#ifndef _RECWND_H	// {
#define _RECWND_H

#include "ui_recwnd.h"
#include <QtNetwork/QHttp>
#include <QFile>
#include <QTimeEdit>
#include <QMainWindow>

class recwnd : public QMainWindow, public Ui::recWnd
{
Q_OBJECT

public:
	recwnd( QWidget *parent = 0);
	void getOneImage();
	void clean();
	void recordStart();
public slots:
	void get();
	void addProgram();
	void delProgram();
	void record();
	void readResponse(bool);
	void imReqFinished(int,bool);
	void imReqDone(bool);
	void imReadResponseHeader(const QHttpResponseHeader &);
	void vidDoubleClicked ( QListWidgetItem * ); 
	void linkReadResponse(bool err);
	void progLinkReadResponse(bool err );
	void regarder( QString );
	void enregistrer( QString );
	void cancelGet();
	void ok();
	void selDestFile();
	void checkProg();
	void showPrefs();
	void readPrefs();
private :
	QHttp *req;
	QHttp *imReq;
	QHttp *linkReq;
	QHttp *progLinkReq;
	QString imName;
	QStringList titleList;
	QStringList imageList;
	QStringList nameList;
	QStringList linkList;
	QString progDestFile;
	QString progKeyword;
	QString progUrl;
	QTimeEdit *progDuration;
	int duration;
	int imGetId;
	int ctr;
	QFile *imFile;
	std::vector<QFile *>toRemove;
	bool cancel;
	bool prog;
	int retries;
	int numProgs;
	QTimer *progTimer;
	bool proxyEnabled;
	QString proxyHost;
	int proxyPort;
	QString proxyUser;
	QString proxyPassword;
	QRegExp vidLinkRE;
	QRegExp vidListRE;
	QString defaultUrl;
	int minSize;
	bool obstinate;
};
#endif	// _RECWND_H }
