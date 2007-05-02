#include <qapplication.h>
#include <qfile.h>
#include <qlineedit.h>
#include "wndsub.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    WndSub w;
	a.setMainWidget( &w );

	if ( argc == 2 )
	{
		if ( QFile::exists( argv[1] ) )
		{
			w.leInputFile->setText( argv[1] );
			w.autoDetectFormat();
			w.loadSubFile();
			w.showInputSubs();
		}
	}
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
