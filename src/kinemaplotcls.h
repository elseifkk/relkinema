/***************************************************************************
 *   Copyright (C) 2011-2012,2014 by Kazuaki Kumagai                       *
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
#ifndef KINEMAPLOTCLS_H
#define KINEMAPLOTCLS_H

#include "kinemaPlot.h"
#include "rwthreadcls.h"

#include <qlistview.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qtable.h>
#include <qmutex.h>
#include <qfontmetrics.h>

int const plot_width_def  = 600;
int const plot_height_def = 400;

int const nlinemax = nrkpmax+nexpmax-1;

class kinemaPlotCls: public kinemaPlot
{
		Q_OBJECT
	public:
		kinemaPlotCls ( QWidget *parent = 0, const char *name = 0, WFlags=0, QTable *table = 0,
		                QString *homedir=0, QString *plotLbl=0,
		                int xcol = 0, int nycol = 0, int *ycols = 0,
		                int nrow_hdr = 0, QFont font = QFont ( "Sans Serif", 11 ) );

	private:
		void setPlotMinMax();
		int getMaxlenytitle();
		void setPlotPoints ( int );
		int getX ( double );
		int getY ( double );
		double tegX ( int );
		double tegY ( int );
		int ylabelMaxLen ( double,double );
		void mess ( QPainter* );
		void getInput ( QString , int );
		void changeFontSize ( int );
		void eraseCoord ( QPainter* );
		void getRootWindowSize ( int*,int* );
		void toggleOnTop();
		void setMeasureGeom ( int,int );
		void drawDots ( QPainter*, int, int, int shape=0 );
		void rescaleX ( double );
		void rescaleY ( double );
		void shiftX ( double );
		void shiftY ( double );
		void showMenu();
		void drawAxis ( QPainter* );
		void drawLegend ( QPainter* );
		void drawPlots ( QPainter* );
		void drawTitle ( QPainter* );
		void drawMeasure ( QPainter* );
		void drawSubMeasure ( QPainter* );
		void printPlot ( QString );
		void setPlotColors ( int );
		void drawFrame ( QPainter* );
		QString bool2str ( bool );
		void setBB ( QString,double );
		void setFontSize ( int );
		void drawCross ( int,int );
		void eraseCross();
		void putCross ( int,int );
		void updateA();
		void procBP ( QPoint );
		void setYLabel();
		void togglePlotOn ( int );
		int countPlotOn();
		void changeLineCol ( int,int );
		void changeLineCol ( int,QColor );
		void showPos(QPoint p);

	private:
		QString *homedir;
		QString *plotLbl;
		int xcol;
		int nycol;
		int ycols[nrkpmax+nexpmax];
		QFont theuifont;
		int theWidth, theHeight;
		double scale;
		bool msrOn;
		bool dotOn;
		bool crossOn;
		int nx,ny;
		double xmin,xmax;
		double ymin,ymax;
		double xmin0,xmax0;
		double ymin0,ymax0;
		QPointArray *pa;
		double **pas;
		int botmar;
		int leftmar;
		double ymaxmar;
		double xmaxmar;
		int vs,hs;
		double x_start,y_start;
		int x0,y0;
		unsigned int lendigitmax;
		bool rep;
		int timerid;
		double mx,my;
		double mx0,my0;
		int nplots;
		QLineEdit *in;
		QLabel *label;
		int iread;
		int fpw,fph,fpm,wE;
		QFont thefont;
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
		QMutex rveto;
		QMutex pveto;
		QMutex mveto;
		QMutex closemt;
		QColor colTitle; // green
		QColor colLegendRect; // gray
		QColor colLegendText; // green
		QColor colAxis; // green
		QColor colSubMeasure; // 0 50 0
		QColor colMeasureLabel; // green
		QColor colMeasure;      // darkgreen
		QRect plotRect;
		bool colorps;
		QFontMetrics *fm;
		int fontsz;
		int cx, cy;
		int linewidth;
		QPixmap *thePlot;
		QRect titleRect;
		QRect xtRect, ytRect;
		bool showlabel;
		QString xt,yt;
		QString yts[nlinemax];
		QString yus[nlinemax];
		int lenxt,lenyt;
		QColor pc[nlinemax];
		bool plotOn[nlinemax];
		QRect legendRect;
		int lg_lw;
		int lg_hmar,lg_vmar;
		int nlegend;
		int theMeasurePrecX;
		int theMeasurePrecY;

		template<typename T>T**AllocMatrix ( int u,int v )
		{
			int i; T**a,*b;
			try { a= ( T** ) new char[ ( sizeof*a+sizeof*b*v ) *u]; }
			catch ( ... ) { a=0; }
			if ( a ) b= ( T* ) ( a+u ); else return 0;
			for ( i=0;i<u;i++,b+=v ) a[i]=b;
			return a;
		}
#define ALLOC_MATRIX(T,U,V) AllocMatrix<T>(U,V)
#define FREE(X) delete[]X

	signals:
		void done();

	private slots:
		void readInput();
		void setXMin();
		void setXMax();
		void setSdx();
		void setNdx();
		void setdx();
		void setYMin();
		void setYMax();
		void setSdy();
		void setNdy();
		void setdy();
		void showUsage();
		void toggleColorPS();
		void saveas ( int mode=0 );
		void toggleDotOn();
		void toggleMsrOn();
		void toggleCrossOn();

	protected:
		void paintEvent ( QPaintEvent* );
		void keyPressEvent ( QKeyEvent* );
		void mouseMoveEvent ( QMouseEvent* );
		void timerEvent ( QTimerEvent* );
		void closeEvent ( QCloseEvent* );
		void mousePressEvent ( QMouseEvent * );
		void mouseReleaseEvent ( QMouseEvent * );
		void mouseDoubleClickEvent ( QMouseEvent * );
};

#endif
