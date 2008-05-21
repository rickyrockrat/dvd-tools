#include "subtitle.h"
#include "submodel.h"
#include "subitemdelegate.h"

#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include "qt4subeditor.h"
#include "subtitle.h"
#include "util.h"

#define TIME_FMT	"hh:mm:ss,zzz"
#define SPU_TIME_FMT	"hh:mm:ss.zzz"
#define NUM_LINES	30

// Constructor
qt4subeditor::qt4subeditor( QMainWindow *parent ) : QMainWindow(parent)
{
	setupUi(this);
	connect(
		pbOpenMovie,
		SIGNAL(clicked()),
		this,
		SLOT(openMovie())
	);
	connect(
		pbPlayPauseMovie,
		SIGNAL(clicked()),
		this,
		SLOT(playPauseMovie())
	);
	connect(
		pbNewSubtitle,
		SIGNAL(clicked()),
		this,
		SLOT(newSubtitle())
	);
	connect(
		pbOpenSubtitle,
		SIGNAL(clicked()),
		this,
		SLOT(openSubtitle())
	);
	connect(
		pbSaveSubtitle,
		SIGNAL(clicked()),
		this,
		SLOT(saveSubtitle())
	);
	connect(
		pbExit,
		SIGNAL(clicked()),
		this,
		SLOT(exitEditor())
	);
	connect(
		this,
		SIGNAL(playMovie()),
		this,
		SLOT(playPauseMovie())
	);
	connect(
		pbApply,
		SIGNAL(clicked()),
		this,
		SLOT(applyToMovie())
	);
	connect(pbFastBack, SIGNAL(clicked()),this,SLOT(fastBack()));
	connect(pbBack, SIGNAL(clicked()),this,SLOT(back()));
	connect(pbForward, SIGNAL(clicked()),this,SLOT(forward()));
	connect(pbFastForward, SIGNAL(clicked()),this,SLOT(fastForward()));
	connect(pbNewSubBegin, SIGNAL(clicked()),this,SLOT(newSubBegin()));
	connect(pbNewSubEnd, SIGNAL(clicked()),this,SLOT(newSubEnd()));
	mplayer = 0;
	movieOpened = false;
	moviePaused = false;
	flagSubBegin = false;
	flagSubEnd = false;

	mplayer = new QProcess(this);
	QStringList arg;

	arg << "-slave";
	arg << "-idle";

	connect(
		mplayer,
		SIGNAL(readyReadStandardOutput()),
		this,
		SLOT(readMplayer())
	);
	mplayer->start( "mplayer", arg );

	std::cout << qApp->arguments().size() << " args" << std::endl;
	if ( qApp->arguments().size() == 2 )
	{
		QString fn = qApp->arguments().at(1);
		loadMovie( fn );
	}
}

void qt4subeditor::exitEditor( )
{
	if ( mplayer )
	{
		mplayer->write("quit\n");
		delete mplayer;
		mplayer = 0;
	}
	close();
}

qt4subeditor::~qt4subeditor( )
{
	if ( mplayer )
	{
		mplayer->write("quit\n");
		delete mplayer;
		mplayer = 0;
	}
}

void qt4subeditor::openMovie()
{
	QString fn = QFileDialog::getOpenFileName ( this, "Select movie" );
	loadMovie( fn );
}

void qt4subeditor::loadMovie( QString fn )
{
	if ( fn.isEmpty() ) return;
	movieFilename = fn;
	QByteArray ba("loadfile ");
	ba.append(fn);
	ba.append("\npause\n");
	mplayer->write( ba );
	mplayer->write("osd 2\n" );
	mplayer->write( "get_time_length\npause\n" );
	moviePaused = true;
}

void qt4subeditor::newSubBegin()
{
	flagSubBegin = true;
	mplayer->write( "get_time_pos\n" );
}

void qt4subeditor::newSubEnd()
{
	flagSubEnd = true;
	mplayer->write( "get_time_pos\n" );
}

void qt4subeditor::readMplayer()
{
	if ( mplayer )
	{
		QByteArray ba = mplayer->readAll();
		//std::cout << ba.constData(); 
		int idx = ba.indexOf( "ANS_LENGTH=" );
		if ( idx != -1 )
		{
			idx += 11;
			QByteArray len = ba.mid( idx, ba.indexOf( '\n', idx ) - idx );
			std::cout << len.constData() << std::endl;
			lblLength->setText( len );
			movieLength = (int)(len.toDouble() * 1000);
			movieOpened = true;
		}
		else
		{
			idx = ba.indexOf( "ANS_TIME_POSITION=" );
			if  ( idx != -1 )
			{
				std::cout << "reading TIME_POS";
				idx += 18;
				QByteArray pos = ba.mid( idx, ba.indexOf( '\n', idx ) - idx );
				if ( flagSubEnd == true )
				{
					std::cout << " for subEnd" << std::endl;
					subEnd = pos.toDouble();
					QTime ts;
					ts = timeFromMilli( subBegin * 1000 );
					QTime te;
					te = timeFromMilli( subEnd *1000 );

					QByteArray b1( ts.toString(TIME_FMT).toAscii() );
					QByteArray b2( te.toString(TIME_FMT).toAscii() );
					std::cout << b1.constData() << " to " << b2.constData() << std::endl;
					Subline *sul1 = new Subline( *new QString(""), Subline::Normal);
					Subline *sul2 = new Subline( *new QString(""), Subline::Normal);
					QVector<Subline> subs;
					subs.push_back(*sul1);
					subs.push_back(*sul2);
					Subtitle *sut = new Subtitle( ts, te, subs );
					SubModel *sm = (SubModel *)lvSubs->model();
					if ( sm ) sm->insertSub(sut);

					flagSubBegin = false;
					flagSubEnd = false;
				}
				else if ( flagSubBegin == true )
				{
					std::cout << " for subBegin" << std::endl;
					subBegin = pos.toDouble();
				}
			}
		}
	}
}

void qt4subeditor::playPauseMovie()
{
	if ( movieOpened )
	{
		if ( moviePaused )
		{
			moviePaused = false;
			mplayer->write( "get_time_pos\n" );
		}
		else
		{
			moviePaused = true;
			mplayer->write( "get_time_pos\n" );
			mplayer->write( "pause\n" );
		}
	}
}

void qt4subeditor::newSubtitle()
{
	subvec.clear();
	SubModel *inputSubsModel = new SubModel( subvec );
    lvSubs->setModel( inputSubsModel );
    SubItemDelegate *delI = new SubItemDelegate( lvSubs );
    lvSubs->setItemDelegate( delI );
}

void qt4subeditor::openSubtitle()
{
	QRegExp subviewer = QRegExp( "[0-9][0-9]:[0-9][0-9]:[0-9][0-9],[0-9][0-9][0-9] --> [0-9][0-9]:[0-9][0-9]:[0-9][0-9],[0-9][0-9][0-9]" );
	QString fn = QFileDialog::getOpenFileName ( this, "Select sub" );
	QString Start;
	QString End;
	QTime ts;
	QTime te;
	QVector<Subline> subs;

    QFile file( fn );

    if ( file.open( QIODevice::ReadOnly ) )
	{
		subvec.clear();
        QTextStream stream( &file );
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
		SubModel *inputSubsModel = new SubModel( subvec );
		lvSubs->setModel( inputSubsModel );
		SubItemDelegate *delI = new SubItemDelegate( lvSubs );
		lvSubs->setItemDelegate( delI );
    }
}

void qt4subeditor::saveSubtitle()
{
	QString fn = QFileDialog::getSaveFileName();
	if ( !fn.isEmpty() ) saveSubToFile( fn );
}

void qt4subeditor::saveSubToFile( QString fn )
{
    QFile file( fn );

    if ( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
        QTextStream stream( &file );
		int numero = 1;
		SubModel *sm = (SubModel *)lvSubs->model();
		QModelIndex idx;
		QVariant v;
		Subtitle *s;
		QTime ts;
		QTime te;
		for (int i = 0; i < sm->rowCount(); i++ )
		{
			idx = sm->index(i);
			v = sm->data( idx, Qt::DisplayRole );
			s = qVariantValue<Subtitle *>(v);
			stream << numero << endl;
			ts = s->getBegin();
			te = s->getEnd();
			stream << ts.toString( TIME_FMT ) << " --> " << te.toString( TIME_FMT ) << endl;
			QVector<Subline> lines = s->getSubs();
			QVector<Subline>::Iterator it;
			for ( it = lines.begin(); it != lines.end(); it++ )
			{
				Subline l = *it;
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

void qt4subeditor::fastBack()
{
	mplayer->write( "seek -60\n" );
	if ( moviePaused ) mplayer->write( "pause\n" );
}

void qt4subeditor::back()
{
	mplayer->write( "seek -10\n" );
	if ( moviePaused ) mplayer->write( "pause\n" );
}

void qt4subeditor::forward()
{
	mplayer->write( "seek +10\n" );
	if ( moviePaused ) mplayer->write( "pause\n" );
}

void qt4subeditor::fastForward()
{
	mplayer->write( "seek +60\n" );
	if ( moviePaused ) mplayer->write( "pause\n" );
}

void qt4subeditor::applyToMovie()
{
	saveSubToFile( "/tmp/temp.srt" );
	mplayer->write( "sub_remove\nsub_load /tmp/temp.srt\nsub_select\n" );
	if ( moviePaused ) mplayer->write( "pause\n" );
}

// vim: ts=4 sw=4
