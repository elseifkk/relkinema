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
#include "kinemaplotcls.h"
#include "mboxcls.h"

#include <math.h>
#include <stdlib.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qbitmap.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qwmatrix.h>
#include <qimage.h>

#include <kmessagebox.h>
#include <kfiledialog.h>


#define MAX2(x, y) ((x) > (y) ? (x) : (y))
#define MIN2(x, y) ((x) > (y) ? (y) : (x))
#define NIN(x) floor(0.5 + (x) )

uchar cross_bits[] = {  0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00,
                        0x00, 0x00
                     };
uchar cross_mask[] = {  0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x00, 0x00,
                        0x3f, 0xfe,
                        0x00, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00,
                        0x80, 0x00
                     };
uchar const dot_mask[]={0xff,0xfd,0xff,0xff};
uchar const dot_bits[]={0x00,0x00,0x00,0x00};
int const messTime=3000;
int const nx_def=10;
int const ny_def=10;
double const rescaleFactor=1./16.;
double const shiftFactor=1./16.;

int const PC_COLOR = 0;
int const PC_MONO  = 1;
int const PC_GRAY  = 2;

int const SAM_ANY   = 0;
int const SAM_IMAGE = 1;
int const SAM_PRINT = 2;

int const num_lines_max=12;
QColor pc[num_lines_max];
QColor const pc_col[num_lines_max]=
{
	"white",
	"skyblue",
	"magenta",
	"cyan",
	"green",
	"orange",
	"yellow",
	"pink",
	"purple",
	"chocolate",
	"gray",
	"seagreen"
};
Qt::PenStyle const ps[num_lines_max]=
{
	Qt::SolidLine,
	Qt::DashLine,
	Qt::DotLine,
	Qt::DashDotLine,
	Qt::DashDotDotLine,
	Qt::SolidLine,
	Qt::DashLine,
	Qt::DotLine,
	Qt::DashDotLine,
	Qt::DashDotDotLine,
	Qt::SolidLine,
	Qt::DashLine
};

kinemaPlotCls::kinemaPlotCls ( QWidget *parent, const char *name, WFlags wf )
		:kinemaPlot ( parent, name, wf )
{
	theWidth=width();
	theHeight=height();
	linewidth=2;
	cx=-1;
	cy=-1;
	scale=1.;
	sdx=0;
	sdy=0;
	drag=0;
	timerid=-1;
	msrOn=true;
	dotOn=false;
	crossOn=false;
	pa=NULL;
	nx=0;
	ny=0;
	mx0=0;
	my0=0;
	ymaxmar=1.06;
	xmaxmar=1.06;
	lendigitmax=0;
	rep=false;
	in=NULL;
	label=NULL;
	fontsz=12;
	iread=0;
	thefont.setFamily ( "Monospace" );
	thefont.setPointSize ( fontsz );
	fm = new QFontMetrics ( thefont );
	fph=fm->height();
	showlegend=true;
	QBitmap cb ( 16, 16, cross_bits, TRUE );
	QBitmap cm ( 16, 16, cross_mask, TRUE );
	setCursor ( QCursor ( cb,cm,7,7 ) );
	setPlotColors ( PC_COLOR );
	colorps=true;
	setBackgroundMode ( Qt::NoBackground );
	getRootWindowSize ( &rootw, &rooth );
	thePlot=new QPixmap ( rootw,rooth );
	if ( rootw<=0||rooth<=0 )
	{
		rootw=2560; //<<<<<<<<<<<<<<
		rooth=1600;
	}
}

void kinemaPlotCls::getInput ( QString caption, int ir )
{
	int xorig=width() /2;
	int yorig=height() /2;
	int inwidth=110;
	int labelwidth;
	if ( rveto.locked() ) return;
	rveto.lock();
	iread=ir;
	label = new QLabel ( this );
	in = new QLineEdit ( this );
	label->setFont ( thefont );
	in->setFont ( thefont );
	label->setPaletteForegroundColor ( "skyblue" );
	label->setPaletteBackgroundColor ( "black" );
	in->setPaletteForegroundColor ( "skyblue" );
	in->setPaletteBackgroundColor ( "black" );
	connect ( in, SIGNAL ( returnPressed() ), this, SLOT ( readInput() ) );
	label->setText ( caption );
	label->adjustSize();
	labelwidth=label->width();
	xorig-= ( labelwidth+inwidth ) /2;
	label->setGeometry ( xorig,yorig,labelwidth,30 );
	in->setGeometry ( xorig+labelwidth,yorig,inwidth,30 );
	label->show();
	in->show();
	in->setFocus();
}

int const IR_NOP =0;
int const IR_XMIN=1;
int const IR_XMAX=2;
int const IR_YMIN=3;
int const IR_YMAX=4;
int const IR_DX  =5;
int const IR_DY  =6;
int const IR_SDX =7;
int const IR_SDY =8;
int const IR_NDX =9;
int const IR_NDY =10;
int const IR_LW  =11;

void kinemaPlotCls::readInput()
{
	bool ok;
	double v;
	in->clearFocus();
	label->clearFocus();
	disconnect ( in, SIGNAL ( returnPressed() ), this, SLOT ( readInput() ) );
	label->close();
	in->close();
	v=in->text().toDouble ( &ok );
	if ( ok )
	{
		switch ( iread )
		{
			case IR_XMIN:
				if ( v<xmax )
				{
					xmin=v;
				}
				break;
			case IR_XMAX:
				if ( v>xmin )
				{
					xmax=v;
				}
				break;
			case IR_YMIN:
				if ( v<ymax )
				{
					ymin=v;
				}
				break;
			case IR_YMAX:
				if ( v>ymin )
				{
					ymax=v;
				}
				break;
			case IR_DX:
				if ( v>0&&v<xmax ) mx0=v;
				break;
			case IR_DY:
				if ( v>0&&v<ymax ) my0=v;
				break;
			case IR_SDX:
				if ( v>=0 ) sdx=v;
				break;
			case IR_SDY:
				if ( v>=0 ) sdy=v;
				break;
			case IR_NDX:
				if ( v>1 )
				{
					mx0=0;
					nx=v;
				}
				break;
			case IR_NDY:
				if ( v>1 )
				{
					my0=0;
					ny=v;
				}
				break;
			case IR_LW:
				if ( v>=1 ) linewidth=v;
				break;
		}
	}
	delete in;
	delete label;
	updateA();
	iread=0;
	rveto.unlock();
}

int const min_pointsize=4;
int const max_pointsize=32;
void kinemaPlotCls::setFontSize ( int sz )
{
	fontsz=sz;
	thefont.setPointSize ( fontsz );
	pveto.lock(); // for safety
	mveto.lock();
	rveto.lock();
	delete fm;
	fm=new QFontMetrics ( thefont );
	fph=fm->height();
	pveto.unlock();
	mveto.unlock();
	rveto.unlock();
}
void kinemaPlotCls::changeFontSize ( int d )
{
	int sz=fontsz+d;
	if ( sz>max_pointsize )
	{
		sz=max_pointsize;
	}
	else if ( sz<min_pointsize )
	{
		sz=min_pointsize;
	}
	setFontSize ( sz );
	updateA();
}

void kinemaPlotCls::toggleOnTop()
{
	int f;
	QPoint p ( geometry().x(),geometry().y() );
	f=getWFlags();
	if ( f&Qt::WStyle_StaysOnTop )
	{
		f^=Qt::WStyle_StaysOnTop;
	}
	else
	{
		f|=Qt::WStyle_StaysOnTop;
	}
	reparent ( 0,f,p,true );
}

void kinemaPlotCls::shiftX ( double f )
{
	double len=xmax-xmin;
	xmax+=len*f;
	xmin+=len*f;
}
void kinemaPlotCls::shiftY ( double f )
{
	double len=ymax-ymin;
	ymax+=len*f;
	ymin+=len*f;
}
void kinemaPlotCls::rescaleX ( double f )
{
	double len=xmax-xmin;
	xmax+=len*f;
	xmin-=len*f;
}
void kinemaPlotCls::rescaleY ( double f )
{
	double len=ymax-ymin;
	ymax+=len*f;
	ymin-=len*f;
}

QString kinemaPlotCls::bool2str ( bool b )
{
	if ( b )
	{
		return "True";
	}
	else
	{
		return "False";
	}
}
void kinemaPlotCls::keyPressEvent ( QKeyEvent *e )
{
	e->accept();
	if ( e->state() == Qt::AltButton
	        || ( e->state() &Qt::AltButton && e->state() &Qt::ControlButton ) )
	{
		switch ( e->key() )
		{
			case Qt::Key_Right:
				setXMax();
				return;
			case Qt::Key_Left:
				setXMin();
				return;
			case Qt::Key_Up:
				setYMax();
				return;
			case Qt::Key_Down:
				setYMin();
				return;
		}
	}

	if ( e->state() == Qt::ShiftButton )
	{
		switch ( e->key() )
		{
			case Qt::Key_X:
				getInput ( "Ticks subdivision in X = ", IR_SDX );
				return;
			case Qt::Key_Y:
				getInput ( "Ticks subdivision in Y = ", IR_SDY );
				return;
		}
	}
	if ( e->state() == Qt::ControlButton )
	{
		switch ( e->key() )
		{
			case Qt::Key_X:
				getInput ( "Major Ticks division in X = ", IR_NDX );
				return;
			case Qt::Key_Y:
				getInput ( "Major Ticks division in Y = ", IR_NDY );
				return;
		}
	}

	switch ( e->key() )
	{
		case Qt::Key_D:
			toggleDotOn();
			return;
		case Qt::Key_C:
			toggleCrossOn();
			return;
		case Qt::Key_M:
			toggleMsrOn();
			return;
		case Qt::Key_W:
			getInput ( "Line width = ", IR_LW );
			return;
		case Qt::Key_X:
			getInput ( "Major Ticks in X = ", IR_DX );
			return;
		case Qt::Key_Y:
			getInput ( "Major Ticks in Y = ", IR_DY );
			return;
		case Qt::Key_K:
			toggleColorPS();
			return;
		case Qt::Key_T:
			toggleOnTop();
			return;
		case Qt::Key_H:
			showUsage();
			return;
		case Qt::Key_F:
			mveto.lock();
			if ( crossOn ) eraseCross();
			if ( windowState() &Qt::WindowFullScreen )
			{
				setWindowState ( Qt::WindowNoState );
			}
			else
			{
				setWindowState ( Qt::WindowFullScreen );
			}
			mveto.unlock();
			return;
		case Qt::Key_L:
			showlegend=!showlegend;
			updateA();
			return;
		case Qt::Key_Plus:
			changeFontSize ( 1 );
			return;
		case Qt::Key_Minus:
			changeFontSize ( -1 );
			return;
		case Qt::Key_Q:
		case Qt::Key_Escape:
			if ( iread==0 )
			{
				close();
			}
			else
			{
				iread=IR_NOP;
				readInput();
			}
			return;
		case Qt::Key_P:
			saveas ( SAM_PRINT );
			return;
		case Qt::Key_S:
			saveas ( SAM_IMAGE );
			return;
		case Qt::Key_Home:
			xmin=xmin0;
			xmax=xmax0;
			ymin=ymin0;
			ymax=ymax0;
			mx0=0;
			my0=0;
			updateA();
			return;
		case Qt::Key_Left:
			if ( e->state() & Qt::ControlButton )
			{
				if ( e->state() & Qt::ShiftButton )
				{
					rescaleX ( 2*rescaleFactor );
				}
				else
				{
					rescaleX ( rescaleFactor );
				}
			}
			else
			{
				if ( e->state() & Qt::ShiftButton )
				{
					shiftX ( 2*shiftFactor );
				}
				else
				{
					shiftX ( shiftFactor );
				}
			}
			updateA();
			return;
		case Qt::Key_Right:
			if ( e->state() & Qt::ControlButton )
			{
				if ( e->state() & Qt::ShiftButton )
				{
					rescaleX ( -2*rescaleFactor );
				}
				else
				{
					rescaleX ( -rescaleFactor );
				}
			}
			else
			{
				if ( e->state() & Qt::ShiftButton )
				{
					shiftX ( -2*shiftFactor );
				}
				else
				{
					shiftX ( -shiftFactor );
				}
			}
			updateA();
			return;
		case Qt::Key_Up:
			if ( e->state() & Qt::ControlButton )
			{
				if ( e->state() & Qt::ShiftButton )
				{
					rescaleY ( -2*rescaleFactor );
				}
				else
				{
					rescaleY ( -rescaleFactor );
				}
			}
			else
			{
				if ( e->state() & Qt::ShiftButton )
				{
					shiftY ( -2*shiftFactor );
				}
				else
				{
					shiftY ( -shiftFactor );
				}
			}
			updateA();
			return;
		case Qt::Key_Down:
			if ( e->state() & Qt::ControlButton )
			{
				if ( e->state() & Qt::ShiftButton )
				{
					rescaleY ( 2*rescaleFactor );
				}
				else
				{
					rescaleY ( rescaleFactor );
				}
			}
			else
			{
				if ( e->state() & Qt::ShiftButton )
				{
					shiftY ( 2*shiftFactor );
				}
				else
				{
					shiftY ( shiftFactor );
				}
			}
			updateA();
			return;
		case Qt::Key_R:
			if ( timerid!=-1 )
			{
				killTimer ( timerid );
				timerid=-1;
			}
			updateA();
			return;
	}
	e->ignore();
}

void kinemaPlotCls::mouseMoveEvent ( QMouseEvent *e )
{
	if ( e->x() <x0||e->y() >y0 )
	{
		e->ignore();
		return;
	}

	if ( drag>=1 )
	{
		double dx,dy;
		drag=2; // useless ?
		dx=drgdx* ( drgpos0.x()-e->pos().x() );
		dy=drgdy* ( drgpos0.y()-e->pos().y() );
		xmax=drgxmax0+dx;
		xmin=drgxmin0+dx;
		ymax=drgymax0+dy;
		ymin=drgymin0+dy;
		updateA(); // repaint?
		return;
	}
	QBrush b ( "black" );
	QString pos=QString::number ( tegX ( e->x() ) ) +", "+QString::number ( tegY ( e->y() ) );
	mveto.lock();
	if ( crossOn ) eraseCross();
	unsigned int l=fm->width ( ( const QString& ) pos );
	if ( crossOn ) drawCross ( e->x(),e->y() );
	mveto.unlock();
	if ( lendigitmax<l ) lendigitmax=l;
	QPainter p ( this );
	p.setFont ( thefont );
	p.setPen ( "yellow" );
	eraseCoord ( &p );
	p.drawText ( 0,height()-1,pos );
}

int kinemaPlotCls::getMaxlenytitle()
{
	unsigned int ml=0;
	for ( int iy=0;iy<nycol;iy++ )
	{
		unsigned int l=fm->width ( ( const QString& ) table->horizontalHeader()->label ( ycols[iy] ) );
		if ( ml<l ) ml=l;
	}
	return ml;
}

void kinemaPlotCls::setPlotMinMax()
{
	double x,y;
	bool ok;
	xmin=1e38;
	xmax=-1e38;
	ymin=1e38;
	ymax=-1e38;
	nplots=table->numRows()-1-1; //<<<<<<<<<<<<<
	xunit=table->text ( 1,xcol );
	yunit=table->text ( 1,ycol );
	for ( int iy=0;iy<nycol;iy++ )
	{
		if ( iy!=xcol&&table->text ( 1,iy ) !=yunit ) yunit="a.u.";
		for ( int ir=2;ir<table->numRows();ir++ )
		{
			x=table->text ( ir,xcol ).toDouble ( &ok );
			if ( ok )
			{
				y=table->text ( ir,ycols[iy] ).toDouble ( &ok );
				if ( ok )
				{
					if ( x<xmin ) xmin=x;
					if ( x>xmax ) xmax=x;
					if ( y<ymin ) ymin=y;
					if ( y>ymax ) ymax=y;
				}
			}
		}
	}
	ymax*=ymaxmar;
	xmax*=xmaxmar;
	xmin0=xmin;
	xmax0=xmax;
	ymin0=ymin;
	ymax0=ymax;
}

void kinemaPlotCls::setPlotPoints ( int iy )
{
	pa = new QPointArray ( nplots );
	for ( int ir=2;ir<table->numRows();ir++ ) //<<<<<<<<<<<<
	{
		pa->setPoint ( ir-2,
		               getX ( table->text ( ir,xcol ).toDouble() ),
		               getY ( table->text ( ir,iy ).toDouble() ) );
	}
}

int kinemaPlotCls::getX ( double x )
{
	return MIN2 ( 32767,MAX2 ( -32767,
	                           + ( double ) ( theWidth-1-x0 ) / ( xmax-xmin ) *x
	                           + ( ( double ) x0*xmax-xmin* ( double ) ( theWidth )-1 ) / ( xmax-xmin ) ) );
}
int kinemaPlotCls::getY ( double y )
{
	return MIN2 ( 32767,MAX2 ( -32767,
	                           - ( double ) y0*y/ ( ymax-ymin )
	                           + ( double ) y0*ymax/ ( ymax-ymin ) ) );

}
double kinemaPlotCls::tegX ( int x )
{
	return ( xmax-xmin ) * ( double ) x/ ( double ) ( theWidth-1-x0 )
	       - ( xmax* ( double ) x0-xmin* ( double ) ( theWidth-1 ) ) / ( double ) ( theWidth-1-x0 );
}
double kinemaPlotCls::tegY ( int y )
{
	return - ( ymax-ymin ) * ( double ) y/ ( double ) y0+ymax;
}

int kinemaPlotCls::ylabelMaxLen ( double y, double my )
{
	int ylenmax=0;
	for ( int iy=0;iy<ny;iy++ )
	{
		y+=my;
		if ( y>=ymax ) break;
		if ( fabs ( y ) <my/2. ) y=0.;
		int ylen=fm->width ( QString::number ( y ) );
		if ( ylenmax<ylen ) ylenmax=ylen;
	}
	return ylenmax;
}

void kinemaPlotCls::timerEvent ( QTimerEvent *e )
{
	killTimer ( timerid );
	timerid=-1;
	updateA();
}

void kinemaPlotCls::eraseCoord ( QPainter *p )
{
	QRect r ( 0,height()-1- ( fph -1 ),lendigitmax, fph );
	p->fillRect ( r, QBrush ( "black" ) );
}

void kinemaPlotCls::saveas ( int mode )
{
	QString filter="";
	switch ( mode )
	{
		case SAM_ANY:
		case SAM_IMAGE:
			filter="*.png|PNG image (*.png)\n*.jpg|JPG image (*.jpg)\n*.xpm|XPM image (*.xpm)\n";
			if ( mode!=SAM_ANY ) break;
		case SAM_PRINT:
			filter.append ( "*.eps|Encapsulated PostScript (*.eps)\n*.ps|PostScript (*.ps)\n" );
			break;
	}
	filter.append ( "*.*|All (*.*)" );

	QString f= KFileDialog::getSaveFileName ( *homedir,filter,this,
	           tr ( "Save plot as" ) );
	if ( f.length() <=0 ) return;
	if ( QFile::exists ( f ) )
	{
		if ( KMessageBox::questionYesNo ( this,
		                                  "File already exists:\n"+f+"\n Replace it?",
		                                  "kinemaPlot: Warning!" )
		        !=KMessageBox::Yes ) return;
	}

	bool ok;
	QString fmt="";
	if ( f.lower().endsWith ( ".png" ) )
	{
		fmt="PNG";
	}
	else if ( f.lower().endsWith ( ".jpg" ) )
	{
		fmt="JPEG";
	}
	else if ( f.lower().endsWith ( ".xpm" ) )
	{
		fmt="XPM";
	}
	else if ( f.lower().endsWith ( ".eps" ) ||f.lower().endsWith ( ".ps" ) )
	{
		printPlot ( f );
		ok=true;
	}
	else
	{
		ok=false;
	}

	if ( !fmt.isEmpty() )
	{
		QPixmap pm ( size() );
		bitBlt ( &pm, 0, 0, this );
		QPainter p ( &pm );
		eraseCoord ( &p );
		ok= pm.save ( f,fmt );
	}

	if ( ok )
	{
		messStr="Plot saved on "+f;
		messCol="skyblue";
	}
	else
	{
		if ( !fmt.isEmpty() )
		{
			messStr="Failed to save file: "+f;
			messCol="red";
		}
		else
		{
			messStr="Unsupported format: "+f;
			messCol="red";
		}
	}
	timerid=startTimer ( messTime );
	updateA();
}

void kinemaPlotCls::mess ( QPainter *p )
{
	int x,y,wx,wy;
	int mar=4;
	wx=fm->width ( ( const QString& ) messStr );
	wy=fph;
	x=width() /2 -wx/2;
	y=height() /2-wy/2;
	p->fillRect ( x-mar,y-mar,wx+mar*2,wy+mar*2,QBrush ( "black" ) );
	p->setFont ( thefont );
	p->setPen ( messCol );
	p->drawText ( x,y+fph ,messStr );
}

void kinemaPlotCls::setMeasureGeom ( int w, int h )
{
	theWidth=w;
	theHeight=h;
	if ( mx0!=0 )
	{
		mx=mx0;
		nx=NIN ( ( xmax-xmin ) /mx0 );
	}
	else
	{
		if ( nx==0 ) nx=nx_def;
		mx=QString::number ( ( xmax-xmin ) /nx,'g',1 ).toDouble();
	}
	if ( my0!=0 )
	{
		my=my0;
		ny=NIN ( ( ymax-ymin ) /my0 );
	}
	else
	{
		if ( ny==0 ) ny=ny_def;
		my=QString::number ( ( ymax-ymin ) /ny,'g',1 ).toDouble();
	}
	x_start=floor ( xmin/mx ) *mx;
	y_start=floor ( ymin/my ) *my;
// text geom
	int ylml=ylabelMaxLen ( y_start,my );
	vs=hs=fph/3;
	leftmar= NIN ( ( double ) ( ylml+fph+hs ) *1.2 );//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	botmar= ( fph+fph+vs );//*1.2;//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// the origin in px
	x0=leftmar;
	y0=theHeight-1-botmar;
// plot region
	plotRect.setLeft ( x0 );
	plotRect.setRight ( theWidth-1 );
	plotRect.setTop ( 0 );
	plotRect.setBottom ( y0 );



}

void kinemaPlotCls::drawDots ( QPainter *p, int ixmin, int iymax, int shape )
{
	p->save();
	p->setBrush ( p->pen().color() );
	for ( int i=0;i<nplots;i++ )
	{
		int x=pa->point ( i ).x()-2;
		int y=pa->point ( i ).y()-2;
		if ( ixmin<=x&&x<theWidth-1&&0<y&&y<=iymax )
			p->drawEllipse ( x,y,5,5 );
	}
	p->restore();
}

void kinemaPlotCls::paintEvent ( QPaintEvent *e )
{
	pveto.lock();
	QPainter p;

	p.begin ( thePlot, this );
	p.setBackgroundColor ( "black" );
	p.eraseRect ( rect() );

	setMeasureGeom ( width(),height() );

// draw Title
	drawTitle ( &p );

// draw measure
	drawSubMeasure ( &p );
	drawMeasure ( &p );

	p.setClipRect ( plotRect );
	drawPlots ( &p );
	p.setClipping ( false );

	if ( nycol>1&&showlegend ) drawLegend ( &p );

	if ( timerid!=-1 ) mess ( &p );

	// draw axis
	drawAxis ( &p );

	p.end();

	bitBlt ( this, 0, 0, thePlot,0,0,width(),height() );

	pveto.unlock();
}

void kinemaPlotCls::drawMeasure ( QPainter *p )
{
	int y00=0;
	int x00=theWidth-1;
	if ( !msrOn )
	{
		x00=x0+10;//<<<<<<<<<<<<
		y00=y0-10;//<<<<<<<<<<<<
	}
	QPen mmp;
	mmp.setColor ( colMeasure );
	mmp.setStyle ( Qt::SolidLine );
	QPen mlp;
	mlp.setColor ( colMeasureLabel );
	mlp.setStyle ( Qt::SolidLine );
	double x=x_start-mx;
	for ( int ix=0;;ix++ )
	{
		x+=mx;
		if ( x<xmin ) continue;
		if ( x>xmax ) break;
		int px=getX ( x );
		p->setPen ( mmp );
		p->drawLine ( px,y0,px,y00 );
		p->setPen ( mlp );
		if ( fabs ( x ) <mx/2. ) x=0.;
		QString s=QString::number ( x );
		int l= fm->width ( ( const QString& ) s ) /2;
		p->drawText ( px-l,y0+fph,s );
	}
	double y=y_start-my;
	for ( int iy=0;;iy++ )
	{
		y+=my;
		if ( y<ymin ) continue;
		if ( y>ymax ) break;
		int py=getY ( y );
		p->setPen ( mmp );
		p->drawLine ( x0,py,x00,py );
		p->setPen ( mlp );
		if ( fabs ( y ) <my/2. ) y=0.;
		QString s=QString::number ( y );
		int l=fm->width ( ( const  QString& ) s );
		p->drawText ( x0-l-hs,py+fph/2, s );
	}
}

void kinemaPlotCls::drawSubMeasure ( QPainter *p )
{
	int y00=0;
	int x00=theWidth-1;
	if ( !msrOn )
	{
		x00=x0+10;//<<<<<<<<<<<<
		y00=y0-10;//<<<<<<<<<<<<
	}
	QPen smp;
	smp.setColor ( colSubMeasure );
	smp.setStyle ( Qt::DotLine );//<<<<<<<<<<<<<<<<
	if ( sdx>1 )
	{
		double x=x_start-mx-mx;
		p->setPen ( smp );
		double smx=mx/sdx;
		for ( int ix=0;;ix++ )
		{
			x+=mx;
			if ( x>xmax ) break;
			for ( int iix=1;iix<sdx;iix++ )
			{
				double xx=x+smx*iix;
				if ( xx<xmin ) continue;
				if ( xx>xmax ) break;
				int px=getX ( xx );
				p->drawLine ( px,y0,px,y00 );
			}
		}
	}
	if ( sdy>1 )
	{
		double y=y_start-my-my;
		p->setPen ( smp );
		double smy=my/sdy;
		for ( int iy=0;;iy++ )
		{
			y+=my;
			if ( y>ymax ) break;
			for ( int iiy=1;iiy<sdy;iiy++ )
			{
				double yy=y+smy*iiy;
				if ( yy<ymin ) continue;
				if ( yy>ymax ) break;
				int py=getY ( yy );
				p->drawLine ( x0,py,x00,py );
			}
		}
	}
}

void kinemaPlotCls::drawTitle ( QPainter *p )
{
	p->setPen ( colTitle );
	p->setFont ( thefont );
	QString xt,yt;
	xt=table->horizontalHeader()->label ( xcol );
	if ( xunit.length() !=0 ) xt.append ( " ("+xunit+")" );

	if ( nycol==1 )
	{
		yt=table->horizontalHeader()->label ( ycol );
	}
	else
	{
		yt="";
	}
	if ( yunit.length() !=0 ) yt.append ( " ("+yunit+")" );

	p->drawText ( ( theWidth+x0 ) /2-fm->width ( ( const QString& ) xt ) /2,theHeight-vs,xt );

	p->rotate ( -90. );
	// x decrease -> y increase, y increase -> x increase
	p->drawText ( - ( ( y0 ) /2+fm->width ( ( const QString& ) yt ) /2 ),fph,yt );
	p->rotate ( 90. );
}

void kinemaPlotCls::drawPlots ( QPainter *p )
{
	QPen pline;
	for ( int iy=0;iy<nycol;iy++ )
	{
		setPlotPoints ( ycols[iy] );
		pline.setStyle ( ps[iy] );
		pline.setColor ( pc[iy] );
		pline.setWidth ( linewidth );
		p->setPen ( pline );
		p->drawPolyline ( *pa );
		if ( dotOn ) drawDots ( p, x0, y0 );
		p->drawPoints ( *pa );
		delete pa;
	}
}

void kinemaPlotCls::drawLegend ( QPainter *p )
{
	int const lengend=fph*2; // <<<<<<<<<<<<<<<
	int const hmar=NIN ( ( double ) fph/2. );
	int const vmar=NIN ( ( double ) fph/3. );
	int const lmar=NIN ( ( double ) fph/4. );
	QPen pline;
	int legendWidth=getMaxlenytitle() +lengend+hmar*2+lmar;
	int legendHeight=nycol* ( fph+lmar )-lmar+vmar*2;
	QRect legendRect;
	legendRect.setLeft ( theWidth-1-legendWidth );
	legendRect.setRight ( theWidth-1 );
	legendRect.setTop ( 0 );
	legendRect.setBottom ( legendHeight );
	p->eraseRect ( legendRect );
	p->setPen ( colLegendRect );
	p->drawRect ( legendRect );
	for ( int iy=0;iy<nycol;iy++ )
	{
		int yy=NIN ( ( double ) ( iy* ( fph+lmar ) ) + ( double ) fph/2.+ ( double ) vmar );
		int xx=legendRect.left() +hmar;
		pline.setStyle ( ps[iy] );
		pline.setColor ( pc[iy] );
		p->setPen ( pline );
		p->drawLine ( xx,yy,xx+lengend,yy );
		p->setPen ( colLegendText );
		p->drawText ( xx+lengend+lmar,yy+fph/2,
		              table->horizontalHeader()->label ( ycols[iy] ) );
	}
}

void kinemaPlotCls::drawFrame ( QPainter *p )
{
	QPen ap;
	ap.setColor ( colAxis );
	ap.setWidth ( 2 );
	p->setPen ( ap );
	p->setBrush ( Qt::NoBrush );
	p->drawRect ( plotRect );
}

void kinemaPlotCls::drawAxis ( QPainter *p )
{
	QPen ap;
	ap.setColor ( colAxis );
	ap.setWidth ( 2 );
	p->setPen ( ap );
	p->drawLine ( x0,y0,theWidth-1,y0 );
	p->drawLine ( x0,y0,x0,0 );

}

void kinemaPlotCls::showUsage()
{
	QString usage=
	    "=== Key bindings ===\n\n"
	    "  arrow key:  \tmove\n"
	    "    +ctrl:    \tmagnify/reduce\n"
	    "    +shft:    \tmove/magnify/reduce large\n"
	    "    +alt:     \tset x/y min/max\n"
	    "    +ctrl+alt:\tsame as above\n\n"
	    "  home:       \tto original position\n"
	    "  plus:       \tenlarge font\n"
	    "  minus:      \tshrink font\n"
	    "  cntl+s:     \tsave image as png/jpg/xpm file\n"
	    "  cntl+p:     \tprint plots on ps/eps file\n"
	    "  X,Y:        \tset dx/dy\n"
	    "    +shft     \tset subdivision in x/y\n"
	    "    +ctrl     \tset division in x/y\n"
	    "  C:          \ttoggle cross-hair cursor\n"
	    "  D:          \ttoggle dots\n"
	    "  F:          \ttoggle full screen\n"
	    "  K:          \tToggle color mode when saving (e)ps\n"
	    "  M:          \ttoggle grid\n"
	    "  R:          \trefresh\n"
	    "  T:          \ttoggle stay on top\n"
	    "  W:          \tset line width\n"
	    "  Escape, Q:  exit";
	mboxCls *m;
	m=new mboxCls ( NULL,NULL,Qt::WDestructiveClose,usage,"RelKinema-Plotter: Usage" );
	m->show();
}

void kinemaPlotCls::closeEvent ( QCloseEvent *e )
{
	if ( rveto.locked() ||pveto.locked() )
	{
		e->ignore();
	}
	else
	{
		delete fm;
		delete thePlot;
		emit done();
		e->accept();
	}
}

void kinemaPlotCls::procLB ( QPoint p )
{
	drag=1;
	drgpos0=p;
	drgxmax0=xmax;
	drgxmin0=xmin;
	drgymax0=ymax;
	drgymin0=ymin;
	// the same as tegX and tegY
	drgdx= ( xmax-xmin ) / ( double ) ( theWidth-1-x0 );
	drgdy=- ( ymax-ymin ) / ( double ) ( y0 );

	QPainter q ( this );
	q.setFont ( thefont );
	q.setPen ( "yellow" );
	q.drawPoint ( p.x(),p.y() );
	QString pos=QString::number ( tegX ( p.x() ) ) +", "+QString::number ( tegY ( p.y() ) );
	q.drawText ( p.x() +4,p.y() +fph+2,pos );
}

void kinemaPlotCls::mousePressEvent ( QMouseEvent *e )
{
	switch ( e->button() )
	{
		case LeftButton:
			procLB ( e->pos() );
			e->accept();
			break;
		case RightButton:
			showMenu();
			e->accept();
			break;
		case MidButton:
			if ( crossOn ) putCross ( e->x(),e->y() );
			e->accept();
			break;
		default:
			e->ignore();
	}
}

void kinemaPlotCls::mouseReleaseEvent ( QMouseEvent *e )
{
	drag=0;
}

void kinemaPlotCls::setXMin()
{
	getInput ( "X Min = ", IR_XMIN );
}
void kinemaPlotCls::setXMax()
{
	getInput ( "X Max = ", IR_XMAX );
}
void kinemaPlotCls::setYMin()
{
	getInput ( "Y Min = ", IR_YMIN );
}
void kinemaPlotCls::setYMax()
{
	getInput ( "Y Max = ", IR_YMAX );
}
void kinemaPlotCls::toggleDotOn()
{
	dotOn=!dotOn;
	updateA();
}
void kinemaPlotCls::toggleMsrOn()
{
	msrOn=!msrOn;
	updateA();
}
void kinemaPlotCls::toggleCrossOn()
{
	crossOn=!crossOn;
	updateA();
}
void kinemaPlotCls::showMenu()
{
	QPopupMenu pm ( this );
	pm.setCheckable ( true );
	pm.setCaption ( "test" );
	pm.insertItem ( "Set X Min",this,SLOT ( setXMin() ) );
	pm.insertItem ( "Set X Max",this,SLOT ( setXMax() ) );
	pm.insertItem ( "Set Y Min",this,SLOT ( setYMin() ) );
	pm.insertItem ( "Set Y Max",this,SLOT ( setYMax() ) );
	pm.insertSeparator();
	int monid=pm.insertItem ( "Show grid", this, SLOT ( toggleMsrOn() ) );
	int donid=pm.insertItem ( "Show dots", this, SLOT ( toggleDotOn() ) );
	int conid=pm.insertItem ( "Show cross-hair", this, SLOT ( toggleCrossOn() ) );
	pm.insertSeparator();
	int pscid=pm.insertItem ( "Color (e)ps", this, SLOT ( toggleColorPS() ) );
	pm.insertItem ( "Save image as",this,SLOT ( saveas() ) );
	pm.insertSeparator();
	pm.insertItem ( "Usage",this,SLOT ( showUsage() ) );
	pm.insertSeparator();
	pm.insertItem ( "Close",this,SLOT ( close() ) );

	pm.setItemChecked ( pscid,colorps );
	pm.setItemChecked ( donid,dotOn );
	pm.setItemChecked ( monid,msrOn );
	pm.setItemChecked ( conid,crossOn );

	pm.exec ( QCursor::pos() );
}

void kinemaPlotCls::printPlot ( QString f )
{
	QPrinter pr;
	pr.setOutputFileName ( f );
	pr.setOutputToFile ( true );
	if ( f.endsWith ( ".ps" ) )
	{
		pr.setPageSize ( QPrinter::A4 );
		pr.setFullPage ( false );
	}
	else
	{
		pr.setFullPage ( true );
	}
	pr.setOrientation ( QPrinter::Landscape );

	if ( colorps )
	{
		pr.setColorMode ( QPrinter::Color );
		setPlotColors ( PC_COLOR );
	}
	else
	{
		pr.setColorMode ( QPrinter::GrayScale );
		setPlotColors ( PC_GRAY );
	}
	pr.setResolution ( 100 ); // <<<<<<

	QRect plotRectOrig=plotRect;
	QPainter p;
	double const sf=8.0;
	theWidth=width() *sf;
	theHeight=height() *sf;
	QWMatrix um ( 1./sf, 0., 0., 1./sf, 0., 0. );

	p.begin ( &pr, this );
	p.setWorldMatrix ( um );
	int fontszorig=fontsz;
	setFontSize ( fontsz*sf );

	setMeasureGeom ( theWidth,theHeight );

	if ( colorps )
	{
		p.setBackgroundColor ( "black" );
	}
	else
	{
		p.setBackgroundColor ( "white" );
	}
	p.eraseRect ( 0, 0, theWidth, theHeight );


// draw Title
	drawTitle ( &p );

// draw measure
	drawSubMeasure ( &p );
	drawMeasure ( &p );

	p.setClipRect ( plotRectOrig );
	drawPlots ( &p );
	p.setClipping ( false );

	if ( nycol>1&&showlegend ) drawLegend ( &p );

	// draw axis
//	drawAxis(&p);
	drawFrame ( &p );
	p.end();

	setFontSize ( fontszorig );
	if ( !colorps ) setPlotColors ( PC_COLOR );

	setBB ( f,sf );
}

void kinemaPlotCls::setBB ( QString f, double sf )
{
	QString ff=f+".kinemaplot-tmp"; // <<<<<<<<<
	double const ptfac=1.389;
	int xpt=NIN ( ( double ) theWidth/ptfac ) /sf;
	int ypt=NIN ( ( double ) theHeight/ptfac ) /sf;
	QString com;
	if ( f.endsWith ( ".eps" ) )
	{
		com="sed 's/%%BoundingBox.*/%%BoundingBox 0 0 "
		    +QString::number ( ypt )
		    +" "
		    +QString::number ( xpt )
		    +"/' ";
	}
	else
	{
		com="sed '/%%BoundingBox.*/d' ";
	}
	com.append ( f+">"+ff );
	system ( com );
	com="mv "+ff+" "+f;
	system ( com );
}

void kinemaPlotCls::setPlotColors ( int mode )
{
	switch ( mode )
	{
		case PC_COLOR:
			colTitle="green"; // green
			colLegendRect="gray"; // gray
			colLegendText="green"; // green
			colAxis="green"; // green
			colSubMeasure=qRgb ( 0,50,0 ); // 0 50 0
			colMeasureLabel="green"; // green
			colMeasure="darkgreen";      // darkgreen
			for ( int i=0;i<num_lines_max;i++ ) pc[i]=pc_col[i];
			break;
		case PC_MONO:
		case PC_GRAY:
			colTitle="black"; // green
			colLegendRect="gray"; // gray
			colLegendText="black"; // green
			colAxis="black"; // green
			colSubMeasure=qRgb ( 50,50,50 ); // 0 50 0
			colMeasureLabel="black"; // green
			colMeasure=qRgb ( 20,20,20 );   // darkgreen
			for ( int i=0;i<num_lines_max;i++ ) pc[i]="black";
			break;
	}
}

void kinemaPlotCls::toggleColorPS()
{
	colorps=!colorps;
	messStr="colorps mode set to "+bool2str ( colorps );
	messCol="skyblue";
	timerid=startTimer ( messTime );
	updateA();
}

void kinemaPlotCls::updateA()
{
	if ( crossOn ) eraseCross();
	update();
}
void kinemaPlotCls::putCross ( int x, int y )
{
	if ( x<0||y<0 ) return;
	QPainter p ( this );
	p.setRasterOp ( Qt::XorROP );
	p.setPen ( "red" );
	p.drawLine ( x0,y,width()-1,y );
	p.drawLine ( x,0,x,y0 );
}
void kinemaPlotCls::drawCross ( int x, int y )
{
	if ( x<0||y<0 ) return;
	putCross ( x,y );
	cx=x;
	cy=y;
}
void kinemaPlotCls::eraseCross()
{
	drawCross ( cx,cy );
	cx=-1;
	cy=-1;
}

#include <X11/Xlib.h>
void kinemaPlotCls::getRootWindowSize ( int *w, int *h )
{
	*w=0;
	*h=0;
	Display *dpy = XOpenDisplay ( NULL );
	if ( dpy==NULL ) return; // cannot open display
	int screen=DefaultScreen ( dpy );
//	Window root=RootWindow ( dpy,screen );
	*h=DisplayHeight ( dpy,screen );
	*w=DisplayWidth ( dpy,screen );
	XCloseDisplay ( dpy );
}

#include "kinemaplotcls.moc"
