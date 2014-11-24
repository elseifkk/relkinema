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
#include "pix.h"
#include "relkinemaCls.h"
#include "resultwindowcls.h"
#include "mdmcls.h"
#include "aboutcls.h"
#include "rwthreadcls.h"
#include "rkcore.h"
#include "rkcalccls.h"
#include "rclistcls.h"
#include "linereadcls.h"
#include "misc.h"

#include <fzc.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

#include <qfile.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qtable.h>
#include <qcursor.h>
#include <qdialog.h>
#include <qcursor.h>
#include <qsplitter.h>
#include <qlayout.h>

#include <kconfig.h>
#include <knuminput.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kfontrequester.h>
#include <kcombobox.h>

int const LEN_ELEMENT = 8;

QFont const font_def ( "Helvetica",11 );
QFont thefont;

QString const FIREFOX = "firefox";
QString webbrowser=FIREFOX;

QString const CONFIGFILE_DEF = ".relkinemarc";
QString const MASSDATA_DEF   = "massdata";

QString HOME;
QString MASSDATA;
QString double_format;

QString const double_format_def="%.6g";

// NIST (2005)
double const hbarc_DEF = 197.326968;
double const AMU_DEF   = 931.494043;
double hbarc; // MeV fm
double AMU;   // in MeV
double Me,Mn,Mp,Mt,Md,Ma;

// Reference Unknown
double const r0_DEF    = 1.2;
double const e2_DEF    = 1.4406466;
double const Bv_DEF    = 15.56;
double const Bs_DEF    = 17.23;
double const Bsym_DEF  = 23.29;
double const Bp_DEF    = 12.00;
double const Bc_DEF    = 0.697;
double const hcRinf_DEF= 13.6056981;

double r0;
double e2;
double Bv;
double Bs;
double Bsym;
double Bp;
double Bc;
double hcRinf;

int const ST_THETALAB = 0;
int const ST_THETACM  = 1;
int const ST_Q        = 2;

int const MT_EXP = 0;
int const MT_CALC= 1;
int const MT_MAN = 2;

double MU[4];

double const EU[4]=
{
	1.0e3,
	1.0,
	1.0e-3,
	1.0e-6
};

double const eps=1.0e-9;

double ( RelKinemaCls::*getThetaBarValue ) ();
double *tbMax, *tbMin;
double fZero=0.;
double fPI=M_PI;
double *tbTarg;
void ( RelKinemaCls::*tbEval ) ( bool );

rwThreadCls *rwt=0;

double const thetaStepMin=0.0001;
int const thetaStepPrec = 4;
double const thetaStepMax = 180.;

RelKinemaCls::RelKinemaCls ( QWidget *parent, const char *name, WFlags wf,
                             const char *v, QString conf )
		:RelKinemaDlg ( parent, name, wf )
		,prkc ( rkc_init() ), pfzc ( fzc_init() )
		,prkc_dmy ( 0 ), pfzc_dmy ( 0 )
		,CONFIGFILE ( "" )
		,K1th ( 0. ),K1cth ( 0. ),Ex ( 0. ),Exmax ( 0. )
		,p1th ( 0. ),p1cth ( 0. )
		,E1 ( 0. ),E2 ( 0. ),E3 ( 0. ),E4 ( 0. )
		,K1 ( 0. ),K2 ( 0. ),K3 ( 0. ),K4 ( 0. )
		,p1 ( 0. ),p2 ( 0. ),p3 ( 0. ),p4 ( 0. )
		,E1c ( 0. ),E2c ( 0. ),E3c ( 0. ),E4c ( 0. )
		,K1c ( 0. ),K2c ( 0. ),K3c ( 0. ),K4c ( 0. )
		,p1c ( 0. ),p2c ( 0. ),p3c ( 0. ),p4c ( 0. )
		,gamma ( 0. ),beta ( 0. )
		,th3 ( 0. ),th4 ( 0. ), theq ( 0. )
		,th3c ( 0. ),th4c ( 0. )
		,J3 ( 0. ),J4 ( 0. )
		,kinemaShift ( 0. ), factorK ( 0. )
		,thetaMax ( 0. ), thetaMaxCM ( 0. )
		,m10 ( 0. ),m20 ( 0. ),m30 ( 0. ),m40 ( 0. )
		,m1 ( 0. ),m2 ( 0. ),m3 ( 0. ),m4 ( 0. )
		,a1 ( 0 ),a2 ( 0 ),a3 ( 0 ),a4 ( 0 )
		,z1 ( 0 ),z2 ( 0 ),z3 ( 0 ),z4 ( 0 )
		,QValue ( 0. )
		,qmin ( 0. ),qmax ( 0. )
		,inv ( false ),thetaMaxNe ( false )
		,K1Set ( false ), ExOk ( false )
		,K3sign ( 1 )
		,timerid ( 0 )
{
	bzero ( &massSet,sizeof ( massSet ) );
	bzero ( &rectSet,sizeof ( rectSet ) );
	bzero ( &exprSet,sizeof ( exprSet ) );

	QString ver;
	myname="RelKinema";
	if ( v!=NULL )
	{
		ver=v;
		myname+=" ver. "+QString ( v );
	}
	else
	{
		ver="unknown";
	}
	setCaption ( myname );
	ver.prepend ( "Release " );
	verLbl->setText ( ver );

	exprBox[0]=expr1Box;
	exprBox[1]=expr2Box;
	exprBox[2]=expr3Box;
	exprBox[3]=expr4Box;
	valBox[0]=val1Box;
	valBox[1]=val2Box;
	valBox[2]=val3Box;
	valBox[3]=val4Box;
	plotBox[0]=plot1Box;
	plotBox[1]=plot2Box;
	plotBox[2]=plot3Box;
	plotBox[3]=plot4Box;

	plotXBox[0]=plotThetaLabBox;
	plotXBox[1]=plotThetaCMBox;
	plotXBox[2]=plotqBox;
	plotXBox[3]=plotK3Box;
	plotXBox[4]=plotp3Box;
	plotXBox[5]=plotJ3Box;
	plotXBox[6]=plotKSBox;
	plotXBox[7]=plotKFBox;
	plotXBox[8]=plotTheta4LabBox;
	plotXBox[9]=plotTheta4CMBox;
	plotXBox[10]=plotK4Box;
	plotXBox[11]=plotp4Box;
	plotXBox[12]=plotJ4Box;
	plotmask=-1;

	for ( int i=0;i<13;i++ ) plotXBox[i]->setChecked ( true );
	if ( pfzc==0 )
	{
		calcBut->setEnabled ( false );
	}
	else
	{
		fzc_set_mode ( pfzc,FZCOPT_NO_AUTO_ADD_PAR | FZCOPT_NO_STDOUT );
		registParams ( prkc,pfzc );
	}

	MU[0]=1.0e3;
	MU[1]=1.0;
	MU[2]=1.0e-3;
	MU[3]=1.0/AMU_DEF;

	getThetaBarValue=&RelKinemaCls::getThetaBarPos;
	tbEval=&RelKinemaCls::thetaLabSlot;
	tbMax=&thetaMax;
	tbMin=&fZero;
	tbTarg=&th3;

	HOME=getenv ( "HOME" );
	if ( HOME.isEmpty() )
		KMessageBox::sorry ( this,"We have no HOME!","error" );
	CONFIGFILE=conf;
	if ( CONFIGFILE.isEmpty() ) CONFIGFILE=HOME+"/";
	if ( CONFIGFILE=="." ) CONFIGFILE.append ( "/" );
	if ( CONFIGFILE.startsWith ( "./" ) ) CONFIGFILE.replace ( 0,1,getenv ( "PWD" ) );
	if ( CONFIGFILE.endsWith ( "/" ) ) CONFIGFILE.append ( CONFIGFILE_DEF );
	if ( !CONFIGFILE.startsWith ( "/" ) ) CONFIGFILE.prepend ( "/" ).prepend ( getenv ( "PWD" ) );
	loadConfig ( CONFIGFILE );
	setPlottables ( plotmask );
	if ( !QFile::exists ( CONFIGFILE ) ) saveConfig ( CONFIGFILE );
	QFileInfo fi ( MASSDATA );
	if ( !QFile::exists ( MASSDATA ) || !fi.isReadable() )
	{
		KMessageBox::sorry ( this,  "Cannot locate mass data!\nChoose mass data directory.","error" );
		confMassDataDirBox->blockSignals ( true );
		massDataDirSlot();
		confMassDataDirBox->blockSignals ( false );
		MASSDATA=confMassDataDirBox->text();
	}
	K1Set=false;
	ExOk=true;

	reinitRelKinema();
	ReactionConditionBox->setEnabled ( false );
	EmissionAngleBox->setEnabled ( false );
	tableBut->setEnabled ( false );
	calcBut->setEnabled ( false );
	EmissionBox->setEnabled ( false );

	thetaStepBox->blockSignals ( true );
	thetaStepBox->setText ( "1.0" );
	thetaStepBox->blockSignals ( false );

	int aa1=a1,aa2=a2,aa3=a3,aa4=a4;
	if ( !nc2.isEmpty() ) ETargBox->setText ( nc2 );
	if ( !nc1.isEmpty() ) EProjBox->setText ( nc1 );
	if ( !nc3.isEmpty() ) EEjecBox->setText ( nc3 );
	if ( !nc4.isEmpty() ) EResiBox->setText ( nc4 );
	if ( aa2>0 ) ATargBox->setText ( QString::number ( aa2 ) );
	if ( aa1>0 ) AProjBox->setText ( QString::number ( aa1 ) );
	if ( aa3>0 ) AEjecBox->setText ( QString::number ( aa3 ) );
	if ( aa4>0 ) AResiBox->setText ( QString::number ( aa4 ) );
	redoSlot();

	bzero ( &flip,sizeof ( flip ) );
	for ( int i=0;i<4;i++ ) flipoff[i]=-i*8;
	for ( int i=4;i<8;i++ ) flipoff[i]=- ( i-4 ) *8;
	flipoff[8]=-4;//<<<<<<<<<<<<
	timerid=startTimer ( 80 );

	MProjLbl->setCursor ( IbeamCursor );
	MTargLbl->setCursor ( IbeamCursor );
	MResiLbl->setCursor ( IbeamCursor );
	MEjecLbl->setCursor ( IbeamCursor );

	K1thLbl->setCursor ( IbeamCursor );
	K1cthLbl->setCursor ( IbeamCursor );
	p1thLbl->setCursor ( IbeamCursor );
	p1cthLbl->setCursor ( IbeamCursor );
	ExmaxLbl->setCursor ( IbeamCursor );
	betaLbl->setCursor ( IbeamCursor );
	gammaLbl->setCursor ( IbeamCursor );
	gammabetaLbl->setCursor ( IbeamCursor );
	thetaMaxLbl->setCursor ( IbeamCursor );
	K3Box->setCursor(IbeamCursor);
	K3cBox->setCursor(IbeamCursor);
	K4Box->setCursor(IbeamCursor);
	K4cBox->setCursor(IbeamCursor);
	p3Box->setCursor(IbeamCursor);
	p3cBox->setCursor(IbeamCursor);
	p4Box->setCursor(IbeamCursor);
	p4cBox->setCursor(IbeamCursor);
	J3Box->setCursor(IbeamCursor);
	J4Box->setCursor(IbeamCursor);
	dK3_dth3Box->setCursor(IbeamCursor);
	KparamBox->setCursor(IbeamCursor);
	val1Box->setCursor(IbeamCursor);
	val2Box->setCursor(IbeamCursor);
	val3Box->setCursor(IbeamCursor);
	val4Box->setCursor(IbeamCursor);
}

int RelKinemaCls::adjCol ( bool *d, int c, int *off )
{
	int const col_min = 200;
	int const col_deg = 2;
	int cc=c;
	if ( *off<0 )
	{
		( *off ) ++;
		return cc;
	}
	if ( *d )
	{
		cc+=col_deg;
		if ( cc>255 )
		{
			*d=false;
			cc=255;
		}
	}
	else
	{
		cc-=col_deg;
		if ( cc<col_min )
		{
			*d=true;
			cc=col_min;
		}
	}
	return cc;
}

void RelKinemaCls::timerEvent ( QTimerEvent *unused )
{
	timerProcMt.lock();
	QLineEdit *L1=NULL;
	QLineEdit *L2=NULL;
	for ( int i=0; i<4;i++ )
	{
		switch ( i )
		{
			case 0:
				L1=ATargBox;
				L2=ETargBox;
				break;
			case 1:
				L1=AProjBox;
				L2=EProjBox;
				break;
			case 2:
				L1=AEjecBox;
				L2=EEjecBox;
				break;
			case 3:
				L1=AResiBox;
				L2=EResiBox;
				break;
		}
		if ( !massSet[i] )
		{
			int c1= adjCol ( &flip[i],L1->paletteBackgroundColor().red(), &flipoff[i] );
			int c2=c1/2+128;
			L1->setPaletteBackgroundColor ( qRgb ( c1,c1,c2 ) );
			L2->setPaletteBackgroundColor ( qRgb ( c1,c2,c1 ) );
			if ( whitedone[i] ) whitedone[i]=false;
		}
		else if ( !whitedone[i] )
		{
			whitedone[i]=true;
			flipoff[i]=-i*8;
			L1->setPaletteBackgroundColor ( "white" );
			L2->setPaletteBackgroundColor ( "white" );
		}
	}

	if ( ReactionConditionBox->isEnabled() )
	{
		if ( !K1Set )
		{
			for ( int i=4;i<8;i++ )
			{
				switch ( i )
				{
					case 4:
						L1=K1Box;
						break;
					case 5:
						L1=K1cBox;
						break;
					case 6:
						L1=p1Box;
						break;
					case 7:
						L1=p1cBox;
						break;
				}

				int c1 = adjCol ( &flip[i],L1->paletteBackgroundColor().red(), &flipoff[i] );
				int c2=c1/2+128;
				L1 ->setPaletteBackgroundColor ( qRgb ( c1,c1,c2 ) );
			}
			if ( whitedone[4] ) whitedone[4]=false;
		}
		else if ( !whitedone[4] )
		{
			for ( int i=4;i<8;i++ ) flipoff[i]=- ( i-4 ) *8;
			K1Box ->setPaletteBackgroundColor ( "white" );
			K1cBox->setPaletteBackgroundColor ( "white" );
			p1Box ->setPaletteBackgroundColor ( "white" );
			p1cBox->setPaletteBackgroundColor ( "white" );
			whitedone[4]=true;
		}

		if ( !ExOk )
		{
			int c1 = adjCol ( &flip[5],ExBox->paletteBackgroundColor().green(),&flipoff[8] );
			int c2=c1/2+128;
			ExBox ->setPaletteBackgroundColor ( qRgb ( c2,c1,c2 ) );
			if ( whitedone[8] ) whitedone[8]=false;
		}
		else if ( !whitedone[8] )
		{
			whitedone[8]=true;
			flipoff[8]=-1;
			ExBox->setPaletteBackgroundColor ( "white" );
		}
	}

	timerProcMt.unlock();
}

void RelKinemaCls::reinitRelKinema()
{
	setParMass();
	setDispFont();
}

void RelKinemaCls::setParMass()
{
	double v;
	Me=ParticleM[PID_ELECTRON];
	Mp=ParticleM[PID_PROTON];
	Mn=ParticleM[PID_NEUTRON];
	Mt=ParticleM[PID_TRITON];
	Md=ParticleM[PID_DEUTERON];
	Ma=ParticleM[PID_ALPHA];
	v=readMass ( "A.dat","e" );
	if ( v>0 ) Me=v;
	v=readMass ( "A.dat","p" );
	if ( v>0 ) Mp=v;
	v=readMass ( "A.dat","n" );
	if ( v>0 ) Mn=v;
	v=readMass ( "A.dat","d" );
	if ( v>0 ) Md=v;
	v=readMass ( "A.dat","t" );
	if ( v>0 ) Mt=v;
	v=readMass ( "A.dat","a" );
	if ( v>0 ) Ma=v;
}

void RelKinemaCls::setDispFont()
{
	if(EmissionAngleBox->width()==1||ReactionBox->width()==1) afSlot();
	setFont ( thefont );
	mainStack->setFont ( thefont );
	QFont f=thefont;
	f.setPointSize ( thefont.pointSize()-1 );
	dK3_dth3_unitLbl->setFont ( f );
	KparamUnitLbl->setFont ( f );
	setScrTypeFont();
	setTitleFont();
}

void RelKinemaCls::setTitleFont()
{
	QFont f=thefont;
	f.setPointSize ( thefont.pointSize() +1 );
	f.setBold ( true );
	pkgLbl->setFont ( f );
	verLbl->setFont ( f );
	mess1Lbl->setFont ( f );
}

void  RelKinemaCls::closeEvent ( QCloseEvent *unused )
{
	saveConfig ( CONFIGFILE );
	close();
	printf ( "thank you\n٩(๑❛ᴗ❛๑)۶\n" );
	exit ( 0 );
}

void RelKinemaCls::saveConfig ( QString f )
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;

	KConfigGroup generalGroup ( conf, "General" );
	KConfigGroup parameterGroup ( conf,"Parameters" );
	KConfigGroup externalGroup ( conf,"Externals" );
	KConfigGroup historyGroup ( conf,"History" );

	generalGroup.writeEntry ( "Display font",thefont );
	generalGroup.writeEntry ( "Mass data directory",MASSDATA );
	generalGroup.writeEntry ( "Double number format",double_format );

	externalGroup.writeEntry ( "Web browser",webbrowser );

	parameterGroup.writeEntry ( "AMU in MeV",      AMU,   true, false, 'g', 16 );
	parameterGroup.writeEntry ( "hbarc in MeV*fm", hbarc, true, false, 'g', 16 );
	parameterGroup.writeEntry ( "r0 in fm",        r0,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "e2 in MeV",       e2,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Bv",              Bv,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Bs",              Bs,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Bsym",            Bsym,  true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Bc",              Bc,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Bp",              Bp,    true, false, 'g', 16 );
	parameterGroup.writeEntry ( "Rydberg energy in eV", hcRinf, true, false, 'g', 16 );

	historyGroup.writeEntry ( "Target name",    nc2 );
	historyGroup.writeEntry ( "Projectile name",nc1 );
	historyGroup.writeEntry ( "Ejectile name",  nc3 );
	historyGroup.writeEntry ( "Residual name",  nc4 );

	historyGroup.writeEntry ( "Target A",    a2 );
	historyGroup.writeEntry ( "Projectile A",a1 );
	historyGroup.writeEntry ( "Ejectile A",  a3 );
	historyGroup.writeEntry ( "Residual A",  a4 );

	historyGroup.writeEntry ( "Incident energy", rkc_get_K1 ( prkc ), true, false, 'g', 16 );
	historyGroup.writeEntry ( "Excitation energy",rkc_get_Ex ( prkc ), true, false, 'g', 16 );
	historyGroup.writeEntry ( "Energy unit", EUnitBox->currentItem() );
	historyGroup.writeEntry ( "Mass unit",MUnitBox->currentItem() );

	historyGroup.writeEntry ( "Macro history",expr1Box->historyItems() );
	historyGroup.writeEntry ( "Macro completion",expr1Box->completionObject()->items() );

	historyGroup.writeEntry ( "Plot mask",plotmask );

	generalGroup.sync();
	parameterGroup.sync();
	externalGroup.sync();
	historyGroup.sync();

	delete conf;
}

void RelKinemaCls::loadConfig ( QString f )
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;

	KConfigGroup generalGroup ( conf, "General" );
	KConfigGroup parameterGroup ( conf,"Parameters" );
	KConfigGroup externalGroup ( conf,"Externals" );
	KConfigGroup historyGroup ( conf, "History" );

	thefont=generalGroup.readFontEntry ( "Display font",&font_def );
	MASSDATA = generalGroup.readEntry ( "Mass data directory",MASSDATA_DEF );
	double_format = generalGroup.readEntry ( "Double number format",double_format_def );
	if ( !checkDoubleNumberFormat ( double_format ) ) double_format=double_format_def;

	webbrowser = externalGroup.readEntry ( "Web browser",FIREFOX );

	AMU=parameterGroup.readDoubleNumEntry ( "AMU in MeV", AMU_DEF );
	MU[0]=1.0e3;
	MU[1]=1.0;
	MU[2]=1.0e-3;
	MU[3]=1.0/AMU;

	hbarc=parameterGroup.readDoubleNumEntry ( "hbarc in MeV*fm", hbarc_DEF );
	r0=parameterGroup.readDoubleNumEntry ( "r0 in fm", r0_DEF );
	e2=parameterGroup.readDoubleNumEntry ( "e2 in MeV", e2_DEF );
	Bv=parameterGroup.readDoubleNumEntry ( "Bv", Bv_DEF );
	Bs=parameterGroup.readDoubleNumEntry ( "Bs", Bs_DEF );
	Bsym=parameterGroup.readDoubleNumEntry ( "Bsym", Bsym_DEF );
	Bp=parameterGroup.readDoubleNumEntry ( "Bp", Bp_DEF );
	Bc=parameterGroup.readDoubleNumEntry ( "Bc", Bc_DEF );
	hcRinf=parameterGroup.readDoubleNumEntry ( "Rydberg energy in eV", hcRinf_DEF );

	plotmask = historyGroup.readNumEntry ( "Plot mask",-1 );

	for ( int i=0;i<nexpmax;i++ )
	{
		exprBox[i]->setHistoryItems ( historyGroup.readListEntry ( "Macro history" ) );
		exprBox[i]->completionObject()->setItems ( historyGroup.readListEntry ( "Macro completion" ) );
	}
	nc2=historyGroup.readEntry ( "Target name","" );
	nc1=historyGroup.readEntry ( "Projectile name","" );
	nc3=historyGroup.readEntry ( "Ejectile name","" );
	nc4=historyGroup.readEntry ( "Residual name","" );

	a2=historyGroup.readNumEntry ( "Target A",     0 );
	a1=historyGroup.readNumEntry ( "Projectile A", 0 );
	a3=historyGroup.readNumEntry ( "Ejectile A",   0 );
	a4=historyGroup.readNumEntry ( "Residual A",   0 );

	K1=historyGroup.readDoubleNumEntry ( "Incident energy",   0.0 );
	Ex=historyGroup.readDoubleNumEntry ( "Excitation energy", 0.0 );
	EUnitBox->setCurrentItem ( historyGroup.readNumEntry ( "Energy unit", 1 ) );
	MUnitBox->setCurrentItem ( historyGroup.readNumEntry ( "Mass unit", 1 ) );
	rkc_set_eunit ( prkc, EUnitBox->currentItem() +1 );
	setEunitLabel();
	changeMassUnitSlot();
	delete conf;
}

double RelKinemaCls::readMass ( QString f, QString n, bool *ok )
{
	QString fin;
	FILE *fp;
	char name[LEN_ELEMENT];
	double value,m;
	unsigned int lin=strlen ( n.latin1() );

	if ( ok!=NULL ) *ok=false;

	fin=MASSDATA;
	fin+="/"+f;
	if ( !QFile::exists ( fin ) ) return 0.;

	fp=fopen ( fin.latin1(),"r" );
	if ( f==NULL ) return 0.;

	m=0.;
	while ( fscanf ( fp,"%s %lf",name,&value ) !=EOF )
	{
		if ( strlen ( name ) !=lin ) continue;
#if defined(HAVE_STRNSTR) && (HAVE_STRNSTR == 1)
		if ( strnstr ( name,n.latin1(),lin ) !=NULL )
#else
		if ( strstr ( name,n.latin1() ) !=NULL )
#endif
		{
			if ( ok!=NULL ) *ok=true;
			m=value; // Mass excess in keV or particle mass in MeV
			break;
		}
	}
	fclose ( fp );

	return m;
}

double RelKinemaCls::calcMass ( int A, int Z )
{
	int N=A-Z;
	double M0;
	double B;

	if ( A<=0||N<=0||Z<=0 ) return -1;

	M0=N*Mn+Z*Mp;

	if ( ( A&1 ) !=0 )  // odd-even or even-odd
	{
		B=0;
	}
	else if ( ( Z&1 ) ==0&& ( N&1 ) ==0 )   // even-even
	{
		B=Bp/sqrt ( A );
	}
	else  // odd-odd
	{
		B=-Bp/sqrt ( A );
	}

	B+=Bv*A-Bs*pow ( A,2./3. )-Bsym*pow ( N-Z,2. ) /A
	   -Bc*Z* ( Z-1 ) /pow ( A,1./3. )  ;

	if ( M0<=B )
	{
		return 0.;
	}
	else
	{
		return M0-B;
	}

}

double RelKinemaCls::getMass ( QString a, QString n, int *A, int *Z, bool calc )
{
	double m;
	bool ok;
	QString f;
	int pid;

	if ( a.isEmpty() )
	{
		if ( calc ) return -1;
		pid=mdmCls::getParticleAZ ( n, A, Z );
		// pid = -1 if not found
	}
	else
	{
		pid=-2;
		*A=a.toUInt ( &ok );
		if ( *A<=0||!ok ) return -1;
		*Z=mdmCls::getZ ( n );
		if ( *Z<=0 ) return -1;
	}

	if ( !calc )
	{
		f="A"+a+".dat";
		m=readMass ( f,n, &ok );

		// m in keV, AMU in MeV, return in MeV
		if ( pid==-2&& ok )
		{
			m= ( *A ) *AMU+m/1000.0;
		}
		else if ( !ok && pid>=0 )
		{
			m=ParticleM[pid];
		}
		else if ( !ok )
		{
			return -1;
		}
		return m;
	}
	else
	{
		return calcMass ( *A,*Z );
	}
}

void RelKinemaCls::unsetMassSlot_0()
{
	unsetMass ( 0 );
}
void RelKinemaCls::unsetMassSlot_1()
{
	unsetMass ( 1 );
}
void RelKinemaCls::unsetMassSlot_2()
{
	unsetMass ( 2 );
}
void RelKinemaCls::unsetMassSlot_3()
{
	unsetMass ( 3 );
}
void RelKinemaCls::setMassSlot_0()
{
	setMassSlot_all();
}
void RelKinemaCls::setMassSlot_1()
{
	setMassSlot_all();
}
void RelKinemaCls::setMassSlot_2()
{
	setMassSlot_all();
}
void RelKinemaCls::setMassSlot_3()
{
	setMassSlot_all();
}

void RelKinemaCls::setMassSlot_all()
{
	for ( int i=0;i<4;i++ )
	{
		if ( !massSet[i] && rectSet[i] ) setMass ( i );
	}
	compRect();
}

void RelKinemaCls::reloadMass_all()
{
	for ( int i=0;i<4;i++ )
	{
		massSet[i]=0;
		if ( rectSet[i] ) setMass ( i, false );
	}
}

void RelKinemaCls::unsetMassSlot_all()
{
	for ( int i=0;i<4;i++ )
	{
		unsetMass ( i , true );
	}
}

double RelKinemaCls::stripMass ( double m, int z )
{
	return m-z*Me;
}
void RelKinemaCls::inverseSlot()
{
	swapIniSlot();
	swapFinSlot();
}
void RelKinemaCls::reverseSlot()
{
	QString a,e;
	for ( int i=0;i<4;i++ ) unsetMass ( i,false );
	a=ATargBox->text();
	e=ETargBox->text();
	ATargBox->blockSignals ( true );
	AResiBox->blockSignals ( true );
	ATargBox->setText ( AResiBox->text() );
	ETargBox->setText ( EResiBox->text() );
	AResiBox->setText ( a );
	EResiBox->setText ( e );
	ATargBox->blockSignals ( false );
	AResiBox->blockSignals ( false );
	a=AProjBox->text();
	e=EProjBox->text();
	AProjBox->blockSignals ( true );
	AEjecBox->blockSignals ( true );
	AProjBox->setText ( AEjecBox->text() );
	EProjBox->setText ( EEjecBox->text() );
	AEjecBox->setText ( a );
	EEjecBox->setText ( e );
	AProjBox->blockSignals ( false );
	AEjecBox->blockSignals ( false );
	setMassSlot_all();
}
void RelKinemaCls::swapFinSlot()
{
	QString a,e;
	a=AEjecBox->text();
	e=EEjecBox->text();
	unsetMass ( 2,false );
	unsetMass ( 3,false );
	AResiBox->blockSignals ( true );
	AEjecBox->blockSignals ( true );
	AEjecBox->setText ( AResiBox->text() );
	EEjecBox->setText ( EResiBox->text() );
	AResiBox->setText ( a );
	EResiBox->setText ( e );
	AResiBox->blockSignals ( false );
	AEjecBox->blockSignals ( false );
	setMassSlot_all();
}
void RelKinemaCls::swapIniSlot()
{
	QString a,e;
	unsetMass ( 0,false );
	unsetMass ( 1,false );
	ATargBox->blockSignals ( true );
	AProjBox->blockSignals ( true );
	a=AProjBox->text();
	e=EProjBox->text();
	AProjBox->setText ( ATargBox->text() );
	EProjBox->setText ( ETargBox->text() );
	ATargBox->setText ( a );
	ETargBox->setText ( e );
	ATargBox->blockSignals ( false );
	AProjBox->blockSignals ( false );
	setMassSlot_all();
}

void RelKinemaCls::setMass ( int id, bool comp )
{
	QString sa,sn,sm;
	QLineEdit *L1=NULL,*AB=NULL,*EB=NULL;
	double *m=NULL,*m0=NULL;
	int *a=NULL,*z=NULL;
	QString *n=NULL;
	QComboBox *MT=NULL;
	bool calc=false;

	if ( comp )
		compRect();

	switch ( id )
	{
		case 0:
			L1=MTargLbl;
			AB=ATargBox;
			EB=ETargBox;
			m=&m2;
			m0=&m20;
			a=&a2;
			z=&z2;
			n=&nc2;
			MT=mass1TypeBox;
			break;
		case 1:
			L1=MProjLbl;
			AB=AProjBox;
			EB=EProjBox;
			m=&m1;
			m0=&m10;
			a=&a1;
			z=&z1;
			n=&nc1;
			MT=mass2TypeBox;
			break;
		case 2:
			L1=MEjecLbl;
			AB=AEjecBox;
			EB=EEjecBox;
			m=&m3;
			m0=&m30;
			a=&a3;
			z=&z3;
			n=&nc3;
			MT=mass3TypeBox;
			break;
		case 3:
			L1=MResiLbl;
			AB=AResiBox;
			EB=EResiBox;
			m=&m4;
			m0=&m40;
			a=&a4;
			z=&z4;
			n=&nc4;
			MT=mass4TypeBox;
			break;
	}

	sa=AB->text().stripWhiteSpace();
	sn=EB->text().stripWhiteSpace();

	if ( MT->currentItem() ==MT_CALC ) calc=true;
	if ( !sa.isEmpty() )
	{
		*a=sa.toUInt();
		sn.replace ( 0,1,sn.at ( 0 ).upper() );
	}
	else if ( sn.isEmpty() )
	{
		massSet[id]=false;
		rectSet[id]=false;
		return;
	}
	EB->blockSignals ( true );
	EB->setText ( sn );
	EB->blockSignals ( false );
	*n=sn;
	*m0=getMass ( sa,sn,a,z, calc );
	if ( *m0>=0 )
	{
		if ( stripBox->isChecked() && *a>0 && MT->currentItem() ==MT_EXP )
		{
			*m=stripMass ( *m0,*z );
		}
		else
		{
			*m=*m0;
		}
		showMass ( *m,L1 );
		massSet[id]=true;
		AB->clearFocus();
		EB->clearFocus();
	}
	else
	{
		// if m==-2 it might be calculated.
		massSet[id]=false;
		L1->setPaletteForegroundColor ( "red" );
		if ( !calc )
		{
			L1->setText ( "Not Found" );
		}
		else
		{
			L1->setText ( "Can't calc" );
		}
	}
	seeIfMassSet();
}

void RelKinemaCls::seeIfMassSet()
{
	msgLbl->clear();
	if ( count4 ( &massSet[0] ) ==4 )
	{
		initReactionConditionBox();
		SetBut->setEnabled ( false );
		if ( abs ( a1 ) +abs ( a2 ) !=abs ( a3 ) +abs ( a4 ) ||
		        z1+z2!=z3+z4 )
		{
			msgLbl->setPaletteForegroundColor ( "red" );
			msgLbl->setText ( "Conservation Law Violation Suspected" );
		}
	}
	else
	{
		setCaption ( myname );
		ReactionBox->setTitle ( "Reaction" );
		theReaction="";
		ReactionConditionBox->setEnabled ( false );
		SetBut->setEnabled ( true );
		msgLbl->clear();
	}
}

void RelKinemaCls::setReactionLbl()
{
	QString s,a,e,r;
	a="";
	if ( a2>0 ) a.sprintf ( "%d",a2 );
	s=a+nc2+"(";
	a="";
	if ( a1>0 ) a.sprintf ( "%d",a1 );
	s+=a+nc1+",";
	a="";
	if ( a3>0 ) a.sprintf ( "%d",a3 );
	e=a+nc3;
	s+=e+")";
	a="";
	if ( a4>0 ) a.sprintf ( "%d",a4 );
	r=a+nc4;
	s+=r;
	theReaction=s;
	if(Ex!=0.0){
		s+="*";
		r+="*";
	}
	setCaption ( myname+": "+s );
	ReactionBox->setTitle ( "Reaction: "+s );
	EmissionAngleBox->setTitle("Emission Angle: "+s);
	emiAngBox->setTitle("Emission: "+e+" - th3");
	recAngBox->setTitle("Recoil: "+r+" - th4");
	ejecKpBox->setTitle("Ejectile: "+e);
	resiKpBox->setTitle("Residual: "+r);
}

void RelKinemaCls::initReactionConditionBox()
{
	rkc_set_mass ( prkc,m1,m2,m3,m4 );
	rkc_set_Ex ( prkc,Ex );
	setReactionLbl();
	ReactionConditionBox->setEnabled ( true );
	anotherSolutionBox->setEnabled ( false );
	E2=m2;
	setQValue();
	setK1th();
	setK1cth();
	showEnergyLE ( K1,K1Box );
	showEnergyLE ( Ex,ExBox );
	rectCondSlot_K1();
}

// m in MeV
void RelKinemaCls::showMass ( double m, QLineEdit *L )
{
	double mm;
	QString s;
	mm=m*MU[MUnitBox->currentItem() ];
	s.sprintf ( double_format,mm );
	L->setPaletteForegroundColor ( "black" );
	L->setText ( s );
	L->setCursorPosition ( 0 );
}

void RelKinemaCls::unsetMass ( int id, bool clear )
{
	QLineEdit *L1=NULL,*EB=NULL, *AB=NULL;
	int *a=NULL, *z=NULL;
	QString *el=NULL;
	bool ok;
	switch ( id )
	{
		case 0:
			L1=MTargLbl;
			EB=ETargBox;
			AB=ATargBox;
			a=&a2;
			z=&z2;
			el=&nc2;
			break;
		case 1:
			L1=MProjLbl;
			EB=EProjBox;
			AB=AProjBox;
			a=&a1;
			z=&z1;
			el=&nc1;
			break;
		case 2:
			L1=MEjecLbl;
			EB=EEjecBox;
			AB=AEjecBox;
			a=&a3;
			z=&z3;
			el=&nc3;
			break;
		case 3:
			L1=MResiLbl;
			EB=EResiBox;
			AB=AResiBox;
			a=&a4;
			z=&z4;
			el=&nc4;
			break;
	}
	if ( clear )
	{
		EB->blockSignals ( true );
		AB->blockSignals ( true );
		EB->clear();
		AB->clear();
		EB->blockSignals ( false );
		AB->blockSignals ( false );
	}
	if ( !AB->text().isEmpty() )
	{
		AB->text().toInt ( &ok );
		if ( !ok )
		{
			AB->blockSignals ( true );
			if ( *a>0 )
			{
				AB->setText ( QString::number ( *a ) );
			}
			else
			{
				AB->clear();
			}
			AB->blockSignals ( false );
			if ( massSet[id] ) return;
		}
	}

	massSet[id]=false;
	rectSet[id]=!EB->text().isEmpty();
	if ( rectSet[id] )
	{
		L1->setText ( "" );
	}
	else
	{
		*el="";
		L1->setText ( "Undefined" );
	}
	*a=0;
	*z=0;
	ReactionConditionBox->setEnabled ( false );
	EmissionAngleBox->setEnabled ( false );
	tableBut->setEnabled ( false );
	calcBut->setEnabled ( false );
	EmissionBox->setEnabled ( false );
	SetBut->setEnabled ( true );
	msgLbl->clear();
	setCaption ( myname );
	theReaction="";
	ReactionBox->setTitle ( "Reaction" );
}

void RelKinemaCls::changeMassUnitSlot()
{
	if ( massSet[0] ) showMass ( m2,MTargLbl );
	if ( massSet[1] ) showMass ( m1,MProjLbl );
	if ( massSet[2] ) showMass ( m3,MEjecLbl );
	if ( massSet[3] ) showMass ( m4,MResiLbl );
}

void RelKinemaCls::compRect()
{
	int i;
	int aa=0,zz=0;
	int ai,af;
	int zi,zf;
	QLineEdit *AB=NULL,*EB=NULL;

	if ( count4 ( &massSet[0] ) !=3 ) return;
	if ( count4 ( &rectSet[0] ) !=3 ) return;

	ai=abs ( a1 ) +abs ( a2 );
	zi=z1+z2;
	af=abs ( a3 ) +abs ( a4 );
	zf=z3+z4;

	for ( i=0;i<4;i++ )
	{
		if ( !rectSet[i] )
		{
			if ( i<=1 )
			{
				aa=af-ai;
				zz=zf-zi;
			}
			else
			{
				aa=ai-af;
				zz=zi-zf;
			}
			switch ( i )
			{
				case 0:
					AB=ATargBox;
					EB=ETargBox;
					break;
				case 1:
					AB=AProjBox;
					EB=EProjBox;
					break;
				case 2:
					AB=AEjecBox;
					EB=EEjecBox;
					break;
				case 3:
					AB=AResiBox;
					EB=EResiBox;
					break;
			}
			break;
		}
	}

	EB->blockSignals ( true );
	AB->blockSignals ( true );

	if ( aa>=0 )
	{
		switch ( aa )
		{
			case 0:
				if ( zz==0 )
				{
					EB->setText ( "g" );
				}
				else if ( abs ( zz ) ==1 )
				{
					EB->setText ( "e" );
				}
				break;
			case 1:
				if ( zz==1 )
				{
					EB->setText ( "p" );
				}
				else if ( zz==0 )
				{
					EB->setText ( "n" );
				}
				break;
			case 2:
				if ( zz==1 ) EB->setText ( "d" );
				break;
			case 3:
				if ( zz==1 ) EB->setText ( "t" );
				break;
			case 4:
				if ( zz==2 ) EB->setText ( "a" );
				break;
		}
		if ( EB->text().isEmpty() && zz<=Zmax && zz>=1 )
		{
			AB->setText ( QString::number ( aa ) );
			EB->setText ( Element[zz-1] );
		}
		rectSet[i]=!EB->text().isEmpty();
	}
	EB->blockSignals ( false );
	AB->blockSignals ( false );

	setMass ( i,false );
}

int RelKinemaCls::count4 ( bool *b )
{
	int c=0;
	for ( int i=0;i<4;i++ )
	{
		if ( b[i] ) c++;
	}
	return c;
}

void RelKinemaCls::setK1cth()
{
	K1cth=rkc_get_K1cMin ( prkc );
	p1cth=rkc_get_p1cMin ( prkc );
	showEnergyL ( K1cth,K1cthLbl,rectCondSym );
	showEnergyL ( p1cth,p1cthLbl,rectCondSym );
}

void RelKinemaCls::setK1th()
{
	K1th=rkc_get_K1Min ( prkc );
	p1th=rkc_get_p1Min ( prkc );
	showEnergyL ( K1th,K1thLbl,rectCondSym );
	showEnergyL ( p1th, p1thLbl,rectCondSym );
}

void RelKinemaCls::setQValue()
{
	QValue=rkc_get_QValue ( prkc );
	if ( QValue>0.0 )
	{
		rectCondSym=">=";
	}
	else
	{
		rectCondSym=">";
	}
	showEnergyLE ( QValue,QValueBox );
}

void RelKinemaCls::showEnergyL ( double E, QLineEdit *L, QString prep )
{
	QString s;
	s.sprintf ( double_format, E );
	L->setText ( s.prepend ( prep ) );
	L->setCursorPosition ( 0 );
}

void RelKinemaCls::setActiveTextColLE ( QString c, QLineEdit *L )
{
	QPalette p=L->palette();
	p.setColor ( QPalette::Active,QColorGroup::Text,QColor ( c ) );
	L->setPalette ( p );
}

void RelKinemaCls::showEnergyLE ( double E, QLineEdit *L )
{
	QString s;
	s.sprintf ( double_format, E );
	L->blockSignals ( true );
	if ( E<0. )
	{
		setActiveTextColLE ( "red",L );
	}
	else
	{
		setActiveTextColLE ( "black",L );
	}
	L->setText ( s );
	L->setCursorPosition ( 0 );
	L->blockSignals ( false );
}

void RelKinemaCls::setE3c ( )
{
	rkc_set_K34c ( prkc );
	K3c=rkc_get_K3c ( prkc );
	E3c=rkc_get_E3c ( prkc );
	p3c=rkc_get_p3c ( prkc );
	K4c=rkc_get_K4c ( prkc );
	E4c=rkc_get_E4c ( prkc );
	p4c=rkc_get_p4c ( prkc );
}

void RelKinemaCls::setExmax ( )
{
	Exmax=rkc_get_ExMax ( prkc );
	if ( Exmax>0 )
	{
		showEnergyL ( Exmax,ExmaxLbl,"< " );
	}
	else
	{
		ExmaxLbl->clear();
	}
}

int const CL_K1  = 0;
int const CL_K1C = 1;
int const CL_P1  = 2;
int const CL_P1C = 3;

void RelKinemaCls::rectCondSlot_p1()
{
	setResultBox ( false );
	bool ok;
	double v=p1Box->text().toDouble ( &ok );
	showStrLE ( "",K1Box );
	showStrLE ( "",K1cBox );
	showStrLE ( "",p1cBox );
	showStrLE ( "",ExmaxLbl );
	if ( !ok || v<0. ) return;

	rkc_set_p1 ( prkc,v );
	p1=rkc_get_p1 ( prkc );
	E1=rkc_get_E1 ( prkc );
	K1=rkc_get_K1 ( prkc );

	procK1();
	procK1_common ( CL_P1 );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::rectCondSlot_p1c()
{
	setResultBox ( false );
	bool ok;
	double v=p1cBox->text().toDouble ( &ok );
	showStrLE ( "",K1Box );
	showStrLE ( "",K1cBox );
	showStrLE ( "",p1Box );
	showStrLE ( "",ExmaxLbl );
	if ( !ok || v<0. ) return;

	rkc_set_p1c ( prkc,v );
	p1c=rkc_get_p1c ( prkc );
	E1c=rkc_get_E1c ( prkc );
	K1c=rkc_get_K1c ( prkc );

	procK1c();
	procK1_common ( CL_P1C );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::procK1_common ( int caller )
{
	setE3c ( );
	setExmax ( );

	showValueLE ( rkc_get_invgamma ( prkc ),gammaLbl );
	showValueLE ( beta,betaLbl );
	showValueLE ( rkc_get_betagamma ( prkc ),gammabetaLbl );

	if ( caller!=CL_K1C ) showEnergyLE ( K1c,K1cBox );
	if ( caller!=CL_P1 ) showEnergyLE ( p1,p1Box );
	if ( caller!=CL_P1C ) showEnergyLE ( p1c,p1cBox );
	if ( caller!=CL_K1 ) showEnergyLE ( K1,K1Box );
}

void RelKinemaCls::procK1()
{
	gamma=rkc_get_gamma ( prkc );
	beta=rkc_get_beta ( prkc );

	K1c=rkc_get_K1c ( prkc );
	p1c=rkc_get_p1c ( prkc );
	E1c=rkc_get_E1c ( prkc );
	p2c=p1c;

	E2c=rkc_get_E2c ( prkc );
	K2c=rkc_get_K2c ( prkc );
}

void RelKinemaCls::rectCondSlot_K1()
{
	setResultBox ( false );
	bool ok=false;
	double v;
	if ( K1Box->text().length() ==1 )
	{
		ok=true;
		char c=K1Box->text().at ( 0 ).latin1();
		switch ( c )
		{
			case 'e':
				v=Me*EU[EUnitBox->currentItem() ];
				break;
			default:
				ok=false;
				break;
		}
		if ( ok ) showValueLE ( v,K1Box );
	}
	if ( !ok ) v=K1Box->text().toDouble ( &ok );
	showStrLE ( "",K1cBox );
	showStrLE ( "",p1Box );
	showStrLE ( "",p1cBox );
	showStrLE ( "",ExmaxLbl );
	if ( !ok || v< 0. ) return;

	rkc_set_K1 ( prkc,v );
	K1=rkc_get_K1 ( prkc );
	E1=rkc_get_E1 ( prkc );
	p1=rkc_get_p1 ( prkc );
	procK1();
	procK1_common ( CL_K1 );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::procK1c()
{
	K1=rkc_get_K1 ( prkc );
	E1=rkc_get_E1 ( prkc );
	p1=rkc_get_p1 ( prkc );
	gamma=rkc_get_gamma ( prkc );
	beta=rkc_get_beta ( prkc );
}

void RelKinemaCls::rectCondSlot_K1c()
{
	setResultBox ( false );
	bool ok;
	double v=K1cBox->text().toDouble ( &ok );
	showStrLE ( "",K1Box );
	showStrLE ( "",p1Box );
	showStrLE ( "",p1cBox );
	showStrLE ( "",ExmaxLbl );
	if ( !ok || v< 0. ) return;
// 
	rkc_set_K1c ( prkc,v );
	K1c=rkc_get_K1c ( prkc );
	E1c=rkc_get_E1c ( prkc );
	p1c=rkc_get_p1c ( prkc );

	procK1c();
	procK1_common ( CL_K1C );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::rectCondSlot_Ex()
{
	setResultBox ( false );
	bool ok;
	double v=ExBox->text().toDouble ( &ok );
	if ( ok && v>=0 )
	{
		rkc_set_Ex ( prkc,v );
		Ex=rkc_get_Ex ( prkc );
		setQValue();
		setK1th();
		setK1cth();
		setE3c ( );
		setResultBox ( checkRectCond() );
		setReactionLbl();
	}
	else
	{
		ExOk=false;
		setResultBox ( false );
	}
}

bool RelKinemaCls::checkRectCond()
{
	K1Set= ( K1>=K1th ) &&! ( QValue==0&&K1==0 );
	ExOk= ( Ex<=Exmax );
/*
	if ( ExOk )
	{
		if ( Ex==0. )
		{
			residualLbl->setPaletteForegroundColor ( "black" );
			ExLbl->clear();
		}
		else
		{
			residualLbl->setPaletteForegroundColor ( "red" );
			ExLbl->setText ( "*" );
		}
	}
*/
	return ExOk&&K1Set;
}

void RelKinemaCls::setThetaMax()
{
	rkc_set_th3Max ( prkc );
	thetaMax=rkc_get_th3Max ( prkc );
	thetaMaxCM=rkc_get_th3cMax ( prkc );
	inv=rkc_get_invKin ( prkc );
	thetaMaxNe=rkc_get_th3MaxNe ( prkc );

	qmax=rkc_get_qMax ( prkc );
	qmin=rkc_get_qMin ( prkc );
}

void RelKinemaCls::setResultBox ( bool on )
{
	if(on&&EmissionAngleBox->width()==1) afSlot();
	EmissionBox->setEnabled ( on );
	EmissionAngleBox->setEnabled ( on );
	tableBut->setEnabled ( on );
	if ( pfzc!=0 ) calcBut->setEnabled ( on );
	if ( on )
	{
		initResultBox();
		K1Set=true;
		ExOk=true;
	}
	else
	{
		K1Set=false;
	}
}

void RelKinemaCls::setEunitLabel()
{
	QString s=EUnitBox->currentText();
	ejecKBox->setTitle("K3 ["+s+"]");
	ejecpBox->setTitle("p3 ["+s+"/c]");
	resiKBox->setTitle("K4 ["+s+"]");
	resipBox->setTitle("p4 ["+s+"/c]");
}

void RelKinemaCls::changeEUnitSlot()
{
	setEunitLabel();

	rkc_set_eunit ( prkc,EUnitBox->currentItem() +1 );

	K1=rkc_get_K1 ( prkc );
	p1=rkc_get_p1 ( prkc );
	K1c=rkc_get_K1c ( prkc );
	p1c=rkc_get_p1c ( prkc );
	K1th=rkc_get_K1Min ( prkc );
	p1th=rkc_get_p1Min ( prkc );
	K1cth=rkc_get_K1cMin ( prkc );
	p1cth=rkc_get_p1cMin ( prkc );

	QValue=rkc_get_QValue ( prkc );
	Ex=rkc_get_Ex ( prkc );
	Exmax=rkc_get_ExMax ( prkc );

	K3=rkc_get_K3 ( prkc );
	K4=rkc_get_K4 ( prkc );
	p3=rkc_get_p3 ( prkc );
	p4=rkc_get_p4 ( prkc );
	K3c=rkc_get_K3c ( prkc );
	K4c=rkc_get_K4c ( prkc );
	p3c=rkc_get_p3c ( prkc );
	p4c=rkc_get_p4c ( prkc );

	showEnergyLE ( K1,K1Box );
	showEnergyLE ( K1c,K1cBox );
	showEnergyLE ( p1,p1Box );
	showEnergyLE ( p1c,p1cBox );

	showEnergyLE ( Ex,ExBox );
	showEnergyLE ( QValue,QValueBox );
	if ( K1th>0 ) showEnergyL ( K1th,K1thLbl,"> " );
	if ( K1cth>0 )	showEnergyL ( K1cth,K1cthLbl,"> " );
	if ( p1th>0 ) showEnergyL ( p1th,p1thLbl,">" );
	if ( p1cth>0 ) showEnergyL ( p1cth,p1cthLbl,">" );
	if ( Exmax>0 ) showEnergyL ( Exmax,ExmaxLbl,"< " );

	showKp3();
	showKp4();
	showKp34c();
	setSpecParam();
}

void RelKinemaCls::showThetaMax()
{
	QString s="<";
	if ( !thetaMaxNe ) s+="=";
	showAngleLE ( thetaMax,thetaMaxLbl,s+" " );
}

void RelKinemaCls::changeAUnitSlot()
{
	double min,max,v;
	rkc_set_aunit ( prkc,degBut->isChecked() ?1:0 );
	thetaMax=rkc_get_th3Max ( prkc );
	thetaMaxCM=rkc_get_th3cMax ( prkc );
	if ( scrTypeBox->currentItem() !=ST_Q )	stepUnitLbl->setText ( getAngleUnit() );
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			th3=rkc_get_th3 ( prkc );
			min=0.;
			max=thetaMax;
			v=th3;
			break;
		case ST_THETACM:
			th3c=rkc_get_th3c ( prkc );
			min=0.;
			max=thetaMaxCM;
			v=th3c;
			break;
		case ST_Q:
			th3c=rkc_get_th3c ( prkc );
			th3=rkc_get_th3 ( prkc );
			min=qmin;
			max=qmax;
			v=theq;
			break;
	}
	showThetaMax();
	bool ok;
	double d=thetaStepBox->text().toDouble ( &ok );
	if ( ok ) setThetaBarStep ( min,max,d,v );
	if ( ThetaLabBox->text().toDouble() == 0. ) resetThetaLab();
	if ( scrTypeBox->currentItem() ==ST_Q ) qSlot ( true );
}

void RelKinemaCls::showValueLE ( double v, QLineEdit *b )
{
	QString s;
	s.sprintf ( double_format,v );
	b->blockSignals ( true );
	if ( v<0. )
	{
		setActiveTextColLE ( "red",b );
	}
	else
	{
		setActiveTextColLE ( "black",b );
	}
	b->setText ( s );
	b->setCursorPosition ( 0 );
	b->blockSignals ( false );
}

void RelKinemaCls::showStrLE ( QString s, QLineEdit *b )
{
	b->blockSignals ( true );
	b->setText ( s );
	b->setCursorPosition ( 0 );
	b->blockSignals ( false );
}

void RelKinemaCls::showAngleLE ( double v, QLineEdit *b, QString p )
{
	QString s;
	s.sprintf ( double_format,v );
	showStrLE ( p+s,b );
}

void RelKinemaCls::initResultBox()
{
	initEmissionAngleBox();
	showKp34c();
}

double RelKinemaCls::getThetaBarPos()
{
	double v=thetaBar->value() *thetaStepBox->text().toDouble() +*tbMin;
	return v>*tbMax? *tbMax: v;
}

void RelKinemaCls::thetaBarSlot()
{
	double v= ( this->*getThetaBarValue ) ();
	if ( v>*tbMax )
	{
		v=*tbMax;
	}
	else if ( v<*tbMin )
	{
		v=*tbMin;
	}
	*tbTarg=v;
	( this->*tbEval ) ( true );
}

void RelKinemaCls::initEmissionAngleBox()
{
	setThetaMax();
	showThetaMax();
	ThetaLabBox->clear();
	ThetaLabBox->setText ( "0" ); // invoke thetalabslot
	scrTypeSlot();
}

void RelKinemaCls::setth4 ( )
{
	th4=rkc_get_th4 ( prkc );
}

void RelKinemaCls::setAngle ( bool on )
{
	if ( !on )
	{
		K3Box->clear();
		K4Box->clear();
		J3Box->clear();
		J4Box->clear();
		p3Box->clear();
		p4Box->clear();
		dK3_dth3Box->clear();
		KparamBox->clear();
	}
	recAngBox->setEnabled ( on );
	scrTypeBox->setEnabled ( on );
	thetaBar->setEnabled ( on );
}

void RelKinemaCls::showTheta4()
{
	showAngleLE ( th4,RThetaLabBox );
	showAngleLE ( th4c,RThetaCMBox );
}

bool RelKinemaCls::setTheta3c ( double v )
{
	if ( nearly_eq ( v,thetaMaxCM ) )
	{
		rkc_set_Theta3c_max ( prkc );
		return true;
	}
	else
	{
		if ( v<0. || v>thetaMaxCM ) return false;
		return rkc_set_Theta3c ( prkc, v ) >=0;
	}
}

void RelKinemaCls::thetaCMSlot ( bool internal )
{
	if ( !internal )
	{
		RThetaLabBox->clear();
		RThetaCMBox->clear();
		showStrLE ( "",qBox );
		showStrLE ( "",ThetaLabBox );
		setAngle ( false );
		bool ok;
		double v=ThetaCMBox->text().toDouble ( &ok );
		if ( !ok || !setTheta3c ( v ) ) return;
		th3c=rkc_get_th3c ( prkc );
	}
	else
	{
		if ( !setTheta3c ( th3c ) ) return;
		showAngleLE ( th3c,ThetaCMBox );
	}
	th4c=rkc_get_th4c ( prkc );
	setK3vCM ();
	setK4 ();
	th3=rkc_get_th3 ( prkc );
	setth4 ( );
	setJ34 ( );
	setSpecParam();
	theq=rkc_get_q ( prkc );
	setAngle ( true );
	showValueLE ( theq,qBox );
	showAngleLE ( th3,ThetaLabBox );
	showTheta4();
	if ( !internal ) updateThetaBar();
	updateExprSlot();
}

void RelKinemaCls::setJ34 ( )
{
	J3=rkc_get_J3 ( prkc );
	J4=rkc_get_J4 ( prkc );
	showValueLE ( J3,J3Box );
	showValueLE ( J4,J4Box );
}

void RelKinemaCls::setK3vCM ( )
{
	E3=rkc_get_E3 ( prkc );
	K3=rkc_get_K3 ( prkc );
	p3=rkc_get_p3 ( prkc );
	showKp3();
}

void RelKinemaCls::showKp34c()
{
	showEnergyLE ( K3c,K3cBox );
	showEnergyLE ( p3c,p3cBox );
	showEnergyLE ( K4c,K4cBox );
	showEnergyLE ( p4c,p4cBox );
}
void RelKinemaCls::showKp3()
{
	showEnergyLE ( K3,K3Box );
	showEnergyLE ( p3,p3Box );
}
void RelKinemaCls::showKp4()
{
	showEnergyLE ( K4,K4Box );
	showEnergyLE ( p4,p4Box );
}

void RelKinemaCls::setK4 ( )
{
	E4=rkc_get_E4 ( prkc );
	K4=rkc_get_K4 ( prkc );
	p4=rkc_get_p4 ( prkc );
	showKp4();
}

void RelKinemaCls::setK3 ( )
{
	K3=rkc_get_K3 ( prkc );
	p3=rkc_get_p3 ( prkc );
	E3=rkc_get_E3 ( prkc );
	showKp3();
}

bool RelKinemaCls::setTheta3 ( double v )
{
	if ( nearly_eq ( v,thetaMax ) )
	{
		rkc_set_Theta3_max ( prkc );
		return true;
	}
	else
	{
		if ( v<0. || v>thetaMax ) return false;
		return rkc_set_Theta3 ( prkc, v ) >=0;
	}
}

void RelKinemaCls::thetaLabSlot ( bool internal )
{
	if ( !internal )
	{
		RThetaLabBox->clear();
		RThetaCMBox->clear();
		showStrLE ( "",qBox );
		showStrLE ( "",ThetaCMBox );
		setAngle ( false );
		bool ok;
		double v=ThetaLabBox->text().toDouble ( &ok );
		if ( !ok || !setTheta3 ( v ) ) return;
		th3=rkc_get_th3 ( prkc );
	}
	else
	{
		if ( !setTheta3 ( th3 ) ) return;
		showAngleLE ( th3,ThetaLabBox );
	}
	setK3 ();
	setK4 ();
	th3c=rkc_get_th3c ( prkc );
	setJ34 ( );
	th4c=rkc_get_th4c ( prkc );
	setth4 ( );
	theq=rkc_get_q ( prkc );
	setSpecParam();
	setAngle ( true );
	showValueLE ( theq,qBox );
	showAngleLE ( th3c,ThetaCMBox );
	showTheta4();
	if ( !internal ) updateThetaBar();
	updateExprSlot();
}

void RelKinemaCls::thetaLabSlot()
{thetaLabSlot ( false );}
void RelKinemaCls::thetaCMSlot()
{thetaCMSlot ( false );}
void RelKinemaCls::qSlot()
{qSlot ( false );}

bool RelKinemaCls::setq ( double v )
{
	if ( nearly_eq ( v,qmin ) )
	{
		rkc_set_q_min ( prkc );
		return true;
	}
	else if ( nearly_eq ( v,qmax ) )
	{
		rkc_set_q_max ( prkc );
		return true;
	}
	else
	{
		if ( v<qmin || v>qmax ) return false;
		return rkc_set_q ( prkc, v ) >=0;
	}
}

void RelKinemaCls::qSlot ( bool internal )
{
	if ( !internal )
	{
		showStrLE ( "",ThetaLabBox );
		showStrLE ( "",ThetaCMBox );
		RThetaLabBox->clear();
		RThetaCMBox->clear();
		setAngle ( false );
		bool ok;
		double v=qBox->text().toDouble ( &ok );
		if ( !ok || !setq ( v ) ) return;
		theq=rkc_get_q ( prkc );
		setAngle ( true );
	}
	else
	{
		if ( !setq ( theq ) ) return;
		showValueLE ( theq,qBox );
	}
	th3c=rkc_get_th3c ( prkc );
	th4c=rkc_get_th4c ( prkc );
	setK3vCM ();
	setK4 ( );
	th3=rkc_get_th3 ( prkc );
	setth4 ( );
	setJ34 ( );
	setSpecParam();
	showAngleLE ( th3,ThetaLabBox );
	showAngleLE ( th3c,ThetaCMBox );
	showTheta4();
	if ( !internal ) updateThetaBar();
	updateExprSlot();
}

void RelKinemaCls::setSpecParam()
{
	QString L1,L2;
	L1=EUnitBox->currentText();
	if ( degBut->isChecked() )
	{
		L1+="/deg";
		L2="1/deg";
	}
	else
	{
		L1+="/rad";
		L2="1/rad";
	}

// dK3/dth3
	kinemaShift=rkc_get_KShift ( prkc );
	showValueLE ( kinemaShift, dK3_dth3Box );
	dK3_dth3_unitLbl->setText ( L1 );

// dp3/dth3/p3
	factorK=rkc_get_KFactor ( prkc );
	showValueLE ( factorK,KparamBox );
	KparamUnitLbl->setText ( L2 );
}

QString RelKinemaCls::getAngleUnit()
{
	if ( degBut->isChecked() )
	{
		return "deg";
	}
	else
	{
		return "rad";
	}
}

void RelKinemaCls::setScrTypeFont()
{
	QLabel *L;
	QFont f=thefont;
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			L=theLabLbl;
			break;
		case ST_THETACM:
			L=theCMLbl;
			break;
		case ST_Q:
			L=theqLbl;
			break;
	}
	theqLbl->setPaletteForegroundColor ( "black" );
	theLabLbl->setPaletteForegroundColor ( "black" );
	theCMLbl->setPaletteForegroundColor ( "black" );
	theLabLbl->setFont ( thefont );
	theCMLbl->setFont ( thefont );
	theqLbl->setFont ( thefont );
	L->setPaletteForegroundColor ( "blue" );
	f.setBold ( true );
	L->setFont ( f );
}

void RelKinemaCls::scrTypeSlot()
{
	bool ok;
	double v=0;
	double d=thetaStepBox->text().toDouble();
	double max=0,min=0;
	void ( RelKinemaCls::*eval ) ();
	QString unit;
	thetaBar->setEnabled ( false );
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			eval=&RelKinemaCls::thetaLabSlot;
			v=ThetaLabBox->text().toDouble ( &ok );
			getThetaBarValue=&RelKinemaCls::getThetaBarPos;
			max=thetaMax;
			min=0.;
			unit=getAngleUnit();
			rkc_set_K3Sign ( prkc,1 );
			anotherSolutionBox->setEnabled ( inv && !thetaMaxNe );
			anotherSolutionBox->setChecked ( false );
			tbMax=&thetaMax;
			tbMin=&fZero;
			tbTarg=&th3;
			tbEval=&RelKinemaCls::thetaLabSlot;
			break;
		case ST_THETACM:
			eval=&RelKinemaCls::thetaCMSlot;
			v=ThetaCMBox->text().toDouble ( &ok );
			getThetaBarValue=&RelKinemaCls::getThetaBarPos;
			max=thetaMaxCM;
			min=0.;
			unit=getAngleUnit();
			anotherSolutionBox->setEnabled ( false );
			anotherSolutionBox->setChecked ( false );
			tbMax=&thetaMaxCM;
			tbMin=&fZero;
			tbTarg=&th3c;
			tbEval=&RelKinemaCls::thetaCMSlot;
			break;
		case ST_Q:
			eval=&RelKinemaCls::qSlot;
			v=qBox->text().toDouble ( &ok );
			max=qmax;
			min=qmin;
			unit="fm-1";
			anotherSolutionBox->setEnabled ( false );
			anotherSolutionBox->setChecked ( false );
			tbMax=&qmax;
			tbMin=&qmin;
			tbTarg=&theq;
			tbEval=&RelKinemaCls::qSlot;
			getThetaBarValue=&RelKinemaCls::getThetaBarPos;
			break;
	}
	stepUnitLbl->setText ( unit );
	if ( !ok || d == 0. ) return;
	setScrTypeFont();
	setThetaBarStep ( min,max,d,v );
	thetaBar->setEnabled ( true );
}

void RelKinemaCls::setThetaBarStep ( double min, double max, double d, double v )
{
	if ( v<min )
	{
		v=min;
	}
	else if ( v>max )
	{
		v=max;
	}

	if ( d<=0. || d>max )
	{
		d=max/100.0;
		if ( d<min ) d=min;
		thetaStepBox->blockSignals ( true );
		QString s;
		s.setNum ( d=getExpValue ( d ) );
		thetaStepBox->setText ( s );
		thetaStepBox->blockSignals ( false );
	}
	int n=floor ( ( max-min ) /d+0.5 );
	if ( *tbMax>n*thetaStepBox->text().toDouble() +*tbMin ) n++;
	thetaBar->setMinValue ( 0 );
	thetaBar->setMaxValue ( n );
	int i=floor ( ( v-*tbMin ) /d+0.5 );
	if ( i<0 )
	{
		i=0;
	}
	else if ( i>n )
	{
		i=n;
	}
	thetaBar->setValue ( i );
}

double RelKinemaCls::getExpValue ( double x )
{
	if ( x<=0.0 ) return 0.0;
	double f=x;
	int i=0;
	if ( x>=1.0 )
	{
		for ( ;; )
		{
			i++;
			f=f/10.0;
			if ( f<=1.0 ) break;
		}
		if ( f<0.5 ) i--;
		return pow ( 1.0e1,i );
	}
	else
	{
		for ( ;; )
		{
			i++;
			f=f*10.0;
			if ( f>=1.0 ) break;
		}
		if ( f>=5.0 ) i--;
		return pow ( 1.0e-1,i );
	}
}

void RelKinemaCls::anotherSolutionSlot()
{
	if ( anotherSolutionBox->isChecked() )
	{
		rkc_set_K3Sign ( prkc,-1 );
	}
	else
	{
		rkc_set_K3Sign ( prkc,1 );
	}
	resetThetaLab();
}

void RelKinemaCls::resetThetaLab()
{
	ThetaLabBox->blockSignals ( true );
	ThetaLabBox->clear();
	ThetaLabBox->blockSignals ( false );
	ThetaLabBox->setText ( "0" );
}

void RelKinemaCls::massTypeProc ( int id, QComboBox *c, QLineEdit *e )
{
	if ( c->currentItem() ==MT_MAN )
	{
		bool ok;
		double v=e->text().toDouble ( &ok );
		if ( ! ( ok&&v>=0 ) ) e->clear();
		e->setPaletteBackgroundColor ( QColor ( 255, 255, 255 ) );
		e->setMouseTracking ( true );
		e->setAcceptDrops ( true );
		e->setFrameShadow ( QLineEdit::Sunken );
		e->setFrameShape ( QLineEdit::LineEditPanel );
		e->setCursorPosition ( 0 );
		e->setReadOnly ( false );
	}
	else
	{
		e->setReadOnly ( true );
		e->setPaletteBackgroundColor ( QColor ( 239, 239, 239 ) );
		e->setMouseTracking ( false );
		e->setAcceptDrops ( false );
		e->setFrameShape ( QLineEdit::NoFrame );
		e->setCursorPosition ( 0 );
		setMass ( id );
	}
}

void RelKinemaCls::mass1TypeSlot()
{
	massTypeProc ( 0,mass1TypeBox,MTargLbl );
}

void RelKinemaCls::mass2TypeSlot()
{
	massTypeProc ( 1,mass2TypeBox,MProjLbl );
}

void RelKinemaCls::mass3TypeSlot()
{
	massTypeProc ( 2,mass3TypeBox,MEjecLbl );
}

void RelKinemaCls::mass4TypeSlot()
{
	massTypeProc ( 3,mass4TypeBox,MResiLbl );
}

void RelKinemaCls::massEditProc ( int id, QLineEdit *e, double *m )
{
	bool ok;
	double v=e->text().toDouble ( &ok );
	if ( ! ( ok&&v>=0 ) )
	{
		ReactionConditionBox->setEnabled ( false );
		EmissionAngleBox->setEnabled ( false );
		tableBut->setEnabled ( false );
		calcBut->setEnabled ( false );
		EmissionBox->setEnabled ( false );
		SetBut->setEnabled ( true );
		msgLbl->clear();
		massSet[id]=false;
	}
	else
	{
		massSet[id]=true;
		*m=v/MU[MUnitBox->currentItem() ];
		seeIfMassSet();
	}
}

void RelKinemaCls::mass1EditSlot()
{
	if ( mass1TypeBox->currentItem() !=MT_MAN ) return;
	massEditProc ( 0, MTargLbl, &m2 );
}

void RelKinemaCls::mass2EditSlot()
{
	if ( mass2TypeBox->currentItem() !=MT_MAN ) return;
	massEditProc ( 1, MProjLbl, &m1 );
}

void RelKinemaCls::mass3EditSlot()
{
	if ( mass3TypeBox->currentItem() !=MT_MAN ) return;
	massEditProc ( 2, MEjecLbl, &m3 );
}

void RelKinemaCls::mass4EditSlot()
{
	if ( mass4TypeBox->currentItem() !=MT_MAN ) return;
	massEditProc ( 3, MResiLbl, &m4 );
}

void RelKinemaCls::stripSlot()
{
	if ( stripBox->isChecked() )
	{
		if ( a1>0&&massSet[1] && mass1TypeBox->currentItem() ==MT_EXP ) m1=stripMass ( m10,z1 );
		if ( a2>0&&massSet[0] && mass2TypeBox->currentItem() ==MT_EXP ) m2=stripMass ( m20,z2 );
		if ( a3>0&&massSet[3] && mass3TypeBox->currentItem() ==MT_EXP ) m3=stripMass ( m30,z3 );
		if ( a4>0&&massSet[4] && mass4TypeBox->currentItem() ==MT_EXP ) m4=stripMass ( m40,z4 );
	}
	else
	{
		if ( a1>0&&massSet[1] ) m1=m10;
		if ( a2>0&&massSet[0] ) m2=m20;
		if ( a3>0&&massSet[2] ) m3=m30;
		if ( a4>0&&massSet[3] ) m4=m40;
	}
	if ( massSet[0] ) showMass ( m2,MTargLbl );
	if ( massSet[1] ) showMass ( m1,MProjLbl );
	if ( massSet[2] ) showMass ( m3,MEjecLbl );
	if ( massSet[3] ) showMass ( m4,MResiLbl );
	seeIfMassSet();
}

void RelKinemaCls::showResultListSlot()
{
	bool ok;
	double d=thetaStepBox->text().toDouble ( &ok );
	if ( !ok || d<=0 )
	{
		KMessageBox::sorry ( this,"Invalid steps of the calculation ","" );
		return;
	}

	bool exprPlot[4]={plot1Box->isChecked(),
	                  plot2Box->isChecked(),
	                  plot3Box->isChecked(),
	                  plot4Box->isChecked()
	                 };
	int np=0;
	for ( int i=0;i<4;i++ ) if ( exprPlot[i] ) np++;

	plotmask=0;
	int col_first=scrTypeBox->currentItem();
	for ( int i=3;i<13;i++ )
	{
		if ( plotXBox[i]->isChecked() ) plotmask|=1<<i;
	}
	switch ( col_first )
	{
		case 0:
			if ( plotXBox[0]->isChecked() ) plotmask|=1<<0;
			if ( plotXBox[1]->isChecked() ) plotmask|=1<<1;
			if ( plotXBox[2]->isChecked() ) plotmask|=1<<2;
			break;
		case 1:
			if ( plotXBox[1]->isChecked() ) plotmask|=1<<0;
			if ( plotXBox[0]->isChecked() ) plotmask|=1<<1;
			if ( plotXBox[2]->isChecked() ) plotmask|=1<<2;
			break;
		case 2:
			if ( plotXBox[2]->isChecked() ) plotmask|=1<<0;
			if ( plotXBox[0]->isChecked() ) plotmask|=1<<1;
			if ( plotXBox[1]->isChecked() ) plotmask|=1<<2;
			break;
	}
	if ( np==0&&plotmask==0 )
	{
		KMessageBox::sorry ( this,"No item specified for table","" );
		return;
	}
	QString range;
	double smin,smax;
	range=scrMinBox->text();
	if ( !range.isEmpty() )
	{
		smin=range.toDouble ( &ok );
		if ( !ok||smin<*tbMin ) smin=*tbMin;
	}
	else
	{
		smin=*tbMin;
	}
	range=scrMaxBox->text();
	if ( !range.isEmpty() )
	{
		smax=range.toDouble ( &ok );
		if ( !ok||smax>*tbMax ) smax=*tbMax;
	}
	else
	{
		smax=*tbMax;
	}
	if ( smax<=smin )
	{
		KMessageBox::sorry ( this,"Invalid range","" );
		return;
	}
	int nr= ( smax-smin ) /d+1;
	if ( ( nr-1 ) *d+smin<smax ) nr++;

	tableBut->setEnabled ( false );
	QString el=EUnitBox->currentText();
	QString pl=el+"/c";
	QString al;
	degBut->isChecked() ? al="deg": al="rad";

	resultWindowCls *win = new resultWindowCls (
	    NULL,NULL,Qt::WDestructiveClose, plotmask, exprPlot, exprBox, col_first,nr,el,al,pl );

	win->homedir=&HOME;
	win->setFont ( thefont );

	win->initResultDescBox ( theReaction, K1, p1, Ex, QValue, beta,gamma,
	                         m1/AMU, m2/AMU, m3/AMU, m4/AMU, double_format,
	                         EUnitBox->currentText() );

	prkc_dmy=rkc_cp ( prkc );
	int n=0;
	for ( int i=0;i<4;i++ ) if ( exprPlot[i] ) n++;
	if ( n!=0 )
	{
		pfzc_dmy=fzc_cp ( pfzc );
		registParams ( prkc_dmy, pfzc_dmy );
	}
	else
	{
		pfzc_dmy=0;
	}
	rwt = new rwThreadCls ( d, smin, smax, nr-1,
	                        prkc_dmy, scrTypeBox->currentItem(), double_format,
	                        col_first, pfzc_dmy, plotmask, exprPlot );
	connect ( win,SIGNAL ( done ( void ) ),this,SLOT ( teDone ( void ) ) );
	win->eth=rwt;
	win->sl=&rwt->sl;
	win->ndone=&rwt->ndone;
	rwt->start();
	win->startPoll();
	win->show();
}

void RelKinemaCls::teDone ( void )
{
	if ( rwt!=0 ) delete rwt;
	if ( prkc_dmy!=0 ) rkc_uinit ( prkc_dmy );
	if ( pfzc_dmy!=0 ) fzc_uinit ( pfzc_dmy );
	rwt=0; prkc_dmy=0; pfzc_dmy=0;
	tableBut->setEnabled ( true );
}

void RelKinemaCls::thetaStepSlot()
{
	bool ok;
	double d=thetaStepBox->text().toDouble ( &ok );
	if ( !ok ) return;
	double max=0,min=0;
	double v;
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			v=ThetaLabBox->text().toDouble ( &ok );
			max=thetaMax;
			min=0.;
			break;
		case ST_THETACM:
			v=ThetaCMBox->text().toDouble ( &ok );
			max=thetaMaxCM;
			min=0.;
			break;
		case ST_Q:
			v=qBox->text().toDouble ( &ok );
			max=qmax;
			min=qmin;
			break;
	}
	if ( d==0. )
	{
		thetaBar->setEnabled ( false );
		return;
	}
	setThetaBarStep ( min,max,d,v );
	thetaBar->setEnabled ( true );
}

void RelKinemaCls::updateThetaBar()
{
	double v=0;
	bool ok;
	double d=thetaStepBox->text().toDouble ( &ok );
	if ( !ok ) return;
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			v=th3;
			break;
		case ST_THETACM:
			v=th3c;
			break;
		case ST_Q:
			v=theq;
			break;
	}
	thetaBar->blockSignals ( true );
	thetaBar->setValue ( floor ( v/d+0.5 ) );
	thetaBar->blockSignals ( false );
}

void RelKinemaCls::mdmSlot()
{
	openMdm();
}

void RelKinemaCls::openMdm ( int page )
{
	Qt::WFlags wf=Qt::WDestructiveClose;
	if ( page!=0 )
	{
		wf|=Qt::WStyle_StaysOnTop;
	}
	mdmCls *win = new mdmCls ( NULL,NULL,wf );
	connect ( win, SIGNAL ( updateMass() ), this, SLOT ( updateMassSlot() ) );
	win->massdata=&MASSDATA;
	win->home=HOME;
	win->AMU=AMU;
	win->Me=Me;
	win->wb=webbrowser;
	win->openMassData ( 0 );
	win->setFont ( thefont );
	win->show();
	win->mdmStack->raiseWidget ( page );
	if ( page!=0 )
	{
		win->mess ( "Select a mass data file","red" );
		win->createMassDataBut->setEnabled ( false );
		win->editDataBut->setEnabled ( true );
	}
}

void RelKinemaCls::keyPressEvent ( QKeyEvent *e )
{
	if ( e->state() ==Qt::ControlButton )
	{
		switch ( e->key() )
		{
			case Qt::Key_S:
				saveRectCondSlot();
				break;
			case Qt::Key_O:
				loadRectCondSlot();
				break;
			default:
				e->ignore();
				return;
		}
	}
	else
	{
		switch ( e->key() )
		{
			case Qt::Key_Q:
				close();
				break;
			case Qt::Key_Escape:
				setFocus(); // escape from edit focus
				break;
			case Qt::Key_T:
				if ( tableBut->isEnabled() ) showResultListSlot();
				break;
			case Qt::Key_D:
				mdmSlot();
				break;
			case Qt::Key_K:
				if ( calcBut->isEnabled() ) calcSlot();
				break;
			case Qt::Key_F5:
				if ( redoBut->isEnabled() ) redoSlot();
				break;
			case Qt::Key_V:
				showAbout();
				break;
			case Qt::Key_C:
				unsetMassSlot_all();
				break;
			case Qt::Key_S:
				toggleSettingsSlot();
				break;
			case Qt::Key_A:
				showMenu();
				break;
			case Qt::Key_1:
				EmissionBox->showPage ( EmissionBox->page ( 0 ) );
				break;
			case Qt::Key_2:
				EmissionBox->showPage ( EmissionBox->page ( 1 ) );
				break;
			case Qt::Key_3:
				EmissionBox->showPage ( EmissionBox->page ( 2 ) );
				break;
			case Qt::Key_M:
				if ( windowState() & Qt::WindowMaximized )
				{
					setWindowState ( Qt::WindowNoState );
				}
				else
				{
					if ( windowState() & Qt::WindowFullScreen )
					{
						setWindowState ( Qt::WindowNoState );
					}
					setWindowState ( Qt::WindowMaximized );
				}
				fsBut->setOn(false);
				break;
			case Qt::Key_F:
				fsSlot();
				break;
			case Qt::Key_U:
				afSlot();
				break;
			case Qt::Key_R:
				ifSlot();
				break;
			case Qt::Key_L:
				ffSlot();
				break;
			default:
				e->ignore();
				return;
		}
	}
	e->accept();
}

void RelKinemaCls::showAbout()
{
	aboutcls subWin ( NULL,NULL,Qt::WDestructiveClose );
	subWin.wb=webbrowser;
	subWin.exec();
}

void RelKinemaCls::toggleSettingsSlot()
{
	settingsBut->toggle();
	settingsSlot();
}

void RelKinemaCls::settingsSlot()
{
	if ( settingsBut->isOn() )
	{
		initSettingsPage ( true );
		redoBut->setEnabled ( false );
		mainStack->raiseWidget ( 1 );
		settingsBut->setText ( "Return to main" );
	}
	else
	{
		redoBut->setEnabled ( true );
		mainStack->raiseWidget ( 0 );
		settingsBut->setText ( "Settings" );
	}
}

void RelKinemaCls::initSettingsPage ( bool first )
{
	confDoubleNumberFormatBox->blockSignals ( true );
	confMassDataDirBox->blockSignals ( true );
	configEdit->blockSignals ( true );
	confMassDataDirLbl->clear();
	confDoubleNumberFormatLbl->clear();

	if ( first )
	{
		confMassDataDirBox->setText ( MASSDATA );
		confDoubleNumberFormatBox->setText ( double_format );
	}

	configEdit->clear();
	rcLbl->setText ( CONFIGFILE );
	int i=0;
	QFile f ( CONFIGFILE );
	if ( f.open ( IO_ReadOnly ) )
	{
		QTextStream stream ( &f );
		QString line;
		while ( !stream.atEnd() )
		{
			line = stream.readLine().simplifyWhiteSpace();
			if ( line.startsWith ( "[" ) &&i!=0 ) configEdit->append ( "\n" );
			configEdit->append ( line );
			i++;
		}
		f.close();
	}
	configEdit->setCursorPosition ( 0,0 );

	fontBut->blockSignals ( true );
	fontBut->setFont ( thefont );
	fontBut->blockSignals ( false );

	confDoubleNumberFormatBox->blockSignals ( false );
	confMassDataDirBox->blockSignals ( false );
	configEdit->blockSignals ( false );
}

bool RelKinemaCls::checkDoubleNumberFormat ( QString fmt )
{
	QString s="";
	double zero=0.;
	bool ok;
	s.sprintf ( fmt,zero );
	zero=s.toDouble ( &ok );
	if ( ok&&zero==0. ) return true;
	return false;
}

void RelKinemaCls::showMessL ( QString m, QString c, QLabel *L )
{
	L->setPaletteForegroundColor ( c );
	L->setText ( m );
}

void RelKinemaCls::confAppSlot()
{
	int rc=0;
	QFileInfo fi ( confMassDataDirBox->text() );
	if ( QFile::exists ( confMassDataDirBox->text() ) && fi.isReadable() )
	{
		MASSDATA=confMassDataDirBox->text();
		showMessL ( "ok","blue",confMassDataDirLbl );
		rc=1;
	}
	else
	{
		showMessL ( "NG", "red", confMassDataDirLbl );
	}

	if ( checkDoubleNumberFormat ( confDoubleNumberFormatBox->text() ) )
	{
		double_format=confDoubleNumberFormatBox->text();
		showMessL ( "ok","blue",confDoubleNumberFormatLbl );
		rc|=2;
	}
	else
	{
		showMessL ( "NG","red",confDoubleNumberFormatLbl );
	}
	if ( rc==3 )
	{
		confAppBut->setEnabled ( false );
		saveConfBut->setEnabled ( true );
	}
	thefont=fontBut->font();
	setDispFont();
}

void RelKinemaCls::redoSlot()
{
	reloadMass_all();
	if ( ReactionConditionBox->isEnabled() )
	{
		showEnergyLE ( K1,K1Box );
		showEnergyLE ( Ex,ExBox );
		rectCondSlot_K1();
	}
}

void RelKinemaCls::saveConfSlot()
{
// first contents of configEdit goes to .config
	QFile f ( CONFIGFILE );
	if ( f.open ( IO_WriteOnly ) )
	{
		QTextStream stream ( &f );
		stream << configEdit->text();
		f.close();
	}
	QString mdn=MASSDATA;
	QString dfn=double_format;
	QFont font=thefont;
	loadConfig ( CONFIGFILE );
	if ( confMassDataDirLbl->text() =="ok" )
	{
		MASSDATA=mdn;
		double_format=dfn;
		thefont=font;
	}
// then the changes on massdata etc on the lineedit will be saved
	saveConfig ( CONFIGFILE );
	reinitRelKinema();
	initSettingsPage();
	saveConfBut->setEnabled ( false );
	confDoubleNumberFormatLbl->clear();
	confMassDataDirLbl->clear();
}

void RelKinemaCls::returnSlot()
{
	setFocus();
}

void RelKinemaCls::massDataDirSlot()
{
	QString s=KFileDialog::getExistingDirectory ( MASSDATA,this,"Choose mass data directory" );
	if ( !s.isEmpty() )
	{
		confMassDataDirBox->setText ( s );
		updateMassSlot();
	}
	else
	{
		if ( KMessageBox::Yes==KMessageBox::questionYesNo ( this,
		        "May I run the Massdata Manager?\n",
		        "You need mass data" ) )  openMdm ( 1 );
	}
}

void RelKinemaCls::registParam ( size_t pfzc_, size_t ref, QString str )
{
	char cstr[LEN_FZCSTR_MAX];
	size_t pcstr= ( size_t ) &cstr[0];
	strcpy ( cstr,str.latin1() );
	fzc_regpar ( pfzc_, ref, pcstr, FZCPK_REAL );
}

void RelKinemaCls::registParams ( size_t prkc_, size_t pfzc_ )
{
	registParam ( pfzc_, rkc_getp_m1 ( prkc_ ), "m1" );
	registParam ( pfzc_, rkc_getp_m2 ( prkc_ ), "m2" );
	registParam ( pfzc_, rkc_getp_m3 ( prkc_ ), "m3" );
	registParam ( pfzc_, rkc_getp_m4 ( prkc_ ), "m4" );

	registParam ( pfzc_, rkc_getp_E1 ( prkc_ ), "E1" );
	registParam ( pfzc_, rkc_getp_E2 ( prkc_ ), "E2" );
	registParam ( pfzc_, rkc_getp_E3 ( prkc_ ), "E3" );
	registParam ( pfzc_, rkc_getp_E4 ( prkc_ ), "E4" );

	registParam ( pfzc_, rkc_getp_p1 ( prkc_ ), "p1" );
	registParam ( pfzc_, rkc_getp_p2 ( prkc_ ), "p2" );
	registParam ( pfzc_, rkc_getp_p3 ( prkc_ ), "p3" );
	registParam ( pfzc_, rkc_getp_p4 ( prkc_ ), "p4" );

	registParam ( pfzc_, rkc_getp_K1 ( prkc_ ), "K1" );
	registParam ( pfzc_, rkc_getp_K3 ( prkc_ ), "K3" );
	registParam ( pfzc_, rkc_getp_K4 ( prkc_ ), "K4" );

	registParam ( pfzc_, rkc_getp_E1c ( prkc_ ), "E1c" );
	registParam ( pfzc_, rkc_getp_E2c ( prkc_ ), "E2c" );
	registParam ( pfzc_, rkc_getp_E3c ( prkc_ ), "E3c" );
	registParam ( pfzc_, rkc_getp_E4c ( prkc_ ), "E4c" );

	registParam ( pfzc_, rkc_getp_p1c ( prkc_ ), "p1c" );
	registParam ( pfzc_, rkc_getp_p2c ( prkc_ ), "p2c" );
	registParam ( pfzc_, rkc_getp_p3c ( prkc_ ), "p3c" );
	registParam ( pfzc_, rkc_getp_p4c ( prkc_ ), "p4c" );

	registParam ( pfzc_, rkc_getp_K1c ( prkc_ ), "K1c" );
	registParam ( pfzc_, rkc_getp_K2c ( prkc_ ), "K2c" );
	registParam ( pfzc_, rkc_getp_K3c ( prkc_ ), "K3c" );
	registParam ( pfzc_, rkc_getp_K4c ( prkc_ ), "K4c" );

	registParam ( pfzc_, rkc_getp_QValue ( prkc_ ), "QValue" );
	registParam ( pfzc_, rkc_getp_Ex ( prkc_ ), "Ex" );
	registParam ( pfzc_, rkc_getp_ExMax ( prkc_ ), "ExMax" );
	registParam ( pfzc_, rkc_getp_K1Min ( prkc_ ), "K1Min" );
	registParam ( pfzc_, rkc_getp_p1Min ( prkc_ ), "p1Min" );
	registParam ( pfzc_, rkc_getp_K1cMin ( prkc_ ), "K1cMin" );
	registParam ( pfzc_, rkc_getp_p1cMin ( prkc_ ), "p1cMin" );

	registParam ( pfzc_, rkc_getp_theM ( prkc_ ), "theM" );
	registParam ( pfzc_, rkc_getp_gamma ( prkc_ ), "gamma" );
	registParam ( pfzc_, rkc_getp_beta ( prkc_ ), "beta" );

	registParam ( pfzc_, rkc_getp_th3Max ( prkc_ ), "th3Max" );
	registParam ( pfzc_, rkc_getp_th4Max ( prkc_ ), "th4Max" );
	registParam ( pfzc_, rkc_getp_th3 ( prkc_ ), "th3" );
	registParam ( pfzc_, rkc_getp_th4 ( prkc_ ), "th4" );
	registParam ( pfzc_, rkc_getp_th3c ( prkc_ ), "th3c" );
	registParam ( pfzc_, rkc_getp_th4c ( prkc_ ), "th4c" );

	registParam ( pfzc_, rkc_getp_theq ( prkc_ ), "q" );
	registParam ( pfzc_, rkc_getp_qMin ( prkc_ ), "qMin" );
	registParam ( pfzc_, rkc_getp_qMax ( prkc_ ), "qMax" );

	registParam ( pfzc_, rkc_getp_J3 ( prkc_ ), "J3" );
	registParam ( pfzc_, rkc_getp_J4 ( prkc_ ), "J4" );

	registParam ( pfzc_, rkc_getp_KShift ( prkc_ ), "KS" );
	registParam ( pfzc_, rkc_getp_KFactor ( prkc_ ), "KF" );
}

void RelKinemaCls::calcSlot()
{
	rkCalcCls *win = new rkCalcCls ( NULL,NULL,Qt::WDestructiveClose,
	                                 &CONFIGFILE, pfzc );
	win->setFont ( thefont );

	if ( !win->initRKC() )
	{
		KMessageBox::sorry ( this,"Failed to start calculator","internal error" );
		return;
	}
	win->show();
}

/*
void RelKinemaCls::stopSlot()
{
	stopBut->setEnabled ( false );
}
*/

void RelKinemaCls::mousePressEvent ( QMouseEvent *e )
{
	switch ( e->button() )
	{
		case RightButton:
			e->accept();
			showMenu();
			break;
		default:
			e->ignore();
			break;
	}
}

void RelKinemaCls::showMenu()
{
	QPixmap pmassdata ( pix_massdata );
	QPixmap pcalc ( pix_calc );
	QPixmap ptable ( pix_table );
	QPixmap psettings ( pix_settings );
	QPixmap pexit ( pix_exit );
	QPixmap pabout ( pix_about );
	QPixmap predo ( pix_redo );
	QPixmap psave ( pix_save );
	QPixmap pload ( pix_load );

	QPopupMenu pm ( this );
	pm.setFont ( thefont );
	QPalette p=pm.palette();
	p.setColor ( QColorGroup::Highlight, qRgb ( 205,234,255 ) );
	p.setColor ( QColorGroup::HighlightedText, qRgb ( 0,71,171 ) );
	pm.setPalette ( p );

	if ( !settingsBut->isOn() && ReactionBox->rect().contains ( QCursor::pos() ) )
	{
		pm.insertItem ( "inverse reaction",this,SLOT ( inverseSlot() ) );
		pm.insertItem ( "reverse reaction",this,SLOT ( reverseSlot() ) );
		pm.insertItem ( "swap initial",this,SLOT ( swapIniSlot() ) );
		pm.insertItem ( "swap final",this,SLOT ( swapFinSlot() ) );
		pm.insertSeparator();
	}

	if ( !settingsBut->isOn() )
	{
		pm.insertItem ( pmassdata,"Manage Mass Data",this,SLOT ( mdmSlot() ),Key_M );
		int idt=pm.insertItem ( ptable,"Show Table",this,SLOT ( showResultListSlot() ),Key_T );
		int idc=pm.insertItem ( pcalc, "Online Calculator",  this, SLOT ( calcSlot() ), Key_K );
		pm.insertSeparator();
		pm.insertItem ( "Check all for plot",this,SLOT ( checkAllPlotSlot() ) );
		pm.insertItem ( "Check none for plot",this,SLOT ( checkNonePlotSlot() ) );
		pm.insertSeparator();
		int idr=pm.insertItem ( predo,"Refresh",this,SLOT ( redoSlot() ), Key_R );
		pm.insertItem ( psettings,"Settings",this,SLOT ( toggleSettingsSlot() ),Key_S );
		pm.setItemEnabled ( idt,tableBut->isEnabled() );
		pm.setItemEnabled ( idc,calcBut->isEnabled() );
		pm.setItemEnabled ( idr,redoBut->isEnabled() );
		pm.insertSeparator();
		pm.insertItem ( "Clear Reaction",this,SLOT ( unsetMassSlot_all() ),Key_C );
		pm.insertItem ( pload, "Load Reaction",this,SLOT ( loadRectCondSlot() ),CTRL+Key_O );
		int ids=pm.insertItem ( psave, "Save Reaction",this,SLOT ( saveRectCondSlot() ),CTRL+Key_S );
		pm.setItemEnabled ( ids, ReactionConditionBox->isEnabled() );
	}
	else
	{
		pm.insertItem ( "Return to Main",this,SLOT ( toggleSettingsSlot() ), Key_S );
	}
	pm.insertSeparator();
	pm.insertSeparator();
	pm.insertItem ( pabout,"About",this,SLOT ( showAbout() ) );
	pm.insertItem ( pexit,"Exit",this,SLOT ( close() ),Key_Q );
	pm.exec ( QCursor::pos() );
}

void RelKinemaCls::setReaction ( QString rc )
{
	QString p,t,e,r;
	int k;
	for ( unsigned int i=0;i<rc.length();i++ )
	{
		if ( rc[i]=='(' )
		{
			k=i;
			break;
		}
		else
		{
			t.append ( rc[i] );
		}
	}
	for ( unsigned int i=k+1;i<rc.length();i++ )
	{
		if ( rc[i]==',' )
		{
			k=i;
			break;
		}
		else
		{
			p.append ( rc[i] );
		}
	}
	for ( unsigned int i=k+1;i<rc.length();i++ )
	{
		if ( rc[i]==')' )
		{
			k=i;
			break;
		}
		else
		{
			e.append ( rc[i] );
		}
	}
	for ( unsigned int i=k+1;i<rc.length();i++ ) r.append ( rc[i] );

	QString A,E;
	nameToAE ( t,&A,&E );
	ATargBox->setText ( A );
	ETargBox->setText ( E );
	nameToAE ( p,&A,&E );
	AProjBox->setText ( A );
	EProjBox->setText ( E );
	nameToAE ( e,&A,&E );
	AEjecBox->setText ( A );
	EEjecBox->setText ( E );
	nameToAE ( r,&A,&E );
	AResiBox->setText ( A );
	EResiBox->setText ( E );
	setMassSlot_all();
}

void RelKinemaCls::nameToAE ( QString name, QString *A, QString *E )
{
	int k;
	*A="";
	*E="";
	for ( unsigned int i=0;i<name.length();i++ )
	{
		if ( name[i].isNumber() )
		{
			A->append ( name[i] );
		}
		else
		{
			k=i;
			break;
		}
	}
	for ( unsigned int i=k;i<name.length();i++ ) E->append ( name[i] );

}

void RelKinemaCls::loadRectCondSlot()
{
	KConfig *conf = new KConfig ( CONFIGFILE );
	if ( conf==NULL ) return;

	rclistCls win ( NULL,NULL,Qt::WDestructiveClose,conf, thefont );
	int i=win.exec();
	delete conf;
	if ( i!=QDialog::Accepted ) return;

	QString rc=win.rc;
	if ( rc.isEmpty() ) return;

	setReaction ( rc.section ( ";",0,0 ) );
	int ie;
	QString eu=rc.section ( ";",3,3 );
	if ( eu=="keV" )
	{
		ie=0;
	}
	else if ( eu=="MeV" )
	{
		ie=1;
	}
	else if ( eu=="GeV" )
	{
		ie=2;
	}
	else if ( eu=="TeV" )
	{
		ie=3;
	}
	EUnitBox->setCurrentItem ( ie );
	changeEUnitSlot();
	K1Box->setText ( rc.section ( ";",1,1 ) );
	ExBox->setText ( rc.section ( ";",2,2 ) );
}

void RelKinemaCls::saveRectCondSlot()
{
	if ( theReaction.isEmpty() ) return;
	KConfig *conf = new KConfig ( CONFIGFILE );
	if ( conf==NULL ) return;

	linereadCls *dlg = new linereadCls ( NULL,NULL,Qt::WDestructiveClose,"Save Reaction Condition: "
	                                     +theReaction,"Any Comments?", thefont );

	if ( dlg->exec() !=QDialog::Accepted ) return;

	QString rem=dlg->input;
	delete dlg;

	QString s=theReaction+";"+QString::number ( K1,'g',16 ) +";"+QString::number ( Ex,'g',16 )
	          +";"+EUnitBox->currentText();

	KConfigGroup saveGroup ( conf, "Save" );
	QStringList sl0;
	QStringList sl=saveGroup.readListEntry ( "list",sl0 );
	if ( !sl.isEmpty() )
	{
		for ( QStringList::Iterator it=sl.begin();it!=sl.end();it++ )
		{
			if ( ( *it ) !=s ) sl0.append ( *it );
		}
	}
	KConfigGroup remGroup ( conf,"Rem" );
	remGroup.writeEntry ( s,rem );
	sl0.append ( s );
	saveGroup.writeEntry ( "list",sl0 );
	remGroup.sync();
	saveGroup.sync();

	delete conf;
	return;
}

void RelKinemaCls::checkAppSlot()
{
	confDoubleNumberFormatLbl->clear();
	confMassDataDirLbl->clear();
	confAppBut->setEnabled ( true );
}

void RelKinemaCls::checkConfSlot()
{
	saveConfBut->setEnabled ( true );
}

void RelKinemaCls::fontSlot ( const QFont &unused )
{
	checkAppSlot();
}

void RelKinemaCls::loadDefConfSlot()
{
	KConfig *conf = new KConfig ( CONFIGFILE );
	conf->deleteGroup ( "General" );
	conf->deleteGroup ( "Parameters" );
	conf->deleteGroup ( "Externals" );
	conf->sync();
	delete conf;
	loadConfig ( CONFIGFILE );
	saveConfig ( CONFIGFILE );
	reinitRelKinema();
	initSettingsPage ( true );
}

void RelKinemaCls::expr1Slot()
{
	exprSlot ( 0 );
}
void RelKinemaCls::expr2Slot()
{
	exprSlot ( 1 );
}
void RelKinemaCls::expr3Slot()
{
	exprSlot ( 2 );
}
void RelKinemaCls::expr4Slot()
{
	exprSlot ( 3 );
}

void RelKinemaCls::exprSlot ( int id )
{
	exprSet[id]=false;
	valBox[id]->setText ( "Undefined" );
	plotBox[id]->setEnabled ( false );
	valBox[id]->setPaletteForegroundColor ( "black" );
}

void RelKinemaCls::exprWarn ( int id, QString mess )
{
	valBox[id]->setPaletteForegroundColor ( "red" );
	valBox[id]->setText ( mess );
}

void RelKinemaCls::expr1SetSlot()
{
	exprSetSlot ( 0 );
}
void RelKinemaCls::expr2SetSlot()
{
	exprSetSlot ( 1 );
}
void RelKinemaCls::expr3SetSlot()
{
	exprSetSlot ( 2 );
}
void RelKinemaCls::expr4SetSlot()
{
	exprSetSlot ( 3 );
}
void RelKinemaCls::expr1SelectSlot()
{
	exprSetSlot ( 0, false );
}
void RelKinemaCls::expr2SelectSlot()
{
	exprSetSlot ( 1, false );
}
void RelKinemaCls::expr3SelectSlot()
{
	exprSetSlot ( 2, false );
}
void RelKinemaCls::expr4SelectSlot()
{
	exprSetSlot ( 3, false );
}

void RelKinemaCls::exprSetSlot ( int id, bool setHist )
{
	QString str=expr[id]+"=\""+exprBox[id]->currentText() +"\"";
	char cstr[LEN_FZCSTR_MAX];
	strcpy ( cstr,str.latin1() );

	int rc=fzc_setparse_formula ( pfzc, ( size_t ) &cstr );
	if ( rc == FZCSTA_MACSET )
	{
		exprSet[id]=true;
		updateExprSlot();
		plotBox[id]->setEnabled ( true );
		if ( setHist )
		{
			exprBox[id]->addToHistory ( exprBox[id]->currentText() );
			for ( int i=0;i<nexpmax;i++ )
			{
				if ( i==id ) continue;
				exprBox[i]->blockSignals ( true );
				QString str=exprBox[i]->currentText();
				exprBox[i]->setHistoryItems ( exprBox[id]->historyItems() );
				exprBox[i]->completionObject()->setItems ( exprBox[id]->completionObject()->items() );
				exprBox[i]->setCurrentText ( str );
				exprBox[i]->blockSignals ( false );
			}
		}
	}
	else
	{
		exprWarn ( id, rkCalcCls::rkcErrStr ( rc ) );
		plotBox[id]->setEnabled ( false );
	}
}

void RelKinemaCls::updateExprSlot()
{
	char cstr[LEN_FZCSTR_MAX];
	for ( int i=0;i<nexpmax;i++ )
	{
		if ( !exprSet[i] ) continue;
		strcpy ( cstr,expr[i].latin1() );
		int rc=fzc_setparse_formula ( pfzc, ( size_t ) &cstr );
		if ( rc==0 )
		{
			rc=fzc_eval ( pfzc );
			if ( rc!=0 )
			{
				exprWarn ( i, "Eval error" );
				continue;
			}
		}
		else
		{
			exprWarn ( i, "Syntax error" );
			continue;
		}
		QString s;
		valBox[i]->setText ( s.sprintf ( double_format,fzc_get_ans ( pfzc ) ) );
	}
}

void RelKinemaCls::setPlottables ( int pm )
{
	for ( int i=0;i<nrkpmax;i++ )
	{
		plotXBox[i]->setChecked ( BT ( pm ,i ) );
	}
}

void RelKinemaCls::checkAllPlotSlot()
{
	setPlottables ( -1 );
}

void RelKinemaCls::checkNonePlotSlot()
{
	setPlottables ( 0 );
}

void RelKinemaCls::clearHistSlot()
{
	for ( int i=0;i<4;i++ )
	{
		exprBox[i]->blockSignals ( true );
		exprBox[i]->clearHistory();
		exprBox[i]->completionObject()->clear();
		exprBox[i]->blockSignals ( false );
	}
}

void RelKinemaCls::updateMassSlot()
{
	setParMass();
	redoSlot();
}

void RelKinemaCls::sfSlot()
{
changeFontSize(-1);
}

void RelKinemaCls::efSlot()
{
changeFontSize(1);
}

void RelKinemaCls::changeFontSize(int d)
{
	int const min_pointsize=4;
	int const max_pointsize=32;
	int p=thefont.pointSize();
	p+=d;
	if ( p<min_pointsize )
	{
		p=min_pointsize;
	}
	else if ( p>max_pointsize )
	{
		p=max_pointsize;
	}
	thefont.setPointSize(p);
	setDispFont();
	if(settingsBut->isOn())
	{
		fontBut->blockSignals ( true );
		fontBut->setFont ( thefont );
		fontBut->blockSignals ( false );
	}
}

void RelKinemaCls::fsSlot()
{
	if ( windowState() & Qt::WindowFullScreen )
	{
		setWindowState ( Qt::WindowNoState );
		fsBut->setOn(false);
	}
	else
	{
		setWindowState ( Qt::WindowFullScreen );
		fsBut->setOn(true);
	}
}

void RelKinemaCls::ifSlot()
{
	QValueList<int> s;
	s.append(500);
	s.append(0);
	theSplitter->setSizes(s);
}

void RelKinemaCls::ffSlot()
{
	QValueList<int> s;
	s.append(0);
	s.append(500);
	theSplitter->setSizes(s);
}

void RelKinemaCls::afSlot()
{
	QValueList<int> s;
	s.append(500);
	s.append(500);
	theSplitter->setSizes(s);
}

#include "relkinemaCls.moc"
