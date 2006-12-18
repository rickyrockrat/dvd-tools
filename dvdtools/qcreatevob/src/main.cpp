#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include "qcreatevob.h"
#include <qregexp.h>
#include <qdir.h>

int main( int argc, char *argv[] )
{
  QApplication app( argc, argv );
  QTranslator appTranslator;
  QString langPath = qApp->applicationDirPath();
  langPath.replace( QRegExp( "(.*/)(.*)" ), "\\1" );
  
  QDir trans(qApp->applicationDirPath());
  if( trans.exists("translations") )
  {
  appTranslator.load( QString( "qcreatevob_" ) + QTextCodec::locale(),
                      qApp->applicationDirPath() + "/translations" );
  } 
  else 
  {
  appTranslator.load( QString( "qcreatevob_" ) + QTextCodec::locale(),
                      langPath + "share/qcreatevob/lang" );
  }

  app.installTranslator( &appTranslator );
  qcreatevob *dialog = new qcreatevob;
  app.setMainWidget( dialog );
  dialog->show();
  return app.exec();
}

