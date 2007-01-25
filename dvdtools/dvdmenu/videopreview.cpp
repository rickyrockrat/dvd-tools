

#include "videopreview.h"
#include "common.h"

#include <iostream>

videoPreviewWnd::videoPreviewWnd( QWidget *w, const char *name, WFlags f )
													: QLabel( w, name, f )
{
	m_dvd = 0;
	m_norm = PAL;
	resize( 720, 576 );
	m_bgcolor = Qt::black;
	m_fgcolor = Qt::white;
	m_font = QFont( "Bitstream Vera Sans Mono", 20 );
	setCaption( "Preview" );
	m_xorig = 50;
	m_yorig = 50;
	m_step = 30;
	m_bgonly = false;
	m_preview = false;
}

videoPreviewWnd::videoPreviewWnd( QListView *t,
		   	QWidget *w, const char *name, WFlags f ) : QLabel( w, name, f )
{
	m_dvd = t;
	m_norm = PAL;
	resize( 720, 576 );
	m_bgcolor = Qt::black;
	m_fgcolor = Qt::white;
	m_font = QFont( "Bitstream Vera Sans Mono", 28 );
	setCaption( "Preview" );
	m_xorig = 50;
	m_yorig = 50;
	m_step = 30;
	m_bgonly = false;
	m_preview = false;
}

videoPreviewWnd::~videoPreviewWnd()
{
}

void videoPreviewWnd::setNorm( norm n )
{
	m_norm = n;
	if ( m_norm == videoPreviewWnd::PAL )
		resize( 720, 576 );
	else
		resize( 720, 480 );
}

videoPreviewWnd::norm videoPreviewWnd::getNorm()
{
	return m_norm;
}

void videoPreviewWnd::drawContents( QPainter *p )
{
	if ( m_dvd )
	{
		setNorm( videoPreviewWnd::PAL );
		if ( m_bgonly || m_preview )
		{
			if ( m_bgpic )
			{
				QPixmap pm( m_bgpic );
				p->drawPixmap( rect(), pm );
			}
			else
			{
				p->fillRect( rect(), QBrush( m_bgcolor ) );
			}
		}
		else
		{
				p->fillRect( rect(), QBrush( black ) );
		}
		p->setFont( m_font );

		QListViewItemIterator it( m_dvd );
		QListViewItem *item;
		QRect r1( 50, 70, 230, 180 );
		QRect r2( 350, 70, 230, 180 );
		QRect r3( 50, 320, 230, 180 );
		QRect r4( 350, 320, 230, 180 );
		QRect tr[ 4 ];
		tr[0] = r1;
		tr[1] = r2;
		tr[2] = r3;
		tr[3] = r4;
		int ct = 0;
		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_DVD )
			{
				if ( m_bgonly || m_preview )
				{
					p->setPen( Qt::white );
					QFontMetrics fm( m_font );
					QRect titleRect = fm.boundingRect( item->text( ID_NAME ) );
					int x = ( 720-titleRect.width() ) / 2;
					p->drawText( x, 30, item->text( ID_NAME ) );
				}
			}
			if ( ( ct / 4 ) == m_currentpage )
				break;
			if ( item->text( ID_IDENT ) == CC_TITLE )
			{
				ct++;
			}
			it++;
		}
		ct = 0;
		int origFontSize = m_font.pointSize();
		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_TITLE )
			{
				if ( m_bgonly || m_preview )
				{
					QPixmap pmt( item->text( ID_PICTURE ) );
					p->drawPixmap( tr[ ct ], pmt );
				}
				if ( !m_bgonly || m_preview )
				{
					p->setPen( m_fgcolor );
					m_font.setPointSize(origFontSize);
					p->setFont( m_font );
					QFontMetrics fm( m_font );
					QRect titleRect = fm.boundingRect( item->text( ID_NAME ) );
					while( titleRect.width() > 280 )
					{
						m_font.setPointSize( m_font.pointSize() - 1 );
						p->setFont( m_font );
						fm = QFontMetrics( m_font );
						titleRect = fm.boundingRect( item->text( ID_NAME ) );
					}

					int x = ( tr[ct].x() + tr[ct].width()/2 ) - titleRect.width()/2;
					p->drawText( x, tr[ct].y()+tr[ct].height() + m_font.pointSize() + 3,
						item->text( ID_NAME ) );
				}
				ct++;
			}
			it++;
		}

		if ( !m_bgonly || m_preview )
		{
			// prev menu
			int ps = m_font.pointSize();
			m_font.setPointSize( ps - 8 );
			p->setFont( m_font );
			if ( m_currentpage > 0 )
			{
				p->drawText( 30, 550, "<<Prev" );
			}
			// next menu
			if ( m_currentpage != m_totalpages - 1 )
			{
				p->drawText( 600, 550, "Next>>" );
			}
			m_font.setPointSize( ps );
		}
	}
}
