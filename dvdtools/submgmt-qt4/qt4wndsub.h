
#ifndef _QT4WNDSUB.H
#define _QT4WNDSUB.H

#include <QListWidgetItem>

#include "ui_qt4wndsub.h"

class Subtitle;

class qt4wndsub : public QWidget, private Ui::qt4wndsub
{
Q_OBJECT

public:
	qt4wndsub(QWidget *parent = 0);

	void checkShift();
	void timeBasedProceed( Subtitle *s );
	void frameBasedProceed( Subtitle *s );
	void autoDetectFormat();
	void loadSubFile(QString fname);
	void showOutputSubs();
	void showInputSubs();
	void saveMicrodvd();
	void saveSubrip();
	void saveSami();
	void saveSubviewer();
	void saveSpumux();
	void saveSsa();
	void loadSsa(QString fname);
	void loadSami(QString fname);
	void loadSubviewer(QString fname);
	void loadMicrodvd(QString fname);
	void loadSubrip(QString fname);
	void loadSpumux(QString fname);
	bool hasFormat();
	void dump();
	QString getInputFile( );
	void setInputFile( char * );

	std::vector<Subtitle *> subvec;
	std::vector<Subtitle *> v_subs;
	QModelIndex *curItem;

public slots:
	void clicSelectInputFile();
	void clicProceed();
	void clickedSave();
	void clicSelectOutputFile();
	void filenameChanged();
	void checkExpand();
	void checkFps();
	void checkAuto();
	void checkShiftPos();
	void genPngForSpumux();
	void checkSubst();
	void clicSelectSubstFile();
	void saveTypeCheck( );
	void subeditclosed(QWidget* ed,QAbstractItemDelegate::EndEditHint h);
	void nextWrongSub();

};
#endif
