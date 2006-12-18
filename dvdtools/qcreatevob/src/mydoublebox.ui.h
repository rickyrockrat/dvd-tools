void myDoubleBox::init()
{
    step = 0.1;
    maxValueD = 10.0;
    minValueD = 0.0;
    maxCharsNum = 1;
    setNum(1.0);
}

void myDoubleBox::setNum(double n)
{
    if (n < maxValueD && n > minValueD)
    {numBox->setText(doubleToText(n));}
    else
    {
	if (n >= maxValueD)
	{numBox->setText(doubleToText(maxValueD));}
	
	if (n <= minValueD)
	{numBox->setText(doubleToText(minValueD));}
    }
}

QString myDoubleBox::doubleToText(double num)
{
    QString s; s.setNum(num);
    return s;
}

void myDoubleBox::setStep(double st)
{
    step = st;
}

void myDoubleBox::setMaxValue(double max)
{
    maxValueD = max;
    if (currentValue() > max)
    {setNum(max);}
}

void myDoubleBox::setMinValue(double min)
{
    minValueD = min;
    if (currentValue() < min)
    {setNum(min);}
}

void myDoubleBox::upValue()
{
  double v = currentValue();
  double out = v+step;
  if (v <= maxValueD)
  {setNum(out);}
  
   if (currentValue() >= maxValueD )
    {setNum(maxValueD); upBtn->setDisabled(true);}
}


void myDoubleBox::downValue()
{
    if (currentValue() > minValueD)
    {setNum(currentValue()-step);}
  
    if (currentValue() <= minValueD )
    {setNum(minValueD); donwBtn->setDisabled(true);}
}


double myDoubleBox::currentValue()
{
    QString s = numBox->text();
 return s.toDouble();
}

QString myDoubleBox::currentText()
{
    return QString(numBox->text());
}

void myDoubleBox::swichValue()
{
    int pos = numBox->cursorPosition();
    QString s = currentText();
    QString cur = s.mid(pos-1, 1);
    setCaption(cur);
    if (cur.find(QRegExp("[0-9.]")) == -1)
    {
    s.remove(pos-1, 1);
    numBox->setText(s);
    numBox->setCursorPosition(pos-1);
    }
    cleanBox();
    cleanChars();
        
   if (currentValue() < maxValueD )
    {upBtn->setEnabled(true);}
   
   if (currentValue() > minValueD )
    {donwBtn->setEnabled(true);}
   setSliderValue(currentValue());
}

void myDoubleBox::setSliderValue(double i)
{
    QString num = QString::number(i*10);
    int value = num.toInt();
    slider->setValue(value);
}

void myDoubleBox::cleanBox()
{
    QString str = currentText();
    int dot = str.find(".");
    int dot2 = str.find(".", dot+1);
    int pos = numBox->cursorPosition();
    int newPos = pos;
    
    if (dot != -1 && dot2 != -1)
    {
	if (dot == pos-1)
	{
	    str.remove(".");
	    str.insert(pos-1, ".");
	    newPos = pos;
	}
	else
	{
	    str.remove(".");
	    str.insert(pos-2, ".");
	   newPos = pos-1;
	}
	numBox->setText(str);
	numBox->setCursorPosition(newPos);
}
}


void myDoubleBox::setMaxChars(int chars)
{
maxCharsNum = chars;
cleanChars();
}

int myDoubleBox::maxChars()
{
return maxCharsNum;
}

void myDoubleBox::cleanChars()
{
    QString s = currentText();
    int dot = s.find(".");
    if (dot != -1)
    {
	dot = dot+1;
	int ok = dot+maxCharsNum;
	int end = s.length();
    
	if (ok < end)
	{
	    s.remove(ok, end-ok);
	    numBox->setText(s);
	}
    }
}


void myDoubleBox::fixNum()
{
if (currentValue() > maxValueD)
    {setNum(maxValueD);}

if (currentValue() < minValueD)
    {setNum(minValueD);}
}


void myDoubleBox::sliderChanged()
{
    double value = slider->value();
    double i = value/10;
    numBox->setText(QString::number(i));
}
