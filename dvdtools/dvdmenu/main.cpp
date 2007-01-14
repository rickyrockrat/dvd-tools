#include <qapplication.h>
#include "dvdmenuwnd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    dvdmenuWnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
