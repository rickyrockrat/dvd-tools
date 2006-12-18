#include <qtimer.h>
#include <sys/types.h>
#include <signal.h>
#include <qmessagebox.h>

void processDlg::main( const QString &proclist )
{
  QString filtred = splitArg( proclist );
  int steps = getProcNumber( filtred );
  stepNr = 100 / steps;
  arg = processFilter( filtred );
  QMessageBox::information( this, "", arg );
  progressAll->reset();
  progressAll->setTotalSteps( 100 );
  progressAll->setProgress( 1 );
  progressCurrent->reset();
  progressCurrent->setTotalSteps( 100 );
  procOut->clear();
  //procOut->setText(filtred);
  statusStr = "";
  killBtn->setEnabled( true );
  exitBtn->setDisabled( true );
  Starting();
}

void processDlg::Starting()
{
  proc = new QProcess( this );
  proc->clearArguments();
  proc->addArgument( "sh" );
  proc->addArgument( "-c" );
  proc->addArgument( arg );
  //   proc->addArgument( "& echo $! > $HOME/pid" );
  //procOut->setText(newArg);
  connect( proc, SIGNAL( readyReadStdout() ),
           this, SLOT( readProcOut() ) );
  connect( proc, SIGNAL( readyReadStderr() ),
           this, SLOT( readProcError() ) );
  connect( proc, SIGNAL( processExited() ),
           this, SLOT( processExit() ) );

  if ( !proc->start() )
  {
    QMessageBox::critical( 0,
                           tr( "Fatal error" ),
                           tr( "Could not start the \"sh\"" ) );
  }
}

void processDlg::progress( int num )
{
  int step = progressAll->progress() + num;
  progressAll->setProgress( step );

}

void processDlg::progress0( int num, int total )
{
  progressCurrent->setProgress( num, total );

}

void processDlg::readProcOut()
{
  QString tmp = proc->readStdout();

  if ( tmp.find( "newStep" ) != -1 )
  {
    progress( stepNr );
    tmp.replace( "newStep", "" );
  }

  if ( tmp.find( "procName=" ) != -1 )
  {
    progress0( 0, 100 );
    status_2->clear();
    status_3->clear();
    status_4->clear();
    int name1 = tmp.find( QRegExp( "procName=" ) );
    int name2 = tmp.find( QRegExp( "," ), name1 );
    name = tmp.mid( name1 + 9, name2 - name1 - 9 );

    tmp.insert( 0, "<table width=\"100%\" border=\"1\"><tr><td bgcolor=\"#6699FF\"><div align=\"center\">" + name + "</div></td></tr></table>" );
    tmp.replace( QRegExp( "(procName=)(.*)" ), "" );
    status->setText( tr( "Runing:" ) + name );
  }

  if ( checkProcOut( tmp ) )
  {
    procOut->append( tmp );
  }
}

void processDlg::readProcError()
{
  stderr = QString( proc->readStderr() );
  if ( checkProcOut( stderr ) )
  {
    if ( stderr.find( "WARNING" ) != -1 | stderr.find( "ERROR" ) != -1 )
    {
      procOut->append( "<font color=\"#aa0000\">" + stderr + "</font>" );
    }
    else
    {
      procOut->append( stderr );
    }

  }
}

void processDlg::processExit()
{
  if ( statusStr.isEmpty() )
  {
    statusStr = tr( "Process exited!" );
  }
  progress0( 100, 100 );
  progressAll->setProgress( 100 );
  status->setText( statusStr );
  killBtn->setDisabled( true );
  exitBtn->setEnabled( true );
  delete proc;
  status_2->clear();
  status_3->clear();
  status_4->clear();
}

bool processDlg::checkProcOut( QString outStr )
{
  // ffmpeg
  if ( outStr.find( "size=" ) != -1 && outStr.find( "time=" ) != -1 )
  {
    QString sizeNr, timeNr, bitrateNr;
    QRegExp rx; rx.setPattern( "(size=.*) ([0-9].*) (time=)(.*) (bitrate=)(.*)" );
    if ( rx.search( outStr ) != -1 )
    {
      sizeNr = rx.cap( 2 );
      timeNr = rx.cap( 4 );
      bitrateNr = rx.cap( 6 );
    }
    if ( QString( status->text() ).find( "multimux" ) == -1 && QString( status->text() ).find( "sox" ) == -1 )
    {
      int s = progressCurrent->progress();
      progress0( s + 10, 0 );
    }
    status_2->setText( "Size " + sizeNr );
    status_3->setText( "Time: " + timeNr );
    status_4->setText( bitrateNr );
    return false;
  }

  // multimux
  if ( outStr.find( "writing" ) != -1 && outStr.find( "file " ) != -1 )
  {
    QString procent;
    QRegExp rx; rx.setPattern( "(.*[(])(.*)%[)]" );
    if ( rx.search( outStr ) != -1 )
    {
      procent = rx.cap( 2 );
      procent.replace( QRegExp( "(.*)([.].*)" ), "\\1" );

    }
    progress0( procent.toInt(), 100 );
    return false;
  }

  // tcmplex scan video
  if ( outStr.find( "Scanning" ) != -1 && outStr.find( "video" ) != -1 && outStr.find( "stream:" ) != -1 )
  {
    QString procent;
    QRegExp rx; rx.setPattern( "(.*[(])(.*)[)]%" );
    if ( rx.search( outStr ) != -1 )
    {
      procent = rx.cap( 2 );

    }
    if ( procent == "99" )
    {
      int slowstep = stepNr / 3;
      progress( slowstep );
    }
    progress0( procent.toInt(), 100 );
    status_2->setText( "Scanning video stream" );
    status_3->clear();
    status_4->clear();
    return false;
  }

  // tcmplex scan audio
  if ( outStr.find( "Scanning" ) != -1 && outStr.find( "audio" ) != -1 && outStr.find( "stream:" ) != -1 )
  {
    QString procent;
    QRegExp rx; rx.setPattern( "(.*[(])(.*)[)]%" );
    if ( rx.search( outStr ) != -1 )
    {
      procent = rx.cap( 2 );
    }
    if ( procent == "99" )
    {
      int slowstep = stepNr / 3;
      progress( slowstep );
    }
    progress0( procent.toInt(), 100 );
    status_2->setText( "Scanning audio stream" );
    status_3->clear();
    status_4->clear();
    return false;
  }

  // tcmplex
  if ( outStr.find( "Multiplexing:" ) != -1 )
  {
    QString procent;
    QRegExp rx; rx.setPattern( "(.*[(])(.*)[)]%" );
    if ( rx.search( outStr ) != -1 )
    {
      procent = rx.cap( 2 );
    }
    if ( procent == "100" )
    {
      int slowstep = stepNr / 3;
      progress( progressAll->progress() - slowstep * 2 );
    }
    progress0( procent.toInt(), 100 );
    status_2->setText( "Multiplexing" );
    status_3->clear();
    status_4->clear();
    return false;
  }

  // transcode
  if ( outStr.find( "encoding frames" ) != -1 )
  {
    QString framesNr, fpsNr, time;
    QRegExp rx; rx.setPattern( "(.*-)([0-9][0-9][0-9][0-9][0-9][0-9])(.[,] )(.*fps)[,] (EMT:.*)[,](.*)" );
    if ( rx.search( outStr ) != -1 )
    {
      framesNr = rx.cap( 2 );
      fpsNr = rx.cap( 4 );
      time = rx.cap( 5 );
    }
    int s = progressCurrent->progress();
    progress0( s + 10, 0 );
    status_2->setText( "Frames: " + framesNr );
    status_3->setText( fpsNr );
    status_4->setText( time );
    return false;
  }

  if ( outStr.find( "Pos:" ) != -1 && outStr.find( "A-V" ) != -1 )
  {
    QStringList list = QStringList::split( " ", outStr );
    QString progress, pos, f, A_V;
    pos = list.grep( QRegExp( "^(.*),[0-9]s$" ) ).join( "" );
    pos.insert( 0, "Pos: " );

    f = list.grep( QRegExp( "^(.*)[0-9]f$" ) ).join( "" );

    A_V = list.grep( "A-V:" ).join( "" );

    progress = list.grep( QRegExp( "%" ) )
               .gres( "(", "" ).gres( "%", "" ).gres( ")", "" ).join( "" );

    progress0( progress.toInt(), 100 );
    status_2->setText( pos );
    status_3->setText( f );
    status_4->setText( A_V );
    return false;
  }
  int s = progressCurrent->progress();
  progress0( s + 10, 0 );
  return true;
}

QString processDlg::showString( QString str )
{
  return QString( str );
}

void processDlg::killProc()
{
  startKillProc();
//   proc->writeToStdin( "q" );
// 
//   proc->tryTerminate();
//   QTimer::singleShot( 5000, proc, SLOT( kill() ) );
  statusStr = tr( "Process killed!" );
}



// PROCFILTER

QString processDlg::splitArg( QString str )
{
  QStringList firstList = QStringList::split( "\n", str );
  firstList.gres( QRegExp( "^#(.*)$" ), "" );
  QStringList list = QStringList::split( "; ", firstList.join( "; " ) );
  QString outStr = list.join( "\n" );
  return QString( outStr );
}

QString processDlg::processFilter( QString str )
{
  QStringList list = QStringList::split( "\n", str );

  QStringList endList;
  QString name;
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    QStringList words = QStringList::split( " ", *it );
    QStringList::Iterator word = words.begin();
    name = *word;
    QString tmp = *it;
    if ( tmp.find( "if " ) == -1 &&
         tmp.find( "elif " ) == -1 &&
         tmp.find( "else" ) == -1 &&
         tmp.find( "fi" ) == -1 &&
         tmp.find( "then" ) == -1 )
    {
      tmp = "echo -e \"procName=" + name + ",\" && " + tmp + "\necho newStep";
    }
    endList << tmp;
  }
  return QString( endList.join( "\n" ) );
}

int processDlg::getProcNumber( QString string )
{
  QStringList arguments = QStringList::split( "\n", string );
  return int( arguments.count() );
}


void processDlg::startKillProc()
{
  int i = proc->processIdentifier();
  QString Pid; Pid.setNum( i );
  killStr = QString( "ps --ppid %1 -o pid" ).arg( Pid );
  killProcess = new QProcess( this );
  killProcess->clearArguments();
  killProcess->addArgument( "sh" );
  killProcess->addArgument( "-c" );
  killProcess->addArgument( killStr );

  connect( killProcess, SIGNAL( readyReadStdout() ),
           this, SLOT( readKillProcOut() ) );
  connect( killProcess, SIGNAL( processExited() ),
           this, SLOT( killStartedProcess() ) );
  killProcess->start();
  
}


void processDlg::readKillProcOut()
{
  pidList << killProcess->readStdout();

}


void processDlg::killStartedProcess()
{
  pidList.gres( "PID", "" );
  for ( QStringList::Iterator it = pidList.begin(); it != pidList.end(); ++it )
  {
    QString num = *it;
    if ( !num.isEmpty() )
    {
      int ProcID = num.toInt();
      kill( ProcID, 9 );
    }
  }
  int i = proc->processIdentifier();
  kill( i, 9 );
  delete killProcess;
}
