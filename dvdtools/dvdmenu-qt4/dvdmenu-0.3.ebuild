# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /500g/src/dvdtools/cvs/dvdtools/dvdmenu-qt4/dvdmenu-0.3.ebuild,v 1.1 2009-06-18 13:57:44 savignien Exp $

inherit eutils python

DESCRIPTION="DVD menu maker tool"
HOMEPAGE="http://dvdtools.sourceforge.net/"
#SRC_URI="http://${PN}.googlecode.com/files/${P}.tar.gz"
SRC_URI="http://sourceforge.net/project/downloading.php?group_id=184940&filename=${P}.tar.gz&a=21454661"

# IUSE="tk wxwindows"
DEPEND="x11-libs/qt >= 4.4.2"
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

src_install() {
	emake DESTDIR="${D}" install || die "make install died"

}

