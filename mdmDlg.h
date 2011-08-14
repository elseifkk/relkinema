/****************************************************************************
** Form interface generated from reading ui file './mdmDlg.ui'
**
** Created: 日  8月 14 19:23:52 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MASSDATAMANAGER_H
#define MASSDATAMANAGER_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;
class QWidgetStack;
class QSpinBox;
class QListBox;
class QListBoxItem;
class QComboBox;
class QLineEdit;
class QCheckBox;
class KURLLabel;
class QFrame;

class massDataManager : public QWidget
{
    Q_OBJECT

public:
    massDataManager( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~massDataManager();

    QPushButton* createMassDataBut;
    QPushButton* editDataBut;
    QLabel* msgLbl;
    QWidgetStack* wStack;
    QWidget* WStackPage;
    QSpinBox* ABox;
    QListBox* EBox;
    QPushButton* newBut;
    QPushButton* delBut;
    QPushButton* appBut;
    QLabel* rawLbl;
    QComboBox* MUnitBox;
    QLineEdit* massBox;
    QCheckBox* showExcessBox;
    QLineEdit* EeditBox;
    QLabel* ZLbl;
    QLabel* textLabel2_2;
    QLabel* textLabel2;
    QWidget* WStackPage_2;
    QLabel* textLabel1_2;
    QPushButton* openMassDataDirBut;
    QPushButton* extMassDataBut;
    QLabel* textLabel1;
    QPushButton* openMassDataFileBut;
    QLineEdit* massDataFileBox;
    QLabel* textLabel1_3;
    KURLLabel* gotoamdc;
    QLineEdit* massDataDirBox;
    QFrame* line1;
    QPushButton* exitBut;

public slots:
    virtual void delSlot();
    virtual void changeASlot();
    virtual void changeUnitSlot();
    virtual void EBoxSlot();
    virtual void massChangedSlot();
    virtual void EChangedSlot();
    virtual void neweSlot();
    virtual void appSlot();
    virtual void editDataSlot();
    virtual void createMassDataSlot();
    virtual void openMassDataDirSlot();
    virtual void openMassDataFileSlot();
    virtual void extMassDataSlot();
    virtual void massDataFileSlot();
    virtual void massDataDirSlot();
    virtual void gotoamdcSlot();

protected:

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

#endif // MASSDATAMANAGER_H
