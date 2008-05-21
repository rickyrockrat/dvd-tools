
#ifndef _UTIL_H
#define _UTIL_H

#include <QTime>

#define TIME_FMT    "hh:mm:ss,zzz"

int milli( const QString &str, char dsep = '.' );
int milli( const QTime &t );
QTime timeFromMilli( int ms );
QTime operator -( const QTime &t1, const QTime &t2 );
QTime operator *( const QTime &t, double factor );
QTime operator +( const QTime &t1, const QTime &t2 );
QTime operator *( const QTime &t1, const QTime &t2 );
double operator /( const QTime &t1, const QTime &t2 );

#endif
