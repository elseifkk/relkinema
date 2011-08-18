/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
 *   elseifkk@gmai.com                                                     *
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

#include "rkCore.h"

void rkCore::warn ( QString proc, QString mess, const double *value )
{
	QString s;
	s="*** "+proc+": "+mess;
	if ( value!=NULL ) s+=" %g";
	s+="\n";
	fprintf ( stderr,s.latin1(),*value );
}

// called internally
double rkCore:: th3c_To_th3 ( double th3c_ )
{
	double c=gamma* ( p3c*cos ( th3c_ ) +beta*E3c ) /p3;
	if ( c>1. )
	{
		warn ( "th3c_To_th34","ACOS overflow detected",&c );
		return 0.;
	}
	else if ( c<-1. )
	{
		warn ( "th3c_To_th34","ACOS underflow detected",&c );
		return M_PI;
	}
	else
	{
		return acos ( c );
	}
}

// called internally
double rkCore::th3_To_th3c ( double th3_ )
{
	double c=gamma* ( p3*cos ( th3_ )-beta*E3 ) /p3c;
	if ( c>1. )
	{
		warn ( "th3_To_th3c","ACOS overflow detected",&c );
		return 0.;
	}
	else if ( c<-1. )
	{
		warn ( "th3_To_th3c","ACOS underflow detected",&c );
		return M_PI;
	}
	else
	{
		return acos ( c );
	}
}

// called internally
double rkCore::th3_To_th4 ( double th3_ )
{
	// cos of open angle
	if ( p4==0. )   // inv
	{
		return M_PI_2;
	}
	double c= ( p1*p1- ( p3*p3+p4*p4 ) ) / ( 2.*p3*p4 );
	if ( c>1. )
	{
		warn ( "th3Tothh4","ACOS overflow detected",&th4 );
		return th4=0.0; //<<<<<<<<<<<<<<<<<
	}
	else if ( c<-1. )
	{
		warn ( "th3Tothh4","ACOS underflow detected",&th4 );
		return M_PI-th3_;
	}
	else
	{
		return acos ( c )-th3_;
	}
}

int rkCore::set_q ( double q_ )
{
	theq=q_;

	th3c=q2tcm ( q_, p1c, p3c );
	set_K3_via_LB();
	set_K4();

	th4c=th3c_To_th4c ( th3c );
	th3=th3c_To_th3 ( th3c );
	th4=th3_To_th4 ( th3 );

	return 0;
}

// called internally
void rkCore::set_K3_via_LB()
{
	E3=gamma* ( E3c+beta*p3c*cos ( th3c ) );
	K3=E3-m3;
	p3=K2p ( K3,m3 );
}

int rkCore::set_Theta3c ( double th3c_ )
{
	th3c=th3c_;
	set_K3_via_LB();
	set_K4();

	th3=th3c_To_th3 ( th3c );
	th4=th3_To_th4 ( th3 );
	th4c=th3c_To_th4c ( th3c );
	theq=tcm2q ( th3c, p1c, p3c );

	return 0;
}

// called internally
void rkCore::set_K4()
{
	E4= ( E1+E2 )-E3;
	K4=E4- ( m4+Ex );
	if ( K4<0. )
	{
		warn ( "setTheta3","K4 underflow detected",&K4 );
		K4=0.;
		E4=m4+Ex;
		p4=0.;
	}
	else
	{
		p4=K2p ( K4,m4+Ex );
	}
}

// called internally
void rkCore::set_K3()
{
	double c= ( cos ( th3 ) );
	if ( invKin ) c=fabs ( c ); //<<<<<<<<<<<<<<<<<<<<<<
	double ss=1.-pow ( beta*c,2. );
	double d=1.-pow ( m3/E3c*gamma,2. ) *ss;
	if ( d<0. ) d=0.;
	E3=E3c/gamma* ( 1.+beta*c*sqrt ( d ) ) /ss;
	K3=E3-m3;
	if ( K3<0. )
	{
		warn ( "setTheta3", "K3 underflow detected",&K3 );
		K3=0.;
		E3=m3;
		p3=0.;
	}
	else
	{
		p3=K2p ( K3,m3 );
	}
}

int rkCore::set_Theta3 ( double th3_ )
{
	th3=th3_;
	set_K3();
	set_K4();

	th4=th3_To_th4 ( th3 );
	th3c=th3_To_th3c ( th3 );
	th4c=th3c_To_th4c ( th3c );
	theq=tcm2q ( th3c, p1c, p3c );

	return 0;
}

// called internally
void rkCore::set_th3Max()
{
	double b=p3c-E3c*beta;
	if ( fabs ( b ) <=eps )
	{
		th3MaxNe=true;
		invKin=false;
		th3Max=M_PI_2;
	}
	else if ( b<0. )
	{
		th3MaxNe=true;
		invKin=true;
		double d=p3c/ ( gamma*beta*m3 );
		if ( d>1. )
		{
			warn ( "set_th3Max","ASIN overflow detected",&d );
			th3Max=M_PI_2;
		}
		th3Max=asin ( d );
	}
	else
	{
		th3MaxNe=false;
		invKin=false;
		th3Max=M_PI;
	}
}

// called internally
void rkCore::set_K4c()
{
	E4c=theM-E3c;
	K4c=E4c- ( m4+Ex );
	if ( K4c<0. )
	{
		warn ( "setK34c", "K4c underflow detected",&K4c );
		K4c=0.;
		E4c= ( m4+Ex );
		p4c=0.;
	}
	else
	{
		p4c=K2p ( K4c,m4+Ex );
	}
}

// called internally
void rkCore::set_K3c()
{
	E3c=theM/2.+ ( m3+ ( m4+Ex ) ) * ( m3- ( m4+Ex ) ) / ( 2.*theM );
	K3c=E3c-m3;
	if ( K3c<0. )
	{
		warn ( "setK34c","K3c underflow detected",&K3c );
		K3c=0.;
		E3c=m3;
		p3c=0.;
	}
	else
	{
		p3c=K2p ( K3c,m3 );
	}
}

void rkCore::set_K34c()
{
	set_K3c();
	set_K4c();
	set_th3Max();
}

//called internally
void rkCore::K1c_To_K1()
{
	double mm2=m2*m2;
	double m12=m1+m2;
	double mm1=m1*m1;
	theM=K1c+m1+sqrt ( K1c*K1c+2.*m1*K1c+mm2 );
	gamma= ( theM*theM-mm1+mm2 ) / ( 2.*m2*theM );
	beta=sqrt ( 1.-1./ ( gamma*gamma ) );
	K1= ( theM-m12 ) * ( theM+m12 ) / ( 2.*m2 );
	E1=K1+m1;
	p1=K2p ( K1,m1 );
}

// called internally
void rkCore::K1_To_K1c()
{
	double m12=m1+m2;
	theM=sqrt ( m12*m12+2.*m2*K1 );
	gamma= ( K1+m12 ) /sqrt ( 2.*K1*m2+m12*m12 );
	beta=sqrt ( K1*K1+2.*K1*m1 ) / ( K1+m12 );
	K1c= ( theM-m1+m2 ) * ( theM-m12 ) / ( 2.*theM );
	E1c=K1c+m1;
	p1c=K2p ( K1c,m1 );
}

void rkCore::set_K1 ( double K1_ )
{
	K1=K1_;
	p1=K2p ( K1_,m1 );
	E1=K1_+m1;
	K1_To_K1c();
}

void rkCore::set_K1c ( double K1c_ )
{
	K1c=K1c_;
	p1c=K2p ( K1c_, m1 );
	E1c=K1c_+m1;
	K1c_To_K1();
}

void rkCore::set_p1 ( double p1_ )
{
	p1=p1_;
	E1=p2E ( p1_,m1 );
	K1=E1-m1;
	K1_To_K1c();
}

void rkCore::set_p1c ( double p1c_ )
{
	p1c=p1c_;
	E1c=p2E ( p1c_,m1 );
	K1c=E1c-m1;
	K1c_To_K1();
}

void rkCore::set_Ex ( double Ex_ )
{
	Ex=Ex_;
	QValue= ( m1+m2 )- ( m3+ ( m4+Ex ) );

	if ( QValue>0. )
	{
		K1Min=0.;
		p1Min=0.;
	}
	else
	{
		K1Min= ( -QValue ) * ( m1+m2+m3+ ( m4+Ex ) ) / ( 2.*m2 );
		p1Min=K2p ( K1Min,m1 );
	}
	double m=sqrt ( pow ( m1+m2,2. ) +2.*m2*K1Min );
	K1cMin= ( m-m1+m2 ) * ( m- ( m1+m2 ) ) / ( 2.*m );
	p1cMin=K2p ( K1cMin,m1 );
}

rkCore::~rkCore()
{
}