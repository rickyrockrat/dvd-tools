#define QUALITYVIEW	0
#define FORMATVIEW	1
#define BITRATEVIEW	2
#define CHANNELVIEW	3
#define BITSVIEW	4

#define SOUNDOUT_CHECK		0
#define SOUNDOUT_SETTINGS	1
#define SOUNDOUT_BITRATE	2
#define SOUNDOUT_QUALITY	3

#include <iostream>

void qcreatevob::init()
{
  connect( myMplex, SIGNAL( writeSettings() ), this, SLOT( writeSettings() ) );
  connect( myMplex, SIGNAL( errorSignal() ), this, SLOT( slotMplexError() ) );
  myCheck = new checkClass();
  connect( myCheck, SIGNAL( sendFormat( QString ) ), this, SLOT( slotUpdateFormat( QString ) ) );
  connect( myCheck, SIGNAL( widthSend( QString ) ), this, SLOT( slotUpdateWidth( QString ) ) );
  connect( myCheck, SIGNAL( heightSen( QString ) ), this, SLOT( slotUpdateHeight( QString ) ) );
  connect( myCheck, SIGNAL( fpsSend( QString ) ), this, SLOT( slotUpdateFPS( QString ) ) );
  connect( myCheck, SIGNAL( framesSend( QString ) ), this, SLOT( slotUpdateFrames( QString ) ) );
  connect( myCheck, SIGNAL( durationSend( QString ) ), this, SLOT( slotUpdateDuration( QString ) ) );
  connect( myCheck, SIGNAL( codecSend( QString ) ), this, SLOT( slotUpdateCodec( QString ) ) );
  connect( myCheck, SIGNAL( qualitySend( int, QString ) ), this, SLOT( slotUpdateQuality( int, QString ) ) );
  connect( myCheck, SIGNAL( formatSend( int, QString ) ), this, SLOT( slotUpdateFormat2( int, QString ) ) );
  connect( myCheck, SIGNAL( bitrateSend( int, QString ) ), this, SLOT( slotUpdateBitrate( int, QString ) ) );
  connect( myCheck, SIGNAL( channelsSend( int, QString ) ), this, SLOT( slotUpdateChannel( int, QString ) ) );
  connect( myCheck, SIGNAL( bitsSend( int, QString ) ), this, SLOT( slotUpdateBits( int, QString ) ) );
  connect( myCheck, SIGNAL( enableMenc() ), this, SLOT( slotMenc() ) );
  connect( myCheck, SIGNAL( updateSoundTracks(int) ), this, SLOT( updateSound(int) ) );
  connect( this, SIGNAL( addAudio(int) ),
		  myMplex, SLOT( addAudio(int) ) );

  mySnd = new sndClass();
  fileFilters = tr( "video files (*.avi *.mpeg *.mpg *.vob *.wmv *.mkv);;*.avi;;MPEG (*.mpg *.mpeg);;*.vob;;Microsoft ASF (*.wmv);;Matroska (*.mkv);;*" );
  subFilters = tr( "sub files (*.srt *.sub *.txt);;*" );
  getStrings();
  readSettings();

	sbSubLeftMargin->setValue( 20 );
	sbSubRightMargin->setValue( 20 );
	sbSubTopMargin->setValue( 20 );
	sbSubBottomMargin->setValue( 20 );

	/* sub fonts must be in ~/.spumux */
	QDir subfontsdir( QDir::homeDirPath() + "/.spumux" );
	if ( !subfontsdir.exists() )
	{
		settingsTabs->page( 4 )->setEnabled( false );
	}
	else
	{
		cbSubFont->insertStringList( subfontsdir.entryList( "*" ) );
	}
	int i;
	for ( i = cbSubFont->count() - 1; i >= 0; i-- )
	{
		if ( ( cbSubFont->text(i).compare( "." ) == 0 )
		|| ( cbSubFont->text(i).compare( ".." ) == 0 ) )
		{
			cbSubFont->removeItem( i );
		}
	}
	sbSubFontSize->setValue( 28 );
	cbSubVertAlign->setCurrentItem( 2 );	// bottom
	cbSubHorAlign->setCurrentItem( 1 );	// center

	soundOutList.clear();
	soundOutList.append( "Surround 5.1" );
	soundOutList.append( "Stereo 2.0" );
	soundOutList.append( "Mono 1.0" );
	soundOutList.append( "Original" );
}

/**
 *  UPDATE INFO
 */
void qcreatevob::slotUpdateFormat( QString s )
{
  fileFormatView->setText( s );
}

void qcreatevob::slotMenc()
{
  enableMencoderBox->setChecked( true );
}

void qcreatevob::slotUpdateWidth( QString s )
{
  widthView->setText( s );
}
void qcreatevob::slotUpdateHeight( QString s )
{
  heightView->setText( s );
  borderView->setText( getBorder( aspectBox->currentText(), widthView->text(), heightView->text() ) );
}
void qcreatevob::slotUpdateFPS( QString s )
{
  fpsView->setText( s );
}
void qcreatevob::slotUpdateFrames( QString s )
{
  framesView->setText( s );
}
void qcreatevob::slotUpdateDuration( QString s )
{
  durationView->setText( s );
}
void qcreatevob::slotUpdateCodec( QString s )
{
  codecView->setText( s );
}
void qcreatevob::slotUpdateQuality( int t, QString s )
{
	if ( t > tableSound->numRows() - 1 )
	{
		tableSound->insertRows( t );
	}
	//qualityView->setText( s );
	tableSound->setText( t, QUALITYVIEW, s );
}
void qcreatevob::slotUpdateFormat2( int t, QString s )
{
	if ( t > tableSound->numRows() - 1 )
	{
		tableSound->insertRows( t );
	}
	//formatView->setText( s );
	tableSound->setText( t, FORMATVIEW, s );
}
void qcreatevob::slotUpdateBitrate( int t, QString s )
{
	if ( t > tableSound->numRows() - 1 )
	{
		tableSound->insertRows( t );
	}
	//bitrateView->setText( s );
	tableSound->setText( t, BITRATEVIEW, s );
}
void qcreatevob::slotUpdateChannel( int t, QString s )
{
	if ( t > tableSound->numRows() - 1 )
	{
		tableSound->insertRows( t );
	}
	//channelsView->setText( s );
	tableSound->setText( t, CHANNELVIEW, s );
}
void qcreatevob::slotUpdateBits( int t, QString s )
{
	if ( t > tableSound->numRows() - 1 )
	{
		tableSound->insertRows( t );
	}
	//bitsView->setText( s );
	tableSound->setText( t, BITSVIEW, s );
}

void qcreatevob::removeSub()
{
	QListViewItem * i = lvSubs->currentItem();

	if ( i ) lvSubs->takeItem( i );
	pbRemoveSub->setEnabled( false );
}

void qcreatevob::addSub()
{
	lvSubs->insertItem( new QListViewItem( lvSubs, leSubInputFile->text(), cbLang->currentText() ) );
}

void qcreatevob::selectedSub(QListViewItem *)
{
	pbRemoveSub->setEnabled( true );
}

/**
 * Open sub File (Input)
 */
void qcreatevob::openSub()
{
  SubFile =
    QFileDialog::getOpenFileName( inputFilePath, subFilters, this );
  if ( !SubFile.isEmpty() )
  {
    leSubInputFile->setText( SubFile );
  }
  else
  {
    SubFile = leSubInputFile->text();
  }
  pbAddSub->setEnabled(!SubFile.isEmpty());
}

/**
 * Open Video File (Input)
 */
void qcreatevob::open()
{
  inputFile =
    QFileDialog::getOpenFileName( inputFilePath, fileFilters, this );
  if ( !inputFile.isEmpty() )
  {
    inputFileLine->setText( inputFile );
  }
  else
  {
    inputFile = inputFileLine->text();
  }
}

/**
 * Select TEMP Directory
 */
void qcreatevob::openTMP()
{
  tmpDirStr =
    QFileDialog::getExistingDirectory( tmpDirStr, this );
  if ( !tmpDirStr.isEmpty() )
  {
    tmpDirLine->setText( tmpDirStr );
  }
  else
  {
    tmpDirStr = tmpDirLine->text();
  }
}

/**
 * Run after input file selection
 */
void qcreatevob::inputFileSelected()
{
  widthView->clear();
  heightView->clear();
  fpsView->clear();
  framesView->clear();
  durationView->clear();
  codecView->clear();
  int i;
  for ( i = tableSound->numRows(); i > 0; i-- )
  {
	  tableSound->removeRow( i - 1 );
  }
  for ( i = tableSoundOut->numRows(); i > 0; i-- )
  {
	  tableSoundOut->removeRow( i - 1 );
  }
  /*
  qualityView->clear();
  formatView->clear();
  bitrateView->clear();
  channelsView->clear();
  bitsView->clear();
  */

  inputFile = inputFileLine->text();
  setTemp();
  getStrings();
  tempChanged();
  if ( QFile::exists( inputFile ) )
  {
    myMplex->changeVobFile( inputFilePath, inputFileName );
    myCheck->main( inputFile );

  }
  writeSettings();
  gammaBox->setNum( 1.0 );
}

void qcreatevob::updateSound(int tracks)
{
	// update SoundOut
	int i;
	for ( i = tableSoundOut->numRows(); i > 0; i-- )
	{
	  tableSoundOut->removeRow( i - 1 );
	}
	for ( i = 0; i < tracks; i++ )
	{
		tableSoundOut->insertRows( i );
		tableSoundOut->setItem( i, 0,
				new QCheckTableItem( tableSoundOut, QString( "Enable" ) ) );
		tableSoundOut->setItem( i, 1,
				new QComboTableItem( tableSoundOut, soundOutList ) );
	}
}

/**
 * Temp Directory changed
 */
void qcreatevob::tempChanged()
{
	setTemp();
	if ( !inputFile.isEmpty() )
	{
		myMplex->videoInFileLine->setText( tmpDirStr + inputFileName + ".m2v" );
		myMplex->main( tmpDirStr, inputFileName );
	}
}

/**
 * Set TEMP Directory
 */
void qcreatevob::setTemp()
{
  if ( QString( tmpDirLine->text() ).isEmpty() )
  {
    tmpDirLine->setText( getTempDir( inputFile ) );
  }
  else
  {
    tmpDirStr = tmpDirLine->text();
  }
}

/**
 * Set Strings
 */
void qcreatevob::getStrings()
{
  inputFileName = getFileName( inputFile );
  inputFilePath = getInputPath( inputFile );
}

/**
 * Create Script
 */
void qcreatevob::addScript()
{
	QString scriptStr = "";
	if ( !inputFile.isEmpty() )
	{
		scriptStr = getScriptBegin( inputFile, myMplex->outFileLine->text() );

		// Mencoder
		if ( enableMencoderBox->isChecked() )
		{
			inputFile = QString( tmpDirStr ).append( inputFileName ).append( ".avi" );
			QString mencoderStr = QString( "mencoder \"%1\" -o \"%2\" -ovc lavc -oac mp3lame -ofps %3 " )
			.arg( inputFileLine->text() ).arg( inputFile ).arg( fpsOutView->text() );

			scriptStr += "## Decode video to avi (mencoder)\n" +
			mencoderStr + "\n\n";
		}

		// Audio
		int i;
		for ( i = 0; i < tableSoundOut->numRows(); i++ )
		{
			QCheckTableItem *chkItem = (QCheckTableItem *)tableSoundOut->item( i, SOUNDOUT_CHECK );
			QComboTableItem *comItem = (QComboTableItem *)tableSoundOut->item( i, SOUNDOUT_SETTINGS );
			QString chanView = tableSound->text( i, CHANNELVIEW );
			QString tra;
			tra.sprintf( "t%d", i );
			QString outChanView = comItem->currentText();
			if ( chkItem->isChecked() && !outChanView.isEmpty() )
			{
				QString ac3 = addK( tmpDirStr + inputFileName + tra + ".ac3" );
				QString finalAc3;
				finalAc3 = addK( tmpDirStr + inputFileName + tra + "_final.ac3" );
				QString remATmp = "";
				if ( remCheckBox->isChecked() )
				{
					remATmp = "on";
				}

				QString audioqu = tableSoundOut->text( i, SOUNDOUT_QUALITY );
				if ( audioqu.isEmpty() )
				{
					audioqu = "48000";
				}
				if ( audioqu = "0" )
				{
					audioqu = "48000";
				}
				QString infAudbrView = tableSoundOut->text( i, SOUNDOUT_BITRATE );

				mySnd->main( chanView,
								tmpDirStr,
								inputFileName,
								inputFile,
								infAudbrView,
								audioqu,
								outChanView,
								remATmp,
								i );

				if ( !mySnd->sndMessage.isEmpty() )
				{
					QMessageBox::information( this, "QCreateVOB",
												mySnd->sndMessage );
				}
				scriptStr += mySnd->sndScript + "\n\n";
			}
		}
		// VIDEO
		QString outFormat;
		QString outAspect;
		QString outFps;
		QString inFps;
		QString importVideo;
		QString outRes;
		QString outFile;
		QString V;
		QString border;
		QString vbitrate;
		QString modfps;
		QString fast;
		QString gamma = "";

		if ( formatBox->currentText() == "pal" ) { outFormat = "--export_prof dvd-pal"; }
		if ( formatBox->currentText() == "ntsc" ) { outFormat = "--export_prof dvd-ntsc"; }
		if ( aspectBox->currentText() == "1:1" ) { outAspect = "--export_asr 1"; }
		if ( aspectBox->currentText() == "4:3" ) { outAspect = "--export_asr 2"; }
		if ( aspectBox->currentText() == "16:9" ) { outAspect = "--export_asr 3"; }
		if ( aspectBox->currentText() == "2.21:1" )
		{
			QMessageBox::information( this, "QCreateVob",
							"You chose a video aspect of 2.21:1.\nWhile there's nothing wrong this that it itself, you should be warned that such ratio is not DVD compliant, and that tools such as dvdauthor will warn you about it.\nIf you intend to turn your vob file into a dvd you probably should choose another video aspect and/or add black border to your video." );
			outAspect = "--export_asr 4";
		}
		if ( QString( fpsView->text() ).find( fpsOutView->text() ) == -1 )
		{
			if ( fpsOutView->text() == "23.976" )
			{
				QMessageBox::information( this, "QCreateVob",
								"You chose a frame rate of 23.976. Nothing wrong with that, except that it's not DVD compliant.\nAt best, you will get plenty of warning from dvdauhtor (or other authoring tool) and your player will probably give you wrong elapsed times.\nYou should probably choose 29,976 as frame rate, a natural choice for ntsc DVDs.\nThis will NOT cause A/V desync since it's only the number of frame in a second that varies, and not the length of the second...");
			}
			outFps = "--export_fps " + fpsOutView->text();
			modfps = "-J modfps";
			inFps = "-f " + fpsView->text();
		}

		if ( !QString( importVideoBox->currentText() ).isEmpty() )
		{
			importVideo = "-x " + importVideoBox->currentText();
		}
		if ( !QString( infoResView->text() ).isEmpty() ) { outRes = "-Z " + infoResView->text(); }
		if ( fastBox->isChecked() ) {fast = ",fast";}
		outFile = tmpDirStr + inputFileName;
		if ( ( !QString( borderView->text() ).isEmpty() )
			&& ( QString( borderView->text() ).compare( "0,0,0,0" ) != 0 ) )
		{
			border = "--pre_clip " + borderView->text();
		}
		if ( ( infoVideobrView->text().compare( "0" ) == 0 ) 
		|| ( QString( infoVideobrView->text() ).isEmpty() ) )
		{
			vbitrate = "";
		}
		else
		{
			if ( QString( infoVideobrView->text() ).isEmpty() )
			{
				vbitrate = ""; 
			}
			else
			{
				vbitrate = "-w " + infoVideobrView->text(); 
			}
		}

		if ( gammaBox->currentText() != "1" )
		{
			gamma = "-G " + gammaBox->currentText();
		}

		if ( !disableVideoBox->isChecked() )
		{
			if ( QString( fileFormatView->text() ) == "UNKNOW" )
			{
				QMessageBox::critical( this, tr( "QCreateVOB" ),
					tr( "ERROR: File format not supported\nby QCreateVOB" ) );
			}
			else
			{
				QString transcode;
				if ( rbFfmpeg->isChecked() )
				{
					transcode = QString( "transcode -i \"%inputFile\" %V -y ffmpeg,null -F mpeg2 %outFormat %border %gamma %vbitrate %outAspect %outFps %inFps %modfps %importVideo %outRes%fast -o \"%outFile\"" );
					/*.arg(inputFile).arg(V).arg(outFormat).arg(border).arg(vbitrate).arg(outAspect).arg(outFps)
					.arg(inFps).arg(modfps).arg(importVideo).arg(outRes).arg(fast).arg(outFile)*/;
				}
				else
				{	// mpeg2enc
					transcode = QString( "transcode -i '%inputFile' %V -y mpeg2enc,null -F 8 %outFormat %border %gamma %vbitrate %outAspect %outFps %inFps %modfps %importVideo %outRes%fast -o '%outFile'" );

				}
				transcode.replace( "%inputFile", inputFile )
				.replace( "%V", V )
				.replace( "%outFormat", outFormat )
				.replace( "%border", border )
				.replace( "%gamma", gamma )
				.replace( "%vbitrate", vbitrate )
				.replace( "%outAspect", outAspect )
				.replace( "%outFps", outFps )
				.replace( "%inFps", inFps )
				.replace( "%modfps", modfps )
				.replace( "%importVideo", importVideo )
				.replace( "%outRes", outRes )
				.replace( "%fast", fast )
				.replace( "%outFile", outFile )
				.replace( "  ", " " );

				scriptStr += "## VIDEO ENCODING\n" + transcode + "\n\n";

			}
		}

		// MPLEXING
		scriptStr += myMplex->createScript();

		// SPUMUXING

		//lvSubs->insertItem( new QListViewItem( lvSubs, leSubInputFile->text(), cbLang->currentText() ) );
		QListViewItemIterator it( lvSubs );
		QListViewItem *subItem;
		QString subFile;
		QString subLang;
		QString w;
		QString h;
		QString subScript;
		QString s1, s2, s3, s4;
		s1.sprintf( "%d", sbSubLeftMargin->value() );
		s2.sprintf( "%d", sbSubRightMargin->value() );
		s3.sprintf( "%d", sbSubTopMargin->value() );
		s4.sprintf( "%d", sbSubBottomMargin->value() );
		QString in, out;
		in = myMplex->outFileLine->text();
		while ( it.current() )
		{
			subItem = it.current();
			subFile = subItem->text( 0 );
			subLang = subItem->text( 1 );
			w = infoResView->text().section( "x", 0, 0 );
			h = infoResView->text().section( "x", 1, 1 );
			subScript = "\necho \"<subpictures>\n"
				"<stream>\n"
				"<textsub filename='" + subFile +"' \n"
				"characterset='ISO8859-1' \n"
				"fontsize='" + sbSubFontSize->text() + "' \n"
				"font='" + cbSubFont->currentText() + "' \n"
				"horizontal-alignment='" + cbSubHorAlign->currentText() + "' \n"
				"vertical-alignment='" + cbSubVertAlign->currentText() + "' \n"
				"left-margin='" + s1 + "' \n"
				"right-margin='" + s2 + "'\n"
				"top-margin='" + s3+ "' \n"
				"bottom-margin='" + s4 + "' \n"
				"subtitle-fps='" + fpsOutView->text() + "'\n"
				"movie-fps='" + fpsOutView->text() + "' \n"
				"movie-width='" + w + "' \n"
				"movie-height='" + h + "'\n"
				"/>\n"
				"</stream>\n"
				"</subpictures>\" > " +tmpDirLine->text() + "/" + subLang + "_sub.xml\n";
			out =  myMplex->outFileLine->text() + "_" + subLang;
			subScript += "spumux " + tmpDirLine->text() + "/" + subLang + "_sub.xml < " + in + " > " + out;
			scriptStr += subScript;
			in = out;
			++it;
		}
		addScriptToList( scriptStr );
	}
}

void qcreatevob::audioSettingChange( int row, int /*col*/ )
{
	// AUDIO: BITRATE Check

	myMplex->clearAudioInput();

	int i, ctr;
	QCheckTableItem *chk;
	for ( ctr = 0, i = 0; i < tableSoundOut->numRows(); i++ )
	{
		chk = (QCheckTableItem *)tableSoundOut->item( row, SOUNDOUT_CHECK );
		if ( chk->isChecked() )
		{
			emit addAudio( ctr++ );
			QComboTableItem *item =
				(QComboTableItem *)tableSoundOut->item( row, SOUNDOUT_SETTINGS );
			tableSoundOut->setText( row,
									SOUNDOUT_BITRATE,
									getAudioBr( item->currentText() ) );
			tableSoundOut->setText( row,
									SOUNDOUT_QUALITY,
									tableSound->text( row, QUALITYVIEW ) );
		}
		else
		{
			tableSoundOut->setText( row, SOUNDOUT_BITRATE, "" );
			tableSoundOut->setText( row, SOUNDOUT_QUALITY, "" );
		}
	}
}

void qcreatevob::settingChange()
{
  // AUDIO: BITRATE Check
  //infoAudiobrView->setText( getAudioBr( audioOutChannel->currentText() ) );

  //VIDEO: FORMAT Check
  fpsOutView->setText( getFps( formatBox->currentText(), "" ) );
  infoResView->setText( getRes( formatBox->currentText(), "" ) );
  borderView->setText( getBorder( aspectBox->currentText(), widthView->text(), heightView->text() ) );
  writeSettings();
}

void qcreatevob::readSettings()
{
  QString seittingsFile = getenv( "HOME" );
  seittingsFile.append( "/.qcreatevob/qcreatevobrc" );

  if ( QFile::exists( seittingsFile ) == true )
  {
    QSettings settings;
    settings.insertSearchPath( QSettings::Unix, QDir::homeDirPath () + "/.qcreatevob/" );
    settings.setPath( "rmt.the-royal.de", "qcreatevob" );
    settings.beginGroup( "/QCreateVOB" );
    // ------------- GEOMETRY -------------- //
    int x = settings.readNumEntry( "/geometry/x", 200 );
    int y = settings.readNumEntry( "/geometry/y", 200 );
    int w = settings.readNumEntry( "/geometry/width", 400 );
    int h = settings.readNumEntry( "/geometry/height", 400 );
    move( x, y );
    resize( w, h );

    // ------------- OTHER -------------- //
    myMplex->vobPathStr = settings.readEntry( "/OutDir" );
    tmpDirLine->setText( settings.readEntry( "/TempDir" ) );
    myMplex->main( settings.readEntry( "/TempDir" ), "" );
	/* what with multiple audio tracks with different settings ?
    audioOutChannel->setCurrentText( settings.readEntry( "/AudioOut" ) );
    if ( !QString( settings.readEntry( "/AudioQuality" ) ).isEmpty() )
    {
      outAudioQualitiLine->setText( settings.readEntry( "/AudioQuality" ) );
    }
	*/
    remCheckBox->setChecked( settings.readBoolEntry( "/AudioRemTemp", true ) );
    formatBox->setCurrentText( settings.readEntry( "/VideoFormat" ) );
    aspectBox->setCurrentText( settings.readEntry( "/VideoAspect" ) );
    if ( !QString( settings.readEntry( "/VideoBitrate" ) ).isEmpty() )
    {
      infoVideobrView->setText( settings.readEntry( "/VideoBitrate" ) );
    }
    fastBox->setChecked( settings.readBoolEntry( "/VideoFast", true ) );
    myMplex->remCheckBox_2->setChecked( settings.readBoolEntry( "/MplexRemoveTemp", true ) );
    if ( settings.readBoolEntry( "/Mplex", true ) )
    {myMplex->disableTcmplex();}
    else
    {myMplex->disableMplex();}
    if ( !QString( settings.readEntry( "/MplexSize" ) ).isEmpty() )
    {myMplex->sizeLine->setText( settings.readEntry( "/MplexSize" ) );}
    else
    {myMplex->sizeLine->setText( "0" );}
    inputFilePath = settings.readEntry( "/InputDir" );

    if ( !QString( settings.readEntry( "/Encoder" ) ).isEmpty() )
	{
		if( settings.readEntry( "/Encoder" ).compare( "ffmpeg" ) )
		{
			rbFfmpeg->setChecked( true );
		}
		else
		{
			rbFfmpeg->setChecked( false );
		}
	}
	else
	{
		rbFfmpeg->setChecked( true );
	}
    settings.endGroup();
    settingChange();
  }
  else
  {
    aspectBox->setCurrentText( "4:3" );
    settingChange();
  }
}


void qcreatevob::writeSettings()
{
  QSettings settings;
  settings.insertSearchPath( QSettings::Unix, QDir::homeDirPath() + "/.qcreatevob/" );
  settings.setPath( "rmt.the-royal.de", "qcreatevob" );
  settings.beginGroup( "/QCreateVOB" );
  // ------------ Geometry ------------- //
  settings.writeEntry( "/geometry/x", x() );
  settings.writeEntry( "/geometry/y", y() );
  settings.writeEntry( "/geometry/width", width() );
  settings.writeEntry( "/geometry/height", height() );

  // ------------ Other ------------- //
  settings.writeEntry( "/InputDir", inputFilePath );
  settings.writeEntry( "/TempDir", tmpDirLine->text() );
  settings.writeEntry( "/OutDir", myMplex->getVobPath() );
	/* what with multiple audio tracks with different settings ?
  settings.writeEntry( "/AudioOut", audioOutChannel->currentText() );
  settings.writeEntry( "/AudioQuality", outAudioQualitiLine->text() );
  */
  settings.writeEntry( "/AudioRemTemp", remCheckBox->isChecked() );
  settings.writeEntry( "/VideoFormat", formatBox->currentText() );
  settings.writeEntry( "/VideoAspect", aspectBox->currentText() );
  settings.writeEntry( "/VideoBitrate", infoVideobrView->text() );
  settings.writeEntry( "/VideoFast", fastBox->isChecked() );
  settings.writeEntry( "/Mplex", myMplex->mplexToolCheckded() );
  settings.writeEntry( "/MplexRemoveTemp", myMplex->remCheckBox_2->isChecked() );
  settings.writeEntry( "/MplexSize", myMplex->sizeLine->text() );
  if ( rbFfmpeg->isChecked() )
  {
	  settings.writeEntry( "/Encoder", "ffmpeg" );
  }
  else
  {
	  settings.writeEntry( "/Encoder", "mpeg2enc" );
  }
  settings.endGroup();
}

/**
 * Process files 
 */
void qcreatevob::startProc()
{
  if ( acceptList() )
  {
    myProc = new processDlg( this );
    myProc->main( ScriptList->itmeStrList.join( "" ) );
    myProc->show();
    myProc->raise();
    myProc->setActiveWindow();
  }
  else
  {
    QMessageBox::critical( this, "QCreateVOB", tr( "Your settings are not correct!" ) );
  }
}


void qcreatevob::changeRes()
{
  infoResView->setText( getRes( formatBox->currentText(), infoResView->text() ) );
}


void qcreatevob::changeFps()
{
  fpsOutView->setText( getFps( formatBox->currentText(), fpsOutView->text() ) );
}

/**
 * Start Border selection dialog
 */
void qcreatevob::getBorderSlot()
{
  QString border = getSelBr( borderView->text() );

  QString newRes = getResForBS( aspectBox->currentText(), widthView->text() );
  QString resX = newRes;
  resX.replace( QRegExp( "(.*),(.*)" ), "\\1" );
  QString resY = newRes;
  resY.replace( QRegExp( "(.*),(.*)" ), "\\2" );

  if ( !resX.isEmpty() && !resY.isEmpty() )
  {
    //     QSplashScreen * splash =
    //       new QSplashScreen(QPixmap::fromMimeSource("images/splash.png"));
    //     splash->show();
    //     splash->message( QObject::tr( "Getting screenshot..." ),
    //                      Qt::AlignRight | Qt::AlignTop, Qt::white );

    myBorder = new borderSelector( this );
    myBorder->main( inputFileLine->text(), border, resX, resY );
    //     splash->finish( myBorder );
    //     delete splash;
    if ( framesView->text() == "Unknow" )
    {
      myBorder->frameNr->setMaxValue( 100000000 );
    }
    else
    {
      myBorder->frameNr->setMaxValue( QString( framesView->text() ).toInt() );
    }

    if ( myBorder->exec() )
    {
      borderView->setText( myBorder->selTop->text() + "," +
                           myBorder->selLeft->text() + "," +
                           myBorder->selButtom->text() + "," +
                           myBorder->selRight->text() );
      myBorder->close();
    }
  }
  else
  {
    QMessageBox::information( this, "QCreateVOB", tr( "Sorry! Can't start \"Border Selection dialog\".\n No input file resolution found! :-(" ) );
  }
}

/**
 * Add Script
 * @param script 
 */
void qcreatevob::addScriptToList( const QString &script )
{
  if ( !inputFile.isEmpty() )
  {
    int number = ScriptList->currentItem();
    if ( ScriptList->findItemName( inputFile ) )
    {
      int ret = QMessageBox::warning( this, tr( "QCreateVOB" ),
                                      tr( "File is allredy exists.\n"
                                          "Do you want to replace changes?" ),
                                      QMessageBox::Yes | QMessageBox::Default,
                                      QMessageBox::No | QMessageBox::Escape );
      if ( ret == QMessageBox::Yes )
        ScriptList->replaceItem( number, script );
    }
    else
    {
      ScriptList->addToList( inputFile, script );
    }
  }
  else
  {
    if ( myMplex->mplexAccepInput() )
    {
      int number = ScriptList->currentItem();
      if ( ScriptList->findItemName( myMplex->outFileLine->text() ) )
      {
        int ret = QMessageBox::warning( this, tr( "QCreateVOB" ),
                                        tr( "File is allredy exists.\n"
                                            "Do you want to replace changes?" ),
                                        QMessageBox::Yes | QMessageBox::Default,
                                        QMessageBox::No | QMessageBox::Escape );
        if ( ret == QMessageBox::Yes )
          ScriptList->replaceItem( number, script );
      }
      else
      {
        ScriptList->addToList( myMplex->outFileLine->text(), script );
      }
    }
  }

}


/**
 * Read/Edit Script
 */
void qcreatevob::readScriptFromList()
{
  int number = ScriptList->currentItem();
  myScript = new scriptedit( this );
  myScript -> main( ScriptList->showItem() );
  if ( myScript->exec() )
  {
    QString tmp = myScript->becomeStr();
    ScriptList->replaceItem( number, tmp );
  }
}

/**
 * Save Script
 */
void qcreatevob::saveScript()
{
	int number = ScriptList->currentItem();
	QString s = ScriptList->showItem();
	QString fn = QFileDialog::getSaveFileName( inputFilePath, "*", this,
					"Save script", "Choose a file name" );
	if ( !fn.isNull() )
	{
		QFile f( fn );

		if ( f.open(IO_WriteOnly) )
		{
			QTextStream stream( &f );
			stream << s;
		}
	}
}




void qcreatevob::slotMplexError()
{
  QMessageBox::warning( this, "QCreateVOB", tr( "Warning your mplex settings are not correct!\nPlease check it." ) );
}

bool qcreatevob::acceptList()
{
  if ( !inputFile.isEmpty() )
  {
    if ( ScriptList->findItemName( inputFile ) )
    {
      return true;
    }
    else
    {
      int ret = QMessageBox::warning( this, tr( "QCreateVOB" ),
                                      tr( "The selected file is not in the list.\n"
                                          "Do you want to add it to list?" ),
                                      QMessageBox::Yes | QMessageBox::Default,
                                      QMessageBox::No | QMessageBox::Escape );
      if ( ret == QMessageBox::Yes )
      {
        addScript();
        return true;
      }
      if ( ret == QMessageBox::No )
      {
        if ( ScriptList->isEmpty() )
        {
          return false;
        }
        else
        {
          return true;
        }
      }
    }

  }
  else
  {
    if ( myMplex->mplexAccepInput() )
    {
      if ( ScriptList->findItemName( myMplex->outFileLine->text() ) )
      {
        return true;
      }
      else
      {
        int ret = QMessageBox::warning( this, tr( "QCreateVOB" ),
                                        tr( "The selected file is not in the list.\n"
                                            "Do you want to add it to list?" ),
                                        QMessageBox::Yes | QMessageBox::Default,
                                        QMessageBox::No | QMessageBox::Escape );
        if ( ret == QMessageBox::Yes )
        {
          addScript();
          return true;
        }
        if ( ret == QMessageBox::No )
        {
          if ( ScriptList->isEmpty() )
          {
            return false;
          }
          else
          {
            return true;
          }
        }
      }
    }
  }
  if ( ScriptList->isEmpty() )
  {
    return false;
  }
  else
  {
    return true;
  }
}
