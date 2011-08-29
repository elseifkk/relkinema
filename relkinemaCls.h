/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
 *   elseifkk@gmail.com                                                    *
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
		double K2p ( double, double );
		double p2E ( double, double );
		void procK1_common ( int );
		void procK1();
		void procK1c();
		void setK1th();
		void setK1cth();
		void setReactionLbl();
		void setResultBox ( bool );
		bool checkRectCond();
		void setE3c ( double );
		void initResultBox();
		void initEmissionAngleBox();
		void showAngleLE ( double, QLineEdit*, QString prep="" );
		void setThetaMax();
		void setExmax ( double );
		double x2Rad ( double d );
		double getTheta3CM ( double );
		void setK3 ( double );
		void showValueLE ( double, QLineEdit* );
		void setJ34 ( double );
		void initThetaBar();
		double tcm2q ( double );
		double q2tcm ( double );
		void setth3 ( double );
		void setth4 ( double );
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
		double theta3ToTheta3CM ( double );
		double theta3CMToTheta3 ( double );
		void setThetaBarStep ( double min, double max, double d, double v );
		void showKp3();
		void showKp4();
		void showKp34c();
		void showTheta4();
		void setK3vCM ( double );
		void setK4 ( double );
		double getThetaBarPosInRad();
		double getThetaBarPos();

	public:
		double th3,th4;
		double th3c,th4c;
		double K3,K4;
		double p3,p4;
		double J3,J4;
		double kinemaShift, factorK;
		double theq; // momentum transfer

	private:
		QString CONFIGFILE;
		double theM;
		double K1th,K1cth,Ex,Exmax;
		double p1th,p1cth;
		double E1,E2,E3,E4;
		double K1,K2;
		double p1,p2;
		double E1c,E2c,E3c,E4c;
		double K1c,K2c,K3c,K4c;
		double p1c,p2c,p3c,p4c;
		double gamma,beta;
		double thetaMax, limThetaCM, limq;
		double m10,m20,m30,m40; // original mass
		double m1,m2,m3,m4; // Projectile, Target, Ejectile, Residual
		int a1,a2,a3,a4;
		int z1,z2,z3,z4;
		double QValue;
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

	public slots:
		void thetaBarSlot();

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
		void confAppSlot();
		void redoSlot();
		void returnSlot();
		void stopSlot();
		void teDone ( resultWindowCls* );
		void massDataDirSlot();

	protected:
		void timerEvent ( QTimerEvent *e );
		void closeEvent ( QCloseEvent *e );
		void keyPressEvent ( QKeyEvent *e );
};

#endif
