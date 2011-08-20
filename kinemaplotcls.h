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
#ifndef KINEMAPLOTCLS_H
#define KINEMAPLOTCLS_H

#include "kinemaPlot.h"
#include <qlistview.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qtable.h>

class kinemaPlotCls: public kinemaPlot
{
		Q_OBJECT
	public:
		kinemaPlotCls ( QWidget *parent = 0, const char *name = 0 );
		void setPlotMinMax();

	public:
		QTable *table;
		int xcol,ycol;
		int ycols[8];
		int nycol;
		QString homedir;

	private:
		void setPlotPoints ( int );
		int getX ( double );
		int getY ( double );
		double tegX ( int );
		double tegY ( int );
		int ylabelMaxLen ( double,double );
		void mess ( QPainter* );
		void saveas();
		void getInput ( QString );
		void changeFontSize ( int );
		void eraseCoord ( QPainter* );
//		void getRootWindowSize ( int*,int* );
		void toggleOnTop();
		void setMeasureGeom();
		void drawDots ( QPainter*, int, int, int shape=0 );
		void rescaleX ( double );
		void rescaleY ( double );
		void shiftX ( double );
		void shiftY ( double );
		void showMenu();

	private:
		bool msrOn;
		bool dotOn;
		int nx,ny;
		double xmin,xmax;
		double ymin,ymax;
		double xmin0,xmax0;
		double ymin0,ymax0;
		QPointArray *pa;
		int botmar;
		int leftmar;
		double ymaxmar;
		double xmaxmar;
		int x0,y0;
		unsigned int lendigitmax;
		bool rep;
		int timerid;
		QString xunit;
		QString yunit;
		double mx,my;
		double mx0,my0;
		int nplots;
		QLineEdit *in;
		QLabel *label;
		int iread;
		int fpw,fph,fpm;
		QFont thefont;
		unsigned int maxlenytitle;
		bool showlegend;
		QString messStr;
		QColor messCol;
		int drag;
		QPoint drgpos0;
		double drgxmax0,drgxmin0;
		double drgymax0,drgymin0;
		double drgdx,drgdy;
		int rootw, rooth;
		int sdx, sdy;

	signals:
		void done();

	private slots:
		void readInput();
		void setXMin();
		void setXMax();
		void setYMin();
		void setYMax();
		void showUsage();

	protected:
		void paintEvent ( QPaintEvent* );
		void keyPressEvent ( QKeyEvent* );
		void mouseMoveEvent ( QMouseEvent* );
		void timerEvent ( QTimerEvent* );
		void closeEvent ( QCloseEvent* );
		void mousePressEvent ( QMouseEvent * );
		void mouseReleaseEvent ( QMouseEvent * );
};

#endif
