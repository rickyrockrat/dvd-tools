
#ifndef _SUBTITLE_H
#define _SUBTITLE_H

#define TIME_FMT	"hh:mm:ss,zzz"

#include <QTextStream>
#include <QTime>
#include <QMetaType>
#include <QVector>

class Subline
{
public:
	enum FMT { Normal, Italic, Bold, Underline, Stroke, DoubleStroke };

	Subline( );
	Subline( const Subline &s );
	Subline( QString & );
	Subline( QString &, FMT );
	~Subline( );

	void setLine( QString & );
	void setFmt( FMT );
	QString &getLine( );
	FMT &getFmt( );
	const QString &getLine( ) const;
	const FMT &getFmt( ) const;
	friend std::ostream& operator << (std::ostream&, const Subline &s );
	friend QTextStream & operator << (QTextStream &, const Subline &s );
private:
	QString _line;
	FMT _fmt;
};

Q_DECLARE_METATYPE( Subline * )

class Subtitle
{
public :
		Subtitle( );
		Subtitle( const Subtitle & );
		Subtitle( int &b, int &e, QVector<Subline> s );
		Subtitle( QTime &b, QTime &e, QVector<Subline> s );
		~Subtitle( );
		void setProblem( bool );
		void setFrameBased( bool );
		void setFbegin( int );
		void setFend( int );
		void setBegin( QTime );
		void setEnd( QTime );
		void setSubs( QVector<Subline> & );

		bool getProblem( ) const;
		bool getFrameBased( ) const;
		int getFbegin( ) const;
		int getFend( ) const;
		QTime getBegin( ) const;
		QTime getEnd( ) const;
		QVector<Subline> &getSubs( );
		QVector<Subline> getSubs( ) const;

		QString getTiming();
private:
		bool _frameBased;
		int _fbegin;
		int _fend;
		QTime _begin;
		QTime _end;
		QVector<Subline> _subs;
		bool _problem;
};

Q_DECLARE_METATYPE( Subtitle  *)

#endif	// SUBTITLE_H
