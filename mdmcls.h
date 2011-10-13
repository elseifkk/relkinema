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
#ifndef MDMCLS_H
#define MDMCLS_H

#include "mdmDlg.h"

// PDG (2009)
#define Mn_DEF   939.565346
#define Mp_DEF   938.272013
#define Me_DEF   0.510998910
// NIST
#define Md_DEF 1875.612793
#define Mt_DEF 2808.920906
#define Ma_DEF 3727.379240

int const PID_GAMMA    =0;
int const PID_ELECTRON =1;
int const PID_NEUTRON  =2;
int const PID_PROTON   =3;
int const PID_DEUTERON =4;
int const PID_TRITON   =5;
int const PID_ALPHA    =6;

double const ParticleM[]=
{
	0,
	Me_DEF,
	Mn_DEF,
	Mp_DEF,
	Md_DEF,
	Mt_DEF,
	Ma_DEF
};

int const ParticleZ[]=
{
	0,
	-1,
	0,
	1,
	1,
	1,
	2,
};

int const ParticleA[]=
{
	0,
	0,
	1,
	1,
	2,
	3,
	4,
};

static const char *Particle[]=
{
	"g",
	"e",
	"n",
	"p",
	"d",
	"t",
	"a",
	0
};

int const Zmax=117;
static const char *Element[]=
{
	"H",
	"He",
	"Li",
	"Be",
	"B",
	"C",
	"N",
	"O",
	"F",
	"Ne",
	"Na",
	"Mg",
	"Al",
	"Si",
	"P",
	"S",
	"Cl",
	"Ar",
	"K",
	"Ca",
	"Sc",
	"Ti",
	"V",
	"Cr",
	"Mn",
	"Fe",
	"Co",
	"Ni",
	"Cu",
	"Zn",
	"Ga",
	"Ge",
	"As",
	"Se",
	"Br",
	"Kr",
	"Rb",
	"Sr",
	"Y",
	"Zr",
	"Nb",
	"Mo",
	"Tc",
	"Ru",
	"Rh",
	"Pd",
	"Ag",
	"Cd",
	"Sn",
	"Sb",
	"Te",
	"I",
	"Xe",
	"Cs",
	"Ba",
	"La",
	"Ce",
	"Pr",
	"Nd",
	"Pm",
	"Sm",
	"Eu",
	"Gd",
	"Tb",
	"Dy",
	"Ho",
	"Er",
	"Tm",
	"Yb",
	"Lu",
	"Hf",
	"Ta",
	"W",
	"Re",
	"Os",
	"Ir",
	"Pt",
	"Au",
	"Hg",
	"Tl",
	"Pb",
	"Bi",
	"Po",
	"At",
	"Rn",
	"Fr",
	"Ra",
	"Ac",
	"Th",
	"Pa",
	"U",
	"Np",
	"Pu",
	"Am",
	"Cu",
	"Bk",
	"Cf",
	"Es",
	"Fm",
	"Md",
	"No",
	"Lr",
	"Rf",
	"Db",
	"Sg",
	"Bh",
	"Hs",
	"Mt",
	"Ds",
	"Rg",
	"Cn",
	"Uut",
	"Uuq",
	"Uup",
	"Uuh",
	"Uus",
	"Uuo",
	0
};
int const Zmin_new=110; // of Ea, should be Ds
static const char *newElement[]=
{
	"Ea",
	"Eb",
	"Ec",
	"Ed",
	"Ee",
	"Ef",
	"Eg",
	"Eh",
	"Ei",
	0
};

class mdmCls: public massDataManager
{
		Q_OBJECT
	public:
		mdmCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0 );
		void openMassData ( int A );

	public:
		QString home;
		double AMU; // in MeV
		double Me,Mn,Mp,Md,Mt,Ma; // in MeV&&
		QString massdata;
		QString wb;

	private:
		void showMass ( QString m );
		void setReady ( bool );
		double readMass ( double );
		void mess ( QString m, QString col="black" );
		int applyChanges ( QTextStream*, bool );
		void warn ( QString );
		void warnDiscard();
		void stopTimer();

	private:
		QString currentFile;
		bool currentFileExists;
		int currentA;
		QStringList MList;
		int discardOk;
		int timer;
		QString warnmess;
		bool blink;

	public:
		static int getZ ( QString );
		static int getParticleAZ ( QString n, int *a, int *z );

	private:
		static int getZ_from_List ( QString, const char** );

	private slots:
		void EBoxSlot();
		void changeUnitSlot();
		void changeASlot();
		void delSlot();
		void neweSlot();
		void massChangedSlot();
		void EChangedSlot();
		void extMassDataSlot();
		void openMassDataFileSlot();
		void openMassDataDirSlot();
		void createMassDataSlot();
		void gotoamdcSlot();
		void editDataSlot();
		void appSlot();
		void massDataDirSlot();
		void massDataFileSlot();

	protected:
		void timerEvent ( QTimerEvent *e );
		void keyPressEvent ( QKeyEvent *e );
		void closeEvent ( QCloseEvent *e );
};

#endif
