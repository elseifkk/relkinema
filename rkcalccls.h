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
#ifndef RKCALCCLS_H
#define RKCALCCLS_H

#include "rkCalcDlg.h"

#include <qwidget.h>

#include <fzc.h>

class rkCalcCls: public rkCalcDlg
{
		Q_OBJECT
	public:
		rkCalcCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0, const QString *configfile = 0 )
		:rkCalcDlg ( parent, name, wf ), configfile (configfile){};
		void setParameter ( const double *v, QString name );
		bool initRKC();

	private:
		void saveConfig(QString);
		void loadConfig(QString);
		void mess ( QString,QColor c="black" , bool nl=true);

	public:
		const QString *configfile;

	private:
		size_t prpnc;
		char cstr[LEN_FZCSTR_MAX];
		size_t pcstr;

	private slots:
		void enterSlot ( void );
		void clearSlot ( void );

	protected:
		void closeEvent ( QCloseEvent* );
		void keyPressEvent ( QKeyEvent* );

};
#endif