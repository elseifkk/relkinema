/****************************************************************************
** Form interface generated from reading ui file './kinemaPlot.ui'
**
** Created: 日  8月 14 19:23:52 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KINEMAPLOT_H
#define KINEMAPLOT_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;

class kinemaPlot : public QWidget
{
    Q_OBJECT

public:
    kinemaPlot( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~kinemaPlot();


protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // KINEMAPLOT_H
