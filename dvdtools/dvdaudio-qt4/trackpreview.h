
#ifndef _PREVIEW_LABEL
#define _PREVIEW_LABEL

#include <QLabel>
#include <QColor>
#include <QTreeWidget>
#include <QPainter>
#include <QFont>

class trackPreviewWnd : public QLabel
{
public :
	enum norm { PAL, NTSC };
	trackPreviewWnd( QWidget *parent = 0 );
	trackPreviewWnd( QTreeWidgetItem *,
		   	QWidget *parent = 0 );
	~trackPreviewWnd();
	void setTrack( QTreeWidgetItem * );
	QTreeWidgetItem *track();
	void setNorm( norm );
	norm getNorm();
	void setXOrig( int x ) { m_xorig = x; };
	void setYOrig( int y ) { m_yorig = y; };
	void setStep( int s ) { m_step = s; };
	virtual void drawContents( QPainter * );
	void setFont( QFont &f ) { m_font = f; };
	void setBgColor( QColor &c ) { m_bgcolor = c; };
	void setFgColor( QColor &c ) { m_fgcolor = c; };
	QFont &getFont() { return m_font; };
	QColor &getBgColor() { return m_bgcolor; };
	QColor &getFgColor() { return m_fgcolor; };
	void setDrawGenre( bool b ) { md_genre = b; };
	void setDrawArtist( bool b ) { md_artist = b; };
	void setDrawAlbum( bool b ) { md_album = b; };
	void setDrawYear( bool b ) { md_year = b; };
	void setDrawTitle( bool b ) { md_title = b; };
	void setDrawTrack( bool b ) { md_track = b; };
	void setDrawDuration( bool b ) { md_duration = b; };
	void setDrawBitrate( bool b ) { md_bitrate = b; };
private :
	QTreeWidgetItem *m_track;
	norm m_norm;
	int m_xorig;
	int m_yorig;
	int m_step;
	QColor m_bgcolor;
	QColor m_fgcolor;
	QFont m_font;
	bool md_genre;
	bool md_artist;
	bool md_album;
	bool md_year;
	bool md_title;
	bool md_track;
	bool md_duration;
	bool md_bitrate;
};

#endif
