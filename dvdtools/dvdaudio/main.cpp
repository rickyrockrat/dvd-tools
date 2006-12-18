#include <qapplication.h>
#include "dvdaudiownd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    dvdaudioWnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
