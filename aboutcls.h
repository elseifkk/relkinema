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
#ifndef ABOUTCLS_H
#define ABOUTCLS_H

#include "about.h"

#include <qpixmap.h>
#include <qmutex.h>

class aboutcls: public aboutdlg
{
		Q_OBJECT
	public:
		aboutcls ( QWidget *parent = 0, const char *name = 0, WFlags wf=0 );
		int tid;
		QImage *img;
		bool drawing;
		QImage *img0;
		int *filled;
		int fillcount;
		bool flash;
		bool pmput;
		QColor fcol;
		int count1,count2,count3,count4;
		QPoint p1,p2,p3,p4,p5;
		int nx,ny;
		QRect aboutLogoRect;
		int yyold;
		QColor fgold;
		QString versionStr,buildStr,copyleftStr;
		QColor setFgCol ( double );
		QString wb;
		double z0;
		bool titledone;
		QMutex tveto;

	private slots:
		void urlSlot();
		void closeSlot();

	protected:
		void timerEvent ( QTimerEvent *e );
		void paintEvent ( QPaintEvent *e );
		void keyPressEvent ( QKeyEvent *e );
};

#endif
