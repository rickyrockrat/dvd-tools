
#ifndef _SUBLISTBOXITEM_H
#define _SUBLISTBOXITEM_H

class SubListBoxItem : public QListBoxItem
{
public :
	SubListBoxItem(Subtitle &, QListBox * );
	Subtitle &getSubtitle();
protected:
	void paint(QPainter *);
	int height ( const QListBox * ) const;
	int width ( const QListBox * ) const;
private:
	Subtitle *_subtitle;
};

#endif /* _SUBLISTBOXITEM_H */
