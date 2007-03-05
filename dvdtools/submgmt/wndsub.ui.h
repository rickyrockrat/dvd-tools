/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <iostream>

#define TIME_FMT	"hh:mm:ss,zzz"
#define SPU_TIME_FMT	"hh:mm:ss.zzz"
#define NUM_LINES	10

QRegExp microdvd = QRegExp( "\\{[0-9]+\\}\\{[0-9]*\\}" );
QRegExp mpl2 = QRegExp( "\\[[0-9]+]\\[[0-9]+]" );
QRegExp subrip = QRegExp("[0-9]+:[0-9]+:[0-9]+\\.[0-9]+,[0-9]+:[0-9]+:[0-9]+\\.[0-9]+");
QRegExp subviewer = QRegExp( "[0-9][0-9]:[0-9][0-9]:[0-9][0-9],[0-9][0-9][0-9] --> [0-9][0-9]:[0-9][0-9]:[0-9][0-9],[0-9][0-9][0-9]" );
QRegExp subviewer2 = QRegExp( "{T [0-9]+:[0-9]+:[0-9]+;[0-9]+" );
QRegExp sami = QRegExp( "<SAMI>" );
QRegExp jacosub = QRegExp( "[0-9]+;[0-9]+:[0-9]+.[0-9]+ [0-9]+;[0-9]+:[0-9]+.[0-9]+" ); 
QRegExp jacosub2 = QRegExp( "@[0-9]+ @[0-9]+" ); 
QRegExp vplayer = QRegExp( "[0-9]+;[0-9]+;[0-9]+" );
QRegExp vplayer2 = QRegExp( "[0-9]+;[0-9]+;[0-9]+ " );
QRegExp rt = QRegExp( "^<" );
QRegExp ssa = QRegExp( "Dialogue: " );
QRegExp pjs = QRegExp( "[0-9]+,[0-9]+,\"." );
QRegExp mpsub = QRegExp( "FORMAT=[0-9]+" );
QRegExp mpsub2 = QRegExp( "FORMAT=TIM." );
QRegExp aqtitle = QRegExp( "-->>" );
QRegExp subrip9 = QRegExp( "\\[[0-9]+:[0-9]+:[0-9]+]" );
QRegExp spumux = QRegExp( "<spu " );

void WndSub::filenameChanged()
{
	pbSave->setEnabled( true );
}

void WndSub::init()
{
	cbType->insertItem( "microdvd" );
	cbType->insertItem( "mpl2" );
	cbType->insertItem( "subrip" );
	cbType->insertItem( "subviewer" );
	cbType->insertItem( "subviewer2" );
	cbType->insertItem( "sami" );
	cbType->insertItem( "jacosub" );
	cbType->insertItem( "jacosub2" );
	cbType->insertItem( "vplayer" );
	cbType->insertItem( "vplayer2" );
	cbType->insertItem( "rt" );
	cbType->insertItem( "ssa" );
	cbType->insertItem( "pjs" );
	cbType->insertItem( "mpsub" );
	cbType->insertItem( "mpsub2" );
	cbType->insertItem( "aqtitle" );
	cbType->insertItem( "subrip9" );
	cbType->insertItem( "spumux" );

	cbTypeO->insertItem( "microdvd" );
	cbTypeO->insertItem( "mpl2" );
	cbTypeO->insertItem( "subrip" );
	cbTypeO->insertItem( "subviewer" );
	cbTypeO->insertItem( "subviewer2" );
	cbTypeO->insertItem( "sami" );	/* 5 */
	cbTypeO->insertItem( "jacosub" );
	cbTypeO->insertItem( "jacosub2" );
	cbTypeO->insertItem( "vplayer" );
	cbTypeO->insertItem( "vplayer2" );
	cbTypeO->insertItem( "rt" );
	cbTypeO->insertItem( "ssa" );
	cbTypeO->insertItem( "pjs" );
	cbTypeO->insertItem( "mpsub" );
	cbTypeO->insertItem( "mpsub2" );
	cbTypeO->insertItem( "aqtitle" );
	cbTypeO->insertItem( "subrip9" );
	cbTypeO->insertItem( "spumux" );	/* 17 */

	comboFpsOrig->insertItem( "23,970" );
	comboFpsOrig->insertItem( "25" );
	comboFpsOrig->insertItem( "29.976" );

	comboFpsDest->insertItem( "23,970" );
	comboFpsDest->insertItem( "25" );
	comboFpsDest->insertItem( "29.976" );

	lbInputSubs->hide();
	lbOutputSubs->hide();
	tlInputSubs->hide();
	tlOutputSubs->hide();
	sbShift->setEnabled( false );
	sbExpand->setEnabled( false );
	comboFpsDest->setEnabled( false );
	tlFps->setEnabled( false );
}

void WndSub::clickedSave()
{
	switch( cbTypeO->currentItem() )
	{
	case 0 :	/* microdvd */
		saveMicrodvd();
		break;
	case 2 : 	/* subrip */
		saveSubrip();
		break;
	case 3 : 	/* subviewer */
		saveSubviewer();
		break;
	case 5 : 	/* sami */
		saveSami();
		break;
	case 17 : 	/* spumux */
		saveSpumux();
		break;
	default :
		QMessageBox::information( this, "submgmt", "Cannot save file : " + cbTypeO->currentText() );
	}
}

void WndSub::clicSelectInputFile()
{
    QString s = QFileDialog::getOpenFileName("","*",this,"Select file", "Select a subtitle file");
    if ( s != NULL )
    {
		leInputFile->setText( s );
    }
	autoDetectFormat();
	loadSubFile();
	showInputSubs();
}


void WndSub::clicSelectOutputFile()
{
    QString s = QFileDialog::getSaveFileName("","*",this,"Save file", "Select a subtitle output file");
    if ( s != NULL )
    {
	leOutputFile->setText( s );
    }
}

void WndSub::checkFps()
{
	if ( cbFps->isChecked() )
	{
		comboFpsOrig->setEnabled( true );
		comboFpsDest->setEnabled( true );
	}
	else
	{
		comboFpsOrig->setEnabled( false );
		comboFpsDest->setEnabled( false );
	}
}

void WndSub::checkExpand()
{
	if ( cbExpand->isChecked() )
	{
		sbExpand->setEnabled( true );
	}
	else
	{
		sbExpand->setEnabled( false );
	}
}

void WndSub::checkShift()
{
	if ( cbShift->isChecked() )
	{
		sbShift->setEnabled( true );
	}
	else
	{
		sbShift->setEnabled( false );
	}
}

void WndSub::checkShiftPos()
{
	if ( cbShiftPos->isChecked() )
	{
		sbShiftPos->setEnabled( true );
	}
	else
	{
		sbShiftPos->setEnabled( false );
	}
}

void WndSub::checkAuto()
{
	if ( cbAuto->isChecked() )
	{
		timeFirstSub->setEnabled( true );
		timeLastSub->setEnabled( true );
		timeFirstSpeak->setEnabled( true );
		timeLastSpeak->setEnabled( true );
		tlFirstSub->setEnabled( true );
		tlLastSub->setEnabled( true );
		tlFirstSpeak->setEnabled( true );
		tlLastSpeak->setEnabled( true );
	}
	else
	{
		timeFirstSub->setEnabled( false );
		timeLastSub->setEnabled( false );
		timeFirstSpeak->setEnabled( false );
		timeLastSpeak->setEnabled( false );
		tlFirstSub->setEnabled( false );
		tlLastSub->setEnabled( false );
		tlFirstSpeak->setEnabled( false );
		tlLastSpeak->setEnabled( false );
	}
}

int milli( const QString &str, char dsep = '.' )
{
	QString h, m, s, ms;
	h = str.section( ':', 0, 0 );
	m = str.section( ':', 1, 1 );
	s = str.section( ':', 2 ).section( dsep, 0, 0 );
	ms = str.section( dsep, 1 );
	return ( h.toUInt() * 3600 + m.toUInt() * 60 + s.toUInt() ) * 1000 + ms.toUInt();
}

int milli( const QTime &t )
{
	return ( t.hour() * 3600 + t.minute() * 60 + t.second() ) * 1000 + t.msec();
}

QTime timeFromMilli( int ms )
{
	int milli, secondes, minutes, heures;
	int h, m, s;

	milli =  ms % 1000;
	s = (int)( (ms - milli) / 1000);
	secondes = s % 60;
	m = (int)( ( s - secondes ) / 60 );
	minutes = m % 60;
	h = (int)( ( m - minutes ) / 60 );
	heures = h;
	
	return QTime( heures, minutes, secondes, milli );
}

QTime operator -( const QTime &t1, const QTime &t2 )
{
	return timeFromMilli( milli(t1) - milli(t2) );
}

QTime operator *( const QTime &t, double factor )
{
	return timeFromMilli( milli(t) * factor );
}
QTime operator +( const QTime &t1, const QTime &t2 )
{
	return timeFromMilli( milli(t1) + milli(t2) );
}
QTime operator *( const QTime &t1, const QTime &t2 )
{
	return timeFromMilli( milli(t1) * milli(t2) );
}

double operator /( const QTime &t1, const QTime &t2 )
{
	return (double)milli(t1) / (double)milli(t2);
}

/*
QTime operator /( const QTime &t1, const QTime &t2 )
{
	return timeFromMilli( int((double)milli(t1) / (double)milli(t2)) );
}
*/

void WndSub::timeBasedProceed( subtitle &s )
{
	static int passage;
	QTime length, lengths;
	QString str;
	QStringList subs;
	QTime &ts = s.begin;
	QTime &te = s.end;
	subs = s.subs;

	if ( cbShift->isChecked() )
	{
		ts = ts.addMSecs( sbShift->value() );
		te = te.addMSecs( sbShift->value() );
	}
	if ( cbExpand->isChecked() )
	{
		ts = s.begin * ( sbExpand->value() / 1000 );
		te = s.end * ( sbExpand->value() / 1000 );
	}
	if ( cbFps->isChecked() )
	{
		ts = s.begin * ( comboFpsOrig->currentText().toDouble() / comboFpsDest->currentText().toDouble() );
		te = s.end * ( comboFpsOrig->currentText().toDouble() / comboFpsDest->currentText().toDouble() );
	}
	if ( cbAuto->isChecked() )
	{
		/* mode auto */
		/* goal is to align first sub with first speaker and
		 * last sub with last speaker
		 *
		 * first we move subs to 0 */

		ts = s.begin - timeFirstSub->time();
		te = s.end - timeFirstSub->time();

		/* Now we expand subs so that they have the same length that
		 * the speakers */
		length = timeLastSpeak->time() - timeFirstSpeak->time();
		lengths = timeLastSub->time() - timeFirstSub->time();

		double factor;
		factor = length / lengths;
		if ( passage == 0 )
		{
			passage++;
			std::cout << "factor=" << factor << std::endl;
		}
		ts = ts * factor; 
		te = te * factor;

		/* and we finish by aligning the first sub to the first speak */
		ts = ts + timeFirstSpeak->time();
		te = te + timeFirstSpeak->time();

	}
	if ( cbShiftPos->isChecked() )
	{
		if ( sbShiftPos->value() > 0 )
		{
			v_subs.push_back( s );
			s.subs.clear();		// first x subtitles are empty
			if ( v_subs.size() >= (sbShiftPos->value()+1) )
			{
				int i = v_subs.size() - sbShiftPos->value() - 1;
				s.subs = v_subs[ i ].subs;
			}
		}
		else if ( sbShiftPos->value() < 0 )
		{
			v_subs.push_back( s );
			s.begin = QTime();	// null time
			s.end = QTime();
			if ( v_subs.size() >= (abs(sbShiftPos->value())+1) )
			{
				int i = v_subs.size() - abs(sbShiftPos->value()) - 1;
				s.begin = v_subs[ i ].begin;
				s.end = v_subs[ i ].end;
			}

		}
	}
		//lbSubs->insertItem( str );
		//lbSubs->insertStringList( subs );
}

void WndSub::frameBasedProceed( subtitle &s )
{
	QString str;
	int length, lengths;
	QStringList subs;
	int &fs = s.fbegin;
	int &fe = s.fend;
	subs = s.subs;
	int fFirstSub;
	int fFirstSpeak;
	int fLastSub;
	int fLastSpeak;

	double fps = comboFpsOrig->currentText().toDouble();
	if ( cbShift->isChecked() )
	{
		fs = fs + (int)( sbShift->value() / 1000 * fps );
		fe = fe + (int)( sbShift->value() / 1000 * fps );
	}
	if ( cbExpand->isChecked() )
	{
		fs = s.fbegin * sbExpand->value() / 1000;
		fe = s.fend * sbExpand->value() / 1000;
	}
	if ( cbFps->isChecked() )
	{
		fs = int( s.fbegin * ( fps / comboFpsDest->currentText().toDouble() ) );
		fe = int( s.fend * ( fps / comboFpsDest->currentText().toDouble() ) );
	}
	if ( cbAuto->isChecked() )
	{
		/* mode auto */
		/* goal is to align first sub with first speaker and
		 * last sub with last speaker
		 *
		 * first we move subs to 0 */

		fFirstSub =  (int)(milli( timeFirstSub->time() ) / fps );
		fFirstSpeak =  (int)(milli( timeFirstSpeak->time() ) / fps );
		fLastSub =  (int)(milli( timeLastSub->time() ) / fps );
		fLastSpeak =  (int)(milli( timeLastSpeak->time() ) / fps );

		fs = s.fbegin - fFirstSub;
		fe = s.fend - fFirstSub;

		/* Now we expand subs so that they have the same length that
		 * the speakers */
		length = fLastSpeak - fFirstSpeak;
		lengths = fLastSub - fFirstSub;

		fs = fs * ( length / lengths ); 
		fe = fe * ( length / lengths );

		/* and we finish by aligning the first sub to the first speak */
		fs = fs + fFirstSpeak;
		fe = fe + fFirstSpeak;

	}
	if ( cbShiftPos->isChecked() )
	{
		if ( sbShiftPos->value() > 0 )
		{
			v_subs.push_back( s );
			s.subs.clear();		// first x subtitles are empty
			if ( v_subs.size() == (sbShiftPos->value()+1) )
			{
				int i = v_subs.size() - sbShiftPos->value() - 1;
				s.subs = v_subs[ i ].subs;
			}
		}
		else if ( sbShiftPos->value() < 0 )
		{
			v_subs.push_back( s );
			s.fbegin = 0;
			s.fend = 0;
			if ( v_subs.size() >= (abs(sbShiftPos->value())+1) )
			{
				int i = v_subs.size() - abs(sbShiftPos->value()) - 1;
				s.fbegin = v_subs[ i ].fbegin;
				s.fend = v_subs[ i ].fend;
			}
		}
	}
		//lbSubs->insertItem( str );
		//lbSubs->insertStringList( subs );
}

void WndSub::clicProceed()
{
	std::vector<subtitle>::iterator it;

	v_subs.clear();
	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		if ( it->FrameBased )
			frameBasedProceed( *it );
		else
			timeBasedProceed( *it );
	}
	showOutputSubs();
}
	
void WndSub::autoDetectFormat()
{
    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
        QTextStream stream( &file );
        QString line;
        int i = 0;
        while ( !stream.atEnd() && ( i < NUM_LINES ) )
		{
			i++;
            line = stream.readLine(); // line of text excluding '\n'
            if ( line.contains( microdvd ) ) {
				cbType->setCurrentItem( 0 );
				i = NUM_LINES + 1;
			} else if ( line.contains( mpl2 ) ) {
				cbType->setCurrentItem( 1 );
				i = NUM_LINES + 2;
			} else if ( line.contains( subrip ) ) {
				cbType->setCurrentItem( 2 );
				i = NUM_LINES + 3;
			} else if ( line.contains( subviewer ) ) {
				cbType->setCurrentItem( 3 );
				i = NUM_LINES + 4;
			} else if ( line.contains( subviewer2 ) ) {
				cbType->setCurrentItem( 4 );
				i = NUM_LINES + 5;
			} else if ( line.contains( sami ) ) {
				cbType->setCurrentItem( 5 );
				i = NUM_LINES + 6;
			} else if ( line.contains( jacosub ) ) {
				cbType->setCurrentItem( 6 );
				i = NUM_LINES + 7;
			} else if ( line.contains( jacosub2 ) ) {
				cbType->setCurrentItem( 7 );
				i = NUM_LINES + 8;
			} else if ( line.contains( spumux ) ) {
				cbType->setCurrentItem( 17 );
				i = NUM_LINES + 18;
			} else if ( line.contains( vplayer ) ) {
				cbType->setCurrentItem( 8 );
				i = NUM_LINES + 9;
			} else if ( line.contains( vplayer2 ) ) {
				cbType->setCurrentItem( 9 );
				i = NUM_LINES +10;
				/* the rt regexp is too weak and
				 * conflicts with the spu one...
			} else if ( line.contains( rt ) ) {
				statusBar()->message( "rt!!" );
				cbType->setCurrentItem( 10);
				i = NUM_LINES + 11;
				*/
			} else if ( line.contains( ssa ) ) {
				cbType->setCurrentItem( 11 );
				i = NUM_LINES + 12;
			} else if ( line.contains( pjs ) ) {
				cbType->setCurrentItem( 12 );
				i = NUM_LINES + 13;
			} else if ( line.contains( mpsub ) ) {
				cbType->setCurrentItem( 13 );
				i = NUM_LINES + 14;
			} else if ( line.contains( mpsub2 ) ) {
				cbType->setCurrentItem( 14 );
				i = NUM_LINES + 15;
			} else if ( line.contains( aqtitle ) ) {
				cbType->setCurrentItem( 15 );
				i = NUM_LINES + 16;
			} else if ( line.contains( subrip9 ) ) {
				cbType->setCurrentItem( 16 );
				i = NUM_LINES + 17;
			}
        }
        file.close();
    }
	cbTypeO->setCurrentItem( cbType->currentItem() );
}

void WndSub::loadSubFile()
{
	switch( cbType->currentItem() )
	{
	case 0 :	/* microdvd */
		loadMicrodvd();
		break;
	case 2 : 	/* subrip */
		loadSubrip();
		break;
	case 3 : 	/* subviewer */
		loadSubviewer();
		break;
	case 5 :	/* sami */
		loadSami();
		break;
	case 17 : 	/* spumux */
		loadSpumux();
		break;
	default :
		QMessageBox::information( this, "submgmt", "Cannot load file" );
	}
}

void WndSub::showOutputSubs()
{
	std::vector<subtitle>::iterator it;
	QTime ts;
	QTime te;
	QStringList subs;
	QString str;

	lbOutputSubs->clear();
	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		if ( it->FrameBased )
		{
			str.sprintf( "Frame %d to %d", it->fbegin, it->fend );
		}
		else
		{
			ts = it->begin;
			te = it->end;
			str = ts.toString( TIME_FMT ) + " to " + te.toString( TIME_FMT );
		}
		subs = it->subs;
		lbOutputSubs->insertItem( str );
		lbOutputSubs->insertStringList( subs );
	}
	tlOutputSubs->show();
	lbOutputSubs->show();
}

void WndSub::showInputSubs()
{
	std::vector<subtitle>::iterator it;
	QTime ts;
	QTime te;
	QTime prevte( 0, 0, 0, 0 );
	QStringList subs;
	QString str;
	int numero = 0, prevfe = 999999;

	lbInputSubs->clear();
	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		if ( it->FrameBased )
		{
			if ( it->fbegin > it->fend )
			{
				std::cout << "Sub no " << numero << " frame debut (" << it->fbegin << ") > frame fin (" << it->fend << ")" << std::endl;
			}
			else if ( it->fbegin < prevfe )
			{
				std::cout << "Sub no " << numero << " frame debut (" << it->fbegin << ") < frame fin precedente (" << prevfe << ")" << std::endl;
			}
			str.sprintf( "Frame %d to %d", it->fbegin, it->fend );
			prevfe = it->fend;
		}
		else
		{
			if ( it->begin > it->end )
			{
				std::cout << "Sub no " << numero << ": " << it->begin.toString( TIME_FMT ) << " > " << it->end.toString( TIME_FMT ) << std::endl;
			}
			else if ( it->begin < prevte )
			{
				std::cout << "Sub no " << numero << ": " << it->begin.toString( TIME_FMT ) << " < " << prevte.toString( TIME_FMT ) << std::endl;
			}
			ts = it->begin;
			te = it->end;
			str = ts.toString( TIME_FMT ) + " to " + te.toString( TIME_FMT );
			prevte = it->end;
		}
		subs = it->subs;
		lbInputSubs->insertItem( str );
		lbInputSubs->insertStringList( subs );
		numero++;
	}
	tlInputSubs->show();
	lbInputSubs->show();
}

void WndSub::saveMicrodvd()
{
	std::vector<subtitle>::iterator it;
	QTextCodec *codec = QTextCodec::codecForName(cbOutputEncoding->currentText());

    QFile file( leOutputFile->text() );
    if ( file.open( IO_WriteOnly | IO_Truncate ) )
	{
        QTextStream stream( &file );
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			stream << "{" << it->fbegin << "}{" << it->fend << "}";

			QStringList::iterator its;
			for ( its = it->subs.begin(); its != it->subs.end(); its++ )
			{
				if ( its != it->subs.begin() ) stream << "|";
				stream << codec->fromUnicode( *its );
			}
			stream << endl;
		}
	}
}

void WndSub::saveSubrip()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<subtitle>::iterator it;
	QTextCodec *codec = QTextCodec::codecForName(cbOutputEncoding->currentText());

    QFile file( leOutputFile->text() );

    if ( file.open( IO_WriteOnly | IO_Truncate ) )
	{
        QTextStream stream( &file );
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			ts = it->begin;
			te = it->end;
			stream << ts.toString( "hh:mm:ss" ) << "." << ts.msec() << "," << te.toString( "hh:mm:ss" ) << "." << te.msec() << endl;

			QStringList::iterator its;
			for ( its = it->subs.begin(); its != it->subs.end(); its++ )
			{
				if ( its != it->subs.begin() ) stream << "[br]";
				stream << codec->fromUnicode( *its );
			}
			stream << endl << endl;
		}
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}

void WndSub::saveSami()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<subtitle>::iterator it;
	int numero;
	QTextCodec *codec = QTextCodec::codecForName(cbOutputEncoding->currentText());

    QFile file( leOutputFile->text() );

    if ( file.open( IO_WriteOnly | IO_Truncate ) )
	{
        QTextStream stream( &file );
		stream << "<SAMI>" << endl;
		stream << "<HEAD>" << endl;
		stream << "   <STYLE TYPE=\"Text/css\">" << endl;
		stream << "   <!--" << endl;
		stream << "      P {margin-left: 29pt; margin-right: 29pt; font-size: 24pt; text-align: center; font-family: Tahoma; font-weight: bold; color: #FFFFFF; background-color: #000000;}" << endl;
		stream << "      .SUBTTL {Name: 'Subtitles'; Lang: en-US; SAMIType: CC;}" << endl;
		stream << "   -->" << endl;
		stream << "   </STYLE>" << endl;
		stream << "</HEAD>" << endl;
		stream << "<BODY>" << endl;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			ts = it->begin;
			te = it->end;
			stream << "<SYNC START=" << milli( ts ) << ">" << endl;

			QStringList::iterator its = it->subs.begin();
			stream << "<P CLASS=SUBTTL>" << *its;
			its++;
			for ( ; its != it->subs.end(); its++ )
			{
				stream << "<br>" << codec->fromUnicode( *its );
			}
			stream << endl;
			stream << "<SYNC START=" << milli( te ) << ">" << endl;
			stream << "	<P CLASS=SUBTTL>&nbsp;" << endl;
			numero++;
		}
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}
void WndSub::saveSubviewer()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<subtitle>::iterator it;
	int numero;
	QTextCodec *codec = QTextCodec::codecForName(cbOutputEncoding->currentText());

    QFile file( leOutputFile->text() );

    if ( file.open( IO_WriteOnly | IO_Truncate ) )
	{
        QTextStream stream( &file );
		numero = 1;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			ts = it->begin;
			te = it->end;
			stream << numero << endl;
			stream << ts.toString( TIME_FMT ) << " --> " << te.toString( TIME_FMT ) << endl;

			QStringList::iterator its;
			for ( its = it->subs.begin(); its != it->subs.end(); its++ )
			{
				stream << codec->fromUnicode( *its ) << endl;
			}
			stream << endl;
			numero++;
		}
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}

void WndSub::saveSpumux()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<subtitle>::iterator it;
	QTextCodec *codec = QTextCodec::codecForName(cbOutputEncoding->currentText());

    QFile file( leOutputFile->text() );

    if ( file.open( IO_WriteOnly | IO_Truncate ) )
	{
        QTextStream stream( &file );
		stream << "<subpictures>" << endl;
		stream << "  <stream>" << endl;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			ts = it->begin;
			te = it->end;
			stream << "    <spu";
			stream << " start=\"" << ts.toString( SPU_TIME_FMT ) << "\"";;
			stream << " end=\"" << te.toString( SPU_TIME_FMT ) << "\"";;

			QStringList::iterator its;
			for ( its = it->subs.begin(); its != it->subs.end(); its++ )
			{
				if ( its != it->subs.begin() ) stream << endl;
				stream << codec->fromUnicode( *its );
			}
			stream << " </spu>" << endl;
		}
		stream << "  </stream>" << endl;
		stream << "</subpictures>" << endl;
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}

void WndSub::loadSami()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QStringList subs;
	QRegExp sync( "SYNC START=" );
	QRegExp br( "<br>" );
	QRegExp spaces( "&nbsp;" );
	QTextCodec *codec = QTextCodec::codecForName(cbInputEncoding->currentText());

    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
            line = codec->toUnicode( stream.readLine()); // line of text excluding '\n'
			if ( line.contains( sync ) )
			{
				Start = line.section( '=', 1, 1 ).section( '>', 0, 0 );

				//std::cout << "start=" << Start.toInt() <  " ms";
				ts = timeFromMilli( Start.toInt() );
				subs.clear();

				line = codec->toUnicode( stream.readLine() );
				QString str = line.section( 'CLASS=SUBTTL>', 1 );
				if ( str.contains( spaces ) ) continue;
				// handle <br>
				QStringList l = QStringList::split( br, str );
				QStringList::Iterator i;
				for ( i = l.begin(); i != l.end(); i++ )
				{
					//std::cout << *i << std::endl;
					subs.push_back( *i );
				}
				
				// get end
				line = codec->toUnicode( stream.readLine() );
				if ( line.contains( sync ) )
				{
					End = line.section( '=', 1, 1 ).section( '>', 0, 0 );
					te = timeFromMilli( End.toInt() );
				}
				
				//std::cout << std::endl;
				subtitle s( ts, te, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}

void WndSub::loadSubviewer()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QStringList subs;
	QTextCodec *codec = QTextCodec::codecForName(cbInputEncoding->currentText());

    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
            line = codec->toUnicode( stream.readLine() ); // line of text excluding '\n'
			if ( line.contains( subviewer ) )
			{
				Start = line.section( ' ', 0, 0 );
				End = line.section( ' ', 2, 2 );
				ts = QTime::fromString( Start );
				te = QTime::fromString( End );
				subs.clear();
				while ( !stream.atEnd() )
				{
					line = codec->toUnicode( stream.readLine() );
					if ( line.isEmpty() )
						break;
					/* text */
					subs.push_back( line );
				}
				subtitle s( ts, te, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}

void WndSub::loadMicrodvd()
{
	QString Start;
	QString End;
	QString Text;
	QStringList subs;
	int start, end;
	QTextCodec *codec = QTextCodec::codecForName(cbInputEncoding->currentText());

    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
            line = codec->toUnicode( stream.readLine() ); // line of text excluding '\n'
			if ( line.contains( microdvd ) )
			{
				/* {xxxx}{yyyy} text 1 | text 2 | ... */
				Start = line.section( '{', 1, 1 );
				Start = Start.section( '}', 0, 1 );
				End = line.section( '{', 2 );
				End = End.section( '}', 0, 0 );
				Text = line.section( '}', 2 );

				start = Start.toUInt( );
				end = End.toUInt( );
				subs.clear();
				subs = QStringList::split( "|", Text );
				subtitle s( start, end, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}

void WndSub::loadSubrip()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QStringList subs;
	QTextCodec *codec = QTextCodec::codecForName(cbInputEncoding->currentText());

    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
            line = codec->toUnicode( stream.readLine() ); // line of text excluding '\n'
			if ( line.contains( subrip ) )
			{
				Start = line.section( ',', 0, 0 );
				End = line.section( ',', 1, 1 );
				ts = QTime::fromString( Start );
				te = QTime::fromString( End );
				subs.clear();
				while ( !stream.atEnd() )
				{
					line = codec->toUnicode( stream.readLine() );
					if ( line.isEmpty() )
						break;
					/* text */
					subs = QStringList::split( "[br]", line );
				}
				subtitle s( ts, te, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}

void WndSub::loadSpumux()
{
	QString Start;
	QString End;
	QString Debut;
	QString Reste;
	QString Chaine;
	QTime ts;
	QTime te;
	QStringList subs;
	QTextCodec *codec = QTextCodec::codecForName(cbInputEncoding->currentText());

    QFile file( leInputFile->text() );

    if ( file.open( IO_ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
            line = codec->toUnicode( stream.readLine() ); // line of text excluding '\n'
			if ( line.contains( spumux ) )
			{
				Debut = line.section( "<spu ", 1 ).section( "start=\"", 0, 0 );
				Start = line.section( "start=\"", 1, 1 ).section( "\"", 0, 0 );
				End = line.section( "end=\"", 1, 1 ).section( "\"", 0, 0 );
				Reste = line.section( "end=\"", 1, 1 ).section( "\"", 1 );
				ts = timeFromMilli( milli( Start, '.' ) );
				te = timeFromMilli( milli( End, '.' ) );
				subs.clear();
				if ( Reste.contains( "</spu>" ) )
				{
					Chaine = Debut + Reste.section( "</spu>", 0, 0 );
					subs.push_back( Chaine );
				}
				else if ( Reste.contains( "/>" ) )
				{
					Chaine = Debut + Reste.section( "/>", 0, 0 );
					subs.push_back( Chaine );
				}
				else
				{
					subs.push_back( Reste );
					while ( !stream.atEnd() )
					{
						line = codec->toUnicode( stream.readLine() );
						if ( line.contains( "</spu>") )
						{
							QString text = line.section( "</spu>", 0, 0 );
							subs.push_back( text );
							break;
						}
						else
						{
							subs.push_back( line );
						}
					}
				}
				subtitle s( ts, te, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}
