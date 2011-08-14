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
#include <kfiledialog.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtable.h>

int const plot_width_def  = 600;
int const plot_height_def = 400;
int const min_pointsize=4;
int const max_pointsize=32;

resultWindowCls::resultWindowCls ( QWidget *parent, const char *name )
		:resultWindow ( parent, name )
{
	msgLbl->clear();
	nplots=0;
}

void resultWindowCls::initResultDescBox ( QString reaction, double incidentEnergy, double incidentMomentum, double Ex, double QValue, double beta, double gamma, double m1, double m2, double m3, double m4 )
{
	resultDescBox->clear();
	this->setCaption ( "Results: "+reaction+", incident "+QString::number ( incidentEnergy ) +" MeV" );
	resultDescBox->append ( "Reaction: "+reaction );
	resultDescBox->append ( "Mass: M2(M1, M3)M4" );
	resultDescBox->append ( QString::number ( m2 ) +" ( "+QString::number ( m1 ) +", "+QString::number ( m3 ) +" ) "+QString::number ( m4 ) );
	resultDescBox->append ( "Incident Energy: "+QString::number ( incidentEnergy ) +" MeV" );
	resultDescBox->append ( "Incident Momentum: "+QString::number ( incidentMomentum ) +" MeV/c" );
	resultDescBox->append ( "Excitation Energy: "+QString::number ( Ex ) +" MeV" );
	resultDescBox->append ( "Q Value: "+QString::number ( QValue ) +" MeV" );
	resultDescBox->append ( "beta: "+QString::number ( beta ) );
	resultDescBox->append ( "1/gamma: "+QString::number ( 1./gamma ) );
	resultDescBox->append ( "gamma*beta: "+QString::number ( gamma*beta ) );
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
		for ( int ir=0;ir<nrow;ir++ )
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
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		if ( resultTable->isColumnSelected ( ic,true ) )
		{
			if ( k>=2 )
			{
				if ( resultTable->text ( 0,cols[1] ) !=resultTable->text ( 0,ic ) ) continue;
			}
			cols[k]=ic;
			k++;
		}
	}
	if ( k<2 )
	{
		cols[0]=0;
		cols[1]=1;
		k=2;
	}
	kinemaPlotCls *win = new kinemaPlotCls();
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
	if ( nplots==0 ) delete this;
}

#include "resultwindowcls.moc"
