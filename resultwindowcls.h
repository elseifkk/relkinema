/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
 *   elseifkk@gmail.com                                                    *
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
#ifndef RESULTWINDOWCLS_H
#define RESULTWINDOWCLS_H

#include "resultWindow.h"

#include <qtable.h>
#include <qmutex.h>

int const nrow_hdr = 2;

int const col_th3  = 0;
int const col_th3c = 1;
int const col_q    = 2;
int const col_K3   = 3;
int const col_p3   = 4;
int const col_J3   = 5;
int const col_ks   = 6;
int const col_fk   = 7;
int const col_th4  = 8;
int const col_th4c = 9;
int const col_K4   = 10;
int const col_p4   = 11;
int const col_J4   = 12;
int const ncol     = 13;

class resultWindowCls: public resultWindow
{
		Q_OBJECT
	public:
		resultWindowCls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0 );

		void initResultDescBox ( QString reaction,
		                         double incidentEnergy, double incidentMomentum,
		                         double Ex, double QValue,
		                         double beta, double gamma,
		                         double m1, double m2, double m3, double m4,
		                         QString double_format );
		void startPoll();
		void initResultTable ( int,QString,QString,QString );

	public:
		QString *homedir;
		QStringList *sl;
		int *ndone;
		int col_first;

	private:
		void setPlotPoints ( int ,int ,int,int );
		void setPlotMinMax ( int,int );
		void changeFontSize ( int );
		int countCTI ( int,int *liid=NULL );
		void adjTable();
		void initCTI();

	private:
		QComboTableItem *cti[ncol];
		int timerid;
		int nrow;
		int nrmax;
		int nplots;

	private slots:
		void saveasSlot();
		void plotSlot();
		void magSlot();
		void redSlot();
		void hideDescSlot();
		void plotDone();
		void rtSlot ( int,int );
		void setTable();

	protected:
		void keyPressEvent ( QKeyEvent* );
		void closeEvent ( QCloseEvent* );
		void timerEvent ( QTimerEvent * );

	signals:
		void done ( void );

};

#endif
