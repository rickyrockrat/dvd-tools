#ifndef _RECWND_H	// {
#define _RECWND_H

#include "ui_recwnd.h"
#include <QtNetwork/QHttp>
#include <QFile>

class recwnd : public QDialog, public Ui::recWnd
{
Q_OBJECT

public:
	recwnd( QWidget *parent = 0);
	void getOneImage();
	void clean();
	void recordStart();
public slots:
	void get();
	void program();
	void record();
	void toggledProxy(bool);
	void readResponse(bool);
	void imReqFinished(int,bool);
	void imReqDone(bool);
	void imReadResponseHeader(const QHttpResponseHeader &);
	void vidDoubleClicked ( QListWidgetItem * ); 
	void linkReadResponse(bool err);
	void progLinkReadResponse(bool err );
	void regarder( QString );
	void enregistrer( QString );
	void canProgram();
	void selectDestFile();
	void cancelGet();
	void ok();
private :
	QHttp *req;
	QHttp *imReq;
	QHttp *linkReq;
	QHttp *progLinkReq;
	QString imName;
	QStringList imageList;
	QStringList nameList;
	QStringList linkList;
	int imGetId;
	int ctr;
	QFile *imFile;
	std::vector<QFile *>toRemove;
	bool cancel;
	bool prog;
	int retries;
};
#endif	// _RECWND_H }
