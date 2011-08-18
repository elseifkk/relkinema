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
#ifndef RWTHREADCLS_H
#define RWTHREADCLS_H

#include "relkinemaCls.h"
#include "resultwindowcls.h"

#include <qthread.h>

class rwThreadCls: public QObject, QThread
{
		Q_OBJECT

	public:

		RelKinemaCls *rk;
		resultWindowCls *rw;
		double eu;
		double au;
		int irmin;
		int irmax;
		int col_first;
		QString fmt;

		rwThreadCls ( RelKinemaCls *rk, resultWindowCls *rw,
		              double eu, double au, int irmin, int irmax, int col_first,
		              QString fmt )
				:rk ( rk ), rw ( rw ), eu ( eu ),
				au ( au ), irmin ( irmin ), irmax ( irmax ), col_first ( col_first ),
				fmt ( fmt ) {}

		virtual void run();

	signals:
		void done ( resultWindowCls* );
};

#endif