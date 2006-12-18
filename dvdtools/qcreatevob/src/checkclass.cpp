#include "checkclass.h"
#include <qregexp.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qmessagebox.h>


void checkClass::main( const QString &file )
{
  inputFile = file;
  checkFileFormat();
}


void checkClass::checkFileFormat()
{
  checkFileFormatProc = new QProcess( this );
  connect( checkFileFormatProc, SIGNAL( readyReadStdout() ),
           this, SLOT( checkFileFormatOut() ) );
  connect( checkFileFormatProc, SIGNAL( processExited() ),
           this, SLOT( deleteProc() ) );

  if ( !inputFile.isEmpty() )
  {
    checkFileFormatProc->clearArguments();
    checkFileFormatProc->addArgument( "file" );
    checkFileFormatProc->addArgument( inputFile );
    checkFileFormatProc->start();
  }
}

void checkClass::deleteProc()
{
delete checkFileFormatProc;
}

void checkClass::checkFileFormatOut()
{
  checkFileFormatOutList = QStringList::split( "\n",
                           checkFileFormatProc->readStdout() );
  checkFile();
}

void checkClass::checkFile()
{
  checkFileProc = new QProcess( this );
  if ( getFormat() == "AVI" )
  {
    connect( checkFileProc, SIGNAL( readyReadStdout() ),
             this, SLOT( getInfoAvi() ) );
    checkFileProc->clearArguments();
    checkFileProc->addArgument( "tcprobe" );
    checkFileProc->addArgument( "-i" );
    checkFileProc->addArgument( inputFile );
    checkFileProc->start();
  }
  if ( getFormat() == "WMV" )
  {
    connect( checkFileProc, SIGNAL( processExited() ),
             this, SLOT( getInfoWmv() ) );
    checkFileProc->clearArguments();
    checkFileProc->addArgument( "ffmpeg" );
    checkFileProc->addArgument( "-y" );
    checkFileProc->addArgument( "-i" );
    checkFileProc->addArgument( inputFile );
    //checkFileProc->addArgument( ">&" );
    checkFileProc->start();
  }
  if ( getFormat() == "MPEG-2" | getFormat() == "MPEG-1" )
  {
    connect( checkFileProc, SIGNAL( processExited() ),
             this, SLOT( getInfoMpg() ) );
    checkFileProc->clearArguments();
    checkFileProc->addArgument( "ffmpeg" );
    checkFileProc->addArgument( "-y" );
    checkFileProc->addArgument( "-i" );
    checkFileProc->addArgument( inputFile );
    //checkFileProc->addArgument( ">&" );
    checkFileProc->start();
  }
  if ( getFormat() == "MPEG system stream")
  {
	//QMessageBox::information( 0, tr("qcreatevob"), tr("MPEG"), 0 );
    connect( checkFileProc, SIGNAL( processExited() ),
             this, SLOT( getInfoMpg() ) );
    checkFileProc->clearArguments();
    checkFileProc->addArgument( "ffmpeg" );
    checkFileProc->addArgument( "-y" );
    checkFileProc->addArgument( "-i" );
    checkFileProc->addArgument( inputFile );
    checkFileProc->start();
  }
  if ( getFormat() == "MKV")
  {
	//QMessageBox::information( 0, tr("qcreatevob"), tr("MPEG"), 0 );
    connect( checkFileProc, SIGNAL( processExited() ),
             this, SLOT( getInfoMkv() ) );
    checkFileProc->clearArguments();
    checkFileProc->addArgument( "ffmpeg" );
    checkFileProc->addArgument( "-y" );
    checkFileProc->addArgument( "-i" );
    checkFileProc->addArgument( inputFile );
    checkFileProc->start();
  }
}

void checkClass::getInfoMkv()

{
 // Create info strings
  QString codec = "";
  QString frames = ""; QString width = "";
  QString height = ""; QString duration = "";
  QString fps = ""; QString quality = "";
  QString format = ""; QString channels = "";
  QString bits = ""; QString bitrate = "";
  QStringList::Iterator it;

  // Check Information
  QStringList infoList = QStringList::split( "\n", checkFileProc->readStderr() );
  
  QStringList length = QStringList::split( ", ", infoList.grep( "Duration:" )
                       .join( "" ) );

  //QStringList Audio = QStringList::split("Audio:", infoList.grep( "Audio:" ).join( "" ) );
  QStringList Audio = infoList.grep( "Audio:" );

  QStringList Video = QStringList::split( ", ", infoList.grep( "Video:" )
                                          .join( "" ) );

  codec = Video.grep( "Video:" ).gres( QRegExp( "(.*)Video: " ), "" ).join( "" );
  frames = "Unknow";
  width = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "x(.*)" ), "" ).join( "" );
  height = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "(.*)x" ), "" ).join( "" );
  duration = length.grep( "Duration:" ).gres( "Duration: ", "" ).join( "" );
  fps = Video.grep( "fps" ).gres( " fps", "" ).join( "" );


  QString TrackList;
  int track;

  for ( track = 0, it = Audio.begin(); it != Audio.end(); it++ )
  {
	  TrackList = (*it);
	  quality = TrackList.section( " Hz,", 0, 0 ).section( " ", -1, -1 );
	  format = TrackList.section( "Audio: ", 1, 1 ).section( ",", 0, 0 );
	  if ( TrackList.contains( "mono" ) != 0 )
	  {
		channels = "1";
	  }
	  if ( TrackList.contains( "stereo" ) != 0 )
	  {
		channels = "2";
	  }
	  if ( TrackList.contains( "5:1" ) != 0 )
	  {
		channels = "6";
	  }
	  if ( TrackList.contains( "4:1" ) != 0 )
	  {
		channels = "5";
	  }
	  bits = "Unknow";
	  //bitrate = TrackList.section( ",", -1, -1 );

	  emit qualitySend( track, quality );
	  emit formatSend( track, format );
	  emit bitrateSend( track, bitrate );
	  emit channelsSend( track, channels );
	  emit bitsSend( track, bits );
	  track++;
  }

  // UPDATE INFO
  emit widthSend( width );
  emit heightSen( height );
  emit fpsSend( fps );
  emit framesSend( frames );
  emit durationSend( duration );
  emit codecSend( codec );
  emit updateSoundTracks(track);
  delete checkFileProc;
}

void checkClass::getInfoAvi()
{
  // Create info strings
  QString codec = "";
  QString frames = ""; QString width = "";
  QString height = ""; QString duration = "";
  QString fps = ""; QString quality = "";
  QString format = ""; QString channels = "";
  QString bits = ""; QString bitrate = "";

  // Check Information
  QStringList infoList = QStringList::split( "\n", checkFileProc->readStdout() ); QStringList aviVideo;
  QStringList aviAudio;
  QStringList length;

  // VIDEO
  aviVideo = QStringList::split( ", ", infoList.grep( "V:" )
                                 .join( "" ) );

  codec = aviVideo.grep( "codec=" )
          .gres( "codec=", "" )
          .join( "" );

  frames = aviVideo.grep( "frames=" )
           .gres( "frames=", "" )
           .join( "" );

  width = aviVideo.grep( "width=" )
          .gres( "width=", "" )
          .join( "" );

  height = aviVideo.grep( "height=" )
           .gres( "height=", "" )
           .join( "" );

  fps = aviVideo.grep( "fps" )
        .gres( QRegExp( "^(.*) V: (.*) fps" ), "\\2" )
        .join( "" );

  // LENGTH
  length = QStringList::split( ", ",
                               infoList.grep( "length: " )
                               .join( "" ) );

  duration = length.grep( "duration=" )
             .gres( "duration=", "" )
             .join( "" );

  // AUDIO
  aviAudio = infoList.grep( "A:" );

  // Handling of multiple audio streams
  QStringList::Iterator it;
  QStringList TrackList;
  int track;
  for ( track=0, it  = aviAudio.begin(); it != aviAudio.end(); it++)
  {
	  TrackList = QStringList::split( ", ", (*it) );
	  quality = TrackList.grep( "Hz" )
            .gres( QRegExp( "^(.*) A: (.* Hz)" ), "\\2" )
            .join( "" );

	  format = TrackList.grep( "format=" )
           .gres( "format=", "" )
           .join( "" );

	  channels = TrackList.grep( "channels=" )
             .gres( "channels=", "" )
             .join( "" );

	  bits = TrackList.grep( "bits=" )
         .gres( "bits=", "" )
         .join( "" );

	  bitrate = TrackList.grep( "bitrate=" )
            .gres( "bitrate=", "" )
            .join( "" );

	  // UPDATE INFO
	  emit qualitySend( track, quality );
	  emit formatSend( track, format );
	  emit channelsSend( track, channels );
	  emit bitrateSend( track, bitrate );
	  emit bitsSend( track, bits );
	  track++;
  }
  emit widthSend( width );
  emit heightSen( height );
  emit fpsSend( fps );
  emit framesSend( frames );
  emit durationSend( duration );
  emit codecSend( codec );
  emit updateSoundTracks(track);
  delete checkFileProc;
}

void checkClass::getInfoWmv()
{
  // Create info strings
  QString codec = "";
  QString frames = ""; QString width = "";
  QString height = ""; QString duration = "";
  QString fps = ""; QString quality = "";
  QString format = ""; QString channels = "";
  QString bits = ""; QString bitrate = "";

  // Check Information
  QStringList infoList = QStringList::split( "\n", checkFileProc->readStderr() );
  
  QStringList length = QStringList::split( ", ", infoList.grep( "Duration:" )
                       .join( "" ) );

  QStringList Audio = QStringList::split( ", ", infoList.grep( "Audio:" )
                                          .join( "" ) );

  QStringList Video = QStringList::split( ", ", infoList.grep( "Video:" )
                                          .join( "" ) );

  codec = Video.grep( "Video:" ).gres( QRegExp( "(.*)Video: " ), "" ).join( "" );
  frames = "Unknow";
  width = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "x(.*)" ), "" ).join( "" );
  height = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "(.*)x" ), "" ).join( "" );
  duration = length.grep( "Duration:" ).gres( "Duration: ", "" ).join( "" );
  fps = Video.grep( "fps" ).gres( " fps", "" ).join( "" );

  QStringList::Iterator it;
  QStringList TrackList;
  int track;

  for ( track = 0, it = Audio.begin(); it != Audio.end(); it++ )
  {
	  TrackList = (*it);
	  quality = TrackList.grep( "Hz" ).join( "" );
	  format = TrackList.grep( "Audio:" ).gres( QRegExp( "(.*)Audio: " ), "" ).join( "" );
	  if ( !( TrackList.grep( "mono" ) ).isEmpty() )
	  {
		channels = "1";
	  }
	  if ( !( TrackList.grep( "stereo" ) ).isEmpty() )
	  {
		channels = "2";
	  }
	  if ( !( TrackList.grep( "5:1" ) ).isEmpty() )
	  {
		channels = "6";
	  }
	  if ( !( TrackList.grep( "4:1" ) ).isEmpty() )
	  {
		channels = "5";
	  }
	  if ( !( TrackList.grep( "channels" ) ).isEmpty() )
	  {
		channels = length.grep( "channels" ).gres( " channels", "" ).join( "" );
	  }
	  bits = "Unknow";
	  bitrate = length.grep( "bitrate:" ).gres( "bitrate: ", "" ).join( "" );

	  emit qualitySend( track, quality );
	  emit formatSend( track, format );
	  emit bitrateSend( track, bitrate );
	  emit channelsSend( track, channels );
	  emit bitsSend( track, bits );
  }

  // UPDATE INFO
  emit widthSend( width );
  emit heightSen( height );
  emit fpsSend( fps );
  emit framesSend( frames );
  emit durationSend( duration );
  emit codecSend( codec );
  emit updateSoundTracks(track);
  delete checkFileProc;
}

void checkClass::getInfoMpg()
{
 // Create info strings
  QString codec = "";
  QString frames = ""; QString width = "";
  QString height = ""; QString duration = "";
  QString fps = ""; QString quality = "";
  QString format = ""; QString channels = "";
  QString bits = ""; QString bitrate = "";

  // Check Information
  QStringList infoList = QStringList::split( "\n", checkFileProc->readStderr() );
  
  QStringList length = QStringList::split( ", ", infoList.grep( "Duration:" )
                       .join( "" ) );

  QStringList Audio = QStringList::split( ", ", infoList.grep( "Audio:" )
                                          .join( "" ) );

  QStringList Video = QStringList::split( ", ", infoList.grep( "Video:" )
                                          .join( "" ) );

  codec = Video.grep( "Video:" ).gres( QRegExp( "(.*)Video: " ), "" ).join( "" );
  frames = "Unknow";
  width = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "x(.*)" ), "" ).join( "" );
  height = Video.grep( QRegExp( "(.*)[0-9]x[0-9].*" ) ).gres( QRegExp( "(.*)x" ), "" ).join( "" );
  duration = length.grep( "Duration:" ).gres( "Duration: ", "" ).join( "" );
  fps = Video.grep( "fps" ).gres( " fps", "" ).join( "" );


  QStringList::Iterator it;
  QStringList TrackList;
  int track;

  for ( track = 0, it = Audio.begin(); it != Audio.end(); it++ )
  {
	  TrackList = (*it);
	  quality = TrackList.grep( "Hz" ).join( "" );
	  format = TrackList.grep( "Audio:" ).gres( QRegExp( "(.*)Audio: " ), "" ).join( "" );
	  if ( !( TrackList.grep( "mono" ) ).isEmpty() )
	  {
		channels = "1";
	  }
	  if ( !( TrackList.grep( "stereo" ) ).isEmpty() )
	  {
		channels = "2";
	  }
	  if ( !( TrackList.grep( "5:1" ) ).isEmpty() )
	  {
		channels = "6";
	  }
	  if ( !( TrackList.grep( "4:1" ) ).isEmpty() )
	  {
		channels = "5";
	  }
	  if ( !( TrackList.grep( "channels" ) ).isEmpty() )
	  {
		channels = length.grep( "channels" ).gres( " channels", "" ).join( "" );
	  }
	  bits = "Unknow";
	  bitrate = length.grep( "bitrate:" ).gres( "bitrate: ", "" ).join( "" );

	  emit qualitySend( track, quality );
	  emit formatSend( track, format );
	  emit bitrateSend( track, bitrate );
	  emit channelsSend( track, channels );
	  emit bitsSend( track, bits );
  }

  // UPDATE INFO
  emit widthSend( width );
  emit heightSen( height );
  emit fpsSend( fps );
  emit framesSend( frames );
  emit durationSend( duration );
  emit codecSend( codec );
  emit updateSoundTracks(track);
  delete checkFileProc;
}

QString checkClass::getFormat()
{
  if ( !( checkFileFormatOutList.grep( "AVI" ) ).isEmpty() )
  {
    sendFormat( "AVI" );
    return QString( "AVI" );
  }

  if ( !( checkFileFormatOutList.grep( "MPEG" ) ).isEmpty() )
  {
    if ( !( checkFileFormatOutList.grep( "v2" ) ).isEmpty() )
    {
      sendFormat( "MPEG-2" );
      return QString( "MPEG-2" );
    }

    if ( !( checkFileFormatOutList.grep( "v1" ) ).isEmpty() )
    {
      sendFormat( "MPEG-1" );
      return QString( "MPEG-1" );
    }

    if ( !( checkFileFormatOutList.grep( "stream" ) ).isEmpty() )
    {
      sendFormat( "MPEG system stream" );
      return QString( "MPEG system stream" );
    }
  }

  if ( !( checkFileFormatOutList.grep( "Microsoft ASF" ) ).isEmpty() )
  {
    sendFormat( "WMV" );
    enableMenc();
    return QString( "WMV" );
  }

  // trying to determine format with extension
  if ( inputFile.right( 4 ).compare( ".mkv" ) == 0 )
  {
    sendFormat( "MKV" );
    enableMenc();
    return QString( "MKV" );
  }

  sendFormat( "UNKNOW" );
  return QString( "UNKNOW" );
}

