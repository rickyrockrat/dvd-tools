
#ifndef SNDCLASSSND_H
#define SNDCLASSSND_H

#include <qobject.h>
#include <qstring.h>

class sndClass : public QObject
{
  Q_OBJECT
public:
  sndClass();
  ~sndClass();

  QString sndScript;
  QString sndMessage;
  QString support;
  QString getRemTmpScript( QString okRemTmp, QString surround,
                           QString finalAc3,
                           QString ac3, QString tmpDirStr,
                           QString inputFileName );
  void message( QString s );

public slots:
  void main( const QString &channels,
             const QString &tmpDir,
             const QString &fileName,
             const QString &inputFile,
             const QString &bitrate,
             const QString &quality,
             const QString &outChannel,
             const QString &remTmp,
			 const int track );

};


#endif
