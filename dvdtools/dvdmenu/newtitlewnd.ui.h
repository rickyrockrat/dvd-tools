/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void newTitle::init()
{
	QStringList lang;

	lang << " ";
	lang << "en";
	lang << "fr";
	lang << "it";
	lang << "es";
	lang << "jp";
	lang << "de";
	cbAudio1->clear();
	cbAudio1->insertStringList( lang );
	cbAudio2->clear();
	cbAudio2->insertStringList( lang );
	cbSub1->clear();
	cbSub1->insertStringList( lang );
	cbSub2->clear();
	cbSub2->insertStringList( lang );
	cbSub3->clear();
	cbSub3->insertStringList( lang );
	cbSub4->clear();
	cbSub4->insertStringList( lang );

	QStringList as;

	as << "4:3";
	as << "16:9";
	cbAspect->clear();
	cbAspect->insertStringList( as );
	

	connect( pbOpenPicture, SIGNAL(clicked()),
		this, SLOT(openPic()));
	connect( pbCancel, SIGNAL(clicked()),
		this, SLOT(reject()));
	connect( pbOk, SIGNAL(clicked()),
		this, SLOT(acceptDialog()));
}

void newTitle::acceptDialog()
{
	title = leTitle->text();
	snapshot = lePicture->text();
	aspect = cbAspect->currentText();
	audio1 = cbAudio1->currentText().simplifyWhiteSpace();
	audio2 = cbAudio2->currentText().simplifyWhiteSpace();
	sub1 = cbSub1->currentText().simplifyWhiteSpace();
	sub2 = cbSub2->currentText().simplifyWhiteSpace();
	sub3 = cbSub3->currentText().simplifyWhiteSpace();
	sub4 = cbSub4->currentText().simplifyWhiteSpace();
	accept();
}

void newTitle::openPic()
{
	QString s = QFileDialog::getOpenFileName(
                    QDir::currentDirPath(),
                    "Picture (*.png;*.jpg)",
                    this,
                    "open file dialog",
                    "Choose a picture for the title" );
	if ( !s.isNull() )
		lePicture->setText( s );
}
