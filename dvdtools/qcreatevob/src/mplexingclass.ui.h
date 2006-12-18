void mplexingclass::delAudio( )
{
	tableAudioInputs->removeRow( tableAudioInputs->currentRow() );
	if ( tableAudioInputs->numRows() < 3 )
	{
		tcmplexTool->setEnabled( true );
		QToolTip::add( tcmplexTool, "Transcode mplex" );
	}
}

void mplexingclass::newAudio( )
{
	int i = tableAudioInputs->numRows();
	addAudio( i );
	openac3( i, 0 );
}

void mplexingclass::addAudio( int row )
{
	int i = tableAudioInputs->numRows();

	tableAudioInputs->insertRows( i );
	TableButton * but = new TableButton( i, 1, "...", tableAudioInputs, "choose" );
	connect ( but, SIGNAL( clic( int, int ) ),
			this,
			SLOT( openac3(int, int) )
			);
	QString s;
	s.sprintf( "%s/%st%d.ac3", tmpDirStr.latin1(), inputFileName.latin1(),row );
	tableAudioInputs->setText( i, 0, s );
	tableAudioInputs->adjustColumn( 0 );
	tableAudioInputs->setCellWidget( i, 1, but );
	tableAudioInputs->adjustColumn( 1 );

	if ( i > 1 )
	{
		tcmplexTool->setChecked( false );
		tcmplexTool->setEnabled( false );
		QToolTip::add( tcmplexTool, "tcmplex can't multiplex more than two audio files. mplex forced" );
		mplexTool->setChecked( true );
	}
	else
	{
		tcmplexTool->setEnabled( true );
		QToolTip::add( tcmplexTool, "Transcode mplex" );
	}
}

void mplexingclass::clearAudioInput( )
{
	int i;
	for ( i = tableAudioInputs->numRows(); i > 0; i-- )
	{
		tableAudioInputs->removeRow( i - 1 );
	}	
}

void mplexingclass::main( const QString tmpdir, const QString inputfilename )
{
  tmpDirStr = tmpdir;
  inputFileName = inputfilename;
}

/**
 * Open M2V Video file
 */
void mplexingclass::openm2v()
{
	QString filters = tr( "video files (*.m2v)" );

	m2vFile = QFileDialog::getOpenFileName( tmpDirStr, filters, this );
	if ( !m2vFile.isEmpty() )
	{
		videoInFileLine->setText( m2vFile );
	}
	else
	{
		m2vFile = videoInFileLine->text();
	}
}

/**
 * Open AC3 Audio File
 */
void mplexingclass::openac3(int row, int /*col*/)
{
	QString filters = tr( "Audio files (*.ac3)" );

	QString ac3File = QFileDialog::getOpenFileName( tmpDirStr, filters, this );
	if ( !ac3File.isEmpty() )
	{
		tableAudioInputs->setText( row, 0, ac3File );
	}
}


/**
 * Save output file (VOB)
 */
void mplexingclass::saveVob()
{
  QString filters = "*.vob";
  vobFileStr =
    QFileDialog::getSaveFileName( vobPathStr, filters, this );
  if ( !vobFileStr.isEmpty() )
  {
    if ( vobFileStr.right( 4 ) == ".vob" )
    {
      outFileLine->setText( vobFileStr );
    }
    else
    {
      vobFileStr.append( ".vob" );
      outFileLine->setText( vobFileStr );
    }
  }
  else
  {
    vobFileStr = outFileLine->text();
  }
}

QString mplexingclass::createScript()
{
	QString m2vFileStr = videoInFileLine->text();
	QString vobFileStr = outFileLine->text();
	QString O;
	QString S;
	QString mplexing;


	if ( mplexAccepInput() )
	{
		if ( mplexTool->isChecked() )
		{
			O = getSync();

			if ( sizeLine->text() == "0" )
			{
				S = "";
			}
			else
			{
				S = "-S " + sizeLine->text();
			}
			mplexing = "## MPLEXING FILE\n";
			mplexing.append( "mplex  -f 8 %S %O -o \"%vobFileStr\" \"%m2vFileStr\"" );
			mplexing.replace( "%S", S )
					.replace( "%O", O )
					.replace( "%vobFileStr", vobFileStr )
					.replace( "%m2vFileStr", m2vFileStr );

			int i;
			for ( i = 0; i < tableAudioInputs->numRows(); i++ )
			{
				mplexing.append( " \"%audioFile\"" );
				mplexing.replace( "%audioFile",
									tableAudioInputs->text( i, 0 ) );
			}
			mplexing.append("\n");
			mplexing.append(removeTmp());
		}
		else
		{
			O = getSync();
			S = "";
			mplexing = "## MPLEXING FILE\n";
			mplexing.append( "tcmplex  -m d %O -o \"%vobFileStr\" -i \"%m2vFileStr\" -p \"%ac3FileStr\"" );
			mplexing.replace( "%S", S )
			.replace( "%O", O )
			.replace( "%vobFileStr", vobFileStr )
			.replace( "%m2vFileStr", m2vFileStr )
			.replace( "%ac3FileStr", tableAudioInputs->text( 0, 0 ) );

			if ( !tableAudioInputs->text( 1, 0 ).isEmpty() )
			{
			mplexing.append( " -s \"%ac3File2Str\"" );
			mplexing.replace( "%ac3File2Str", tableAudioInputs->text( 1, 0 ) );
			}
			mplexing.append( "\n" );
			mplexing.append(removeTmp());

		}
		return QString( mplexing );
	}
	else
	{
		errorSignal();
	}
	return QString( "" );
}

QString mplexingclass::getSync()
{
  QString out;
  QString num = syncBox->text();

  if ( mplexTool->isChecked() )
  {
    if ( num == "0" )
    {
      out = "";
    }
    else
    {
      out = "-O " + num;
    }
  }
  else
  {
    if ( num == "0" )
    {
      out = "";
    }
    else
    {
      if ( num.toInt() < 0 )
      {
        num.replace( "-", "" );
        out = "-D " + num;
      }
      else
      {
        out = "-D 0," + num;
      }
    }
  }

  return QString( out );
}

bool mplexingclass::mplexAccepInput()
	{
	QString str1 = videoInFileLine->text();
	QString str2 = tableAudioInputs->text( 0, 0 );
	QString str3 = outFileLine->text();

	if ( !str1.isEmpty() && !str2.isEmpty() && !str3.isEmpty() )
	{
		return true;
	}
	else
	{
		return false;
	}
	}

void mplexingclass::vobSelected()
{
  vobPathStr = getVobPath();
}

void mplexingclass::disableMplex()
{
  mplexTool->setChecked( false );
  tcmplexTool->setChecked( true );
}

void mplexingclass::disableTcmplex()
{
  mplexTool->setChecked( true );
  tcmplexTool->setChecked( false );
}

QString mplexingclass::getVobPath()
{
  QString str = outFileLine->text();
  str.replace( QRegExp( "^(.*/)(.*)" ), "\\1" );
  return QString( str );
}

bool mplexingclass::mplexToolCheckded()
{
  if ( mplexTool->isChecked() )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void mplexingclass::changeVobFile( QString inputPath, QString inputName )
{
  if ( vobPathStr.isEmpty() )
  {
    outFileLine->setText( inputPath + inputName + ".vob" );
  }
  else
  {
    outFileLine->setText( vobPathStr + inputName + ".vob" );

  }
}

void mplexingclass::writeSet()
{
  writeSettings();
}

QString mplexingclass::removeTmp()
{
	if (remCheckBox_2->isChecked())
	{
		QString m2vFileStr = videoInFileLine->text();
		int i;
		QString s("rm -f \"%1\"");
		s.replace( "%1", m2vFileStr);
		for ( i = 0; i < tableAudioInputs->numRows(); i++ )
		{
			s.append( " \"%audio\"" );
			s.replace( "%audio", tableAudioInputs->text( i, 0 ) );
		}
		s.append( "\n" );
		return s;
	}
	else
	{
		return QString("# Rempve temp is disabled");
	}


}
