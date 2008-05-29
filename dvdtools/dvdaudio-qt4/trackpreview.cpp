
#include "trackpreview.h"
#include "common.h"

#include <iostream>

trackPreviewWnd::trackPreviewWnd( QWidget *w ) : QLabel( w )
{
	m_track = 0;
	m_norm = PAL;
	resize( 720, 576 );
	m_bgcolor = Qt::black;
	m_fgcolor = Qt::white;
	m_font = QFont( "Bitstream Vera Sans Mono", 20 );
	//FIXME setCaption( "Preview" );
	m_xorig = 50;
	m_yorig = 50;
	m_step = 30;
}

trackPreviewWnd::trackPreviewWnd( QTreeWidgetItem *t,
		   	QWidget *w ) : QLabel( w )
{
	m_track = t;
	m_norm = PAL;
	resize( 720, 576 );
	m_bgcolor = Qt::black;
	m_fgcolor = Qt::white;
	m_font = QFont( "Bitstream Vera Sans Mono", 28 );
	//FIXME setCaption( "Preview" );
	m_xorig = 50;
	m_yorig = 50;
	m_step = 30;
}

trackPreviewWnd::~trackPreviewWnd()
{
}

void trackPreviewWnd::setNorm( norm n )
{
	m_norm = n;
	if ( m_norm == trackPreviewWnd::PAL )
		resize( 720, 576 );
	else
		resize( 720, 480 );
}

void trackPreviewWnd::setTrack( QTreeWidgetItem *t )
{
	m_track = t;
}

trackPreviewWnd::norm trackPreviewWnd::getNorm()
{
	return m_norm;
}
QTreeWidgetItem *trackPreviewWnd::track()
{
	return m_track;
}

void trackPreviewWnd::drawContents( QPainter *p )
{
	if ( m_track )
	{
		setNorm( trackPreviewWnd::PAL );
		p->fillRect( rect(), QBrush( m_bgcolor ) );
		p->setPen( m_fgcolor );
		p->setFont( m_font );
		QFontMetrics fm( m_font );
		m_step = fm.lineSpacing();

		int y = m_yorig;
		int x = m_xorig;
		if ( md_genre )
		{
			p->drawText( x, y, m_track->text( ID_GENRE ) );
			y+=m_step;
		}
		if ( md_artist )
		{
			p->drawText( x, y, m_track->text( ID_ARTIST ) );
			y+=m_step;
		}
		if ( md_album )
		{
			p->drawText( x, y, m_track->text( ID_ALBUM ) );
			y+=m_step;
		}
		if ( md_year )
		{
			p->drawText( x, y, m_track->text( ID_YEAR ) );
			y+=m_step;
		}
		if ( md_title )
		{
			p->drawText( x, y, m_track->text( ID_NAME ) );
			y+=m_step;
		}
		if ( md_track )
		{
			p->drawText( x, y, "Track " + m_track->text( ID_TRACK ) );
			y+=m_step;
		}
		if ( md_duration )
		{
			p->drawText( x, y, m_track->text( ID_DURATION ) );
			y+=m_step;
		}
		if ( md_bitrate )
		{
			p->drawText( x, y, m_track->text( ID_BITRATE ) );
			y+=m_step;
		}
	}
}
