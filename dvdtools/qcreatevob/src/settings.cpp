//
// C++ Implementation: settings
//
// Description:
//
//
// Author: chapaev <chapaev@the-royal.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "settings.h"

settingsclass::settingsclass( QObject *parent, const char *name )
    : QObject( parent, name )
{
  seittingsFile = getenv( "HOME" );
  seittingsFile.append( "/.qcreatevob.conf" );
}

QString settingsclass::readSettings( QString name )
{
  QStringList list = readFile( seittingsFile );
  if ( !( list.grep( name ) ).isEmpty() )
  {
    QString option = list.grep( name ).join( "" );
    QString rm = name + "=";
    option.remove( rm );
    return option;
  }
  else
  {
    return QString();
  }
}

void settingsclass::writeSettings( QString name, QString str )
{
  QStringList list = readFile( seittingsFile );
  if ( !( list.grep( name ) ).isEmpty() )
  {
    QString rxStr = "^" + name + "(.*)$"; // "^name(.*)$"
    QString newSet = name + "=" + str;
    list.gres( QRegExp( rxStr ), newSet );
  }
  else
  {
    QString newStr = name + "=" + str;
    list << newStr;
  }

  QFile file( seittingsFile );
  if ( file.open( IO_WriteOnly | IO_Translate ) )
  {
    QTextStream out( &file );
    out.setEncoding( QTextStream::UnicodeUTF8 );
    out << list.join( "\n" );
    file.close();
  }
  else
  {
    QMessageBox::warning( 0, "QCreateVOB", "WARNING: Cant't write settingsfile!" );
  }
}

QStringList settingsclass::readFile( QString myFile )
{
  QFile file( myFile );
  if ( file.open( IO_ReadOnly | IO_Translate ) )
  {
    QTextStream in( &file );
    in.setEncoding( QTextStream::UnicodeUTF8 );
    file.close();
    return QStringList::split( "\n", in.read() );
  }
  else
  {
    QMessageBox::warning( 0, "QCreateVOB", "WARNING: Cant't read settingsfile!" );
    return QStringList();
  }
}
