#include <QFileDialog>
#include <QDir>

#include "qt4newtitlewnd.h"

qt4newtitleWnd::qt4newtitleWnd( QWidget *parent)
	: QDialog( parent )
{
	setupUi( this );
	QStringList lang;

	lang << " ";
	lang << "en";
	lang << "fr";
	lang << "it";
	lang << "es";
	lang << "jp";
	lang << "de";
	cbAudio1->clear();
	cbAudio1->addItems( lang );
	cbAudio2->clear();
	cbAudio2->addItems( lang );
	cbSub1->clear();
	cbSub1->addItems( lang );
	cbSub2->clear();
	cbSub2->addItems( lang );
	cbSub3->clear();
	cbSub3->addItems( lang );
	cbSub4->clear();
	cbSub4->addItems( lang );

	QStringList as;

	as << "4:3";
	as << "16:9";
	cbAspect->clear();
	cbAspect->addItems( as );
	

	connect( pbOpenPicture, SIGNAL(clicked()),
		this, SLOT(openPic()));
	connect( pbCancel, SIGNAL(clicked()),
		this, SLOT(reject()));
	connect( pbOk, SIGNAL(clicked()),
		this, SLOT(acceptDialog()));
}

void qt4newtitleWnd::acceptDialog()
{
	title = leTitle->text();
	snapshot = lePicture->text();
	aspect = cbAspect->currentText();
	audio1 = cbAudio1->currentText();
	audio2 = cbAudio2->currentText();
	sub1 = cbSub1->currentText();
	sub2 = cbSub2->currentText();
	sub3 = cbSub3->currentText();
	sub4 = cbSub4->currentText();
	accept();
}

void qt4newtitleWnd::openPic()
{
	QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a picture for the title",
                    QDir::currentPath(),
                    "Picture (*.png *.jpg)"
		    );
	if ( !s.isNull() )
		lePicture->setText( s );
}
