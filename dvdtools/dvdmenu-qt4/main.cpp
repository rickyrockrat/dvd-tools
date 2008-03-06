#include <qapplication.h>
#include "qt4dvdmenuwnd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    qt4dvdmenuWnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
