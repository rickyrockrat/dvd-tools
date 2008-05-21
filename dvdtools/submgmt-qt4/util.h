
#ifndef _UTIL_H
#define _UTIL_H

#include <QTime>

#define TIME_FMT    "hh:mm:ss,zzz"

int milli( const QTime &t );
QTime timeFromMilli( int ms );

#endif
