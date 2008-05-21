
#include "util.h"

int milli( const QString &str, char dsep )
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

