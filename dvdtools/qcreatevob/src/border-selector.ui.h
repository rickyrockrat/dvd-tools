#include <qsplashscreen.h>
#include <qdir.h>
#include <qapplication.h>


void borderSelector::main( const QString &file, const QString &border, const QString &X, const QString &Y )
{
  myScr = new scrclass();
  inFile = file;

  QString BL = border;
  BL.replace( QRegExp( "(.*),(.*),(.*),(.*)" ), "\\2" );
  selLeft->setValue ( filter( BL ) );

  QString BT = border;
  BT.replace( QRegExp( "(.*),(.*),(.*),(.*)" ), "\\1" );
  selTop->setValue ( filter( BT ) );

  QString BR = border;
  BR.replace( QRegExp( "(.*),(.*),(.*),(.*)" ), "\\4" );
  selRight->setValue ( filter( BR ) );

  QString BB = border;
  BB.replace( QRegExp( "(.*),(.*),(.*),(.*)" ), "\\3" );
  selButtom->setValue ( filter( BB ) );

  XStr = X;
  YStr = Y;
  setWindow( X.toInt(), Y.toInt() );
  setEditor( X.toInt(), Y.toInt() );
  getImage();

}

/**
 * Window geometry
 * @param X 
 * @param Y 
 */
void borderSelector::setWindow( int X, int Y )
{
  if ( X < 445 )
  {
    X = 445;
  }
  resize( QSize( X + 4, Y + 120 ) );
  frame14->setGeometry( 1, Y + 2, X, 104 );
}

/**
 * Set editor
 * @param X 
 * @param Y 
 * @param border 
 */
void borderSelector::setEditor( int X, int Y )
{
  QString BL = selLeft->text();
  QString BT = selTop->text();
  QString BR = selRight->text();
  QString BB = selButtom->text();

  if ( X < 445 )
  {
    editorFrame->setGeometry( ( 445 - X ) / 2, 1, X, Y );
  }
  else
  {
    editorFrame->setGeometry( 1, 1, X, Y );
  }
  imageLabel->setGeometry( 0 + borderLeft( BL.toInt() ),
                           0 + borderTop( BT.toInt() ),
                           X + borderRight( BR.toInt(), BL.toInt() ),
                           Y + borderButtom( BB.toInt(), BT.toInt() ) );

}

int borderSelector::borderTop( int top )
{
  int out;
  out = top * ( -1 );
  return int( out );
}

int borderSelector::borderLeft( int left )
{
  int out;
  out = left * ( -1 );
  return int( out );
}

int borderSelector::borderRight( int right, int left )
{
  int out;
  out = right + left;
  return int( out );
}

int borderSelector::borderButtom( int buttom, int top )
{
  int out;
  out = buttom + top;
  return int( out );
}

void borderSelector::borderChanged()
{
  QString BL = selLeft->text();
  QString BT = selTop->text();
  QString BR = selRight->text();
  QString BB = selButtom->text();
  int X = XStr.toInt();
  int Y = YStr.toInt();

  imageLabel->setGeometry( 0 + borderLeft( BL.toInt() ),
                           0 + borderTop( BT.toInt() ),
                           X + borderRight( BR.toInt(), BL.toInt() ),
                           Y + borderButtom( BB.toInt(), BT.toInt() ) );
}

int borderSelector::filter( const QString &str )
{
  double tmp = str.toDouble();
  int out = str.toInt();
  tmp = tmp / 2;
  QString s; s.setNum( tmp );

  if ( s.find( "." ) != -1 )
  {
    out = out - 1;
  }
  return int( out );
}

void borderSelector::getImage()
{

  QString splashPath = qApp->applicationDirPath();
  splashPath.replace( QRegExp( "(.*/)(.*)" ), "\\1" );
  QString slpashscreen;
  QDir sp( qApp->applicationDirPath() );
  if ( sp.exists( "images" ) )
  {
    slpashscreen = qApp->applicationDirPath() + "/images/splash.png";
  }
  else
  {
    slpashscreen = splashPath + "share/qcreatevob/images/splash.png";
  }

  QSplashScreen * splash
  = new QSplashScreen( QPixmap::fromMimeSource( slpashscreen ) );
  splash->show();
  splash->message( QObject::tr( "Getting screenshot..." ),
                   Qt::AlignRight | Qt::AlignTop, Qt::white );


  QString range = getFrameRange();
  QString outFile = getenv( "HOME" );
  outFile.append( "/qcreatevob-shot" );

  QString runScript = "transcode -i \"%inputFile\" -c %range --progress_off -y im,null -V -o \"%outFile\"";
  runScript.replace( "%inputFile", inFile )
  .replace( "%range", range )
  .replace( "%outFile", outFile );


  if ( myScr->runScript( runScript ) )
  {
    loadImage( QString( getenv( "HOME" ) ).append( "/qcreatevob-shot000000.jpg" ) );
  }
  else
  {
    imageLabel->setText( "Can't get q screenshot,\nplease take a screenshot manualy!" );
  }
  delete splash;
}

void borderSelector::loadImage( const QString &str )
{
  QString myImage = str;
  QImage image( myImage );
  imageLabel->setPixmap( image );
}

QString borderSelector::getFrameRange()
{
  QString frame1 = frameNr->text();
  int getFrame2 = frame1.toInt() + 1;
  QString frame2; frame2.setNum( getFrame2 );
  return QString( frame1 + "-" + frame2 );
}

void borderSelector::openFile()
{
  QString filters = "*.jpg *.png *.bmp";
  QString newImage =
    QFileDialog::getOpenFileName( getenv( "HOME" ), filters, this );
  if ( !newImage.isEmpty() )
  {
    loadImage( newImage );
  }
}

void borderSelector::remove()
{
  if ( QFile::remove( QString( getenv( "HOME" ) ).append( "/qcreatevob-shot000000.jpg" ) ) == false )
    {
      QMessageBox::warning( this, "QCreateVOB", "Can't remove screenshot\n\"" + QString( getenv( "HOME" ) ).append( "/qcreatevob-shot000000.jpg" ) + "\"\n please remove is manulay" );
    }
}


