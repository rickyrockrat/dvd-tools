#include <QApplication>
#include <QFile>
#include <QLineEdit>
#include <QTextStream>
#include "qt4subeditor.h"

#include <iostream>

/*
class MyApp : public QApplication
{
public:
	MyApp( int argc, char *argv[] ) : QApplication( argc, argv)
	{
	}
	~MyApp() {}
	bool notify( QObject * receiver, QEvent * event )
	{
		try
		{
		QApplication::notify( receiver, event );
		}
		catch( std::exception )
		{
			std::cout << "erreur" << std::endl;
		}
		return true;
	}
};
*/

int main( int argc, char ** argv )
{
	//MyApp a( argc, argv );
	QApplication a( argc, argv );
	qt4subeditor *w = new qt4subeditor(0);

	w->show();
	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	return a.exec();
}

//vim: ts=4 sw=4
