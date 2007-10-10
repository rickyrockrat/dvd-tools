
#ifndef _SUBTITLE_H
#define _SUBTITLE_H

#define TIME_FMT	"hh:mm:ss,zzz"

class Subline
{
public:
	enum FMT { Normal, Italic, Bold, Underline, Stroke, DoubleStroke };

	Subline( );
	Subline( const Subline &s );
	Subline( QString s );
	Subline( QString s, FMT f );
	void setLine( QString & );
	void setFmt( FMT & );
	QString getLine( ) const;
	FMT getFmt( ) const;
	friend std::ostream& operator << (std::ostream&, const Subline &s );
	friend QTextStream & operator << (QTextStream &, const Subline &s );
private:
	QString _line;
	FMT _fmt;
};


class Subtitle
{
public :
		Subtitle( const Subtitle & );
		Subtitle( int &b, int &e, std::vector<Subline> &s );
		Subtitle( QTime &b, QTime &e, std::vector<Subline> &s );
		void setFrameBased( bool );
		void setFbegin( int );
		void setFend( int );
		void setBegin( QTime );
		void setEnd( QTime );
		void setSubs( std::vector<Subline> );

		bool getFrameBased( ) const;
		int getFbegin( ) const;
		int getFend( ) const;
		QTime getBegin( ) const;
		QTime getEnd( ) const;
		std::vector<Subline> getSubs( ) const;

		QString getTiming();
private:
		bool _frameBased;
		int _fbegin;
		int _fend;
		QTime _begin;
		QTime _end;
		std::vector<Subline> _subs;
};

#endif	// SUBTITLE_H
