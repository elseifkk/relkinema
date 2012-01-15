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

#include "aboutcls.h"
#include "config.h"

#include <math.h>
#include <stdlib.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qimage.h>
#include <qprocess.h>

#include <kurllabel.h>

#define ENABLE_LOGO_FADEIN
#define ENABLE_FADEIN

#ifndef ENABLE_FADEIN
#undef ENABLE_LOGO_FADE_IN
#endif

int const countmax=200;
int const flashR=128;
int const flashG=128;
int const flashB=255;
int const timertick = 20; // ms
int const titleRoleSpeed = 4; // relative to timertick
int const fadeinDelayRatio = 5 ; // to count max

aboutcls::aboutcls ( QWidget *parent, const char *name, WFlags wf )
		:aboutdlg ( parent, name, wf )
{
	QString date=__DATE__;
	QString time=__TIME__;
	QString version="Version: ";
#ifdef VERSION
	version+=VERSION;
#else
	version+="unknown";
#endif
#ifdef __HOST__
	QString host=__HOST__;
#else
	QString host="unknown";
#endif
	BuildLbl->setText ( "Build: "+date+" "+time+"\n  @"+host );
	VersionLbl->setText ( version );
#ifdef ENABLE_FADEIN
	this->aboutLogoRect=this->aboutLogo->geometry();
	this->ny=this->aboutLogo->height();
	this->nx=this->aboutLogo->width();
	this->pmput=false;
	this->flash=false;
	this->drawing=false;
	titledone=false;
	count1=-1;
	count2=count1-countmax/fadeinDelayRatio;
	count3=count2-countmax/fadeinDelayRatio;
	count4=count3-countmax/fadeinDelayRatio*3;
	fcol=qRgb ( 240,240,240 );
	QColor bg=paletteBackgroundColor();
	TitleLbl->setPaletteForegroundColor ( bg );
	p1=TitleLbl->pos();
	VersionLbl->setPaletteForegroundColor ( bg );
	p2=VersionLbl->pos();
	BuildLbl->setPaletteForegroundColor ( bg );
	p3=BuildLbl->pos();
	CopyleftLbl->setPaletteForegroundColor ( bg );
	p4=CopyleftLbl->pos();
	this->img0 = new QImage ( aboutLogo->pixmap()->convertToImage() );
	this->img = new QImage ( img0->smoothScale ( this->nx,this->ny ) );
#ifdef ENABLE_LOGO_FADEIN
	this->filled= ( int * ) calloc ( this->ny,sizeof ( int ) );
	for ( int i=0;i<ny;i++ )
	{
		filled[i]=i;
	}
	for ( int i=ny-1;i>=2;i-- )
	{
		int j=random() %this->ny;
		if ( j==i ) continue;
		filled[i]^=filled[j];
		filled[j]^=filled[i];
		filled[i]^=filled[j];
	}
	this->fillcount=0;
	this->aboutLogo->hide();
#endif
	tid=startTimer ( timertick );
#endif
}

void aboutcls::urlSlot()
{
	QProcess p;
	p.addArgument ( wb );
	p.addArgument ( urlLbl->text() );
	p.start();
}

void aboutcls::paintEvent ( QPaintEvent *e )
{
#ifdef ENABLE_LOGO_FADEIN
	if ( this->pmput )
	{
		QPainter p ( this );
		p.drawImage ( this->aboutLogoRect,*this->img );
	}
#endif
}

QColor aboutcls::setFgCol ( double z )
{
	int r=fcol.red() * z;
	int g=fcol.green() * z;
	int b=fcol.blue() * z;
	if ( r>255 ) r=255;  if ( g>255 ) g=255;  if ( b>255 ) b=255;
	return qRgb ( r,g,b );
}

void aboutcls::timerEvent ( QTimerEvent *e )
{
	int r,g,b;
	float z;
	float zz;
	QPoint p;

	tveto.lock();

	count1++;
	count2++;
	count3++;
	count4++;

#ifdef ENABLE_LOGO_FADEIN
	if ( !this->drawing && fillcount<this->ny )
	{
		this->drawing=true;
		QPainter pp ( this );
		int iy=filled[fillcount];
		pp.setRasterOp ( Qt::CopyROP );
		pp.drawImage ( this->aboutLogoRect.x(),this->aboutLogoRect.y() +iy,*this->img,0,iy,this->nx,1 );
		this->drawing=false;
		fillcount++;
	}

	else if ( !this->pmput )
	{
		this->pmput=true;
		paintEvent ( 0 );
	}
#endif
	if ( !titledone )
	{
		z= ( float ) count1/ ( float ) countmax;
		zz=z*4;
		if ( this->flash )
		{
			z=z-z0;
			if ( z>0.5 ) z=1-z;
			if ( z<0 )
			{
				z=0;
				titledone=true;
			}
			r=flashR* ( 1-z ); g=flashG* ( 1-z ); b=flashB* ( 1-z );
		}
		else
		{
			if ( zz>=1 )
			{
				r=flashR;g=flashG;b=flashB; // flash
				zz=1;
				z0=z;
				this->flash=true;
			}
			else
			{
				r=fcol.red() * ( 1-zz );  g=fcol.green() * ( 1-zz );  b=fcol.blue() * ( 1-zz );

			}
		}
		if ( !this->flash )
		{
			p.setY ( 10 );
			p.setX ( 400+zz* ( 20-400 ) );
			TitleLbl->move ( p );
		}
		TitleLbl->setPaletteForegroundColor ( qRgb ( r,g,b ) );
	}
	if ( count4>=0&&count4<=countmax )
	{
		z= ( float ) count4/ ( float ) countmax;
		VersionLbl->setPaletteForegroundColor ( setFgCol ( z ) );
	}
	if ( count3>=0&&count3<=countmax )
	{
		z= ( float ) count3/ ( float ) countmax;
		BuildLbl->setPaletteForegroundColor ( setFgCol ( z ) );
		p.setX ( 220 );
		p.setY ( 80+z* ( 110-80 ) );
		BuildLbl->move ( p );
	}
	if ( count2>=0&&count2<=countmax )
	{
		z= ( float ) count2/ ( float ) countmax;
		CopyleftLbl->setPaletteForegroundColor ( setFgCol ( z ) );
	}
	if ( count4>countmax ) killTimer ( tid );
	
	tveto.unlock();
}

void aboutcls::closeSlot()
{
#ifdef ENABLE_FADEIN
	killTimer ( tid );
	free ( filled );
	delete img;
	delete img0;
#endif
	done ( 0 );
}

void aboutcls::keyPressEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Q:
		case Qt::Key_Escape:
			closeSlot();
			break;
	}
}

#include "aboutcls.moc"
