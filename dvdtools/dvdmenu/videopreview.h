
#ifndef _PREVIEW_LABEL
#define _PREVIEW_LABEL

#include <qlabel.h>
#include <qcolor.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qfont.h>
#include <qpixmap.h>

class videoPreviewWnd : public QLabel
{
public :
	enum norm { PAL, NTSC };
	videoPreviewWnd( QWidget *, const char *name = 0, WFlags f = 0 );
	videoPreviewWnd( QListView *,
		   	QWidget *, const char *name = 0, WFlags f = 0 );
	~videoPreviewWnd();
	void setNorm( norm );
	norm getNorm();
	void setXOrig( int x ) { m_xorig = x; };
	void setYOrig( int y ) { m_yorig = y; };
	void setStep( int s ) { m_step = s; };
	virtual void drawContents( QPainter * );
	QFont &setFont( QFont &f ) { m_font = f; };
	QFont &setBgColor( QColor &c ) { m_bgcolor = c; };
	void setBgOnly( bool b ) { m_bgonly = b; };
	void setPreview( bool b ) { m_preview = b; };
	QFont &setBgPic( QString &s ) { m_bgpic = s; };
	QFont &setFgColor( const QColor &c ) { m_fgcolor = c; };
	QFont &getFont() { return m_font; };
	QColor &getBgColor() { return m_bgcolor; };
	QString &getBgPic() { return m_bgpic; };
	QColor &getFgColor() { return m_fgcolor; };
	void setDrawGenre( bool b ) { md_genre = b; };
	void setDrawArtist( bool b ) { md_artist = b; };
	void setDrawAlbum( bool b ) { md_album = b; };
	void setDrawYear( bool b ) { md_year = b; };
	void setDrawTitle( bool b ) { md_title = b; };
	void setDrawTrack( bool b ) { md_video = b; };
	void setDrawDuration( bool b ) { md_duration = b; };
	void setDrawBitrate( bool b ) { md_bitrate = b; };
	void setTotalPages( int n ) { m_totalpages = n; };
	void setCurrentPage( int n ) { m_currentpage = n; };
private :
	QListView *m_dvd;
	norm m_norm;
	int m_xorig;
	int m_yorig;
	int m_step;
	bool m_bgonly;
	bool m_preview;
	QColor m_bgcolor;
	QString m_bgpic;
	QColor m_fgcolor;
	QFont m_font;
	bool md_genre;
	bool md_artist;
	bool md_album;
	bool md_year;
	bool md_title;
	bool md_video;
	bool md_duration;
	bool md_bitrate;
	int m_totalpages;
	int m_currentpage;
};

#endif
