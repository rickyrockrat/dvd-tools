
#ifndef _QT4GENPNGWND_H
#define _QT4GENPNGWND_H

#include "ui_qt4genpngwnd.h"
#include <QDialog>

class qt4genpngwnd : public QDialog, public Ui::qt4genpngwnd
{
Q_OBJECT

public:
qt4genpngwnd(QWidget *parent = 0);

public slots:
void selNormalFont();
void selItalicFont();
void selBoldFont();
void selSubColor();
};

#endif
