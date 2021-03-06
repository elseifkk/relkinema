/***************************************************************************
 *   Copyright (C) 2010,2014 by Kazuaki Kumagai                            *
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
#ifndef MBOXCLS_H
#define MBOXCLS_H

#include "message.h"

class mboxCls: public mBox
{
		Q_OBJECT
	public:
		mboxCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0, QString mess="", QString title="", QFont font = QFont ( "Sans Serif", 11), int w=300, int h=200 );

	public:

	protected:
		void keyPressEvent ( QKeyEvent* );
		void closeEvent ( QCloseEvent * );
};

#endif
