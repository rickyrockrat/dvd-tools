#include <QApplication>
#include "qt4dvdaudiownd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    qt4dvdaudioWnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
