#include <qapplication.h>
#include <qstring.h>
#include <qprocess.h>
#include <qstringlist.h>
#include <qfile.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

#define MAXL 4096

#define ERROR -1
#include "scr.h"

scrclass::scrclass( QObject *parent, const char *name )
    : QObject( parent, name )
{}
bool scrclass::findTool()
{
  QString path = getenv( "PATH" );
  if ( !path.isEmpty() )
  {
    QStringList list = QStringList::split( ":", path );
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
      QString tmp = *it + "/sh";
      if ( QFile::exists( tmp ) )
      {
        sh = tmp;
        return true;
      }

    }
    return false;
  }
  else
  {
    return false;
  }
}

bool scrclass::runScript( QString script )
{
  if ( findTool() )
  {
    if ( !script.isEmpty() && getProcOutput( sh, "sh", "-c", script ) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

QString scrclass::getProcOutput( QString path, QString arg0, QString arg1, QString arg2 )
{
  int fd[ 2 ];
  pid_t pid;
  FILE* pipe_reader;
  char buffer[ MAXL ];

  if ( pipe( fd ) == ERROR )
    return QString();

  if ( ( pid = fork() ) == ERROR )
    return QString();

  switch ( pid )
  {
  case -1:
    return QString();
    break;
  case 0:
    close( fd[ 0 ] );
    if ( fd[ 1 ] != STDOUT_FILENO )
    {
      if ( dup2( fd[ 1 ], STDOUT_FILENO ) != STDOUT_FILENO )
        return QString();
      close( fd[ 1 ] );
    }
    if ( execl( path.latin1(), arg0.latin1(), arg1.latin1(), arg2.latin1(), NULL ) == ERROR )
      return QString();
    break;
  default:
    sleep( 1 );
    close( fd[ 1 ] );

    if ( ( pipe_reader = fdopen( fd[ 0 ], "r" ) ) == NULL )
      return QString();

    fread( &buffer, MAXL, 1, pipe_reader );

    fclose( pipe_reader );

    printf( "path = %s", buffer );

    if ( waitpid( pid, NULL, 0 ) == ERROR )
      return QString();
    break;
  }

  return QString( buffer );
}


