/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
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
#include "rwthreadcls.h"
#include "rkcore.h"

#include <fzc.h>

void rwThreadCls::setRKCValue()
{
	QString s;
	switch ( col_first )
	{
		case 0:
			if ( BT ( plotmask, rkp_th3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3 ( prkc ) ) );
			if ( BT ( plotmask, rkp_th3c ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3c ( prkc ) ) );
			if ( BT ( plotmask, rkp_q ) ) sl.append ( s.sprintf ( fmt, rkc_get_q ( prkc ) ) );
			break;
		case 1:
			if ( BT ( plotmask, rkp_th3c ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3c ( prkc ) ) );
			if ( BT ( plotmask, rkp_th3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3 ( prkc ) ) );
			if ( BT ( plotmask, rkp_q ) ) sl.append ( s.sprintf ( fmt, rkc_get_q ( prkc ) ) );
			break;
		case 2:
			if ( BT ( plotmask, rkp_q ) ) sl.append ( s.sprintf ( fmt, rkc_get_q ( prkc ) ) );
			if ( BT ( plotmask, rkp_th3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3 ( prkc ) ) );
			if ( BT ( plotmask, rkp_th3c ) ) sl.append ( s.sprintf ( fmt, rkc_get_th3c ( prkc ) ) );
			break;
	}
	if ( BT ( plotmask, rkp_K3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_K3 ( prkc ) ) );
	if ( BT ( plotmask, rkp_p3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_p3 ( prkc ) ) );
	if ( BT ( plotmask, rkp_J3 ) ) sl.append ( s.sprintf ( fmt, rkc_get_J3 ( prkc ) ) );
	if ( BT ( plotmask, rkp_ks ) ) sl.append ( s.sprintf ( fmt, rkc_get_KShift ( prkc ) ) );
	if ( BT ( plotmask, rkp_kf ) ) sl.append ( s.sprintf ( fmt, rkc_get_KFactor ( prkc ) ) );
	if ( BT ( plotmask, rkp_th4 ) ) sl.append ( s.sprintf ( fmt, rkc_get_th4 ( prkc ) ) );
	if ( BT ( plotmask, rkp_th4c ) ) sl.append ( s.sprintf ( fmt, rkc_get_th4c ( prkc ) ) );
	if ( BT ( plotmask, rkp_K4 ) ) sl.append ( s.sprintf ( fmt, rkc_get_K4 ( prkc ) ) );
	if ( BT ( plotmask, rkp_p4 ) ) sl.append ( s.sprintf ( fmt, rkc_get_p4 ( prkc ) ) );
	if ( BT ( plotmask, rkp_J4 ) ) sl.append ( s.sprintf ( fmt, rkc_get_J4 ( prkc ) ) );
	for ( int i=0;i<nexpmax;i++ )
	{
		if ( !ext[i] ) continue;
		char cstr[LEN_FZCSTR_MAX];
		strcpy ( cstr,expr[i].latin1() );
		if ( fzc_set_formula ( pfzc, ( size_t ) &cstr ) == 0 &&
		        fzc_eval ( pfzc ) == 0 )
		{
			fzc_get_strans ( pfzc, ( size_t ) &cstr );
			sl.append ( cstr );
		}
		else
		{
			sl.append ( "" );
		}
	}
}

void rwThreadCls::run()
{
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
}
