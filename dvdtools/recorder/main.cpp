#include <qapplication.h>
#include "recwnd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    recWnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
