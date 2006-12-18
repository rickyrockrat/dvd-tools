void scriptList::addToList(QString name, QString item)
{
  scriptListBox->insertItem(name);
  itmeStrList << item;
}

void scriptList::removeFromList()
{
  if (scriptListBox->currentItem() != -1)
    {
      int curInt = scriptListBox->currentItem();
      uint curItem = curInt;
      QStringList::Iterator it;
      it = itmeStrList.at( curItem );
      itmeStrList.remove(it);
      scriptListBox->removeItem(curInt);
    }
}

QString scriptList::showItem()
{
  QString out = "";
  if (scriptListBox->currentItem() != -1)
    {
      uint curItem = scriptListBox->currentItem();
      QStringList::Iterator it;
      it = itmeStrList.at( curItem );
      out = *it;
    }
  return QString(out);
}

void scriptList::replaceItem(int i, QString item)
{
  if (i >= 0)
    {
      uint curItem = i;
      QStringList::Iterator it;
      it = itmeStrList.at( curItem );
      itmeStrList.gres(*it, item);
    }
}

void scriptList::doubleClick()
{
  emit doubleClicked();
}

bool scriptList::findItemName(QString str)
{
  QListBoxItem* item = scriptListBox->findItem(str, ExactMatch);
  if (item)
    {
      return true;
    }
  else
    {
      return false;
    }
  return false;
}

int scriptList::currentItem()
{
  return int(scriptListBox->currentItem());
}

bool scriptList::isEmpty()
{
  if (QString(scriptListBox->text(-1)).isEmpty())
    {
      return true;
    }
  return false;
}
