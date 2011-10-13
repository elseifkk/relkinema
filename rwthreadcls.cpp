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
#include "rwthreadcls.h"
#include "rkcore.h"

void rwThreadCls::setRKCValue()
{
	QString s;
	sl.append ( s.sprintf ( fmt, rkc_get_th3 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_th3c ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_q ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_K3 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_p3 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_J3 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_KShift ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_KFactor ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_th4 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_th4c ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_K4 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_p4 ( prkc ) ) );
	sl.append ( s.sprintf ( fmt, rkc_get_J4 ( prkc ) ) );
}

void rwThreadCls::run()
{
	prkc=rkc_cp ( prkc );
	double x;
	int ( *rkc_setter ) ( const size_t, const double );
	switch ( sid )
	{
		case 0:
			rkc_setter=rkc_set_Theta3;
			break;
		case 1:
			rkc_setter=rkc_set_Theta3c;
			break;
		case 2:
			rkc_setter=rkc_set_q;
			break;
	}
	( *rkc_setter ) ( prkc,-1.0 );
	setRKCValue();
	for ( int ir=1;ir<=irmax;ir++ )
	{
		x=ir*step+rmin;
		( *rkc_setter ) ( prkc,x );
		setRKCValue();
		ndone=ir;
	}
	ndone=-1;
	rkc_uinit ( prkc );
}
