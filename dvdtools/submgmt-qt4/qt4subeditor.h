
#ifndef _QT4SUBEDITOR_H
#define _QT4SUBEDITOR_H

#include <QListWidgetItem>
#include <QProcess>

#include "ui_qt4subeditor.h"

class Subtitle;

class qt4subeditor : public QMainWindow, private Ui::qt4subeditor
{
Q_OBJECT

public:
	qt4subeditor(QMainWindow *parent = 0);
	~qt4subeditor();
	void saveSubToFile( QString fn );

public slots:
	void openMovie();
	void playPauseMovie();
	void newSubtitle();
	void openSubtitle();
	void saveSubtitle();
	void saveSubtitleAs();
	void readMplayer();
	void exitEditor();
	void fastBack();
	void back();
	void forward();
	void fastForward();
	void applyToMovie();
	void newSubBegin();
	void newSubEnd();
signals:
	void playMovie();
private :
	void loadMovie( QString);
	QProcess *mplayer;
	QString movieFilename;
	int movieLength;	// in milliseconds
	bool movieOpened;
	bool moviePaused;
	bool flagSubBegin;
	bool flagSubEnd;
	double subBegin;
	double subEnd;
	std::vector<Subtitle *>subvec;
	QString currentSubFilename;
};
#endif
