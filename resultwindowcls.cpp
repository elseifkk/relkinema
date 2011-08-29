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
#include "resultwindowcls.h"
#include "kinemaplotcls.h"

#include <qtextedit.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtable.h>

#include <kfiledialog.h>
#include <kmessagebox.h>

int const plot_width_def  = 600;
int const plot_height_def = 400;
int const min_pointsize=4;
int const max_pointsize=32;

resultWindowCls::resultWindowCls ( QWidget *parent, const char *name, WFlags wf )
		:resultWindow ( parent, name, wf )
{
	msgLbl->clear();
	nplots=0;
}

void resultWindowCls::initResultDescBox ( QString reaction, double incidentEnergy, double incidentMomentum, double Ex, double QValue, double beta, double gamma, double m1, double m2, double m3, double m4, QString fmt )
{
	QString s,t;
	resultDescBox->clear();
	setCaption ( "Results: "+reaction+", incident "+QString::number ( incidentEnergy ) +" MeV" );
	resultDescBox->append ( "Reaction: "+reaction );
	resultDescBox->append ( "Mass: M2(M1, M3)M4" );
	t=s.sprintf ( fmt,m2 ) +" ( ";
	t+=s.sprintf ( fmt,m1 ) +", ";
	t+=s.sprintf ( fmt,m3 ) +" ) ";
	t+=s.sprintf ( fmt,m4 );
	resultDescBox->append ( t );
	resultDescBox->append ( "Incident Energy: "+s.sprintf ( fmt, incidentEnergy ) +" MeV" );
	resultDescBox->append ( "Incident Momentum: "+s.sprintf ( fmt, incidentMomentum ) +" MeV/c" );
	resultDescBox->append ( "Excitation Energy: "+s.sprintf ( fmt, Ex ) +" MeV" );
	resultDescBox->append ( "Q Value: "+s.sprintf ( fmt, QValue ) +" MeV" );
	resultDescBox->append ( "beta: "+s.sprintf ( fmt, beta ) );
	resultDescBox->append ( "1/gamma: "+s.sprintf ( fmt, 1./gamma ) );
	resultDescBox->append ( "gamma*beta: "+s.sprintf ( fmt, gamma*beta ) );
}

void resultWindowCls::initCTI()
{
	QStringList s;
	s.append ( "" );
	s.append ( "X" );
	s.append ( "Y" );
	QPalette pal=resultTable->palette();
	QPalette palorig=pal;
	pal.setColor(QColorGroup::ButtonText,"blue");
	resultTable->setPalette(pal);
	resultTable->horizontalHeader()->setPalette(palorig);
	resultTable->verticalHeader()->setPalette(palorig);
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		cti[ic] = new QComboTableItem ( resultTable,NULL );
		cti[ic]->setStringList ( s );
		resultTable->setItem ( 0,ic,cti[ic] );
	}
	for ( int ir=1;ir<resultTable->numRows();ir++ )
	{
		resultTable->setRowReadOnly ( ir,true );
	}
}

void resultWindowCls::saveasSlot()
{
	int const ncol=resultTable->numCols();
	int const nrow=resultTable->numRows();

	QString f= KFileDialog::getSaveFileName ( homedir,
	           "*.csv|text file(*.csv)\n*.txt|text file(*.txt)\n*.*|All(*.*)",
	           this,
	           tr ( "Save result as" ) );
	if ( f.length() <=0 ) return;

	QString delim;
	if ( f.endsWith ( ".csv" ) )
	{
		delim=",";
	}
	else
	{
		delim=" ";
	}
	QFile file ( f );
	if ( file.open ( IO_WriteOnly ) )
	{
		QTextStream stream ( &file );

		stream << "# " <<resultDescBox->text().replace ( "\n","\n# " );
		stream << "\n#\n#";

		int ic;  // section no
		int iic; // column

		for ( ic=0;ic<ncol;ic++ )
		{
			if ( ic!=0 ) stream << delim;
			iic=resultTable->horizontalHeader()->mapToSection ( ic );
			stream << resultTable->horizontalHeader()->label ( iic );
		}
		stream << "\n#";
		for ( int ir=1;ir<nrow;ir++ ) //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		{
			for ( ic=0;ic<ncol;ic++ )
			{
				if ( ic!=0 ) stream << delim;
				iic=resultTable->horizontalHeader()->mapToSection ( ic );
				stream << resultTable->text ( ir,ic );
			}
			stream << "\n";
		}
		file.close();
		msgLbl->setPaletteForegroundColor ( "blue" );
		msgLbl->setText ( "saved as "+f );
	}
	else
	{
		QMessageBox::critical ( this, "relkinema: ", "Cannot open file!\n"+f, tr ( "&ok" ) );
		msgLbl->setPaletteForegroundColor ( "red" );
		msgLbl->setText ( "saving "+f+" failed!" );
	}
}

void resultWindowCls::keyPressEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Q:
		case Qt::Key_Escape:
			close();
			break;
		case Qt::Key_P:
			plotSlot();
			break;
		case Qt::Key_S:
			saveasSlot();
			break;
		case Qt::Key_Plus:
			changeFontSize ( 1 );
			break;
		case Qt::Key_Minus:
			changeFontSize ( -1 );
			break;
		case Qt::Key_F:
			if ( resultDescBox->isVisible() )
			{
				resultDescBox->hide();
				hideDescBut->setOn ( true );
			}
			else
			{
				resultDescBox->show();
				hideDescBut->setOn ( false );
			}
			break;
	}
}

void resultWindowCls::plotSlot()
{
	int cols[resultTable->numCols() ];
	int k=0;
	int xc,yc;
	int px=-1;

	xc=countCTI ( 1,&px );
	yc=countCTI ( 2 );
	if ( xc>1 )
	{
		KMessageBox::sorry ( this, "More than one column selected for x-data","error" );
		return;
	}
	if ( xc==1 )
	{
		k++;
		cols[0]=px;
	}
	if ( yc==0 )
	{
		for ( int ic=0;ic<resultTable->numCols();ic++ )
		{
			if ( ic==px ) continue;
			if ( resultTable->isColumnSelected ( ic,true ) )
			{
				QComboTableItem *c;
				int ci;
				if ( k==0 )
				{
					ci=1;
				}
				else
				{
					ci=2;
				}
				c= ( QComboTableItem* ) resultTable->item ( 0,ic );
				c->setCurrentItem ( ci );
				cols[k]=ic;
				k++;
			}
		}
	}
	else if ( xc==1 )
	{
		for ( int ic=0;ic<resultTable->numCols();ic++ )
		{
			QComboTableItem *c;
			c= ( QComboTableItem* ) resultTable->item ( 0,ic );
			if ( c->currentItem() ==2 )
			{
				cols[k]=ic;
				k++;
			}
		}
	}
	else
	{
		KMessageBox::sorry ( this, "No y-data selected","error" );
		return;
	}
	switch ( k )
	{
		case 0:
			cols[0]=0;
			cols[1]=1;
			k=2;
			break;
		case 1:
			cols[1]=0;
			k=2;
			break;
	}
	kinemaPlotCls *win = new kinemaPlotCls ( NULL,NULL, Qt::WDestructiveClose );
	win->resize ( plot_width_def, plot_height_def );
	win->setCaption ( this->caption().replace ( "Results:","Plots: " ) );
	win->table=resultTable;
	win->xcol=resultTable->horizontalHeader()->mapToSection ( cols[0] );
	win->ycol=resultTable->horizontalHeader()->mapToSection ( cols[1] );
	win->nycol=k-1;
	for ( int iy=1;iy<=k-1;iy++ )
	{
		win->ycols[iy-1]=cols[iy];
	}
	win->setPlotMinMax();
	win->homedir=homedir;
	connect ( win,SIGNAL ( done() ),this,SLOT ( plotDone() ) );
	nplots++;
	exitBut->setEnabled ( false );
	win->show();
}

void resultWindowCls::changeFontSize ( int d )
{
	QFont f=font();
	int p=f.pointSize();
	p+=d;
	if ( p<min_pointsize )
	{
		p=min_pointsize;
	}
	else if ( p>max_pointsize )
	{
		p=max_pointsize;
	}
	f.setPointSize ( p );
	setFont ( f );
	adjTable();
}

void resultWindowCls::adjTable()
{
	for ( int i=0;i<resultTable->numRows();i++ )
	{
		resultTable->adjustRow ( i );
	}
	for ( int i=0;i<resultTable->numCols();i++ )
	{
		resultTable->adjustColumn ( i );
	}
	QFontMetrics fm ( font() );
	int l=fm.width ( ( const QString& ) QString::number ( resultTable->numRows() ) );
	int l2=fm.width ( ( const QString& ) QString ( "unit" ) );
	if ( l<l2 ) l=l2;
	l= ( double ) l*1.4;//<<<<<<<<<<<<<<
	resultTable->setLeftMargin ( l );
	resultTable->verticalHeader()->setFixedWidth ( l );
	int h=resultTable->rowHeight ( 0 );
	resultTable->setTopMargin ( h );
	resultTable->horizontalHeader()->setFixedHeight ( h );
}

void resultWindowCls::redSlot()
{
	changeFontSize ( -1 );
}

void resultWindowCls::magSlot()
{
	changeFontSize ( 1 );
}

void resultWindowCls::hideDescSlot()
{
	if ( hideDescBut->isOn() )
	{
		resultDescBox->hide();
	}
	else
	{
		resultDescBox->show();
	}
}

void resultWindowCls::plotDone()
{
	nplots--;
	if ( nplots==0 ) exitBut->setEnabled ( true );
}

void resultWindowCls::closeEvent ( QCloseEvent *e )
{
	if ( nplots==0 )
	{
		e->accept();
	}
	else
	{
		e->ignore();
	}
}

int resultWindowCls::countCTI ( int i, int *liid )
{
	int s=0;
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		if ( cti[ic]->currentItem() ==i )
		{
			s++;
			if ( liid!=NULL ) *liid=ic;
		}
	}
	return s;
}

void resultWindowCls::rtSlot ( int r, int c )
{
}

#include "resultwindowcls.moc"
