

#include "prefswnd.h"
#include <QSettings>

#include <iostream>
#include <QtDebug>

prefswnd::prefswnd( QWidget *parent )
	: QDialog( parent )
{
	setupUi(this);

	connect( pbOk, SIGNAL(clicked()),
			this, SLOT(saveExit()));
	connect( pbCancel, SIGNAL(clicked()),
			this, SLOT(reject()));
	connect( ckEnableProxy, SIGNAL(stateChanged(int)),
			this, SLOT(toggleProxy()));
	readPrefs();
}

void prefswnd::toggleProxy()
{
	gbProxy->setEnabled( ckEnableProxy->isChecked() );
}

void prefswnd::saveExit()
{
	QSettings prefs( "dvdtools","recorder");

	prefs.setValue("proxy/enabled", ckEnableProxy->isChecked() );
	if ( ckEnableProxy->isChecked( ) )
	{
		prefs.setValue("proxy/host", leProxyHost->text() );
		prefs.setValue("proxy/port", sbProxyPort->value() );
		prefs.setValue("proxy/user", leProxyUser->text() );
		prefs.setValue("proxy/password", leProxyPassword->text() );
	}
	prefs.setValue("misc/vidlinkre", leLinkRE->text() );
	prefs.setValue("misc/vidlistre", leListRE->text() );
	prefs.setValue("misc/defaulturl", leDefaultUrl->text() );
	prefs.setValue("misc/maxretries", sbRetries->value() );
	prefs.setValue("misc/minsize", sbMinSize->value() );

	accept();
}

void prefswnd::readPrefs()
{
	QSettings prefs( "dvdtools","recorder");
	qDebug() << prefs.fileName();
	bool proxyEnabled = prefs.value("proxy/enabled").toBool();
	QString proxyHost;
	int proxyPort;
	QString proxyUser;
	QString proxyPassword;

	if ( proxyEnabled )
	{
		proxyHost = prefs.value("proxy/host").toString();
		proxyPort = prefs.value("proxy/port").toInt();
		proxyUser = prefs.value("proxy/user").toString();
		proxyPassword = prefs.value("proxy/password").toString();
	}
	QString vidLinkRE = prefs.value("misc/vidlinkre").toString();
	QString vidListRE = prefs.value("misc/vidlistre").toString();
	QString defaultUrl = prefs.value("misc/defaulturl").toString();
	int retries = prefs.value("misc/maxretries").toInt();
	int minSize = prefs.value("misc/minsize").toInt();

	ckEnableProxy->setChecked( proxyEnabled );
	gbProxy->setEnabled( proxyEnabled );
	leProxyHost->setText( proxyHost );
	sbProxyPort->setValue( proxyPort );
	leProxyUser->setText( proxyUser );
	leProxyPassword->setText( proxyPassword );
	leLinkRE->setText( vidLinkRE );
	leListRE->setText( vidListRE );
	leDefaultUrl->setText( defaultUrl );
	sbRetries->setValue( retries );
	sbMinSize->setValue( minSize );
}

