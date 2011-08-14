/****************************************************************************
** Form interface generated from reading ui file './relkinemaDlg.ui'
**
** Created: 日  8月 14 19:23:54 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RELKINEMADLG_H
#define RELKINEMADLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QWidgetStack;
class QGroupBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QButtonGroup;
class QRadioButton;
class QScrollBar;
class KDoubleNumInput;
class QTabWidget;
class QTextEdit;

class RelKinemaDlg : public QWidget
{
    Q_OBJECT

public:
    RelKinemaDlg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~RelKinemaDlg();

    QPushButton* redoBut;
    QPushButton* exitBut;
    QPushButton* aboutBut;
    QPushButton* massdataBut;
    QWidgetStack* mainStack;
    QWidget* kinemaPage;
    QGroupBox* ReactionBox;
    QLabel* textLabel3_3_4_3;
    QLabel* textLabel3_3_4_3_3;
    QPushButton* ClearBut;
    QLineEdit* EResiBox;
    QLineEdit* AResiBox;
    QLabel* msgLbl;
    QLineEdit* EEjecBox;
    QLineEdit* AEjecBox;
    QLineEdit* EProjBox;
    QLineEdit* AProjBox;
    QLineEdit* ETargBox;
    QLineEdit* ATargBox;
    QLabel* textLabel3_3_4_2;
    QGroupBox* MassBox;
    QComboBox* mass2TypeBox;
    QComboBox* mass1TypeBox;
    QComboBox* mass3TypeBox;
    QComboBox* mass4TypeBox;
    QLineEdit* MResiLbl;
    QLineEdit* MTargLbl;
    QLineEdit* MProjLbl;
    QComboBox* MUnitBox;
    QLineEdit* MEjecLbl;
    QCheckBox* stripBox;
    QLabel* textLabel3_3_4_2_2;
    QPushButton* SetBut;
    QGroupBox* EmissionAngleBox;
    QGroupBox* RecoilAngleBox;
    QLabel* textLabel3_3_3_4_2;
    QLabel* textLabel3_3_3_2_3_2;
    QLineEdit* RThetaCMBox;
    QLineEdit* RThetaLabBox;
    QButtonGroup* buttonGroup1;
    QRadioButton* radBut;
    QRadioButton* degBut;
    QScrollBar* thetaBar;
    QLabel* textLabel1;
    QGroupBox* EmissionEnergyBox_2_3;
    QLineEdit* thetaMaxLbl;
    QLineEdit* ThetaCMBox;
    QLineEdit* ThetaLabBox;
    QLabel* textLabel3_3_3_2_3_3_2;
    QLabel* theCMLbl;
    QLabel* theqLbl;
    QLabel* theLabLbl;
    QLineEdit* qBox;
    QComboBox* scrTypeBox;
    QLabel* stepUnitLbl;
    QPushButton* stopBut;
    KDoubleNumInput* thetaStepBox;
    QGroupBox* ReactionConditionBox;
    QCheckBox* anotherSolutionBox;
    QGroupBox* IncidentEnergyBox;
    QLabel* textLabel3_3_3;
    QLabel* textLabel3_3_3_2;
    QLineEdit* K1cthLbl;
    QLineEdit* K1cBox;
    QLineEdit* K1Box;
    QLineEdit* K1thLbl;
    QGroupBox* IncidentMomentumBox;
    QLabel* textLabel3_3_3_5;
    QLabel* textLabel3_3_3_2_4;
    QLineEdit* p1cthLbl;
    QLineEdit* p1cBox;
    QLineEdit* p1Box;
    QLineEdit* p1thLbl;
    QLabel* textLabel4_2;
    QLineEdit* ExmaxLbl;
    QLineEdit* QValueBox;
    QLabel* textLabel4_2_2;
    QLineEdit* ExBox;
    QComboBox* EUnitBox;
    QLabel* textbetaLbl;
    QLineEdit* betaLbl;
    QLineEdit* gammaLbl;
    QLabel* textgammainvLbl;
    QLabel* textgammabetaLbl;
    QLineEdit* gammabetaLbl;
    QTabWidget* EmissionBox;
    QWidget* tab;
    QGroupBox* ejectileEnergyBox;
    QLineEdit* K3Box;
    QLineEdit* J3Box;
    QLabel* k3unitLbl;
    QLineEdit* K3cBox;
    QLabel* k3cunitLbl;
    QLabel* textLabel3_3_3_2_2_5;
    QLabel* textLabel3_3_3_2_2;
    QLabel* textLabel3_3_3_3;
    QGroupBox* recoilEnergyBox;
    QLineEdit* K4Box;
    QLabel* k4unitLbl;
    QLineEdit* J4Box;
    QLineEdit* K4cBox;
    QLabel* k4cunitLbl;
    QLabel* textLabel3_3_3_2_2_5_2;
    QLabel* textLabel3_3_3_2_2_3;
    QLabel* textLabel3_3_3_3_3;
    QWidget* tab_2;
    QGroupBox* EmissionEnergyBox_2_2_3;
    QLabel* textLabel3_3_3_3_4;
    QLabel* textLabel3_3_3_2_2_2;
    QLineEdit* p3cBox;
    QLineEdit* p3Box;
    QLabel* p3unitLbl;
    QLabel* p3cunitLbl;
    QGroupBox* EmissionEnergyBox_2_2_2_2;
    QLabel* textLabel3_3_3_3_3_2;
    QLabel* textLabel3_3_3_2_2_3_2;
    QLineEdit* p4cBox;
    QLineEdit* p4Box;
    QLabel* p4unitLbl;
    QLabel* p4cunitLbl;
    QGroupBox* specBox;
    QLineEdit* KparamBox;
    QLineEdit* dK3_dth3Box;
    QLabel* textLabel3_3_3_3_2_2;
    QLabel* textLabel3_3_3_3_2;
    QLabel* dK3_dth3_unitLbl;
    QLabel* KparamUnitLbl;
    QWidget* settingsPage;
    QLabel* rcLbl;
    QTextEdit* configEdit;
    QGroupBox* groupBox14;
    QLineEdit* confMassDataDirBox;
    QLineEdit* confDoubleNumberFormatBox;
    QLabel* textLabel1_3;
    QLabel* textLabel1_3_2;
    QLabel* confDoubleNumberFormatLbl;
    QLabel* confMassDataDirLbl;
    QPushButton* saveConfBut;
    QPushButton* confAppBut;
    QLabel* logoLbl;
    QLabel* verLbl;
    QLabel* pkgLbl;
    QLabel* textLabel2_2;
    QLabel* textLabel2_2_2;
    QPushButton* tableBut;
    QPushButton* settingsBut;

public slots:
    virtual void unsetMassSlot_0();
    virtual void unsetMassSlot_1();
    virtual void unsetMassSlot_2();
    virtual void unsetMassSlot_3();
    virtual void setMassSlot_0();
    virtual void setMassSlot_1();
    virtual void setMassSlot_2();
    virtual void setMassSlot_3();
    virtual void setMassSlot_all();
    virtual void unsetMassSlot_all();
    virtual void changeMassUnitSlot();
    virtual void rectCondSlot_K1();
    virtual void rectCondSlot_K1c();
    virtual void rectCondSlot_Ex();
    virtual void changeEUnitSlot();
    virtual void changeAUnitSlot();
    virtual void thetaLabSlot();
    virtual void thetaCMSlot();
    virtual void qSlot();
    virtual void thetaBarSlot();
    virtual void scrTypeSlot();
    virtual void anotherSolutionSlot();
    virtual void mass1TypeSlot();
    virtual void mass2TypeSlot();
    virtual void mass3TypeSlot();
    virtual void mass4TypeSlot();
    virtual void mass1EditSlot();
    virtual void mass1EnterSlot();
    virtual void mass2EditSlot();
    virtual void mass3EditSlot();
    virtual void mass4EditSlot();
    virtual void stripSlot();
    virtual void showResultListSlot();
    virtual void thetaStepSlot();
    virtual void mdmSlot();
    virtual void showAbout();
    virtual void rectCondSlot_p1c();
    virtual void rectCondSlot_p1();
    virtual void settingsSlot();
    virtual void confAppSlot();
    virtual void redoSlot();
    virtual void saveConfSlot();
    virtual void returnSlot();
    virtual void stopSlot();

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
    QPixmap image7;
    QPixmap image8;
    QPixmap image9;

};

#endif // RELKINEMADLG_H
