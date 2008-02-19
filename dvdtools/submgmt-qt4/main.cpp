#include <QApplication>
#include <QFile>
#include <QLineEdit>
#include <QTextStream>
#include "qt4wndsub.h"

#include <iostream>

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

int main( int argc, char ** argv )
{
	MyApp a( argc, argv );
	qt4wndsub *w = new qt4wndsub(0);

	if ( argc == 2 )
	{
		if ( QFile::exists( argv[1] ) )
		{
			w->setInputFile( argv[1] );
			w->autoDetectFormat();
			w->loadSubFile(w->getInputFile());
			w->showInputSubs();
		}
	}
	w->show();
	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	return a.exec();
}

//vim: ts=4 sw=4
