#include "snd.h"

sndClass::sndClass()
{}
sndClass::~sndClass()
{}

/**
 * become strings
 * @param channels 
 * @param tmpDir 
 * @param fileName 
 * @param inputFile 
 * @param bitrate 
 * @param quality 
 * @param outChannel 
 */
void sndClass::main( const QString &channels,
                     const QString &tmpDir,
                     const QString &fileName,
                     const QString &inputFile,
                     const QString &bitrate,
                     const QString &quality,
                     const QString &outChannel,
                     const QString &remTmp,
					 const int track )
{
  sndMessage = "";
  sndScript = QString( "## Converting audio\n" );
  QString inFileFilterd = QString( "\"%1\"" ).arg( inputFile );
  QString tra;
  tra.sprintf( "t%d", track );
  QString outFileFiltred = "\"" + tmpDir + fileName + tra + ".ac3\"";
  QString outFileFiltredWav = "\"" + tmpDir + fileName + tra + ".wav\"";
  QString pipe = "\"" + tmpDir + "pipe\"";
  QString finalAc3 = "\"" + tmpDir + fileName + tra + "_final.ac3\"";
  QString left = "\"" + tmpDir + "left.wav" + "\"";
  QString right = "\"" + tmpDir + "right.wav" + "\"";
  QString surround_left = "\"" + tmpDir + "surround_left.wav" + "\"";
  QString surround_right = "\"" + tmpDir + "surround_right.wav" + "\"";
  QString center = "\"" + tmpDir + "center.wav" + "\"";
  QString lfe = "\"" + tmpDir + "lfe.wav" + "\"";
  QString audioBr;

  if ( bitrate == "0" )
  {
	  audioBr = "";
  }
  else
  {
	  audioBr = " -ab " + bitrate;
  }

  // ----------------------------------------------------------------------------------------
  if ( channels == "1" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {

      // make stereo
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 2 " + outFileFiltred + "\n" );
      // make wav
      sndScript.append( "ffmpeg -y -i " +
                        outFileFiltred + " -f wav " +
                        outFileFiltredWav + "\n" );

      // Extract left channel
      sndScript.append( "sox -V " + outFileFiltredWav + " -r " + quality + " -c1 " + left + " avg -l\n" );

      // Extract right channel
      sndScript.append( "sox -V " + outFileFiltredWav + " -r " + quality + " -c1 " + right + " avg -r\n" );

      // create back left & right
      sndScript.append( "cp -f " + left + " " + surround_left + "\n" );
      sndScript.append( "cp -f " + right + " " + surround_right + "\n" );

      // Create center channel
      sndScript.append( "soxmix -V " + left + " " + right + " -r " + quality + " -c1 " + center + "\n" );

      // Create lfe channel
      sndScript.append( "sox -V -v 0.5 " + center + " " + lfe + " lowp 150\n" );

      // Create final ac3
      sndScript.append( "multimux -d 0,0,0,15,15,0 " +
                        left + " " + center + " " +
                        right + " " + surround_left + " " +
                        surround_right + " " + lfe +
                        " -o /dev/stdout | ffmpeg -y -i - " +
						audioBr + " -ar " +
                        quality + " -ac 6 -f ac3 " + finalAc3 + "\n" );
      sndScript.append( getRemTmpScript( remTmp, outChannel,
                                         finalAc3,
                                         outFileFiltred, tmpDir,
                                         fileName ) );
    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }

  // ----------------------------------------------------------------------------------------
  if ( channels == "2" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
      // make wav
      sndScript.append( "ffmpeg -y -i " +
                        outFileFiltred + " -f wav " +
                        outFileFiltredWav + "\n" );

      // Extract left channel
      sndScript.append( "sox -V " + outFileFiltredWav + " -r " + quality + " -c1 " + left + " avg -l\n" );

      // Extract right channel
      sndScript.append( "sox -V " + outFileFiltredWav + " -r " + quality + " -c1 " + right + " avg -r\n" );

      // create back left & right
      sndScript.append( "cp -f " + left + " " + surround_left + "\n" );
      sndScript.append( "cp -f " + right + " " + surround_right + "\n" );

      // Create center channel
      sndScript.append( "soxmix -V " + left + " " + right + " -r " + quality + " -c1 " + center + "\n" );

      // Create lfe channel
      sndScript.append( "sox -V -v 0.5 " + center + " " + lfe + " lowp 150\n" );

      // Create final ac3
      sndScript.append( "multimux -d 0,0,0,15,15,0 " +
                        left + " " + center + " " +
                        right + " " + surround_left + " " +
                        surround_right + " " + lfe +
                        " -o /dev/stdout | ffmpeg -y -i - " +
                        audioBr + " -ar " +
                        quality + " -ac 6 -f ac3 " + finalAc3 + "\n" );

      sndScript.append( getRemTmpScript( remTmp, outChannel,
                                         finalAc3,
                                         outFileFiltred, tmpDir,
                                         fileName ) );


    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }
  // ----------------------------------------------------------------------------------------
  if ( channels == "3" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {
      //message(channels);
      //support = "no";
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 6 -f ac3 " + outFileFiltred + "\n" );

    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }

  // ----------------------------------------------------------------------------------------
  if ( channels == "4" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {
      //message(channels);
      //support = "no";

      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 6 -f ac3 " + outFileFiltred + "\n" );

    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }

  // ----------------------------------------------------------------------------------------
  if ( channels == "5" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {
      //message(channels);
      //support = "no";

      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 6 -f ac3 " + outFileFiltred + "\n" );

    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }

  // ----------------------------------------------------------------------------------------
  if ( channels == "6" )
  {
    // ++++++ SURROUND ++++++
    if ( outChannel == "Surround 5.1" )
    {
      support = "yes";
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );

    }

    // ++++++ STEREO ++++++
    if ( outChannel == "Stereo 2.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd + 
                        audioBr + " -ar " + quality +
                        " -ac 2 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ MONO ++++++
    if ( outChannel == "Mono 1.0" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -ac 1 -f ac3 " + outFileFiltred + "\n" );
    }

    // ++++++ ORIGINAL ++++++
    if ( outChannel == "Original" )
    {
      sndScript.append( "ffmpeg -y -i " +
                        inFileFilterd +
                        audioBr + " -ar " + quality +
                        " -f ac3 " + outFileFiltred + "\n" );
    }
  }
}

QString sndClass::getRemTmpScript( QString okRemTmp, QString surround,
                                   QString finalAc3,
                                   QString ac3, QString tmpDirStr,
                                   QString inputFileName )
{
  QString script;
  if ( okRemTmp == "on" && surround == "Surround 5.1" )
  {
    script += "mv -f " + finalAc3 + " " + ac3 + "\n" +
              "rm -f " + "\"" + tmpDirStr + "left.wav\" " +
              "\"" + tmpDirStr + "right.wav\" " +
              "\"" + tmpDirStr + "surround_left.wav\" " +
              "\"" + tmpDirStr + "surround_right.wav\" " +
              "\"" + tmpDirStr + "center.wav\" " +
              "\"" + tmpDirStr + "lfe.wav\" " +
              "\"" + tmpDirStr + inputFileName + ".wav\"\n\n";
  }
  return QString( script );
}

void sndClass::message(QString s)
{
sndMessage = QString( tr( "Sorry! Coverting from %1 channels audio\n" ) +
                            tr( "to surround 5.1 at the moment not\n" ) +
                            tr( "supported! :-( swiching to extract\n" ) +
                            tr( "original sound!" )).arg(s);
}
