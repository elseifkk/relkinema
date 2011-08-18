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
#include "relkinemaCls.h"
#include "resultwindowcls.h"
#include "mdmcls.h"
#include "aboutcls.h"
#include "rwthreadcls.h"
#include "rkCore.h"

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
#include <qmessagebox.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qtable.h>

#include <kconfig.h>
#include <knuminput.h>

#define LEN_ELEMENT 8

#define fontname_def "Sans Serif"
#define fontsize_def "11"

QString fontname="Helvetica";
int fontsize=11;
QFont thefont;

#define FIREFOX "firefox"
QString webbrowser=FIREFOX;

#define CONFIG "/.relkinemarc"
#define MASSDATA_DEF "massdata"

QString HOME;
QString MASSDATA;
QString double_format;

QString const double_format_def="%.6g";

// NIST (2005)
#define hbarc_DEF "197.326968"
#define AMU_DEF "931.494043"
double hbarc = 197.326968; // MeV fm
double AMU   = 931.494043; // in MeV
double const RTD=180./M_PI;
double Me,Mn,Mp,Mt,Md,Ma;

// Reference Unknown
#define r0_DEF   "1.2"
#define e2_DEF   "1.4406466"
#define Bv_DEF   "15.56"
#define Bs_DEF   "17.23"
#define Bsym_DEF "23.29"
#define Bp_DEF   "12.00"
#define Bc_DEF   "0.697"
#define hcRinf_DEF   "13.6056981"

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

QString const EUnitLbl[4]=
{
	"keV",
	"MeV",
	"GeV",
	"TeV"
};

double const MU[4]=
{
	1.0e3,
	1.0,
	1.0e-3,
	1.0/AMU
};

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

rwThreadCls *rwt;

RelKinemaCls::RelKinemaCls ( QWidget *parent, const char *name )
		:RelKinemaDlg ( parent, name )
		,th3 ( 0 ),th4 ( 0 )
		,th3c ( 0 ),th4c ( 0 )
		,K3 ( 0 ),K4 ( 0 )
		,p3 ( 0 ),p4 ( 0 )
		,J3 ( 0 ),J4 ( 0 )
		,kinemaShift ( 0 ), factorK ( 0 )
		,theq ( 0 )

		,theM ( 0 )
		,K1th ( 0 ),K1cth ( 0 ),Ex ( 0 ),Exmax ( 0 )
		,p1th ( 0 ),p1cth ( 0 )
		,E1 ( 0 ),E2 ( 0 ),E3 ( 0 ),E4 ( 0 )
		,K1 ( 0 ),K2 ( 0 )
		,p1 ( 0 ),p2 ( 0 )
		,E1c ( 0 ),E2c ( 0 ),E3c ( 0 ),E4c ( 0 )
		,K1c ( 0 ),K2c ( 0 ),K3c ( 0 ),K4c ( 0 )
		,p1c ( 0 ),p2c ( 0 ),p3c ( 0 ),p4c ( 0 )
		,gamma ( 0 ),beta ( 0 )
		,thetaMax ( 0 ), limThetaCM ( 0 ), limq ( 0 )
		,m10 ( 0 ),m20 ( 0 ),m30 ( 0 ),m40 ( 0 )
		,m1 ( 0 ),m2 ( 0 ),m3 ( 0 ),m4 ( 0 )
		,a1 ( 0 ),a2 ( 0 ),a3 ( 0 ),a4 ( 0 )
		,z1 ( 0 ),z2 ( 0 ),z3 ( 0 ),z4 ( 0 )
		,QValue ( 0 )
		,qmin ( 0 ),qmax ( 0 )
		,inv ( 0 ),thetaMaxNe ( 0 )
		,K1Set ( 0 ), ExOk ( 0 )
		, K3sign ( 1 )
		,timerid ( 0 )
{
	bzero ( &massSet,sizeof ( massSet ) );
	bzero ( &rectSet,sizeof ( rectSet ) );

	getThetaBarValue=&RelKinemaCls::getThetaBarPosInRad;
	tbEval=&RelKinemaCls::thetaLabSlot;
	tbMax=&thetaMax;
	tbMin=&fZero;
	tbTarg=&th3;

	HOME=getenv ( "HOME" );
	if ( HOME.length() ==0 )
		QMessageBox::critical ( this, "relkinema: ", "We have no HOME!", tr ( "&ok" ) );
	loadConfig ( HOME+CONFIG );
	if ( !QFile::exists ( HOME+CONFIG ) ) saveConfig ( HOME+CONFIG );
	if ( !QFile::exists ( MASSDATA ) )
		QMessageBox::warning ( this, "relkinema: ", "Cannot locate massdata!", tr ( "&ok" ) );

	K1Set=false;
	ExOk=true;
	reinitRelKinema();
	ReactionConditionBox->setEnabled ( false );
	EmissionAngleBox->setEnabled ( false );
	tableBut->setEnabled ( false );
	EmissionBox->setEnabled ( false );

	thetaStepBox->setPrecision ( 4 );
	thetaStepBox->setMaxValue ( 1.0e4 );
	thetaStepBox->blockSignals ( true );
	thetaStepBox->setValue ( 1.0 );
	thetaStepBox->blockSignals ( false );

	int aa1=a1,aa2=a2,aa3=a3,aa4=a4;
	if ( nc2.length() >0 ) ETargBox->setText ( nc2 );
	if ( nc1.length() >0 ) EProjBox->setText ( nc1 );
	if ( nc3.length() >0 ) EEjecBox->setText ( nc3 );
	if ( nc4.length() >0 ) EResiBox->setText ( nc4 );
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
}

void RelKinemaCls::setVer ( const char *v )
{
	QString ver=v;
	ver.prepend ( "Release " );
	verLbl->setText ( ver );
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

void RelKinemaCls::timerEvent ( QTimerEvent *e )
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

	thefont.setFamily ( fontname );
	thefont.setPointSize ( fontsize );
	setFont ( thefont );
	mainStack->setFont ( thefont );
	QFont f=thefont;
	f.setPointSize ( fontsize-1 );
	k3unitLbl->setFont ( f );
	k4unitLbl->setFont ( f );
	k3cunitLbl->setFont ( f );
	k4cunitLbl->setFont ( f );
	p3unitLbl->setFont ( f );
	p4unitLbl->setFont ( f );
	p3cunitLbl->setFont ( f );
	p4cunitLbl->setFont ( f );
	dK3_dth3_unitLbl->setFont ( f );
	KparamUnitLbl->setFont ( f );
}

void  RelKinemaCls::closeEvent ( QCloseEvent *e )
{
	saveConfig ( HOME+CONFIG );
	close();
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

	generalGroup.writeEntry ( "Font name",fontname );
	generalGroup.writeEntry ( "Font size",QString::number ( fontsize ) );
	generalGroup.writeEntry ( "Mass data directory",MASSDATA );
	generalGroup.writeEntry ( "Double number format",double_format );

	externalGroup.writeEntry ( "Web browser",webbrowser );

	parameterGroup.writeEntry ( "AMU in MeV", AMU_DEF );
	parameterGroup.writeEntry ( "hbarc in MeV*fm", QString::number ( hbarc ) );
	parameterGroup.writeEntry ( "r0 in fm", QString::number ( r0 ) );
	parameterGroup.writeEntry ( "e2 in MeV", QString::number ( e2 ) );
	parameterGroup.writeEntry ( "Bv", QString::number ( Bv ) );
	parameterGroup.writeEntry ( "Bs", QString::number ( Bs ) );
	parameterGroup.writeEntry ( "Bsym", QString::number ( Bsym ) );
	parameterGroup.writeEntry ( "Bc", QString::number ( Bc ) );
	parameterGroup.writeEntry ( "Bp", QString::number ( Bp ) );
	parameterGroup.writeEntry ( "Rydberg energy in eV", QString::number ( hcRinf ) );

	historyGroup.writeEntry ( "Target name",    nc2 );
	historyGroup.writeEntry ( "Projectile name",nc1 );
	historyGroup.writeEntry ( "Ejectile name",  nc3 );
	historyGroup.writeEntry ( "Residual name",  nc4 );

	historyGroup.writeEntry ( "Target A",    QString::number ( a2 ) );
	historyGroup.writeEntry ( "Projectile A",QString::number ( a1 ) );
	historyGroup.writeEntry ( "Ejectile A",  QString::number ( a3 ) );
	historyGroup.writeEntry ( "Residual A",  QString::number ( a4 ) );

	historyGroup.writeEntry ( "Incident energy in MeV",QString::number ( K1 ) );
	historyGroup.writeEntry ( "Excitation energy in MeV",QString::number ( Ex ) );

	generalGroup.sync();
	parameterGroup.sync();
	externalGroup.sync();
	historyGroup.sync();

	delete conf;
}

void RelKinemaCls::loadConfig ( QString f )
{
	bool ok;
	double v;
	int i;
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;

	KConfigGroup generalGroup ( conf, "General" );
	KConfigGroup parameterGroup ( conf,"Parameters" );
	KConfigGroup externalGroup ( conf,"Externals" );
	KConfigGroup historyGroup ( conf, "History" );

	fontname=generalGroup.readEntry ( "Font name",fontname_def );
	i=generalGroup.readEntry ( "Font size",fontsize_def ).toInt ( &ok );
	if ( ok&&i>0 ) fontsize=i;
	MASSDATA = generalGroup.readEntry ( "Mass data directory",MASSDATA_DEF );
	double_format = generalGroup.readEntry ( "Double number format",double_format_def );
	if ( !checkDoubleNumberFormat ( double_format ) ) double_format=double_format_def;

	webbrowser = externalGroup.readEntry ( "Web browser",FIREFOX );

	v=parameterGroup.readEntry ( "AMU in MeV", AMU_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) AMU=v;

	v=parameterGroup.readEntry ( "hbarc in MeV*fm", hbarc_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) hbarc=v;

	v=parameterGroup.readEntry ( "r0 in fm", r0_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) r0=v;

	v=parameterGroup.readEntry ( "e2 in MeV", e2_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) e2=v;

	v=parameterGroup.readEntry ( "Bv", Bv_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) Bv=v;

	v=parameterGroup.readEntry ( "Bs", Bs_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) Bs=v;

	v=parameterGroup.readEntry ( "Bsym", Bsym_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) Bsym=v;

	v=parameterGroup.readEntry ( "Bp", Bp_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) Bp=v;

	v=parameterGroup.readEntry ( "Bc", Bc_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) Bc=v;

	v=parameterGroup.readEntry ( "Rydberg energy in eV", hcRinf_DEF ).toDouble ( &ok );
	if ( ok&&v>0 ) hcRinf=v;

	nc2=historyGroup.readEntry ( "Target name","" );
	nc1=historyGroup.readEntry ( "Projectile name","" );
	nc3=historyGroup.readEntry ( "Ejectile name","" );
	nc4=historyGroup.readEntry ( "Residual name","" );

	a2=historyGroup.readEntry ( "Target A","" ).toInt();
	a1=historyGroup.readEntry ( "Projectile A","" ).toInt();
	a3=historyGroup.readEntry ( "Ejectile A","" ).toInt();
	a4=historyGroup.readEntry ( "Residual A","" ).toInt();

	K1=historyGroup.readEntry ( "Incident energy in MeV","" ).toDouble();
	Ex=historyGroup.readEntry ( "Excitation energy in MeV","" ).toDouble();

	delete conf;
}


double RelKinemaCls::readMass ( QString f, QString n )
{
	QString fin;
	FILE *fp;
	char name[LEN_ELEMENT];
	double value,m;
	unsigned int lin=strlen ( n.latin1() );

	fin=MASSDATA;
	fin+="/"+f;
	if ( !QFile::exists ( fin ) ) return -2;

	fp=fopen ( fin.latin1(),"r" );
	if ( f==NULL ) return -2;

	m=-2;
	while ( fscanf ( fp,"%s %lf",name,&value ) !=EOF )
	{

		if ( strlen ( name ) !=lin ) continue;
#if defined(HAVE_STRNSTR) && (HAVE_STRNSTR == 1)
		if ( strnstr ( name,n.latin1(),lin ) !=NULL )
#else
		if ( strstr ( name,n.latin1() ) !=NULL )
#endif
		{
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

	if ( a.length() ==0 )
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
		m=readMass ( f,n );

		// m in keV, AMU in MeV, return in MeV
		if ( pid==-2&& m>=0 )
		{
			m= ( *A ) *AMU+m/1000.0;
		}
		else if ( m==-2 && pid>=0 )
		{
			m=ParticleM[pid];
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
//	setFocus();
}
void RelKinemaCls::setMassSlot_1()
{
	setMassSlot_all();
//	setFocus();
}
void RelKinemaCls::setMassSlot_2()
{
	setMassSlot_all();
//	setFocus();
}
void RelKinemaCls::setMassSlot_3()
{
	setMassSlot_all();
//	setFocus();
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
	return m-z*Me+z*hcRinf*1.e-6;
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
	if ( sa.length() !=0 )
	{
		*a=sa.toUInt();
		sn.replace ( 0,1,sn.at ( 0 ).upper() );
	}
	else if ( sn.length() ==0 )
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
		if ( stripBox->isChecked() &&*a>0 )
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
		if ( !calc )
		{
			L1->setText ( "Not Found" );
		}
		else
		{
			L1->setText ( "Cant calc" );
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
		setCaption ( "RelKinema" );
		ReactionBox->setTitle ( "Reaction" );
		theReaction="";
		ReactionConditionBox->setEnabled ( false );
		SetBut->setEnabled ( true );
	}
}

void RelKinemaCls::setReactionLbl()
{
	QString s,a;
	a="";
	if ( a2>0 ) a.sprintf ( "%d",a2 );
	s=a+nc2+"(";
	a="";
	if ( a1>0 ) a.sprintf ( "%d",a1 );
	s+=a+nc1+",";
	a="";
	if ( a3>0 ) a.sprintf ( "%d",a3 );
	s+=a+nc3+")";
	a="";
	if ( a4>0 ) a.sprintf ( "%d",a4 );
	s+=a+nc4;
	theReaction=s;
	setCaption ( "RelKinema: "+s );
	s="Reaction: "+s;
	ReactionBox->setTitle ( s );
}

void RelKinemaCls::initReactionConditionBox()
{
	setReactionLbl();
	ReactionConditionBox->setEnabled ( true );
	anotherSolutionBox->setEnabled ( false );
	E2=m2;
	theM=m1+m2;
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
	L->setText ( s );
	L->setCursorPosition ( 0 );
}

void RelKinemaCls::unsetMass ( int id, bool clear )
{
	QLineEdit *L1=NULL,*EB=NULL, *AB=NULL;
	int *a=NULL, *z=NULL;
	QString *el=NULL;
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
	massSet[id]=false;
	rectSet[id]=EB->text().length() >0;
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
	if ( clear )
	{
		EB->clear();
		AB->clear();
	}
	ReactionConditionBox->setEnabled ( false );
	EmissionAngleBox->setEnabled ( false );
	tableBut->setEnabled ( false );
	EmissionBox->setEnabled ( false );
	SetBut->setEnabled ( true );
	setCaption ( "Relkinema" );
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

	int k;
	for ( i=0;i<4;i++ )
	{
		if ( !massSet[i] )
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
			k=i;
			break;
		}
	}

	EB->blockSignals ( true );
	AB->blockSignals ( true );

	EB->clear();
	if ( aa>=0&&zz>=0 )
	{
		switch ( aa )
		{
			case 0:
				if ( zz==0 )
				{
					EB->setText ( "g" );
				}
				else if ( zz==1 )
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
		if ( EB->text().length() ==0 )
		{
			AB->setText ( QString::number ( aa ) );
			EB->setText ( Element[zz-1] );
		}
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
	double m=sqrt ( pow ( m1+m2,2 ) +2*m2*K1th );
	K1cth= ( m-m1+m2 ) * ( m- ( m1+m2 ) ) / ( 2*m );
	if ( K1cth<0. )
	{
		fprintf ( stderr,"*** K1cth underflow detected.\n" );
		K1cth=0.;
	}
	p1cth=sqrt ( K1cth* ( K1cth+2.*m1 ) );
	showEnergyL ( K1cth,K1cthLbl,"> " );
	showEnergyL ( p1cth,p1cthLbl,"> " );
}

void RelKinemaCls::setK1th()
{
	if ( QValue<0 )
	{
		K1th= ( -QValue ) * ( m1+m2+m3+ ( m4+Ex ) ) / ( 2.0*m2 );
		p1th=sqrt ( K1th* ( K1th+2.*m1 ) );
	}
	else
	{
		K1th=0;
		p1th=0;
	}
	showEnergyL ( K1th,K1thLbl,"> " );
	showEnergyL ( p1th, p1thLbl,"> " );
}
double RelKinemaCls::K2p ( double K, double M )
{
	return sqrt ( K* ( K+2.*M ) );
}
double RelKinemaCls::p2E ( double p, double M )
{
	return sqrt ( p*p+M*M );
}

void RelKinemaCls::setQValue()
{
	QValue=m1+m2- ( m3+ ( m4+Ex ) );
	showEnergyLE ( QValue,QValueBox );
}

void RelKinemaCls::showEnergyL ( double E, QLineEdit *L, QString prep )
{
	QString s;
	double e=E*EU[EUnitBox->currentItem() ];
	s.sprintf ( double_format,e ); // in MeV
	L->setText ( s.prepend ( prep ) );
	L->setCursorPosition ( 0 );
}

void RelKinemaCls::showEnergyLE ( double E, QLineEdit *L )
{
	QString s;
	double e=E*EU[EUnitBox->currentItem() ];
	s.sprintf ( double_format,e ); // in MeV
	L->blockSignals ( true );
	L->setText ( s );
	L->setCursorPosition ( 0 );
	L->blockSignals ( false );
}

void RelKinemaCls::setE3c ( double m )
{
	E3c=m/2+ ( m3+ ( m4+Ex ) ) * ( m3- ( m4+Ex ) ) / ( 2*m );
	K3c=E3c-m3;
	if ( K3c<0 )
	{
		fprintf ( stderr,"*** K3c underflow detected: %g\n",K3c );
		K3c=0;
		E3c=m3;
		p3c=0;
	}
	else
	{
		p3c=K2p ( K3c,m3 );
	}

	E4c=m-E3c;
	K4c=E4c- ( m4+Ex );
	if ( K4c<0 )
	{
		fprintf ( stderr,"*** K4c underflow detected: %g\n",K4c );
		K4c=0;
		E4c= ( m4+Ex );
		p4c=0;
	}
	else
	{
		p4c=K2p ( K4c,m4+Ex );
	}

	qmax= ( p1c+p3c ) /hbarc;
	qmin = fabs ( p1c-p3c ) /hbarc; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

void RelKinemaCls::setExmax ( double m )
{
	Exmax=m- ( m3+m4 );
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
	if ( !ok ) return;
	if ( v<=0 ) return;
	p1=v/EU[EUnitBox->currentItem() ];
	E1=p2E ( p1,m1 );
	K1=E1-m1;
	if ( K1<0 ) K1=0;
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
	if ( !ok ) return;
	if ( v<=0 ) return;
	p1c=v/EU[EUnitBox->currentItem() ];
	E1c=p2E ( p1c,m1 );
	K1c=E1c-m1;
	if ( K1c<0 ) K1c=0;
	procK1c();
	procK1_common ( CL_P1C );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::procK1_common ( int caller )
{
	setE3c ( theM );
	setExmax ( theM );

	showValueLE ( 1./gamma,gammaLbl );
	showValueLE ( beta,betaLbl );
	showValueLE ( gamma*beta,gammabetaLbl );

	if ( caller!=CL_K1C ) showEnergyLE ( K1c,K1cBox );
	if ( caller!=CL_P1 ) showEnergyLE ( p1,p1Box );
	if ( caller!=CL_P1C ) showEnergyLE ( p1c,p1cBox );
	if ( caller!=CL_K1 ) showEnergyLE ( K1,K1Box );
}

void RelKinemaCls::procK1()
{
	double m12=m1+m2;
	gamma= ( K1+m12 ) /sqrt ( 2.*K1*m2+m12*m12 );
	beta=sqrt ( K1*K1+2.*K1*m1 ) / ( K1+m12 );
	theM=sqrt ( m12*m12+2.*m2*K1 );

	K1c= ( theM-m1+m2 ) * ( theM-m12 ) / ( 2.*theM );
	E1c=K1c+m1;
	p1c=K2p ( K1c,m1 );
}

void RelKinemaCls::rectCondSlot_K1()
{
	setResultBox ( false );
	bool ok;
	double v=K1Box->text().toDouble ( &ok );
	showStrLE ( "",K1cBox );
	showStrLE ( "",p1Box );
	showStrLE ( "",p1cBox );
	showStrLE ( "",ExmaxLbl );
	if ( !ok ) return;
	if ( v<=0 ) return;
	K1=v/EU[EUnitBox->currentItem() ];
	E1=K1+m1;
	p1=K2p ( K1,m1 );
	procK1();
	procK1_common ( CL_K1 );
	setResultBox ( checkRectCond() );
}

void RelKinemaCls::procK1c()
{
	double mm2=m2*m2;
	double m12=m1+m2;
	double mm1=m1*m1;
	theM=K1c+m1+sqrt ( K1c*K1c+2.*m1*K1c+mm2 );

	K1= ( theM-m12 ) * ( theM+m12 ) / ( 2.*m2 );
	E1=K1+m1;
	p1=K2p ( K1,m1 );

	gamma= ( theM*theM-mm1+mm2 ) / ( 2.*m2*theM );
	beta=sqrt ( 1.-1./ ( gamma*gamma ) );
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
	if ( !ok ) return;
	if ( v<=0 ) return;
	K1c=v/EU[EUnitBox->currentItem() ];
	E1c=K1c+m1;
	p1c=K2p ( K1c,m1 );
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
		Ex=v/EU[EUnitBox->currentItem() ];
		setQValue();
		setK1th();
		setK1cth();
		setE3c ( theM );
		setResultBox ( checkRectCond() );
	}
	else
	{
		ExOk=false;
		setResultBox ( false );
	}
}

bool RelKinemaCls::checkRectCond()
{
	K1Set= ( K1>=K1th );
	ExOk= ( Ex<=Exmax );
	if ( ExOk )
	{
		if ( Ex==0. )
		{
			ExLbl->clear();
		}
		else
		{
			ExLbl->setText ( "*" );
		}
	}
	return ExOk&&K1Set;
}

void RelKinemaCls::setThetaMax()
{
	double b=p3c-E3c*beta;
	if ( fabs ( b ) <=eps )
	{
		thetaMaxNe=true;
		inv=false;
		thetaMax=M_PI_2;
	}
	else if ( b<0 )
	{
		thetaMaxNe=false;
		inv=true;
		double d=p3c/ ( gamma*beta*m3 );
		if ( d>1 )
		{
			fprintf ( stderr,"*** ASIN overflow detected: %g\n",d );
			d=1;
		}
		thetaMax=asin ( d );
		int K3sign_old=K3sign;
		K3sign=0;
		setK3 ( thetaMax );
		K3sign=K3sign_old;
		limThetaCM=theta3ToTheta3CM ( thetaMax );
		limq=tcm2q ( limThetaCM );
	}
	else
	{
		thetaMaxNe=false;
		inv=false;
		thetaMax=M_PI;
	}
}

void RelKinemaCls::setResultBox ( bool on )
{
	EmissionBox->setEnabled ( on );
	EmissionAngleBox->setEnabled ( on );
	tableBut->setEnabled ( on );
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

void RelKinemaCls::changeEUnitSlot()
{
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
	if ( Exmax>0 ) showEnergyL ( Exmax,ExmaxLbl,"> " );

	int i=EUnitBox->currentItem();

	k3unitLbl->setText ( EUnitLbl[i] );
	k3cunitLbl->setText ( EUnitLbl[i] );
	k4unitLbl->setText ( EUnitLbl[i] );
	k4cunitLbl->setText ( EUnitLbl[i] );
	p3unitLbl->setText ( EUnitLbl[i]+"/c" );
	p3cunitLbl->setText ( EUnitLbl[i]+"/c" );
	p4unitLbl->setText ( EUnitLbl[i]+"/c" );
	p4cunitLbl->setText ( EUnitLbl[i]+"/c" );

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
	showAngleLE ( th3,ThetaLabBox );
	showThetaMax();
	if ( scrTypeBox->currentItem() !=ST_Q )	stepUnitLbl->setText ( getAngleUnit() );
	scrTypeSlot();
}

void RelKinemaCls::showValueLE ( double v, QLineEdit *b )
{
	QString s;
	s.sprintf ( double_format,v );
	b->blockSignals ( true );
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
	double u=1;
	if ( degBut->isChecked() ) u=RTD;
	QString s;
	s.sprintf ( double_format,v*u );
	showStrLE ( p+s,b );
}

void RelKinemaCls::initResultBox()
{
	initEmissionAngleBox();
	showKp34c();
}

double RelKinemaCls::tcm2q ( double Theta3CM )
{
	double q=p1c*p1c+p3c*p3c-2.*p1c*p3c*cos ( Theta3CM );
	if ( q<0 )
	{
		fprintf ( stderr,"tcm2q: *** SQRT overflow detected: %g\n",q );
		q=0;
	}
	return sqrt ( q ) /hbarc;
}

double RelKinemaCls::q2tcm ( double q )
{
	double c= ( p1c*p1c+p3c*p3c-pow ( q*hbarc,2 ) ) / ( 2.*p1c*p3c );
	if ( c>1 )
	{
		fprintf ( stderr,"q2tcm: *** ACOS overflow detected: %g\n",c );
		c=1;
	}
	return acos ( c );
}

double RelKinemaCls::getThetaBarPos()
{
	return thetaBar->value() *thetaStepBox->value();
}
double RelKinemaCls::getThetaBarPosInRad()
{
	return thetaBar->value() *thetaStepBox->value() /RTD;
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

double RelKinemaCls::x2Rad ( double d )
{
	if ( degBut->isChecked() )
	{
		return d/RTD;
	}
	else
	{
		return d;
	}
}

void RelKinemaCls::setth4 ( double Theta3 )
{
	// cos of open angle
	th4= ( p1*p1- ( p3*p3+p4*p4 ) ) / ( 2.*p3*p4 );
	if ( p4==0 )   // inv
	{
		th4=M_PI_2;
		return;
	}
	if ( th4>1. )
	{
		fprintf ( stderr,"*** th4: ACOS overflow detected: %g\n",th4 );
		th4=1.;
	}
	else if ( th4<-1. )
	{
		fprintf ( stderr,"*** th4: ACOS underflow detected: %g\n",th4 );
		th4=-1.;
	}
	th4=acos ( th4 )-Theta3;
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
	RecoilAngleBox->setEnabled ( on );
	scrTypeBox->setEnabled ( on );
	thetaBar->setEnabled ( on );
}

void RelKinemaCls::showTheta4()
{
	showAngleLE ( th4,RThetaLabBox );
	showAngleLE ( th4c,RThetaCMBox );
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
		if ( !ok ) return;
		if ( v<0 ) return;
		v=x2Rad ( v );
		if ( v>M_PI ) return;
		th3c=v;
		updateThetaBar();
	}
	else
	{
		showAngleLE ( th3c,ThetaCMBox );
	}
	th4c=M_PI-th3c;
	setK3vCM ( th3c );
	setK4 ( E3 );
	th3=theta3CMToTheta3 ( th3c );
	setth4 ( th3 );
	setJ34 ( th3c );
	setSpecParam();
	theq=tcm2q ( th3c );
	setAngle ( true );
	showValueLE ( theq,qBox );
	showAngleLE ( th3,ThetaLabBox );
	showTheta4();
}

void RelKinemaCls::setJ34 ( double theta3CM )
{
	double c;
	c=cos ( theta3CM ); //theta4CM = pi - theta3CM;
	J3= ( gamma*p3* ( p3c+beta*E3c*c ) ) / ( p3c*p3c );
	J4= ( gamma*p4* ( p4c-beta*E4c*c ) ) / ( p4c*p4c );
	showValueLE ( J3,J3Box );
	showValueLE ( J4,J4Box );
}

void RelKinemaCls::setK3vCM ( double theta3cm )
{
	E3=gamma* ( E3c+beta*p3c*cos ( theta3cm ) );
	K3=E3-m3;
	p3=K2p ( K3,m3 );
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

void RelKinemaCls::setK4 ( double E3_ )
{
	E4= ( E1+E2 )-E3_;
	K4=E4- ( m4+Ex );
	if ( K4<0 )
	{
		fprintf ( stderr,"*** K4 underflow detected: %g\n",K4 );
		K4=0;
		E4=m4+Ex;
		p4=0;
	}
	else
	{
		p4=K2p ( K4,m4+Ex );
	}
	showKp4();
}

void RelKinemaCls::setK3 ( double theta3 )
{
	double c,ss,d;
	c= ( cos ( theta3 ) );
	if ( inv ) c=fabs ( c ); //<<<<<<<<<<<<<<<<<<<<<<
	ss=1-pow ( beta*c,2 );
	d=1-pow ( m3/E3c*gamma,2 ) *ss;
	if ( d<0 ) d=0;
	E3=E3c/gamma* ( 1+K3sign*beta*c*sqrt ( d ) ) /ss;
	K3=E3-m3;
	if ( K3<0 )
	{
		fprintf ( stderr,"*** K3 underflow detected: %g\n",K3 );
		K3=0;
		E3=m3;
		p3=0;
	}
	else
	{
		p3=K2p ( K3,m3 );
	}
	showKp3();
}

double RelKinemaCls::theta3ToTheta3CM ( double t )
{
	double c=gamma* ( p3*cos ( t )-beta*E3 ) /p3c;
	if ( c>1. )
	{
		fprintf ( stderr,"*** th3: ACOS overflow detected: %g\n",c );
		return 0.;
	}
	else if ( c<-1. )
	{
		fprintf ( stderr,"*** th3: ACOS underflow detected: %g\n",c );
		return M_PI;
	}
	return acos ( c );
}
double RelKinemaCls::theta3CMToTheta3 ( double t )
{
	double c=gamma* ( p3c*cos ( t ) +beta*E3c ) /p3;
	if ( c>1. )
	{
		fprintf ( stderr,"*** th3c: ACOS overflow detected: %g\n",c );
		return 0.;
	}
	else if ( c<-1. )
	{
		fprintf ( stderr,"*** th3c: ACOS underflow detected: %g\n",c );
		return M_PI;
	}
	return acos ( c );
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
		if ( !ok ) return;
		v=x2Rad ( v );
		if ( v>thetaMax || v<0. ) return;
		th3=v;
		updateThetaBar();
	}
	else
	{
		showAngleLE ( th3,ThetaLabBox );
	}
	setK3 ( th3 );
	setK4 ( E3 );
	th3c=theta3ToTheta3CM ( th3 );
	setJ34 ( th3c );
	th4c=M_PI-th3c;
	setth4 ( th3 );
	theq=tcm2q ( th3c );
	setSpecParam();
	setAngle ( true );
	showValueLE ( theq,qBox );
	showAngleLE ( th3c,ThetaCMBox );
	showTheta4();
}

void RelKinemaCls::thetaLabSlot()
{thetaLabSlot ( false );}
void RelKinemaCls::thetaCMSlot()
{thetaCMSlot ( false );}
void RelKinemaCls::qSlot()
{qSlot ( false );}

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
		if ( !ok ) return;
		if ( v<qmin || v>qmax ) return;
		theq=v;
		setAngle ( true );
		updateThetaBar();
	}
	else
	{
		showValueLE ( theq,qBox );
	}
	th3c=q2tcm ( theq );
	th4c=M_PI-th3c;
	setK3vCM ( th3c );
	setK4 ( E3 );
	th3=theta3CMToTheta3 ( th3c );
	setth4 ( th3 );
	setJ34 ( th3c );
	setSpecParam();

	showAngleLE ( th3,ThetaLabBox );
	showAngleLE ( th3c,ThetaCMBox );
	showTheta4();
}

void RelKinemaCls::setSpecParam()
{
	int ie;
	double u1,u2;
	QString L1,L2;

	ie=EUnitBox->currentItem();
	u1=EU[ie];
	L1=EUnitLbl[ie];
	if ( degBut->isChecked() )
	{
		u1/=RTD;
		u2=1./RTD;
		L1+="/deg";
		L2="1/deg";
	}
	else
	{
		L1+="/rad";
		L2="1/rad";
		u2=1.;
	}

// dK3/dth3
	kinemaShift=sin ( th3 ) *J3*gamma*p3c*beta; // in MeV/rad
	showValueLE ( kinemaShift*u1, dK3_dth3Box );
	dK3_dth3_unitLbl->setText ( L1 );

// dp3/dth3/p3
	factorK=kinemaShift*E3/ ( p3*p3 ); // in 1/rad
	showValueLE ( factorK*u2,KparamBox );
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

void RelKinemaCls::scrTypeSlot()
{
	bool ok;
	double v=0;
	double d=thetaStepBox->value();
	double max=0,min=0;
	void ( RelKinemaCls::*eval ) ();
	QString unit;
	QLabel *L;
	thetaBar->setEnabled ( false );
	theqLbl->setPaletteForegroundColor ( "black" );
	theLabLbl->setPaletteForegroundColor ( "black" );
	theCMLbl->setPaletteForegroundColor ( "black" );
	theLabLbl->setFont ( thefont );
	theCMLbl->setFont ( thefont );
	theqLbl->setFont ( thefont );
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			eval=&RelKinemaCls::thetaLabSlot;
			v=ThetaLabBox->text().toDouble ( &ok );
			if ( degBut->isChecked() )
			{
				getThetaBarValue=&RelKinemaCls::getThetaBarPosInRad;
				max=thetaMax*RTD;
			}
			else
			{
				getThetaBarValue=&RelKinemaCls::getThetaBarPos;
				max=thetaMax;
			}
			min=0;
			unit=getAngleUnit();
			L=theLabLbl;
			K3sign=1;
			if ( inv )
			{
				anotherSolutionBox->setEnabled ( true );
				anotherSolutionBox->setChecked ( false );
			}
			tbMax=&thetaMax;
			tbMin=&fZero;
			tbTarg=&th3;
			tbEval=&RelKinemaCls::thetaLabSlot;
			break;
		case ST_THETACM:
			eval=&RelKinemaCls::thetaCMSlot;
			v=ThetaCMBox->text().toDouble ( &ok );
			if ( degBut->isChecked() )
			{
				getThetaBarValue=&RelKinemaCls::getThetaBarPosInRad;
				max=180;
			}
			else
			{
				getThetaBarValue=&RelKinemaCls::getThetaBarPos;
				max=M_PI;
			}
			min=0;
			unit=getAngleUnit();
			L=theCMLbl;
			anotherSolutionBox->setEnabled ( false );
			anotherSolutionBox->setChecked ( false );
			tbMax=&fPI;
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
			L=theqLbl;
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
	if ( !ok ) return;
	if ( d==0. ) return;

	L->setPaletteForegroundColor ( "blue" );
	QFont f=thefont;
	f.setBold ( true );
	L->setFont ( f );
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
		thetaStepBox->setValue ( getExpValue ( d ) );
	}
	int imax=floor ( max/d+0.5 );
	int imin=floor ( min/d+0.5 );
	if ( thetaMaxNe ) imax--;
	thetaBar->setMinValue ( imin );
	thetaBar->setMaxValue ( imax );
	thetaBar->setValue ( floor ( v/d+0.5 ) );
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
		K3sign=-1;
	}
	else
	{
		K3sign=1;
	}
	thetaBar->setValue ( 0 );
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
		EmissionBox->setEnabled ( false );
		SetBut->setEnabled ( true );
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
		if ( a1>0&&massSet[1] ) m1=stripMass ( m10,z1 );
		if ( a2>0&&massSet[0] ) m2=stripMass ( m20,z2 );
		if ( a3>0&&massSet[3] ) m3=stripMass ( m30,z3 );
		if ( a4>0&&massSet[4] ) m4=stripMass ( m40,z4 );
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
	resultWindowCls *win = new resultWindowCls();
	win->initResultDescBox ( theReaction, K1, p1, Ex, QValue, beta,gamma,
	                         m1/AMU, m2/AMU, m3/AMU, m4/AMU, double_format );
	win->homedir=HOME;
	win->setFont ( thefont );

	int col_first=scrTypeBox->currentItem();

	int ie=EUnitBox->currentItem();
	double eu=EU[ie];
	QString el=EUnitLbl[ie];
	QString pl=el+"/c";
	QString al;
	double au;
	if ( degBut->isChecked() )
	{
		al="deg";
		au=RTD;
	}
	else
	{
		au=1.;
		al="rad";
	}
	int irmin=thetaBar->minValue();
	int irmax=thetaBar->maxValue();
	int nr=irmax-irmin+1;

	win->resultTable->setNumRows ( nr+1 );
	win->resultTable->setText ( 0,col_th3,       al );
	win->resultTable->setText ( 0,col_th3c,      al );
	win->resultTable->setText ( 0,col_q,     "1/fm" );
	win->resultTable->setText ( 0,col_K3,        el );
	win->resultTable->setText ( 0,col_p3,        pl );
	win->resultTable->setText ( 0,col_J3,        "" );
	win->resultTable->setText ( 0,col_ks, el+"/"+al );
	win->resultTable->setText ( 0,col_fk,   "1/"+al );
	win->resultTable->setText ( 0,col_th4,       al );
	win->resultTable->setText ( 0,col_th4c,      al );
	win->resultTable->setText ( 0,col_K4,        el );
	win->resultTable->setText ( 0,col_p4,        pl );
	win->resultTable->setText ( 0,col_J4,        "" );
	win->resultTable->verticalHeader()->setLabel ( 0,"unit" );

	tableBut->setEnabled ( false );
	rwt = new rwThreadCls ( this, win, eu, au, irmin, irmax, col_first, double_format );
	connect ( rwt,SIGNAL ( done ( resultWindowCls* ) ),this,SLOT ( teDone ( resultWindowCls* ) ) );
	stopBut->setEnabled ( true );
	rwt->run();
}

void RelKinemaCls::teDone ( resultWindowCls *win )
{
	win->show();
	tableBut->setEnabled ( true );
	stopBut->setEnabled ( false );
}

void RelKinemaCls::thetaStepSlot()
{
	double d=thetaStepBox->value();
	double max=0,min=0;
	double v;
	bool ok;
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			v=ThetaLabBox->text().toDouble ( &ok );
			max=thetaMax;
			if ( degBut->isChecked() ) max*=RTD;
			min=0;
			break;
		case ST_THETACM:
			v=ThetaCMBox->text().toDouble ( &ok );
			if ( degBut->isChecked() )
			{
				max=180.0;
			}
			else
			{
				max=M_PI;
			}
			min=0;
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
}

void RelKinemaCls::updateThetaBar()
{
	double v=0;
	double d=thetaStepBox->value();
	switch ( scrTypeBox->currentItem() )
	{
		case ST_THETALAB:
			v=th3*RTD;
			break;
		case ST_THETACM:
			v=th3c*RTD;
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
	mdmCls *win = new mdmCls();
	win->massdata=MASSDATA;
	win->home=HOME;
	win->AMU=AMU;
	win->Me=Me;
	win->Mp=Mp;
	win->Mn=Mn;
	win->Mt=Mt;
	win->Ma=Ma;
	win->Md=Md;
	win->wb=webbrowser;
	win->openMassData ( 0 );
	win->setFont ( thefont );
	win->show();
}

void RelKinemaCls::keyPressEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Q:
		case Qt::Key_Escape:
			close();
			break;
		case Qt::Key_T:
			if ( tableBut->isEnabled() ) showResultListSlot();
			break;
		case Qt::Key_M:
			mdmSlot();
			break;
		case Qt::Key_H:
			showAbout();
			break;
		case Qt::Key_C:
			unsetMassSlot_all();
			break;
		case Qt::Key_S:
			settingsSlot();
			break;
	}
}

void RelKinemaCls::showAbout()
{
	aboutcls subWin;
	subWin.wb=webbrowser;
	subWin.exec();
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
	if ( first )
	{
		confDoubleNumberFormatLbl->clear();
		confMassDataDirLbl->clear();
		confMassDataDirBox->setText ( MASSDATA );
		confDoubleNumberFormatBox->setText ( double_format );
	}

	configEdit->clear();
	rcLbl->setText ( HOME+CONFIG );
	int i=0;
	QFile f ( HOME+CONFIG );
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
	if ( QFile::exists ( confMassDataDirBox->text() ) )
	{
		MASSDATA=confMassDataDirBox->text();
		showMessL ( "ok","blue",confMassDataDirLbl );
	}
	else
	{
		showMessL ( "Invalid Directory", "red", confMassDataDirLbl );
	}

	if ( checkDoubleNumberFormat ( confDoubleNumberFormatBox->text() ) )
	{
		double_format=confDoubleNumberFormatBox->text();
		showMessL ( "ok","blue",confDoubleNumberFormatLbl );
	}
	else
	{
		showMessL ( "Invalid Format!","red",confDoubleNumberFormatLbl );
	}
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
	QFile f ( HOME+CONFIG );
	if ( f.open ( IO_WriteOnly ) )
	{
		QTextStream stream ( &f );
		stream << configEdit->text();
		f.close();
	}
	QString mdn=MASSDATA;
	QString dfn=double_format;
	loadConfig ( HOME+CONFIG );
	MASSDATA=mdn;
	double_format=dfn;
// then the changes on massdata etc on the lineedit will be saved
	saveConfig ( HOME+CONFIG );
	initSettingsPage();
	reinitRelKinema();
}

void RelKinemaCls::returnSlot()
{
	setFocus();
}

void RelKinemaCls::stopSlot()
{
}

#include "relkinemaCls.moc"
