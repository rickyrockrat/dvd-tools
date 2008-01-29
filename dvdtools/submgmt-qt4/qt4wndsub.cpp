#include <iostream>
#include <QPainter>
#include <QRadioButton>
#include <QPixmap>
#include <QTextStream>
#include <QMenu>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QProgressDialog>

#include "subtitle.h"
#include "qt4genpngwnd.h"
#include "qt4editsub.h"
#include "submodel.h"
#include "subitemdelegate.h"

#include "qt4wndsub.h"

#define TIME_FMT	"hh:mm:ss,zzz"
#define SPU_TIME_FMT	"hh:mm:ss.zzz"
#define NUM_LINES	30

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
QRegExp ssa = QRegExp( "Format: " );
QRegExp pjs = QRegExp( "[0-9]+,[0-9]+,\"." );
QRegExp mpsub = QRegExp( "FORMAT=[0-9]+" );
QRegExp mpsub2 = QRegExp( "FORMAT=TIM." );
QRegExp aqtitle = QRegExp( "-->>" );
QRegExp subrip9 = QRegExp( "\\[[0-9]+:[0-9]+:[0-9]+]" );
QRegExp spumux = QRegExp( "<spu " );

std::vector<QString> vHeader;

void qt4wndsub::filenameChanged()
{
	pbSave->setEnabled( true );
}

// Constructor
qt4wndsub::qt4wndsub( QWidget *parent ) : QWidget(parent)
{
	setupUi(this);
	cbType->insertItem( 0, "microdvd" );
	cbType->insertItem( 1, "mpl2" );
	cbType->insertItem( 2, "subrip" );
	cbType->insertItem( 3, "subviewer" );
	cbType->insertItem( 4, "subviewer2" );
	cbType->insertItem( 5, "sami" );
	cbType->insertItem( 6, "jacosub" );
	cbType->insertItem( 7, "jacosub2" );
	cbType->insertItem( 8, "vplayer" );
	cbType->insertItem( 9, "vplayer2" );
	cbType->insertItem( 10, "rt" );
	cbType->insertItem( 11, "ssa" );
	cbType->insertItem( 12, "pjs" );
	cbType->insertItem( 13, "mpsub" );
	cbType->insertItem( 14, "mpsub2" );
	cbType->insertItem( 15, "aqtitle" );
	cbType->insertItem( 16, "subrip9" );
	cbType->insertItem( 17, "spumux" );

	cbTypeO->insertItem( 0, "microdvd" );
	cbTypeO->insertItem( 1, "mpl2" );
	cbTypeO->insertItem( 2, "subrip" );
	cbTypeO->insertItem( 3, "subviewer" );
	cbTypeO->insertItem( 4, "subviewer2" );
	cbTypeO->insertItem( 5, "sami" );	/* 5 */
	cbTypeO->insertItem( 6, "jacosub" );
	cbTypeO->insertItem( 7, "jacosub2" );
	cbTypeO->insertItem( 8, "vplayer" );
	cbTypeO->insertItem( 9, "vplayer2" );
	cbTypeO->insertItem( 10, "rt" );
	cbTypeO->insertItem( 11, "ssa" );
	cbTypeO->insertItem( 12, "pjs" );
	cbTypeO->insertItem( 13, "mpsub" );
	cbTypeO->insertItem( 14, "mpsub2" );
	cbTypeO->insertItem( 15, "aqtitle" );
	cbTypeO->insertItem( 16, "subrip9" );
	cbTypeO->insertItem( 17, "spumux" );	/* 17 */

	comboFpsOrig->insertItem( 0, "23,970" );
	comboFpsOrig->insertItem( 1, "25" );
	comboFpsOrig->insertItem( 2, "29.976" );

	comboFpsDest->insertItem( 0, "23,970" );
	comboFpsDest->insertItem( 1, "25" );
	comboFpsDest->insertItem( 2, "29.976" );

	comboSaveFps->insertItem( 0, "23,970" );
	comboSaveFps->insertItem( 1, "25" );
	comboSaveFps->insertItem( 2, "29.976" );

	lvInputSubs->hide();
	lvOutputSubs->hide();
	tlInputSubs->hide();
	tlOutputSubs->hide();
	sbShift->setEnabled( false );
	sbExpand->setEnabled( false );
	comboFpsDest->setEnabled( false );
	tlFps->setEnabled( false );
	
	curItem = 0;

	connect( pbGenPng, SIGNAL(clicked()),
		this, SLOT(genPngForSpumux()));

	connect( cbSubst, SIGNAL(clicked()),
		this, SLOT(checkSubst()));

	connect( pbSelectSubstFile, SIGNAL(clicked()),
		this, SLOT(clicSelectSubstFile()));

	connect (cbTypeO, SIGNAL(activated(int)),
		this, SLOT(saveTypeCheck()));

	connect (pbNextWrongSub, SIGNAL(clicked()),
		this, SLOT(nextWrongSub()));

}

void qt4wndsub::subeditclosed(QWidget* ed,QAbstractItemDelegate::EndEditHint h)
{
	std::cout << "edit finish" << std::endl;
}

void qt4wndsub::saveTypeCheck( )
{
	int i = cbTypeO->currentIndex();
	if ( i == 0 )
	{	// frame based format
		if ( ( subvec.size() > 0 )
		&& ( !subvec[0]->getFrameBased() ) )
		{
			tlDestFps->setEnabled( true );
			comboSaveFps->setEnabled( true );
			return;
		}
	}
	else
	{	// time based formats 
		if ( ( subvec.size() > 0 )
		&& ( subvec[0]->getFrameBased() ) )
		{
			tlDestFps->setEnabled( true );
			comboSaveFps->setEnabled( true );
			return;
		}
	}
	tlDestFps->setEnabled( false );
	comboSaveFps->setEnabled( false );
	return;
}

void qt4wndsub::clickedSave()
{
	switch( cbTypeO->currentIndex() )
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
	case 11 : 	/* ssa */
		saveSsa();
		break;
	case 17 : 	/* spumux */
		saveSpumux();
		break;
	default :
		QMessageBox::information( this, "submgmt", "Cannot save file : " + cbTypeO->currentText() );
	}
}

void qt4wndsub::clicSelectInputFile()
{
    QString s = QFileDialog::getOpenFileName(this, tr("Select File"),
                            "",
                            tr("All files (*)"));
    if ( s != NULL )
    {
		leInputFile->setText( s );
    }
	autoDetectFormat();
	loadSubFile( leInputFile->text() );
	showInputSubs();
}


void qt4wndsub::clicSelectSubstFile()
{
    QString s = QFileDialog::getOpenFileName(this, tr("Select Subst File"),
                            "",
                            tr("All files (*)"));
    if ( s != NULL )
    {
		leSubstFile->setText( s );
    }
}

void qt4wndsub::clicSelectOutputFile()
{
    QString s = QFileDialog:: getSaveFileName(this, tr("Save File"),
                            "",
                            tr("Images (*.png *.xpm *.jpg)"));
    if ( s != NULL )
    {
	leOutputFile->setText( s );
    }
}

void qt4wndsub::checkSubst()
{
	if ( cbSubst->isChecked() )
	{
		leSubstFile->setEnabled( true );
		pbSelectSubstFile->setEnabled( true );
	}
	else
	{
		leSubstFile->setEnabled( false );
		pbSelectSubstFile->setEnabled( false );
	}
}

void qt4wndsub::checkFps()
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

void qt4wndsub::checkExpand()
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

void qt4wndsub::checkShift()
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

void qt4wndsub::checkShiftPos()
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

void qt4wndsub::checkAuto()
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

void qt4wndsub::timeBasedProceed( Subtitle *s )
{
	static int passage;
	QTime length, lengths;
	QString str;
	QTime ts = s->getBegin();
	QTime te = s->getEnd();

	if ( cbShift->isChecked() )
	{
		ts = ts.addMSecs( sbShift->value() );
		te = te.addMSecs( sbShift->value() );
	}
	if ( cbExpand->isChecked() )
	{
		ts = s->getBegin() * ( sbExpand->value() / 1000 );
		te = s->getEnd() * ( sbExpand->value() / 1000 );
	}
	if ( cbFps->isChecked() )
	{
		ts = s->getBegin() * ( comboFpsOrig->currentText().toDouble() / comboFpsDest->currentText().toDouble() );
		te = s->getEnd() * ( comboFpsOrig->currentText().toDouble() / comboFpsDest->currentText().toDouble() );
	}
	if ( cbAuto->isChecked() )
	{
		/* mode auto */
		/* goal is to align first sub with first speaker and
		 * last sub with last speaker
		 *
		 * first we move subs to 0 */

		ts = s->getBegin() - timeFirstSub->time();
		te = s->getEnd() - timeFirstSub->time();

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
	s->setBegin( ts );
	s->setEnd( te );
	if ( cbShiftPos->isChecked() )
	{
		if ( sbShiftPos->value() > 0 )
		{
			v_subs.push_back( s );
			s->getSubs().clear();		// first x subtitles are empty
			if ( v_subs.size() >= (sbShiftPos->value()+1) )
			{
				int i = v_subs.size() - sbShiftPos->value() - 1;
				s->setSubs( v_subs[ i ]->getSubs() );
			}
		}
		else if ( sbShiftPos->value() < 0 )
		{
			v_subs.push_back( s );
			s->setBegin( QTime() );	// null time
			s->setEnd( QTime() );
			if ( v_subs.size() >= (abs(sbShiftPos->value())+1) )
			{
				int i = v_subs.size() - abs(sbShiftPos->value()) - 1;
				s->setBegin( v_subs[ i ]->getBegin() );
				s->setEnd( v_subs[ i ]->getEnd() );
			}

		}
	}
}

void qt4wndsub::frameBasedProceed( Subtitle *s )
{
	QString str;
	int length, lengths;
	int fs = s->getFbegin();
	int fe = s->getFend();
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
		fs = s->getFbegin() * sbExpand->value() / 1000;
		fe = s->getFend() * sbExpand->value() / 1000;
	}
	if ( cbFps->isChecked() )
	{
		fs = int( s->getFbegin() * ( fps / comboFpsDest->currentText().toDouble() ) );
		fe = int( s->getFend() * ( fps / comboFpsDest->currentText().toDouble() ) );
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

		fs = s->getFbegin() - fFirstSub;
		fe = s->getFend() - fFirstSub;

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
	s->setFbegin( fs );
	s->setFend( fe );
	if ( cbShiftPos->isChecked() )
	{
		if ( sbShiftPos->value() > 0 )
		{
			v_subs.push_back( s );
			s->getSubs().clear();		// first x subtitles are empty
			if ( v_subs.size() == (sbShiftPos->value()+1) )
			{
				int i = v_subs.size() - sbShiftPos->value() - 1;
				s->setSubs( v_subs[ i ]->getSubs() );
			}
		}
		else if ( sbShiftPos->value() < 0 )
		{
			v_subs.push_back( s );
			s->setFbegin( 0 );
			s->setFend( 0 );
			if ( v_subs.size() >= (abs(sbShiftPos->value())+1) )
			{
				int i = v_subs.size() - abs(sbShiftPos->value()) - 1;
				s->setFbegin( v_subs[ i ]->getFbegin() );
				s->setFend( v_subs[ i ]->getFend() );
			}
		}
	}
}

void qt4wndsub::clicProceed()
{
	std::vector<Subtitle *>::iterator it;

	v_subs.clear();
	if ( cbSubst->isChecked() )
	{
		std::vector<Subtitle *> vorig = subvec;
		loadSubFile( leSubstFile->text());
		if ( subvec.size() == vorig.size() )
		{
			int ctr;
			for ( ctr = 0, it = vorig.begin(); it != vorig.end(); it++ )
			{
				(*it)->setSubs( subvec[ctr++]->getSubs() );
			}
			subvec = vorig;
		}
		else
		{
			QMessageBox::information(this, "Submgmt", "Cannot substitute because the number of subtitles differs" );
		}
	}
	else
	{
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			if ( (*it)->getFrameBased() )
				frameBasedProceed( *it );
			else
				timeBasedProceed( *it );
		}
	}
	showOutputSubs();
}
	
void qt4wndsub::autoDetectFormat()
{
    QFile file( leInputFile->text() );

    if ( file.open( QIODevice::ReadOnly ) )
	{
        QTextStream stream( &file );
        QString line;
        int i = 0;
        while ( !stream.atEnd() && ( i < NUM_LINES ) )
		{
			i++;
            line = stream.readLine(); // line of text excluding '\n'
            if ( line.contains( microdvd ) ) {
				cbType->setCurrentIndex( 0 );
				i = NUM_LINES + 1;
			} else if ( line.contains( mpl2 ) ) {
				cbType->setCurrentIndex( 1 );
				i = NUM_LINES + 2;
			} else if ( line.contains( subrip ) ) {
				cbType->setCurrentIndex( 2 );
				i = NUM_LINES + 3;
			} else if ( line.contains( subviewer ) ) {
				cbType->setCurrentIndex( 3 );
				i = NUM_LINES + 4;
			} else if ( line.contains( subviewer2 ) ) {
				cbType->setCurrentIndex( 4 );
				i = NUM_LINES + 5;
			} else if ( line.contains( sami ) ) {
				cbType->setCurrentIndex( 5 );
				i = NUM_LINES + 6;
			} else if ( line.contains( jacosub ) ) {
				cbType->setCurrentIndex( 6 );
				i = NUM_LINES + 7;
			} else if ( line.contains( jacosub2 ) ) {
				cbType->setCurrentIndex( 7 );
				i = NUM_LINES + 8;
			} else if ( line.contains( spumux ) ) {
				cbType->setCurrentIndex( 17 );
				i = NUM_LINES + 18;
			} else if ( line.contains( vplayer ) ) {
				cbType->setCurrentIndex( 8 );
				i = NUM_LINES + 9;
			} else if ( line.contains( vplayer2 ) ) {
				cbType->setCurrentIndex( 9 );
				i = NUM_LINES +10;
				/* the rt regexp is too weak and
				 * conflicts with the spu one...
			} else if ( line.contains( rt ) ) {
				statusBar()->message( "rt!!" );
				cbType->setCurrentIndex( 10);
				i = NUM_LINES + 11;
				*/
			} else if ( line.contains( ssa ) ) {
				cbType->setCurrentIndex( 11 );
				i = NUM_LINES + 12;
			} else if ( line.contains( pjs ) ) {
				cbType->setCurrentIndex( 12 );
				i = NUM_LINES + 13;
			} else if ( line.contains( mpsub ) ) {
				cbType->setCurrentIndex( 13 );
				i = NUM_LINES + 14;
			} else if ( line.contains( mpsub2 ) ) {
				cbType->setCurrentIndex( 14 );
				i = NUM_LINES + 15;
			} else if ( line.contains( aqtitle ) ) {
				cbType->setCurrentIndex( 15 );
				i = NUM_LINES + 16;
			} else if ( line.contains( subrip9 ) ) {
				cbType->setCurrentIndex( 16 );
				i = NUM_LINES + 17;
			}
        }
        file.close();
    }
	cbTypeO->setCurrentIndex( cbType->currentIndex() );
}

void qt4wndsub::loadSubFile(QString fname)
{
	switch( cbType->currentIndex() )
	{
	case 0 :	/* microdvd */
		loadMicrodvd(fname);
		break;
	case 2 : 	/* subrip */
		loadSubrip(fname);
		break;
	case 3 : 	/* subviewer */
		loadSubviewer(fname);
		break;
	case 5 :	/* sami */
		loadSami(fname);
		break;
	case 11 :	/* ssa */
		loadSsa(fname);
		break;
	case 17 : 	/* spumux */
		loadSpumux(fname);
		break;
	default :
		QMessageBox::information( this, "submgmt", "Cannot load file" );
	}
}

void qt4wndsub::showOutputSubs()
{
	std::vector<Subtitle *>::iterator it;
	QTime ts;
	QTime te;
	QString str;

	SubModel *outputSubsModel = new SubModel(subvec);
	lvOutputSubs->setModel( outputSubsModel );

	SubItemDelegate *delO = new SubItemDelegate( lvOutputSubs );
	lvOutputSubs->setItemDelegate( delO );
	tlOutputSubs->show();
	lvOutputSubs->show();
}

void qt4wndsub::showInputSubs()
{
	std::vector<Subtitle *>::iterator it;
	QTime ts;
	QTime te;
	QTime prevte( 0, 0, 0, 0 );
	int numero = 0, prevfe = 999999;

	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		if ( (*it)->getFrameBased() )
		{
			if ( (*it)->getFbegin() > (*it)->getFend() )
			{
				std::cout << "Sub no " << numero << " frame debut (" << (*it)->getFbegin() << ") > frame fin (" << (*it)->getFend() << ")" << std::endl;
				(*it)->setProblem( true );
			}
			else if ( (*it)->getFbegin() < prevfe )
			{
				std::cout << "Sub no " << numero << " frame debut (" << (*it)->getFbegin() << ") < frame fin precedente (" << prevfe << ")" << std::endl;
				(*it)->setProblem( true );
			}
			prevfe = (*it)->getFend();
		}
		else
		{
			if ( (*it)->getBegin() > (*it)->getEnd() )
			{
				QString s1, s2;
				s1 = (*it)->getBegin().toString( TIME_FMT );
				s2 = (*it)->getEnd().toString( TIME_FMT );
				QByteArray a1 = s1.toAscii();
				QByteArray a2 = s2.toAscii();
				std::cout << "Sub no " << numero << ": " << a1.constData() << " > " << a2.constData() << std::endl;
				(*it)->setProblem( true );
			}
			else if ( (*it)->getBegin() < prevte )
			{
				QString s1, s2;
				s1 = (*it)->getBegin().toString( TIME_FMT );
				s2 = prevte.toString( TIME_FMT );
				QByteArray a1 = s1.toAscii();
				QByteArray a2 = s2.toAscii();
				std::cout << "Sub no " << numero << ": " << a1.constData() << " < " << a2.constData() << std::endl;
				(*it)->setProblem( true );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			prevte = (*it)->getEnd();
		}
		numero++;
	}
	SubModel *inputSubsModel = new SubModel( subvec );
	lvInputSubs->setModel( inputSubsModel );
	SubItemDelegate *delI = new SubItemDelegate( lvInputSubs );
	connect( delI,
	SIGNAL(closeEditor( QWidget *, QAbstractItemDelegate::EndEditHint)),
	this, SLOT(subeditclosed(QWidget*,QAbstractItemDelegate::EndEditHint))
	);
	lvInputSubs->setItemDelegate( delI );
	tlInputSubs->show();
	lvInputSubs->show();
}

void qt4wndsub::saveMicrodvd()
{
	if ( hasFormat() )
	{
		if ( QMessageBox::question( this, "Format",
				"Advanced formats (bold, italics, ...) cannot be saved in this"
				" kind of file. Do you wish to continue anyway ?",
				QMessageBox::Yes,
				QMessageBox::No ) 
				== QMessageBox::No )
		{
			return;
		}
	}
	std::vector<Subtitle *>::iterator it;

    QFile file( leOutputFile->text() );
    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			Subtitle *st = *it;
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the times in frame using the framerate
				int fr = ((double)milli( st->getBegin() ) * fDest ) / 1000.0;
				st->setFbegin( fr );
				fr = ((double)milli( st->getEnd() ) * fDest ) / 1000.0;
				st->setFend( fr );
			}
			stream << "{" << st->getFbegin() << "}{" << st->getFend() << "}";

			QVector<Subline>::iterator its;
			for ( its = st->getSubs().begin(); its != st->getSubs().end(); its++ )
			{
				if ( its != st->getSubs().begin() ) stream << "|";
					stream << its->getLine();
			}
			stream << endl;
		}
	}
}

void qt4wndsub::saveSubrip()
{
	if ( hasFormat() )
	{
		if ( QMessageBox::question( this, "Format",
				"Advanced formats (bold, italics, ...) cannot be saved in this"
				" kind of file. Do you wish to continue anyway ?",
				QMessageBox::Yes,
				QMessageBox::No ) 
				== QMessageBox::No )
		{
			return;
		}
	}
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<Subtitle *>::iterator it;

    QFile file( leOutputFile->text() );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the frames in time using the framerate
				int fr = ((double)milli( (*it)->getBegin() ) * fDest ) / 1000.0;
				QTime ts = timeFromMilli( (*it)->getFbegin() * 1000.0 / fDest ); 
				(*it)->setBegin( ts );
				QTime te = timeFromMilli( (*it)->getFend() * 1000.0 / fDest ); 
				(*it)->setEnd( te );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			stream << ts.toString( "hh:mm:ss" ) << "." << ts.msec() << "," << te.toString( "hh:mm:ss" ) << "." << te.msec() << endl;

			QVector<Subline>::iterator its;
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				if ( its != (*it)->getSubs().begin() ) stream << "[br]";
				stream << its->getLine();
			}
			stream << endl << endl;
		}
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}

void qt4wndsub::saveSami()
{
	if ( hasFormat() )
	{
		if ( QMessageBox::question( this, "Format",
				"Advanced formats (bold, italics, ...) cannot be saved in this"
				" kind of file. Do you wish to continue anyway ?",
				QMessageBox::Yes,
				QMessageBox::No ) 
				== QMessageBox::No )
		{
			return;
		}
	}
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<Subtitle *>::iterator it;
	int numero;

    QFile file( leOutputFile->text() );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
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
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the frames in time using the framerate
				int fr = ((double)milli( (*it)->getBegin() ) * fDest ) / 1000.0;
				QTime ts = timeFromMilli( (*it)->getFbegin() * 1000.0 / fDest ); 
				(*it)->setBegin( ts );
				QTime te = timeFromMilli( (*it)->getFend() * 1000.0 / fDest ); 
				(*it)->setEnd( te );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			stream << "<SYNC START=" << milli( ts ) << ">" << endl;

			QVector<Subline>::iterator its = (*it)->getSubs().begin();
			stream << "<P CLASS=SUBTTL>" << *its;
			its++;
			for ( ; its != (*it)->getSubs().end(); its++ )
			{
				stream << "<br>" << its->getLine();
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
void qt4wndsub::saveSubviewer()
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<Subtitle *>::iterator it;
	int numero;

    QFile file( leOutputFile->text() );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		numero = 1;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the frames in time using the framerate
				int fr = ((double)milli( (*it)->getBegin() ) * fDest ) / 1000.0;
				QTime ts = timeFromMilli( (*it)->getFbegin() * 1000.0 / fDest ); 
				(*it)->setBegin( ts );
				QTime te = timeFromMilli( (*it)->getFend() * 1000.0 / fDest ); 
				(*it)->setEnd( te );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			stream << numero << endl;
			stream << ts.toString( TIME_FMT ) << " --> " << te.toString( TIME_FMT ) << endl;

			QVector<Subline>::const_iterator its;
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				Subline l = *its;
				if ( l.getFmt( ) == Subline::Bold )
				{
					stream << "<b>";
				}
				if ( l.getFmt( ) == Subline::Italic )
				{
					stream << "<i>";
				}
				if ( l.getFmt( ) == Subline::Underline )
				{
					stream << "<u>";
				}
				if ( l.getFmt( ) == Subline::Stroke )
				{
					stream << "<s>";		// what is the tag for stroke ?
				}
				if ( l.getLine().length() )
				{
					stream << l.getLine();
				}
				if ( l.getFmt( ) == Subline::Stroke )
				{
					stream << "</s>";		// what is the tag for stroke ?
				}
				if ( l.getFmt( ) == Subline::Underline )
				{
					stream << "</u>";
				}
				if ( l.getFmt( ) == Subline::Italic )
				{
					stream << "</i>";
				}
				if ( l.getFmt( ) == Subline::Bold )
				{
					stream << "</b>";
				}
				stream << endl;
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

void qt4wndsub::saveSpumux()
{
	if ( hasFormat() )
	{
		if ( QMessageBox::question( this, "Format",
				"Advanced formats (bold, italics, ...) cannot be saved in this"
				" kind of file.\nYou should consider the generate png option"
				"to preserve the format. Do you wish to continue anyway ?",
				QMessageBox::Yes,
				QMessageBox::No ) 
				== QMessageBox::No )
		{
			return;
		}
	}
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<Subtitle *>::iterator it;

    QFile file( leOutputFile->text() );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		stream << "<subpictures>" << endl;
		stream << "  <stream>" << endl;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the frames in time using the framerate
				int fr = ((double)milli( (*it)->getBegin() ) * fDest ) / 1000.0;
				QTime ts = timeFromMilli( (*it)->getFbegin() * 1000.0 / fDest ); 
				(*it)->setBegin( ts );
				QTime te = timeFromMilli( (*it)->getFend() * 1000.0 / fDest ); 
				(*it)->setEnd( te );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			stream << "    <spu";
			stream << " start=\"" << ts.toString( SPU_TIME_FMT ) << "\"";
			stream << " end=\"" << te.toString( SPU_TIME_FMT ) << "\" ";

			QVector<Subline>::iterator its;
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				if ( its != (*it)->getSubs().begin() ) stream << endl;
				stream << " image=\"" << its->getLine() << "\"";
			}
			stream << " />" << endl;
		}
		stream << "  </stream>" << endl;
		stream << "</subpictures>" << endl;
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
}

void qt4wndsub::saveSsa()
{
	QTime ts;
	QTime te;
    QFile file( leOutputFile->text() );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		if ( vHeader.size() > 0 )
		{
			std::vector<QString>::iterator sit;
			for ( sit = vHeader.begin(); sit != vHeader.end(); sit++ )
			{
				stream << *sit << endl;
			}
		}
		else
		{
			stream << "[Script Info]" << endl;
			stream << "; Script generated by submgmt v0.4 " << endl;
			stream << "Title: Default submgmt header" << endl;
			stream << "ScriptType: v4.00+" << endl;
			stream << "PlayResX: 0" << endl;
			stream << "PlayResY: 0" << endl;
			stream << "WrapStyle: 0" << endl;
			stream << "Last Style Storage: Default" << endl;
			stream << "Video File: none" << endl;
			stream << "Video Aspect Ratio: 0" << endl;
			stream << "Video Zoom: 8" << endl;
			stream << "Video Position: 178218" << endl;
			stream << "" << endl;
			stream << "[V4+ Styles]" << endl;
			stream << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding" << endl;
			stream << "Style: Default,Trebuchet MS,22,&H00888714,&H0000FFFF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1" << endl;
			stream << "" << endl;
			stream << "[Events]" << endl;
		}
		stream << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text" << endl;
		std::vector<Subtitle *>::iterator it;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			if ( comboSaveFps->isEnabled( ) )
			{
				double fDest = comboSaveFps->currentText().toDouble();
				// convert the frames in time using the framerate
				int fr = ((double)milli( (*it)->getBegin() ) * fDest ) / 1000.0;
				QTime ts = timeFromMilli( (*it)->getFbegin() * 1000.0 / fDest ); 
				(*it)->setBegin( ts );
				QTime te = timeFromMilli( (*it)->getFend() * 1000.0 / fDest ); 
				(*it)->setEnd( te );
			}
			ts = (*it)->getBegin();
			te = (*it)->getEnd();

			stream << "Dialogue: 0";
			QByteArray a1 = ts.toString( SPU_TIME_FMT).toAscii();
			QByteArray a2 = te.toString( SPU_TIME_FMT).toAscii();
			stream << a1.constData();
			stream << ",";
			stream << a2.constData();
			stream << ",Default,,0000,0000,0000,,";
			QVector<Subline>::iterator its;
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				if ( its->getFmt() == Subline::Bold )
				{
					stream << "{\\b1}";
				}
				if ( its->getFmt() == Subline::Italic )
				{
					stream << "{\\i1}";
				}
				// what are the other tags ?
				stream << its->getLine();
				if ( its->getFmt() == Subline::Bold )
				{
					stream << "{\\b0}";
				}
				if ( its->getFmt() == Subline::Italic )
				{
					stream << "{\\i0}";
				}
			}
			stream << endl;
		}
	}
}

void qt4wndsub::loadSsa(QString fname)
{
	QString Start;
	QString End;
	QString Text;
	QTime ts;
	QTime te;
	QVector<Subline> subs;
	QRegExp dial( "Dialogue:" );
	int hh, mm, ss, cc;

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
		vHeader.clear();
        while ( !stream.atEnd() )
		{
			line = stream.readLine(); // line of text excluding '\n'
			if ( line.contains( QRegExp( "^Format:" ) ) )
			{
/* Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text */
				// do nothing for we have our own format when we save the file
			}
			else if ( line.contains( dial ) )
			{
/* Dialogue: 0,0:00:41.80,0:00:43.19,Default,,0000,0000,0000,,{\i1} I am sending you the code. {\i0} */
				Start = line.section( ':', 1 ).section( ',', 1, 1 );
				hh = Start.section( ":", 0, 0 ).toInt();
				mm = Start.section( ":", 1, 1 ).toInt();
				ss = Start.section( ":", 2 ).section( ".", 0, 0 ).toInt();
				cc = Start.section( ".", 1 ).toInt();
				ts = QTime( hh, mm, ss, cc );

				End = line.section( ':', 1 ).section( ',', 2, 2 );
				hh = End.section( ":", 0, 0 ).toInt();
				mm = End.section( ":", 1, 1 ).toInt();
				ss = End.section( ":", 2 ).section( ".", 0, 0 ).toInt();
				cc = End.section( ".", 1 ).toInt();
				te = QTime( hh, mm, ss, cc );

				Text = line.section( ':', 1 ).section( ',', 9 );
				
				subs.clear();

				Subline::FMT style;
				QString sText;
				if ( Text.contains( "{\\i1}" ) )
				{
					sText = Text.remove( "{\\i1} " ).remove( " {\\i0}" );
					style = Subline::Italic;
				}
				else if ( Text.contains( "{\\b1}" ) )
				{
					sText = Text.remove( "{\\b1} " ).remove( " {\\b0}" );
					style = Subline::Bold;
				}
				else
				{
					sText = Text;
					style = Subline::Normal;
				}
				// We ignore all other tags
				sText = sText.remove( QRegExp( "{.*}" ) );
				QStringList l = sText.split( "\\N" );
				QStringList::Iterator i;
				for ( i = l.begin(); i != l.end(); i++ )
				{
					QString *newS = &QString( *i );
					Subline *sul = new Subline( *newS, style );
					subs.push_back( *sul );
				}
				
				Subtitle *sut = new Subtitle( ts, te, subs );
				subvec.push_back( sut );
			}
			else
			{
				vHeader.push_back( line );
			}
		}
        file.close();
    }
}

void qt4wndsub::loadSami(QString fname)
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QVector<Subline> subs;
	QRegExp sync( "SYNC START=" );
	QRegExp br( "<br>" );
	QRegExp spaces( "&nbsp;" );

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
        while ( !stream.atEnd() )
		{
            line = stream.readLine(); // line of text excluding '\n'
			if ( line.contains( sync ) )
			{
				Start = line.section( '=', 1, 1 ).section( '>', 0, 0 );

				//std::cout << "start=" << Start.toInt() <  " ms";
				ts = timeFromMilli( Start.toInt() );
				subs.clear();

				line = stream.readLine( );
				QString str = line.section( 'CLASS=SUBTTL>', 1 );
				if ( str.contains( spaces ) ) continue;
				// handle <br>
				QStringList l = str.split( br );
				QStringList::Iterator i;
				for ( i = l.begin(); i != l.end(); i++ )
				{
					//std::cout << *i << std::endl;
					Subline *sl = new Subline( *i );
					subs.push_back( *sl );
				}
				
				// get end
				line = stream.readLine();
				if ( line.contains( sync ) )
				{
					End = line.section( '=', 1, 1 ).section( '>', 0, 0 );
					te = timeFromMilli( End.toInt() );
				}
				
				//std::cout << std::endl;
				Subtitle *s = new Subtitle( ts, te, subs );
				subvec.push_back( s );
			}
		}
        file.close();
    }
}

void qt4wndsub::loadSubviewer(QString fname)
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QVector<Subline> subs;

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
        while ( !stream.atEnd() )
		{
            line = stream.readLine(); // line of text excluding '\n'
			if ( line.contains( subviewer ) )
			{
				Start = line.section( ' ', 0, 0 );
				End = line.section( ' ', 2, 2 );
				ts = QTime::fromString( Start );
				te = QTime::fromString( End );
				subs.clear();
				while ( !stream.atEnd() )
				{
					line = stream.readLine();
					if ( line.isEmpty() )
						break;
					/* text */
					QString *s = new QString();
					Subline::FMT style;
					if ( line.contains( "<i>" ) )
					{
						style = Subline::Italic;
						*s = line.remove( QRegExp( "<i>" ) ).remove( QRegExp( "</i>") );
					}
					else if ( line.contains( "<b>" ) )
					{
						style = Subline::Bold;
						*s = line.remove( QRegExp( "<b>" ) ).remove( QRegExp( "</b>" ) );
					}
					else
					{
						style = Subline::Normal;
						*s = line;
					}
					Subline *sl = new Subline( *s, style );
					subs.push_back( *sl );
				}
				Subtitle *st = new Subtitle( ts, te, subs );
				subvec.push_back( st );
			}
		}
        file.close();
    }
}

void qt4wndsub::loadMicrodvd(QString fname)
{
	QString Start;
	QString End;
	QString Text;
	QVector<Subline> subs;
	int start, end;

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
        while ( !stream.atEnd() )
		{
            line = stream.readLine( ); // line of text excluding '\n'
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
				QStringList sl = Text.split( "|" );
				QStringList::Iterator it = sl.begin();
				for ( ; it != sl.end(); it++ )
				{
					Subline *sl = new Subline( *it );
					subs.push_back( *sl );
				}
				Subtitle *st = new Subtitle( start, end, subs );
				subvec.push_back( st );
			}
		}
        file.close();
    }
}

void qt4wndsub::loadSubrip(QString fname)
{
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QVector<Subline> subs;

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
        while ( !stream.atEnd() )
		{
            line = stream.readLine( ); // line of text excluding '\n'
			if ( line.contains( subrip ) )
			{
				Start = line.section( ',', 0, 0 );
				End = line.section( ',', 1, 1 );
				ts = QTime::fromString( Start );
				te = QTime::fromString( End );
				subs.clear();
				while ( !stream.atEnd() )
				{
					line = stream.readLine();
					if ( line.isEmpty() )
						break;
					/* text */
					QStringList sl = line.split( "[br]" );
					QStringList::Iterator it = sl.begin();
					for ( ; it != sl.end(); it++ )
					{
						Subline *sl = new Subline( *it );
						subs.push_back( *sl );
					}
				}
				Subtitle *st = new Subtitle( ts, te, subs );
				subvec.push_back( st );
			}
		}
        file.close();
    }
}

void qt4wndsub::loadSpumux(QString fname)
{
	QString Start;
	QString End;
	QString Debut;
	QString Reste;
	QString Chaine;
	QTime ts;
	QTime te;
	QVector<Subline> subs;

    QFile file( fname );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
		if ( cbInputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
        QString line;
        while ( !stream.atEnd() )
		{
            line = stream.readLine( ); // line of text excluding '\n'
			if ( line.contains( spumux ) )
			{
				Debut = line.section( "<spu ", 1 ).section( "start=\"", 0, 0 );
				Start = line.section( "start=\"", 1, 1 ).section( "\"", 0, 0 );
				End = line.section( "end=\"", 1, 1 ).section( "\"", 0, 0 );
				Reste = line.section( "end=\"", 1, 1 ).section( "\"", 1 );
				ts = timeFromMilli( milli( Start, '.' ) );
				te = timeFromMilli( milli( End, '.' ) );
				subs.clear();
				Subline *sl;
				if ( Reste.contains( "</spu>" ) )
				{
					Chaine = Debut + Reste.section( "</spu>", 0, 0 );
					sl = new Subline( Chaine );
					subs.push_back( *sl );
				}
				else if ( Reste.contains( "/>" ) )
				{
					Chaine = Debut + Reste.section( "/>", 0, 0 );
					sl = new Subline( Chaine );
					subs.push_back( *sl );
				}
				else
				{
					sl = new Subline( Reste );
					subs.push_back( *sl );
					while ( !stream.atEnd() )
					{
						line = stream.readLine();
						if ( line.contains( "</spu>") )
						{
							QString text = line.section( "</spu>", 0, 0 );
							sl = new Subline( text );
							subs.push_back( *sl );
							break;
						}
						else
						{
							sl = new Subline( line );
							subs.push_back( *sl );
						}
					}
				}
				Subtitle *st = new Subtitle( ts, te, subs );
				subvec.push_back( st );
			}
		}
        file.close();
    }
}


void qt4wndsub::genPngForSpumux()
{
	qt4genpngwnd *gw = new qt4genpngwnd( 0 );
	if ( gw->exec() == QDialog::Rejected )
		return;
	setCursor( Qt::WaitCursor );
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	std::vector<Subtitle *>::iterator it;

	QString xmlname;
	QString name;
	QByteArray a1 = gw->leBaseName->text().toAscii();
	xmlname.sprintf( "%s.xml", a1.constData() );
    QFile file( xmlname );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		if ( cbOutputEncoding->currentText() == "ISO 8859-1" )
		{
			stream.setCodec(QTextCodec::codecForName("ISO8859-1"));
		}
		else
		{
			stream.setCodec(QTextCodec::codecForName("UTF8"));
		}
		stream << "<subpictures>" << endl;
		stream << "  <stream>" << endl;
		int ctr = 0;
		QProgressDialog progress("Generating files...", "Abort", 0, 
			subvec.size(), this);
		progress.setWindowModality(Qt::WindowModal);
		progress.setWindowTitle( "PNG Progress" );
		int pgr = 0;
		for ( it = subvec.begin(); it != subvec.end(); it++ )
		{
			progress.setValue( pgr++ );
			if (progress.wasCanceled()) break;
			ts = (*it)->getBegin();
			te = (*it)->getEnd();
			stream << "    <spu";
			stream << " start=\"" << ts.toString( SPU_TIME_FMT ) << "\"";;
			stream << " end=\"" << te.toString( SPU_TIME_FMT ) << "\"";;

			int maxw = 0, maxh = 0, interligne = 10;
			QVector<Subline>::iterator its;
			QFontMetrics fm( gw->pbNormalFont->font() );
			QRect r;
			// computing metrics
			int x0 = gw->sbLeftMargin->value();
			int y0 = 0 + gw->sbTopMargin->value();
			int l0 = 720 - gw->sbRightMargin->value();
			int h0;
			if ( gw->rbPAL->isChecked() )
			{
				h0 = 576 - gw->sbBottomMargin->value();
			}
			else
			{
				h0 = 480 - gw->sbBottomMargin->value();
			}
			int flags = 0;
			switch ( gw->cbHoriz->currentIndex() )
			{
			case 1 : // Left alignment
				flags |= Qt::AlignLeft;
			break;
			case 2 : // Right alignment
				flags |= Qt::AlignRight;
			break;
			default : // center
				flags |= Qt::AlignHCenter;
			break;
			}
			if ( gw->cbVert->currentIndex() == 1 )
			{	// top align
				flags |= Qt::AlignTop;
			}
			else
			{
				flags |= Qt::AlignBottom;
			}
			QString maxs( "" );
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				if ( its->getFmt() == Subline::Italic )
				{
					fm = QFontMetrics( gw->pbItalicFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				else if ( its->getFmt() == Subline::Bold )
				{
					fm = QFontMetrics( gw->pbBoldFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				else
				{
					fm = QFontMetrics( gw->pbNormalFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				if ( r.width() > maxw )
				{
					maxs = its->getLine();
					maxw = r.width();
				}
				maxh += r.height() + fm.leading() + fm.descent();
			}
			int dispow = 720 - gw->sbLeftMargin->value() - gw->sbRightMargin->value();	
			int dispoh;
			if ( maxw > dispow )
			{
				maxw = dispow;
				QMessageBox::information( this, "submgmt",
										"Subtitle :\n" + maxs + "\nis too wide");
			}
			if ( gw->rbPAL->isChecked() )
			{
				dispoh = 576 - gw->sbTopMargin->value() - gw->sbBottomMargin->value();	
			}
			else
			{
				dispoh = 480 - gw->sbTopMargin->value() - gw->sbBottomMargin->value();	
			}
			if ( maxh > dispoh )
			{
				maxh = dispoh;
				QMessageBox::information( this, "submgmt", "Subtitles are too high" );
			}

			// drawing the text
			QPixmap gensub( maxw, maxh );
			gensub.fill( Qt::black );
			QPainter p( &gensub );
			int clip = 10;
			QColor textcol = gw->pbSubColor->palette().color( QPalette::Button);
			if ( ( (textcol.red()-clip) <= 0 )
				&& ( (textcol.green()-clip) <= 0 )
				&& ( (textcol.blue()-clip) <= 0 ) )
			{
				QMessageBox::information( this, "submgmt", "The subtitle color you choose is too near black." );
				return;

			}
			int rc, gc, bc;
			textcol.getRgb( &rc, &gc, &bc );
			QRgb textrgb = qRgba( rc, gc, bc, 255 );	// opaque pixel
			int x = 0, y = 0;
			for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
			{
				if ( its->getFmt() == Subline::Italic )
				{
					fm = QFontMetrics( gw->pbItalicFont->font() );
					p.setFont( gw->pbItalicFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				else if ( its->getFmt() == Subline::Bold )
				{
					fm = QFontMetrics( gw->pbBoldFont->font() );
					p.setFont( gw->pbBoldFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				else
				{
					fm = QFontMetrics( gw->pbNormalFont->font() );
					p.setFont( gw->pbNormalFont->font() );
					r = fm.boundingRect( x0, y0, l0, h0, flags, its->getLine(), -1 );
				}
				switch ( gw->cbHoriz->currentIndex() )
				{
				case 1 : // Left alignment
				x = 0;
				break;
				case 2 : // Right alignment
				x = maxw - r.width();
				break;
				default : // center
				x = ( maxw - r.width() ) / 2;
				break;
				}
				p.setPen( QPen( textcol ) );
				p.setBrush( QBrush( textcol, Qt::SolidPattern ) );
				p.drawText( x, y+r.height()+fm.leading(), its->getLine() );
				y += r.height() + fm.leading();
			}

			// computing offset
			int xoff = 0, yoff = 0;
			if ( gw->cbVert->currentIndex() == 1 )
			{	// top align
				yoff = gw->sbTopMargin->value();
			}
			else
			{
				if ( gw->rbPAL->isChecked() )
					yoff = 576-(gensub.height() + gw->sbBottomMargin->value() );
				else
					yoff = 480-(gensub.height() + gw->sbBottomMargin->value() );
			}
			if ( gw->cbHoriz->currentIndex() == 1 )
			{	// Left
				xoff = gw->sbLeftMargin->value();
			}
			else if ( gw->cbHoriz->currentIndex() == 2 )
			{	// Right
				xoff = 720 - gw->sbRightMargin->value() - gensub.width();
			}
			else
			{	// Center
				xoff = (720 - gw->sbRightMargin->value() - gw->sbLeftMargin->value() - gensub.width() ) / 2;
			}
			if ( xoff < 0 ) xoff = 0;
			if ( yoff < 0 ) yoff = 0;

			QImage img = gensub.toImage();
			//FIXME img.setAlphaBuffer( true );
			//std::cout << "numcol=" << img.numColors() << std::endl;
			for ( int x = 0; x < img.width(); x++ )
			{
				for ( int y = 0; y < img.height(); y++ )
				{
					QRgb rgb = img.pixel( x, y );
					QColor c( rgb );
					if ( ( (c.red()-clip) <= 0 )
						&& ( (c.green()-clip) <= 0 )
						&& ( (c.blue()-clip) <= 0 ) )
					{
						img.setPixel( x, y, 0 /*black.rgb()*/ );
					}
					else
						img.setPixel( x, y, textrgb );
				}
			}
			//img.setNumColors( 3 );
			QByteArray a1 = gw->leBaseName->text().toAscii();
			name.sprintf( "%s%4.4d.png",a1.constData(), ctr++ );
			img.save( name, "PNG" );

			stream << " image=\"" << name << "\"";
			//stream << " transparent=\"000000\"";
			if ( gw->cbForced->isChecked() )
			{
				stream << " force=\"yes\"";
			}
			stream << " xoffset=\"" << xoff << "\"";
			stream << " yoffset=\"" << yoff << "\"";
			stream << " />" << endl;
		}
		progress.setValue( subvec.size() );
		stream << "  </stream>" << endl;
		stream << "</subpictures>" << endl;
	}
	else
	{
		QMessageBox::information( this, "submgmt", "Erreur ecriture fichier");
	}
	delete gw;
	setCursor( Qt::ArrowCursor );
}

bool qt4wndsub::hasFormat()
{
	std::vector<Subtitle *>::iterator it;
	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		QVector<Subline>::iterator its;
		for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
		{
			if ( its->getFmt() != Subline::Normal )
				return true;
		}
	}
	return false;
}

void qt4wndsub::dump()
{
	static int passage;

	std::cout << passage++ << " passage" << std::endl;
	std::vector<Subtitle *>::iterator it;
	QVector<Subline>::const_iterator its;

	for ( it = subvec.begin(); it != subvec.end(); it++ )
	{
		for ( its = (*it)->getSubs().begin(); its != (*it)->getSubs().end(); its++ )
		{
			QString s = its->getLine();
			std::cout << "s=" << &s << std::endl;
			QByteArray a1 = s.toAscii();
			std::cout << a1.constData() << std::endl;
		}
	}
}

QString qt4wndsub::getInputFile( )
{
	return leInputFile->text();
}

void qt4wndsub::setInputFile( char *fn )
{
	leInputFile->setText( fn );
}

void qt4wndsub::nextWrongSub()
{
	static int i;
	SubModel *inputSubsModel = (SubModel*)lvInputSubs->model();
	QModelIndex index;
	QVariant var;
	Subtitle *_subtitle;
	while ( i < inputSubsModel->rowCount() )
	{
		index = inputSubsModel->index( i, 0 );
		var = index.model()->data(index, Qt::DisplayRole);
		_subtitle = var.value<Subtitle *>();
		i++;
		if ( _subtitle->getProblem() )
		{
			lvInputSubs->setCurrentIndex( index );
			break;
		}
	}
	if ( i == inputSubsModel->rowCount() ) i = 0;
}

