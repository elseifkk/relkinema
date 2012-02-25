/***************************************************************************
 *   Copyright (C) 2011-2012 by Kazuaki Kumagai                            *
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
#ifndef RKCALCCLS_H
#define RKCALCCLS_H

#include "rkCalcDlg.h"

#include <qwidget.h>

#include <fzc.h>

class rkCalcCls: public rkCalcDlg
{
		Q_OBJECT
	public:
		rkCalcCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0, const QString *configfile = 0, size_t pfzc = 0 )
				:rkCalcDlg ( parent, name, wf ), configfile ( configfile ), pfzc ( pfzc ) {nextpara=0;};
		bool initRKC();

	private:
		void saveConfig ( QString );
		void loadConfig ( QString );
		void mess ( QString,QColor c="black", bool ap=true );

	public:
		const QString *configfile;

	private:
		size_t pfzc;
		int nextpara;

	private slots:
		void enterSlot ( void );
		void clearSlot ( void );
		void clearHistSlot ( void );

	protected:
		void closeEvent ( QCloseEvent* );
		void keyPressEvent ( QKeyEvent* );

};
#endif
