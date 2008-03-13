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
public slots:
	void get();
	void toggledProxy(bool);
	void readResponse(bool);
	void imReqFinished(int,bool);
	void imReqDone(bool);
	void imReadResponseHeader(const QHttpResponseHeader &);
	void vidDoubleClicked ( QListWidgetItem * ); 
	void linkReadResponse(bool err);
	void regarder( QString );
	void enregistrer( QString );
private :
	QHttp *req;
	QHttp *imReq;
	QHttp *linkReq;
	QString imName;
	QStringList imageList;
	QStringList nameList;
	QStringList linkList;
	int imGetId;
	int ctr;
	QFile *imFile;
};
#endif	// _RECWND_H }
