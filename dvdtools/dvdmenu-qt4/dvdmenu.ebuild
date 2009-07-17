# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /500g/src/dvdtools/cvs/dvdtools/dvdmenu-qt4/dvdmenu.ebuild,v 1.1 2009-07-17 08:27:31 savignien Exp $

# inherit eutils python
EAPI=2

DESCRIPTION="DVD menu maker tool"
HOMEPAGE="http://dvdtools.sourceforge.net/"
SRC_URI="http://downloads.sourceforge.net/sourceforge/dvdtools/dvdmenu-0.4.tar.gz"
RESTRICT=mirror

DEPEND=">=x11-libs/qt-4.4.2"
RDEPEND="media-video/mjpegtools
	media-sound/toolame
	media-sound/sox
	media-video/dvdauthor"
KEYWORDS="amd64 x86"
LICENSE="GPL-2"
SLOT="0"

src_unpack() {
	unpack ${A}
	cd "${S}"
}

src_compile() {

	cd "${S}"
	qmake dvdmenu.pro || die "qmake failed"
	emake || die "make failed"
}

src_install() {
	emake DESTDIR="${D}" install || die "make install died"

}

