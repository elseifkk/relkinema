/****************************************************************************
** Form interface generated from reading ui file './message.ui'
**
** Created: 日  8月 14 19:23:53 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MBOX_H
#define MBOX_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;

class mBox : public QWidget
{
    Q_OBJECT

public:
    mBox( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~mBox();


protected:

protected slots:
    virtual void languageChange();

};

#endif // MBOX_H
