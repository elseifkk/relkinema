/****************************************************************************
** Form interface generated from reading ui file './about.ui'
**
** Created: 日  8月 14 19:23:51 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QLabel;
class QTextEdit;
class KURLLabel;
class QPushButton;

class aboutdlg : public QDialog
{
    Q_OBJECT

public:
    aboutdlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~aboutdlg();

    QFrame* line3;
    QLabel* aboutLogo;
    QTextEdit* textEdit1;
    QLabel* VersionLbl;
    QLabel* BuildLbl;
    KURLLabel* urlLbl;
    QLabel* textLabel5;
    QLabel* TitleLbl;
    QPushButton* close;
    QLabel* CopyleftLbl;

public slots:
    virtual void urlSlot();
    virtual void closeSlot();

protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;

};

#endif // ABOUTDLG_H
