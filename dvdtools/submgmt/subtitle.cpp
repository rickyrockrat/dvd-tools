
#include <qdatetime.h>
#include <qstringlist.h>
#include <vector>
#include <ostream>

#include "subtitle.h"

Subline::Subline( )
{
}

Subline::Subline( const Subline &s )
{
	_line = s.getLine();
	_fmt = s.getFmt();
}

Subline::Subline( QString s )
{
	_line = s;
	_fmt = Normal;
}

Subline::Subline( QString s, FMT f )
{
	_line = s;
	_fmt = f;
}

std::ostream& operator << (std::ostream& out, const Subline &s )
{
	out << s.getLine().ascii();
	return out;
}

QTextStream & operator << (QTextStream & out, const Subline &s )
{
	//out << codec->fromUnicode( s.line );
	out << s.getLine();
	return out;
}

void Subline::setLine( QString & s )
{
	_line = s;
}

void Subline::setFmt( FMT &f )
{
	_fmt = f;
}

QString Subline::getLine( ) const
{
	return _line;
}

Subline::FMT Subline::getFmt( ) const
{
	return _fmt;
}

Subtitle::Subtitle( const Subtitle &s )
{
	_frameBased = s.getFrameBased();
	_begin = s.getBegin();
	_end = s.getEnd();
	_fbegin = s.getFbegin();
	_fend = s.getFend();
	_subs = s.getSubs();
}

Subtitle::Subtitle( int &b, int &e, std::vector<Subline> &s )
{
	_fbegin = b;
	_fend = e;
	_subs = s;
	_frameBased = true;
}

Subtitle::Subtitle( QTime &b, QTime &e, std::vector<Subline> &s )
{
	_begin = b;
	_end = e;
	_subs = s;
	_frameBased = false;
}

void Subtitle::setFrameBased( bool b )
{
	_frameBased = b;
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

void Subtitle::setSubs( std::vector<Subline> s )
{
	_subs = s;
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

std::vector<Subline> Subtitle::getSubs( ) const
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
