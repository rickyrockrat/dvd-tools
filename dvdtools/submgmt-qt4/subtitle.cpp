
#include <qdatetime.h>
#include <qstringlist.h>
#include <QVector>
#include <ostream>
#include <QTextStream>

#include "subtitle.h"

Subline::Subline( )
{
	_line = QString("");
	_fmt = Normal;
}

Subline::Subline( const Subline &s )
{
	_line = QString( s.getLine() );
	_fmt = s.getFmt();
}

Subline::Subline( QString &s )
{
	_line = s;
	_fmt = Normal;
}

Subline::Subline( QString &s, FMT f )
{
	_line = s;
	_fmt = f;
}

Subline::~Subline( )
{
}

std::ostream& operator << (std::ostream& out, const Subline &s )
{
	QByteArray asciiData = s.getLine().toAscii();
	out << asciiData.constData();
	return out;
}

QTextStream & operator << (QTextStream & out, const Subline &s )
{
	//out << codec->fromUnicode( s.line );
	out << s.getLine();
	return out;
}

void Subline::setLine( QString &s )
{
	_line = s;
}

void Subline::setFmt( FMT f )
{
	_fmt = f;
}

QString &Subline::getLine( )
{
	return _line;
}

const QString &Subline::getLine( ) const
{
	return _line;
}

Subline::FMT &Subline::getFmt( )
{
	return _fmt;
}

const Subline::FMT &Subline::getFmt( ) const
{
	return _fmt;
}

Subtitle::Subtitle( )
{
	_frameBased =false;
	_begin = QTime();
	_end = QTime();
	_fbegin = 0;
	_fend = 0;
	_subs.clear();
	_problem = false;
}

Subtitle::Subtitle( const Subtitle &s )
{
	_frameBased = s.getFrameBased();
	_begin = s.getBegin();
	_end = s.getEnd();
	_fbegin = s.getFbegin();
	_fend = s.getFend();
	QVector<Subline>::iterator it;
	/*
	for (it = s.getSubs().begin(); it != s.getSubs().end(); it++ )
	{
		_subs.push_back( *it );
	}
	*/
	_subs = s.getSubs();
	_problem = false;
}

Subtitle::Subtitle( int &b, int &e, QVector<Subline> s )
{
	_fbegin = b;
	_fend = e;
	/*
	QVector<Subline>::iterator it;
	for (it = s.begin(); it != s.end(); it++ )
	{
		_subs.push_back( *it );
	}
	*/
	_subs = s;
	_frameBased = true;
	_problem = false;
}

Subtitle::Subtitle( QTime &b, QTime &e, QVector<Subline> s )
{
	_begin = b;
	_end = e;
	/*
	QVector<Subline>::iterator it;
	for (it = s.begin(); it != s.end(); it++ )
	{
		_subs.push_back( *it );
	}
	*/
	_subs = s;
	_frameBased = false;
	_problem = false;
}

Subtitle::~Subtitle( )
{
}

void Subtitle::setFrameBased( bool b )
{
	_frameBased = b;
}

void Subtitle::setProblem( bool b )
{
	_problem = b;
}

void Subtitle::setFbegin( int i )
{
	_fbegin = i;
}

void Subtitle::setFend( int i )
{
	_fend = i;
}

void Subtitle::setBegin( QTime t )
{
	_begin = t;
}

void Subtitle::setEnd( QTime t )
{
	_end = t;
}

void Subtitle::setSubs( QVector<Subline> &s )
{
	/*
	_subs.clear();
	QVector<Subline>::const_iterator it;
	for ( it = s.begin(); it != s.end(); it++ )
	{
		_subs.push_back( *it );
	}
	*/
	_subs = s;
}


bool Subtitle::getProblem( ) const
{
	return _problem;
}

bool Subtitle::getFrameBased( ) const
{
	return _frameBased;
}

int Subtitle::getFbegin( ) const
{
	return _fbegin;
}

int Subtitle::getFend( ) const
{
	return _fend;
}

QTime Subtitle::getBegin( ) const
{
	return _begin;
}

QTime Subtitle::getEnd( ) const
{
	return _end;
}

QVector<Subline> &Subtitle::getSubs( )
{
	return _subs;
}

QVector<Subline> Subtitle::getSubs( ) const
{
	return _subs;
}

QString Subtitle::getTiming()
{
	QString str;
	if ( _frameBased )
	{
		str.sprintf( "Frame %d to %d", _fbegin, _fend );
	}
	else
	{
		QTime ts = _begin;
		QTime te = _end;
		str = ts.toString( TIME_FMT ) + " to " + te.toString( TIME_FMT );
	}
	return str;
}
