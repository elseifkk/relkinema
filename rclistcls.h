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
#ifndef RCLISTCLS_H
#define RCLISTCLS_H

#include "rclist.h"

#include <qlistview.h>

#include <kconfig.h>

class rclistCls: public rclist
{
		Q_OBJECT

	public:
		rclistCls ( QWidget *parent = 0, const char *name = 0, WFlags wf = 0,
		            KConfig *conf = 0, QFont font=QFont ( "Sans Serif", 11 ) );

	public:
		QString rc;

	private:
		void setTable();
		QString getRc()
		{
			QString rc=listBox->currentItem()->text ( 0 )
			           +";"+listBox->currentItem()->text ( 1 ).remove (
			               listBox->currentItem()->text ( 1 ).length()-4,4 )
			           +";"+listBox->currentItem()->text ( 2 ).remove (
			               listBox->currentItem()->text ( 2 ).length()-4,4 )
			           +";"+listBox->currentItem()->text ( 1 ).right ( 3 );
			return rc;
		};

	private:
		KConfig *conf;
		QStringList sl;

	private slots:
		void delSlot();
		void loadSlot();
};

#endif