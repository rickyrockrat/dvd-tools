#ifdef KDE_SUBMGMT
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <krootpixmap.h>
#else
#include <qapplication.h>
#endif
#include "wndsub.h"

int main( int argc, char ** argv )
{
#ifdef KDE_SUBMGMT
	KCmdLineArgs::init(argc, argv, "submgmt", "sub",
			        "subtitle manager", "1.0.0" );
    KApplication a( argc, argv );
#else
    QApplication a( argc, argv );
#endif
    WndSub w;
	a.setMainWidget( &w );

    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
