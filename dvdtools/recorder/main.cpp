#include <QApplication>
#include <QPainter>
#include <QtNetwork/QHttp>
#include "recwnd.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    recwnd w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
