#include <QApplication>
#include "wndQrip.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    wndQrip w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
