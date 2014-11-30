/***************************************************************************
 *   Copyright (C) 2011,2014 by Kazuaki Kumagai                            *
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
#include <qtextedit.h>
#include <qcursor.h>

mboxCls::mboxCls ( QWidget *parent, const char *name, WFlags wf,
		   QString mess , QString title, QFont font, int w, int h )
		:mBox ( parent, name, wf )
{
	textEdit->setReadOnly(true);
	setFont ( font );
	setCaption ( title );
	textEdit->setFont(font);
	textEdit->setText(mess);
	resize(w,h);
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
			e->accept();
			return;
	}
	e->ignore();
}

void mboxCls::closeEvent ( QCloseEvent *e )
{
	if(textEdit!=0) delete textEdit;
	textEdit=0;
	e->accept();
}

#include "mboxcls.moc"
