/****************************************************************************
** Form interface generated from reading ui file './resultWindow.ui'
**
** Created: 日  8月 14 19:23:55 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;
class QSplitter;
class QTextEdit;
class QTable;

class resultWindow : public QWidget
{
    Q_OBJECT

public:
    resultWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~resultWindow();

    QPushButton* saveasBut;
    QPushButton* plotBut;
    QPushButton* redBut;
    QPushButton* magBut;
    QPushButton* hideDescBut;
    QLabel* msgLbl;
    QPushButton* exitBut;
    QSplitter* splitter1;
    QTextEdit* resultDescBox;
    QTable* resultTable;

public slots:
    virtual void saveasSlot();
    virtual void plotSlot();
    virtual void redSlot();
    virtual void magSlot();
    virtual void hideDescSlot();

protected:
    QGridLayout* resultWindowLayout;
    QHBoxLayout* layout6;
    QSpacerItem* spacer1_2;
    QSpacerItem* spacer1_2_2;
    QSpacerItem* spacer1_4;
    QSpacerItem* spacer1;
    QSpacerItem* spacer1_3;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;

};

#endif // RESULTWINDOW_H
