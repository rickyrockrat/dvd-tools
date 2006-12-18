#ifndef CHECKCLASS_H
#define CJECKCLASS_H

#include <qobject.h>
#include <qprocess.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

class checkClass : public QObject  {
  Q_OBJECT
public: 
  
  QString inputFile;
  QString format;
  QProcess *checkFileFormatProc;
  QProcess *checkFileProc;
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
};

#endif
