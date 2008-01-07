
#ifndef _QT4EDITSUB.H
#define _QT4EDITSUB.H

#include <QDialog>
#include "subitemdelegate.h"
#include "ui_qt4editsub.h"

class qt4editsub : public QDialog, private Ui::qt4editsub
{
Q_OBJECT

public:
qt4editsub(QWidget *parent = 0);
void setSub( Subtitle sub );
Subtitle &getSub( );
void setup( );

Subtitle *subtitle;

public slots:
void deleteLineClicked( );
void insertLineClicked( );
void boldClicked( );
void italicClicked( );
void underlineClicked( );
void strikeoutClicked( );
void valid( );

};

#endif
