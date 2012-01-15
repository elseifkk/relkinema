/***************************************************************************
 *   Copyright (C) 2011 by Kazuaki Kumagai                                 *
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
#ifndef RWTHREADCLS_H
#define RWTHREADCLS_H

#include <qthread.h>
#include <qstringlist.h>

#define BT(var,pos) ((var) & (1<<(pos)))

int const rkp_th3  =  0;
int const rkp_th3c =  1;
int const rkp_q    =  2;
int const rkp_K3   =  3;
int const rkp_p3   =  4;
int const rkp_J3   =  5;
int const rkp_ks   =  6;
int const rkp_kf   =  7;
int const rkp_th4  =  8;
int const rkp_th4c =  9;
int const rkp_K4   = 10;
int const rkp_p4   = 11;
int const rkp_J4   = 12;
int const nrkpmax  = 13;
int const nexpmax  = 4;
QString const expr[nexpmax]={"expr1", "expr2", "expr3", "expr4"};

class rwThreadCls: public QThread
{
	public:
		rwThreadCls ( double step, double rmin, int irmax, size_t prkc, int sid, QString fmt,
		              int col_first, size_t pfzc, int plotmask, bool *ext_in )
				:step ( step ), rmin ( rmin ), irmax ( irmax ),
				prkc ( prkc ), sid ( sid ), fmt ( fmt ),
				col_first ( col_first ), pfzc ( pfzc ), plotmask ( plotmask ) {ndone=0; memcpy ( &ext, ext_in, sizeof ( bool ) *nexpmax );}
	public:
		QStringList sl;
		int ndone;

	private:
		double step;
		double rmin;
		int irmax;
		size_t prkc;
		int sid;
		QString fmt;
		int col_first;
		size_t pfzc;
		bool ext[nexpmax];
		int plotmask;

	private:
		void setRKCValue();
		virtual void run();
};

#endif
