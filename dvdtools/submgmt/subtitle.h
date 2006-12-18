
#ifndef _SUBTITLE_H
#define _SUBTITLE_H

#include <qdatetime.h>
#include <qstringlist.h>

class subtitle
{
	public :
		subtitle( int &b, int &e, QStringList &s )
		{
			fbegin = b;
			fend = e;
			subs = s;
			FrameBased = true;
		}
		subtitle( QTime &b, QTime &e, QStringList &s )
		{
			begin = b;
			end = e;
			subs = s;
			FrameBased = false;
		}
		bool FrameBased;
		int fbegin;
		int fend;
		QTime begin;
		QTime end;
		QStringList subs;
};

#endif	// SUBTITLE_H
