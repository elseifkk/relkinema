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
#ifndef RKCORE_H
#define RKCORE_H

#include <math.h>
#include <stdio.h>

#include <qstring.h>

class rkCore
{
	public:
		rkCore();
		~rkCore();

	public:

		// must be called in this order

		// (1) mandatory
		void setMass ( double m1_, double m2_, double m3_, double m4_ )
		{
			m1=m1_;m2=m2_;m3=m3_;m4=m4_;
			theM=m1+m2;
			E2=m2;
			massSet=true;
		};

		// (2) mandatory
		void set_Ex ( double Ex_ );
		// (3) getter
		double ge_tQValue() {return QValue;};
		double get_K1Min() {return K1Min;};
		double get_p1Min() {return p1Min;};
		double get_K1cMin() {return K1cMin;};
		double get_p1cMin() {return p1cMin;};

		// (4) one of four mandatory
		void set_K1 ( double K1_ );
		void set_K1c ( double K1c_ );
		void set_p1 ( double p1_ );
		void set_p1c ( double p1c_ );
		// (5) getter
		double get_Beta() {return beta;};
		double get_Gamma() {return gamma;};
		double get_TheM() {return theM;};
		double get_ExMax() {return theM- ( m3+m4 );};

		// (6) mandatory
		void set_K34c();
		// getter
		double get_qMax() {return p1c+p3c;};
		double get_qMin() {return fabs ( p1c-p3c );};
		double get_K3c() {return K3c;};
		double get_K4c() {return K4c;};
		double get_p3c() {return p3c;};
		double get_p4c() {return p4c;};

		// (7) three of one mandatory
		int set_Theta3 ( double theta3_ );
		int set_Theta3c ( double theta3_ );
		int set_q ( double theta3_ );
		// getter
		double get_K3() {return K3;};
		double get_K4() {return K4;};
		double get_p3() {return p3;};
		double get_p4() {return p4;};
		double get_theta3() {return th3;};
		double get_theta4() {return th4;};
		double get_theta3c() {return th3c;};
		double get_theta4c() {return th4c;};
		double get_q() {return theq;};
		double get_jacobian3()
		{
			return ( gamma*p3* ( p3c+beta*E3c*cos ( th3c ) ) ) / ( p3c*p3c );
		}
		double get_jacobian4()
		{
			return ( gamma*p4* ( p4c+beta*E4c*cos ( th4c ) ) ) / ( p4c*p4c );
		}

	public:
		static double p2E ( double p, double m ) {return sqrt ( p*p+m*m );};
		static double K2p ( double K, double m ) {return sqrt ( K* ( K+2.*m ) );};
		static double E2p ( double E, double m )
		{
			double p=E*E+m*m;
			if ( p<0 )
			{
				warn ( "E2p", "SQRT underflow detected",&p );
				return 0.;
			}
			return sqrt ( p );
		};
		static double q2tcm ( double q, double pi, double pf )
		{
			double c= ( pi*pi+pf*pf-q*q ) / ( 2.*pi*pf );
			if ( c>1 )
			{
				warn ( "q2tcm","ACOS overflow detected",&c );
				return 0.;
			}
			else if ( c<-1. )
			{
				return M_PI;
			}
			else
			{
				return acos ( c );
			}
		}
		static double tcm2q ( double tcm, double pi, double pf )
		{
			double q=pi*pi+pf*pf-2.*pi*pf*cos ( tcm );
			if ( q<0. )
			{
				warn ( "tcm2q","SQRT overflow detected",&q );
				return 0.;
			}
			else
			{
				return sqrt ( q );
			}
		}

	private:
		static void warn ( QString proc, QString mess, const double *value=NULL );
		void K1c_To_K1();
		void K1_To_K1c();
		void set_th3Max();
		void set_K3c();
		void set_K4c();
		void set_K3();
		void set_K4();
		void set_K3_via_LB();
		double th3c_To_th3 ( double );
		double th3_To_th3c ( double );
		double th3_To_th4 ( double );
		double th3c_To_th4c ( double th3c_ ) {return M_PI-th3c_;};

	private:
		double eps;

	private:
		bool massSet;
		// all in MeV and rad.
		double m10,m20,m30,m40; // original mass
		double m1,m2,m3,m4; // Projectile, Target, Ejectile, Residual
		double E1,E2,E3,E4;
		double p1,p2,p3,p4;
		double K1,K2,K3,K4;
		double E1c,E2c,E3c,E4c;
		double K1c,K2c,K3c,K4c;
		double p1c,p2c,p3c,p4c;
		double QValue;
		double Ex;
		double K1Min, p1Min;
		double K1cMin, p1cMin;
		double theM;
		double gamma,beta;
		double th3Max;
		bool th3MaxNe, invKin;
		double th3, th4;
		double theq;
		double th3c, th4c;
};

#endif