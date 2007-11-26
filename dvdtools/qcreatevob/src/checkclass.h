#ifndef CHECKCLASS_H
#define CJECKCLASS_H

#include <qobject.h>
#include <qprocess.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

class checkClass : public QObject
{
	Q_OBJECT
public: 

	QString inputFile;
	QString format;
	QProcess *checkFileFormatProc;
	QProcess *checkFileProc;
	QProcess *checkFileProc1;
	QStringList checkFileFormatOutList;
	QString getFormat();

public slots:
	void main(const QString &file);
	void checkFileFormat();
	void checkFileFormatOut();
	void checkFile();
	void getInfoAvi();
	void getInfoWmv();
	void getInfoMpg();
	void getInfoMkv();
	void getInfoMkv1();
	void deleteProc();

signals:
	void sendFormat( QString );
	void widthSend(QString);
	void heightSen(QString);
	void fpsSend(QString);
	void framesSend(QString);
	void durationSend(QString);
	void codecSend(QString);
	void qualitySend(int,QString);
	void formatSend(int,QString);
	void bitrateSend(int,QString);
	void channelsSend(int,QString);
	void bitsSend(int,QString);
	void enableMenc();
	void updateSoundTracks(int);
	void audioLang( int, QString );
	void audioTrackNumber( int, QString );
	void inputSubCodec( int, QString );
	void inputSubLang( int, QString );
	void inputSubTrackNumber( int, QString );
	void attName( int, QString );
	void attSize( int, QString );
	void attMime( int, QString );
	void attUid( int, QString );
	void addChapter( int, QString );
};

#endif
