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
 *   Free Software oundation, Inc.,                                       *
 *   59 Temple Place- Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/
#include "rclistcls.h"
#include <qlistview.h>
#include <qpushbutton.h>
#include <kconfig.h>

rclistCls::rclistCls ( QWidget *parent, const char *name, WFlags wf,
                       KConfig *conf, QFont font )
		:rclist ( parent, name, wf ), conf ( conf )
{
	setFont ( font );
	setTable();
	rc="";
}

void rclistCls::setTable()
{
	KConfigGroup saveGroup ( conf,"Save" );
	QStringList sl0;
	sl=saveGroup.readListEntry ( "list",sl0 );
	if ( sl.count() !=0 )
	{
		QListViewItem *item;
		item=NULL;
		KConfigGroup remGroup ( conf, "Rem" );
		for ( QStringList::Iterator it=sl.begin();it!=sl.end();it++ )
		{
			item = new QListViewItem ( listBox,item );
			QString s=*it;
			QString eu;
			eu=s.section ( ';',3,3 );
			item->setText ( 0,s.section ( ';',0,0 ) ); // reaction
			item->setText ( 1,s.section ( ';',1,1 ) +" "+eu ); // K1
			item->setText ( 2,s.section ( ';',2,2 ) +" "+eu ); // Ex
			item->setText ( 3,remGroup.readEntry ( s,"none" ) ); // Rem
		}
		listBox->setSelected ( listBox->firstChild(),true );
		loadBut->setEnabled ( true );
		delBut->setEnabled ( true );
	}
	else
	{
		loadBut->setEnabled ( false );
		delBut->setEnabled ( false );
	}
}

void rclistCls::delSlot()
{
	QStringList sl0;
	QString drc;
	drc=getRc();

	for ( QStringList::Iterator it=sl.begin();it!=sl.end();it++ )
	{
		QString s=*it;
		if ( s!=drc )  sl0.append ( s );
	}
	KConfigGroup saveGroup ( conf,"Save" );
	saveGroup.writeEntry ( "list",sl0 );
	KConfigGroup remGroup ( conf,"Rem" );
	remGroup.deleteEntry ( drc );
	saveGroup.sync();
	remGroup.sync();

	listBox->clear();
	setTable();
}

void rclistCls::loadSlot()
{
	rc=getRc();
	accept();
}
