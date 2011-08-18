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

#include <qpainter.h>
#include <qpixmap.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qbitmap.h>
#include <kmessagebox.h>

#include <kfiledialog.h>

#include <X11/Xlib.h>

#define MAX2(x, y) ((x) > (y) ? (x) : (y))
#define MIN2(x, y) ((x) > (y) ? (y) : (x))


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
uchar dot_mask[]={0xff,0xfd,0xff,0xff};
uchar dot_bits[]={0x00,0x00,0x00,0x00};
int const messTime=3000;
int const nx_def=10;
int const ny_def=10;

kinemaPlotCls::kinemaPlotCls ( QWidget *parent, const char *name )
		:kinemaPlot ( parent, name )
{
	drag=0;
	timerid=-1;
	msrOn=true;
	dotOn=false;
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
	fph=12;
	fpw=7;
	fpm=2;
	iread=0;
	thefont.setFamily ( "Monospace" );
	thefont.setPointSize ( fph );
	showlegend=true;
	QBitmap cb ( 16, 16, cross_bits, TRUE );
	QBitmap cm ( 16, 16, cross_mask, TRUE );
	setCursor ( QCursor ( cb,cm,7,7 ) );
	getRootWindowSize ( &rootw, &rooth );
}

void kinemaPlotCls::getInput ( QString caption )
{
	int xorig=width() /2;
	int yorig=height() /2;
	int inwidth=110;
	int labelwidth=caption.length() *fpw+ ( caption.length()-1 ) *fpm;
	xorig-= ( labelwidth+inwidth ) /2;

	label = new QLabel ( this );
	in = new QLineEdit ( this );
	connect ( in, SIGNAL ( returnPressed() ), this, SLOT ( readInput() ) );
	label->setGeometry ( xorig,yorig,labelwidth,30 );
	in->setGeometry ( xorig+labelwidth+1,yorig,inwidth,30 );
	label->setFont ( thefont );
	in->setFont ( thefont );
	label->setText ( caption );
	label->setPaletteForegroundColor ( "skyblue" );
	label->setPaletteBackgroundColor ( "black" );
	in->setPaletteForegroundColor ( "skyblue" );
	in->setPaletteBackgroundColor ( "black" );
	label->show();
	in->show();
	in->setFocus();
}

int const IR_XMIN=1;
int const IR_XMAX=2;
int const IR_YMIN=3;
int const IR_YMAX=4;
int const IR_DX  =5;
int const IR_DY  =6;

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
		}

	}
	delete in;
	delete label;
	update();
	iread=0;
}

int const min_pointsize=4;
int const max_pointsize=32;

void kinemaPlotCls::changeFontSize ( int d )
{
	fph+=d;
	if ( fph>max_pointsize )
	{
		fph=max_pointsize;
	}
	else if ( fph<min_pointsize )
	{
		fph=min_pointsize;
	}
	fpw=floor ( ( double ) fph/1.5+0.5 ); // <<<<<<<<<<<<<<<<<<<<<<<
	thefont.setPointSize ( fph );
	update();
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

void kinemaPlotCls::keyPressEvent ( QKeyEvent *e )
{
	if ( e->state() == Qt::AltButton
	        || ( e->state() &Qt::AltButton && e->state() &Qt::ControlButton ) )
	{
		switch ( e->key() )
		{
			case Qt::Key_Right:
				iread=IR_XMAX;
				getInput ( "X Max = " );
				return;
			case Qt::Key_Left:
				iread=IR_XMIN;
				getInput ( "X Min = " );
				return;
			case Qt::Key_Up:
				iread=IR_YMAX;
				getInput ( "Y Max = " );
				return;
			case Qt::Key_Down:
				iread=IR_YMIN;
				getInput ( "Y Min = " );
				return;
		}
	}

	double dx=mx/2.;
	double dy=my/2.;

	switch ( e->key() )
	{
		case Qt::Key_D:
			dotOn=!dotOn;
			update();
			return;
		case Qt::Key_M:
			msrOn=!msrOn;
			update();
			return;
		case Qt::Key_X:
			iread=IR_DX;
			getInput ( "DX = " );
			return;
		case Qt::Key_Y:
			iread=IR_DY;
			getInput ( "DY = " );
			return;
		case Qt::Key_P:
			toggleOnTop();
			return;
		case Qt::Key_H:
			showUsage();
			return;
		case Qt::Key_F:
			if ( windowState() &Qt::WindowFullScreen )
			{
				setWindowState ( Qt::WindowNoState );
			}
			else
			{
				setWindowState ( Qt::WindowFullScreen );
			}
			return;
		case Qt::Key_L:
			showlegend=!showlegend;
			update();
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
				iread=-1;
				readInput();
			}
			return;
		case Qt::Key_S:
			saveas();
			return;
		case Qt::Key_Home:
			xmin=xmin0;
			xmax=xmax0;
			ymin=ymin0;
			ymax=ymax0;
			mx0=0;
			my0=0;
			update();
			return;
		case Qt::Key_Left:
			if ( e->state() & Qt::ShiftButton ) dx=mx;
			xmax+=dx/2.;
			if ( e->state() & Qt::ControlButton )
			{
				xmin-=dx/2.;
			}
			else
			{
				xmin+=dx/2.;
			}
			update();
			return;
		case Qt::Key_Right:
			if ( e->state() & Qt::ShiftButton ) dx=mx;
			xmax-=dx/2.;
			if ( e->state() & Qt::ControlButton )
			{
				xmin+=dx/2.;
			}
			else
			{
				xmin-=dx/2.;
			}
			update();
			return;
		case Qt::Key_Up:
			if ( e->state() & Qt::ShiftButton ) dy=my;
			ymax-=dy/2.;
			if ( e->state() & Qt::ControlButton )
			{
				ymin+=dy/2.;
			}
			else
			{
				ymin-=dy/2.;
			}
			update();
			return;
		case Qt::Key_Down:
			if ( e->state() & Qt::ShiftButton ) dy=my;
			ymax+=dy/2.;
			if ( e->state() & Qt::ControlButton )
			{
				ymin-=dy/2.;
			}
			else
			{
				ymin+=dy/2.;
			}
			update();
			return;
		case Qt::Key_R:
			if ( timerid!=-1 )
			{
				killTimer ( timerid );
				timerid=-1;
			}
			update();
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
		update(); // repaint?
		return;
	}

	QBrush b ( "black" );
	QString pos=QString::number ( tegX ( e->x() ) ) +", "+QString::number ( tegY ( e->y() ) );
	if ( lendigitmax<pos.length() ) lendigitmax=pos.length();
	QPainter p ( this );
	p.setFont ( thefont );
	p.setPen ( "yellow" );
	eraseCoord ( &p );
	p.drawText ( 0,height()-1,pos );
}

void kinemaPlotCls::setPlotMinMax()
{
	double x,y;
	bool ok;
	xmin=1e38;
	xmax=-1e38;
	ymin=1e38;
	ymax=-1e38;
	nplots=table->numRows()-1;
	xunit=table->text ( 0,xcol );
	yunit=table->text ( 0,ycol );
	maxlenytitle=0;
	for ( int iy=0;iy<nycol;iy++ )
	{
		if ( maxlenytitle<table->horizontalHeader()->label ( ycols[iy] ).length() )
			maxlenytitle=table->horizontalHeader()->label ( ycols[iy] ).length();
		for ( int ir=1;ir<table->numRows();ir++ )
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
	for ( int ir=1;ir<table->numRows();ir++ )
	{
		pa->setPoint ( ir-1,
		               getX ( table->text ( ir,xcol ).toDouble() ),
		               getY ( table->text ( ir,iy ).toDouble() ) );
	}
}

int kinemaPlotCls::getX ( double x )
{
	return MIN2 ( width() +1,MAX2 ( -1, ( x-xmin ) / ( xmax-xmin ) * ( width()-1-leftmar ) +leftmar ) );
}
int kinemaPlotCls::getY ( double y )
{
	return MIN2 ( height() +1,MAX2 ( -1,height()-1- ( ( y-ymin ) / ( ymax-ymin ) * ( height()-1-botmar ) +botmar ) ) );
}
double kinemaPlotCls::tegX ( int x )
{
	return ( double ) ( x-leftmar ) / ( double ) ( width()-1-leftmar ) * ( xmax-xmin ) +xmin;
}
double kinemaPlotCls::tegY ( int y )
{
	return ( double ) ( height()-1-botmar-y ) / ( double ) ( height()-1-botmar ) * ( ymax-ymin ) +ymin;
}

int kinemaPlotCls::ylabelMaxLen ( double y, double my )
{
	int ylen=0,ylenmax=0;
	for ( int iy=0;iy<ny;iy++ )
	{
		y+=my;
		if ( y>=ymax ) break;
		if ( fabs ( y ) <my/2. ) y=0.;
		ylen=QString::number ( y ).length();
		if ( ylenmax<ylen ) ylenmax=ylen;
	}
	return ylenmax;
}

void kinemaPlotCls::timerEvent ( QTimerEvent *e )
{
	killTimer ( timerid );
	timerid=-1;
	update();
}

void kinemaPlotCls::eraseCoord ( QPainter *p )
{
	QRect r ( 0,height()- ( fph+2 ), ( lendigitmax+1 ) * ( fpw+fpm ) +8,fph+2 );//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	p->fillRect ( r, QBrush ( "black" ) );
}

void kinemaPlotCls::saveas()
{
	QPixmap pm ( size() );
	bitBlt ( &pm, 0, 0, this );
	QPainter p ( &pm );
	eraseCoord ( &p );

	QString f= KFileDialog::getSaveFileName ( homedir,
	           "*.png|PNG image (*.png)\n*.jpg|JPG image (*.jpg)\n*.xpm|XPM image (*.xpm)\n*.*|All (*.*)",
	           this,
	           tr ( "Save plot as" ) );
	if ( f.length() <=0 ) return;
	if ( QFile::exists ( f ) )
	{
		KMessageBox::questionYesNo ( this,
		                             "File already exists:\n"+f+"\n Replace it?",
		                             "kinemaPlot: Warning!" );
	}

	QString fmt="PNG";
	if ( f.endsWith ( ".jpg" ) )
	{
		fmt="JPEG";
	}
	else if ( f.endsWith ( ".xpm" ) )
	{
		fmt="XPM";
	}

	if ( pm.save ( f,fmt ) )
	{
		messStr="Plot saved on "+f;
		messCol="skyblue";
	}
	else
	{
		messStr="Failed to save file: "+f;
		messCol="red";
	}
	timerid=startTimer ( messTime );
	update();
}

void kinemaPlotCls::mess ( QPainter *p )
{
	int x,y,wx,wy;
	int mar=4;
	x=width() /2- ( messStr.length() /2 ) * ( fpw+fpm );
	y=height() /2;
	wx=messStr.length() * ( fpw+fpm+1 );//<<<<<<<<<<<<<<<<<<<<<<
	wy=fph;
	p->fillRect ( x-mar,y-fph-mar,wx+mar*2,wy+mar*2,QBrush ( "black" ) );
	p->setFont ( thefont );
	p->setPen ( messCol );
	p->drawText ( x,y ,messStr );
}

void kinemaPlotCls::setMeasureGeom()
{
	if ( mx0!=0 )
	{
		mx=mx0;
		nx=floor ( ( xmax-xmin ) /mx0+0.5 );
	}
	else
	{
		if ( nx==0 ) nx=nx_def;
		mx=QString::number ( ( xmax-xmin ) /nx,'g',1 ).toDouble();
	}
	if ( my0!=0 )
	{
		my=my0;
		ny=floor ( ( ymax-ymin ) /my0+0.5 );
	}
	else
	{
		if ( ny==0 ) ny=ny_def;
		my=QString::number ( ( ymax-ymin ) /ny,'g',1 ).toDouble();
	}
}

void kinemaPlotCls::drawDots ( QPainter *p, int ixmin, int iymax, int shape )
{
	p->save();
	p->setBrush ( p->pen().color() );
	for ( int i=0;i<nplots;i++ )
	{
		int x=pa->point ( i ).x()-2;
		int y=pa->point ( i ).y()-2;
		if ( ixmin<=x&&x<width()-1&&0<y&&y<=iymax )
			p->drawEllipse ( x,y,5,5 );
	}
	p->restore();
}

void kinemaPlotCls::paintEvent ( QPaintEvent *e )
{
	QPixmap pm ( size() );
	QPainter p;
	p.begin ( &pm, this );
	pm.fill ( "black" );

	setMeasureGeom();

	int kx=xmin/mx;
	int ky=ymin/my;
	double x=kx*mx;
	double y=ky*my;

	int ylml=ylabelMaxLen ( y,my );

	int hs= ylml *fpw+ ( ylml-1 ) *fpm+fph;
	leftmar= ( hs ) *1.4;
	hs= ( leftmar-hs ) /2;
	int vs=fph*2;
	botmar= ( vs ) *1.4;
	vs= ( botmar-vs ) /2;
// the origin
	x0=getX ( xmin );
	y0=getY ( ymin );
	QRect plotRect;
	plotRect.setLeft ( x0 );
	plotRect.setRight ( width()-1 );
	plotRect.setTop ( 0 );
	plotRect.setBottom ( y0 );

// draw Title
	p.setPen ( "green" );
	p.setFont ( thefont );
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
	p.drawText ( getX ( ( xmin+xmax ) /2. )- ( xt.length() *fpw+ ( xt.length()-1 ) *fpm ) /2,y0+2* ( fph+vs ),xt );
	p.rotate ( -90. );
	// x decrease -> y increase, y increase -> x increase
	p.drawText ( - ( getY ( ( ymax+ymin ) /2. ) + ( yt.length() *fpw + ( yt.length()-1 ) *fpm ) /2 ),fph,yt ); //<<<<<<
	p.rotate ( 90. );

// draw measure
	int ix,iy;
	int px,py;
	x-=mx;//<<<<<<<<<<<<<
	y-=my;//<<<<<<<<<<<<<
	int y00=0;
	int x00=width()-1;
	if ( !msrOn )
	{
		x00=x0+10;//<<<<<<<<<<<<
		y00=y0-10;//<<<<<<<<<<<<
	}
	for ( ix=0;ix<nx*2;ix++ )
	{
		x+=mx;
		if ( x<xmin ) continue;
		if ( x>xmax ) break;
		px=getX ( x );
		p.setPen ( "darkgreen" );
		p.drawLine ( px,y0,px,y00 );
		p.setPen ( "green" );
		if ( fabs ( x ) <mx/2. ) x=0.;
		QString s=QString::number ( x );
		int l= ( s.length() *fpw+ ( s.length()-1 ) *fpm ) /2;
		p.drawText ( px-l,y0+fph+vs,s );
	}

	for ( iy=0;iy<ny*2;iy++ )
	{
		y+=my;
		if ( y<ymin ) continue;
		if ( y>ymax ) break;
		py=getY ( y );
		p.setPen ( "darkgreen" );
		p.drawLine ( x0,py,x00,py );
		p.setPen ( "green" );
		if ( fabs ( y ) <my/2. ) y=0.;
		QString s=QString::number ( y );
		int l=s.length();
		p.drawText ( x0- ( l*fpw+ ( l-1 ) *fpm+8 ),py+fph/2,s ); //<<<<<<<<<<<<<<<<<<<<<<<<<<
	}

	PenStyle const ps[]=
	{
		Qt::SolidLine,
		Qt::DotLine,
		Qt::SolidLine,
		Qt::DotLine
	};
	QColor const pc[]=
	{
		"white",
		"white",
		"skyblue",
		"skyblue"
	};
	QPen pline;
	p.setBrush ( Qt::SolidPattern );
	p.setClipRect ( plotRect );
	for ( iy=0;iy<nycol;iy++ )
	{
		setPlotPoints ( ycols[iy] );
		pline.setStyle ( ps[iy] );
		pline.setColor ( pc[iy] );
		p.setPen ( pline );
		p.drawPolyline ( *pa );
		if ( dotOn ) drawDots ( &p, x0, y0 );
//		p.drawPoints(*pa);
		delete pa;
	}
	if ( nycol>1&&showlegend )
	{
		int const lengend=48;
		int const hmar=10;
		int const vmar=10;
		int const lmar=8;
		int legendWidth=maxlenytitle*fpw+ ( maxlenytitle-1 ) *fpm+lengend+hmar*2+lmar;
		int legendHeight=nycol* ( fph+lmar )-lmar+vmar*2;
		QRect legendRect;
		legendRect.setLeft ( width()-1-legendWidth );
		legendRect.setRight ( width()-1 );
		legendRect.setTop ( 0 );
		legendRect.setBottom ( legendHeight );
		p.eraseRect ( legendRect );
		p.setPen ( "gray" );
		p.drawRect ( legendRect );
		for ( iy=0;iy<nycol;iy++ )
		{
			int yy=iy* ( fph+lmar ) +fph/2+vmar;
			int xx=legendRect.left() +hmar;
			pline.setStyle ( ps[iy] );
			pline.setColor ( pc[iy] );
			p.setPen ( pline );
			p.drawLine ( xx,yy,xx+lengend,yy );
			p.setPen ( "green" );
			p.drawText ( xx+lengend+lmar,yy+fph/2,
			             table->horizontalHeader()->label ( ycols[iy] ) );
		}
	}

	if ( timerid!=-1 )
	{
		mess ( &p );
	}

	// draw axis
	p.setPen ( "green" );
	// X
	p.drawLine ( x0,y0,width()-1,y0 );
	// Y
	p.drawLine ( x0,y0,x0,0 );

	p.end();
	bitBlt ( this, 0, 0, &pm );
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
	    "  cntl+s:     \tsave as png/jpg/xpm\n"
	    "  X,Y:        \tset dx/dy\n"
	    "  P:          \ttoggle stay on top\n"
	    "  R:          \trefresh\n"
	    "  F:          \ttoggle full screen\n"
	    "  M:          \ttoggle draw measure\n"
	    "  D:          \ttoggle draw dots\n"
	    "  Escape, Q:  exit";
	mboxCls *m;
	m=new mboxCls ( NULL,NULL,usage,"RelKinema-Plotter: Usage" );
	m->show();
}

void kinemaPlotCls::closeEvent ( QCloseEvent *e )
{
	emit done();
	delete this;
}

void kinemaPlotCls::mousePressEvent ( QMouseEvent *e )
{
	drag=1;
	drgpos0=e->pos();
	drgxmax0=xmax;
	drgxmin0=xmin;
	drgymax0=ymax;
	drgymin0=ymin;
	// the same as tegX and tegY
	drgdx= ( xmax-xmin ) / ( double ) ( width()-1-leftmar );
	drgdy=- ( ymax-ymin ) / ( double ) ( height()-1-botmar );

	QPainter p ( this );
	p.setFont ( thefont );
	p.setPen ( "yellow" );
	p.drawPoint ( e->x(),e->y() );
	QString pos=QString::number ( tegX ( e->x() ) ) +", "+QString::number ( tegY ( e->y() ) );
	p.drawText ( e->x() +4,e->y() +fph+2,pos );
}

void kinemaPlotCls::mouseReleaseEvent ( QMouseEvent *e )
{
	drag=0;
}

void kinemaPlotCls::getRootWindowSize ( int *w, int *h )
{
	*w=0;
	*h=0;
	Display *dpy = XOpenDisplay ( NULL );
	if ( dpy==NULL ) return; // cannot open display
	int screen=DefaultScreen ( dpy );
	Window root=RootWindow ( dpy,screen );
	*h=DisplayHeight ( dpy,screen );
	*w=DisplayWidth ( dpy,screen );
	XCloseDisplay ( dpy );
}

#include "kinemaplotcls.moc"
