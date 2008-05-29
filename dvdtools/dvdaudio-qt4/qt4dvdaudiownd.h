
#ifndef _QT4DVDAUDIOWND_H	// {
#define _QT4DVDAUDIOWND_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QProcess>
#include "ui_qt4dvdaudiownd.h"
#include "common.h"
#include <iostream>

class qt4dvdaudioWnd : public QMainWindow, public Ui::dvdaudioWnd
{
	Q_OBJECT

public:
	qt4dvdaudioWnd( QWidget *parent = 0 );
	virtual ~qt4dvdaudioWnd( );
	QTreeWidgetItem *dvdItem;
	QTreeWidgetItem *albumItem;
	QTreeWidgetItem *trackItem;
	QProcess *procEncode;
	QProcess *mpginfoProc;
	QColor bgcolor;
	int albumCtr;
	QString trackDir;
	QString workingDir;
	QFont font;
	QStringList *files;
	QString bgPic;
public slots:
	void init();
	void downClicked();
	void upClicked();
	void newDVD();
	void newAlbum();
	void lvClicked( QTreeWidgetItem *item );
	void addTracks();
	void selectBgColor();
	void selectFont();
	void newProject();
	void openProject();
	void saveProject();
	void encode();
	void readProcSox();
	void readProcMplayer();
	void readProcMpginfo();
	QString stringListeEnd( QStringList &liste, int depart );
	void preview();
	void removeItem( );
	void makeMenu( QStringList albums );
	void makeDvd( );
	void setOptions( trackPreviewWnd *t );
	void cleanAlbumNames(QStringList &albums);
	void genMenuCommand( QStringList albums, const QColor &color, QString outputName, int pg, int npages, std::vector<QRect> *vr );
	void selectBgPic( );
	void clipColors( QImage &img, const QColor &destcol );
};
#endif // } QT4DVDAUDIOWND_H

//vim: ts=4 sw=4
