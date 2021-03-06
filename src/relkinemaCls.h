/***************************************************************************
 *   Copyright (C) 2011-2014 by Kazuaki Kumagai                            *
 *   elseifkk@users.sf.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef RELKINEMACLS_H
#define RELKINEMACLS_H

#include "relkinemaDlg.h"
#include "resultwindowcls.h"
#include "rwthreadcls.h"

#include <qmutex.h>

class RelKinemaCls: public RelKinemaDlg
{
		Q_OBJECT

	public:
		RelKinemaCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0,
		               const char *v=0, QString conf="" );

	private:
		double getMass ( QString a, QString n, int *A, int *Z, bool calc=false );
		double readMass ( QString f, QString n, bool *ok=NULL );
		void setMass ( int id, bool comp=true );
		void reloadMass_all();
		void unsetMass ( int id, bool clear=false );
		void showMass ( double, QLineEdit * );
		void compRect();
		int count4 ( bool *b );
		void setQValue();
		void showEnergyLE ( double, QLineEdit * );
		void showEnergyL ( double, QLineEdit *, QString prep="" );
		void initReactionConditionBox();
		void procK1_common ( int );
		void procK1();
		void procK1c();
		void setK1th();
		void setK1cth();
		void setReactionLbl();
		void setResultBox ( bool );
		bool checkRectCond();
		void setE3c();
		void initResultBox();
		void initEmissionAngleBox();
		void showAngleLE ( double, QLineEdit*, QString prep="" );
		void setThetaMax();
		void setExmax();
		void setK3();
		void showValueLE ( double, QLineEdit* );
		void setJ34();
		void initThetaBar();
		void setth3();
		void setth4();
		void setAngle ( bool );
		double calcMass ( int, int );
		void massTypeProc ( int id, QComboBox*,QLineEdit* );
		void seeIfMassSet();
		void massEditProc ( int id, QLineEdit*, double* );
		void loadConfig ( QString );
		void saveConfig ( QString );
		double stripMass ( double m, int z );
		void updateThetaBar();
		void setSpecParam();
		void initSettingsPage ( bool first=false );
		bool checkDoubleNumberFormat ( QString );
		void showMessL ( QString,QString,QLabel* );
		void reinitRelKinema();
		int adjCol ( bool*,int,int* );
		double getExpValue ( double x );
		void showThetaMax();
		void showStrLE ( QString s, QLineEdit *b );
		QString getAngleUnit();
		void setThetaBarStep ( double min, double max, double d, double v );
		void showKp3();
		void showKp4();
		void showKp34c();
		void showTheta4();
		void setK3vCM();
		void setK4();
		double getThetaBarPos();
		void resetThetaLab();
		void showMenu();
		void setReaction ( QString );
		void nameToAE ( QString, QString*, QString* );
		void registParams ( size_t, size_t );
		void registParam ( size_t, size_t, QString );
		void exprWarn ( int,QString );
		void setPlottables ( int );
		void setScrTypeFont();
		void setTitleFont();
		void setActiveTextColLE ( QString,QLineEdit* );
		bool setq ( double );
		bool setTheta3 ( double );
		bool setTheta3c ( double );
		void openMdm ( int page=0 );
		void setParMass();
		void changeFontSize(int);
		void setEunitLabel();

	private:
		size_t prkc;
		size_t pfzc;
		size_t prkc_dmy;
		size_t pfzc_dmy;
		QString CONFIGFILE;
		double K1th,K1cth,Ex,Exmax;
		double p1th,p1cth;
		double E1,E2,E3,E4;
		double K1,K2,K3,K4;
		double p1,p2,p3,p4;
		double E1c,E2c,E3c,E4c;
		double K1c,K2c,K3c,K4c;
		double p1c,p2c,p3c,p4c;
		double gamma,beta;
		double th3,th4,theq;
		double th3c,th4c;
		double J3,J4;
		double kinemaShift, factorK;
		double thetaMax, thetaMaxCM;
		double m10,m20,m30,m40; // original mass
		double m1,m2,m3,m4; // Projectile, Target, Ejectile, Residual
		int a1,a2,a3,a4;
		int z1,z2,z3,z4;
		double QValue;
		QString rectCondSym;
		double qmin,qmax;
		bool inv,thetaMaxNe;
		bool K1Set;
		bool ExOk;
		int K3sign;
		int timerid;
		bool massSet[4];
		bool rectSet[4];
		bool flip[8];
		QMutex timerProcMt;
		int flipoff[9];
		bool whitedone[9];
		QString nc1,nc2,nc3,nc4;
		QString theReaction;
		bool exprSet[4];
		KHistoryCombo *exprBox[4];
		QLineEdit *valBox[4];
		QCheckBox *plotBox[4];
		QCheckBox *plotXBox[nrkpmax];
		int plotmask;
		QString myname;

	private slots:
		void setMassSlot_0();
		void setMassSlot_1();
		void setMassSlot_2();
		void setMassSlot_3();
		void unsetMassSlot_0();
		void unsetMassSlot_1();
		void unsetMassSlot_2();
		void unsetMassSlot_3();
		void unsetMassSlot_all();
		void setMassSlot_all();
		void changeMassUnitSlot();
		void rectCondSlot_K1();
		void rectCondSlot_K1c();
		void rectCondSlot_Ex();
		void rectCondSlot_p1();
		void rectCondSlot_p1c();
		void changeEUnitSlot();
		void changeAUnitSlot();
		void thetaBarSlot();
		void thetaCMSlot();
		void thetaLabSlot();
		void qSlot();
		void thetaCMSlot ( bool internal );
		void thetaLabSlot ( bool internal );
		void qSlot ( bool internal );
		void mass1EditSlot();
		void mass2EditSlot();
		void mass3EditSlot();
		void mass4EditSlot();
		void scrTypeSlot();
		void anotherSolutionSlot();
		void mass4TypeSlot();
		void mass3TypeSlot();
		void mass2TypeSlot();
		void mass1TypeSlot();
		void stripSlot();
		void thetaStepSlot();
		void showResultListSlot();
		void mdmSlot();
		void showAbout();
		void saveConfSlot();
		void settingsSlot();
		void toggleSettingsSlot();
		void confAppSlot();
		void redoSlot();
		void returnSlot();
		void teDone ();
		void massDataDirSlot();
		void calcSlot();
		void reverseSlot();
		void inverseSlot();
		void swapIniSlot();
		void swapFinSlot();
		void saveRectCondSlot();
		void loadRectCondSlot();
		void fontSlot ( const QFont & );
		void checkConfSlot();
		void checkAppSlot();
		void setDispFont();
		void loadDefConfSlot();
		void exprSlot ( int );
		void expr1Slot();
		void expr2Slot();
		void expr3Slot();
		void expr4Slot();
		void exprSetSlot ( int, bool setHist=true );
		void expr1SetSlot();
		void expr2SetSlot();
		void expr3SetSlot();
		void expr4SetSlot();
		void expr1SelectSlot();
		void expr2SelectSlot();
		void expr3SelectSlot();
		void expr4SelectSlot();
		void updateExprSlot();
		void checkAllPlotSlot();
		void checkNonePlotSlot();
		void clearHistSlot();
		void updateMassSlot();
		void sfSlot();
		void efSlot();
		void fsSlot();
		void ifSlot();
		void ffSlot();
		void afSlot();


	protected:
		void timerEvent ( QTimerEvent *e );
		void closeEvent ( QCloseEvent *e );
		void keyPressEvent ( QKeyEvent *e );
		void mousePressEvent ( QMouseEvent * );
};

#endif
