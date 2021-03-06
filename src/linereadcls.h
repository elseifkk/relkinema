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
#ifndef LINEREADCLS_H
#define LINEREADCLS_H

#include "lineread.h"

#include <qlabel.h>

class linereadCls: public lineread
{
		Q_OBJECT

	public:
		linereadCls ( QWidget *parent = 0, const char *name = 0, WFlags wf = 0,
		              QString caption = "", QString prompt = "", QFont f=QFont ( "Sans Serif", 11 ) )
				:lineread ( parent,name,wf )
		{
			setCaption ( caption );
			lineLbl->setText ( prompt );
			input="";
			setFont ( f );
		};

	public:
		QString input;

	private slots:
		void enterSlot();
		void cancelSlot();
};






#endif
