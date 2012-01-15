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
#include "mboxcls.h"
#include <qlabel.h>
#include <qevent.h>

mboxCls::mboxCls ( QWidget *parent, const char *name, WFlags wf, QString mess , QString title, QFont font )
		:mBox ( parent, name, wf )
{
	setFont ( font );
	QLabel *mlbl=new QLabel ( this );
	setCaption ( title );
	mlbl->setFont ( font );
	mlbl->setText ( mess );
	mlbl->adjustSize();
	resize ( mlbl->size().width(),mlbl->size().height() );
	mlbl->move ( ( width()-mlbl->width() ) /2., ( height()-mlbl->height() ) /2. );
}

void mboxCls::keyPressEvent ( QKeyEvent*e )
{
	switch ( e->key() )
	{
		case Qt::Key_Q:
		case Qt::Key_Escape:
		case Qt::Key_Return:
		case Qt::Key_Enter:
			close();
	}
}

void mboxCls::closeEvent ( QCloseEvent *e )
{
//	delete mlbl; cause SIGINV
	e->accept();
}

#include "mboxcls.moc"
