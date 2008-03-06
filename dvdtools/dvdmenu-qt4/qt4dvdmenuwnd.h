#ifndef _QT4DVDMENUWD_H	// {
#define _QT4DVDMENUWD_H

#include "ui_qt4dvdmenuwnd.h"
#include "common.h"
#include <QMainWindow>
#include <QTreeWidget>
#include <QPixmap>
#include <QTextStream>
#include <QProcess>
#include <QColor>
#include <QDir>

#include <iostream>
#include <unistd.h>	// pour sleep

class qt4dvdmenuWnd : public QMainWindow, public Ui::qt4dvdmenuWnd
{
Q_OBJECT

public:
	qt4dvdmenuWnd( QWidget *parent = 0 );
public slots:
	void newDVD();
	void addTitle();
	void lvClicked( QTreeWidgetItem *item );
	void addVideo();
	void selectBgColor();
	void selectFont();
	void newProject();
	void openProject();
	void saveProject();
	void encode();
	QString stringListeEnd( QStringList &liste, int depart );
	QPixmap drawMenu( QPixmap &fond, int currentpage, int totalpages );
	QPixmap &drawButtons( QPixmap &fond, QColor c, int currentpage, int totalpages, std::vector<QRect> *vrects );
	void preview();
	void removeItem( );
	void makeMenu( QStringList titles );
	void makeDvd( );
	void cleanAlbumNames(QStringList &titles);
	void genMenuCommand( QStringList titles, QTextStream &stream, QString color, QString outputName, int pg, int npages );
	void selectBgPic( );
	void selectSound( );
	void clipColors( QImage &img, const QColor &destcol );
private:
	QTreeWidgetItem *dvdItem;
	QTreeWidgetItem *titleItem;
	QTreeWidgetItem *videoItem;
	QProcess *procEncode;
	QProcess *mpginfoProc;
	QColor bgcolor;
	int titleCtr;
	QString videoDir;
	QString workingDir;
	QString menuSound;
	QString bgPic;
	QFont font;
	QRect r1;
	QRect r2;
	QRect r3;
	QRect r4;
	QRect tabr[4];
	QStringList *files;
};

#endif	//} _QT4DVDMENUWD_H
// vim: nows nosm ts=4 sw=4
