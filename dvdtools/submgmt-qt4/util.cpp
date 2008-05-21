
#include "util.h"

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

