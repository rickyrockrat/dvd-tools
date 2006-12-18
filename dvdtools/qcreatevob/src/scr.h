#ifndef SCR_H
#define SCR_H

#include <qobject.h>

/**
@author Chapaev
*/
class scrclass : public QObject
{
  Q_OBJECT
public:
  scrclass(QObject *parent = 0, const char *name = 0);
  QString sh;

public slots:
  bool runScript(QString script);
  bool findTool();
  QString getProcOutput(QString path, QString arg0, QString arg1, QString arg2);

};

#endif
