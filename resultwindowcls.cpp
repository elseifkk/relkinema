/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
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
#include <stdlib.h>

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
#include <qtimer.h>

#include <kfiledialog.h>
#include <kmessagebox.h>

resultWindowCls::resultWindowCls ( QWidget *parent, const char *name, WFlags wf, int plotmask, bool *ext_in, KHistoryCombo **exprBox, int col_first, int nr, QString el, QString al, QString pl )
		:resultWindow ( parent, name, wf )
{
	this->plotmask=plotmask;
	msgLbl->clear();
	nplots=0;
	timerid=0;
	ncolext=0;
	ncol=0;
	QString plotLbl[ncolmax]=
	{
		"Theta3LAB","Theta3CM","q","K3","p3","do3CM/do3LAB","dK3/dTheta3LAB","1/p3/(dp3/dTheta3LAB)",
		"Theta4LAB","Theta4CM","K4","p4","do4CM/do4LAB"
	};
	QString uLbl[ncolmax]={al,al,"1/fm",el,pl,"",el+"/"+al,"1/"+al,al,al,el,pl,""};

	switch ( col_first )
	{
		case 1:
			plotLbl[0]="Theta3CM";
			plotLbl[1]="Theta3LAB";
			plotLbl[2]="q";
			break;
		case 2:
			plotLbl[0]="q";
			plotLbl[1]="Theta3LAB";
			plotLbl[2]="Theta3CM";
			uLbl[0]="1/fm";
			uLbl[2]=al;
			break;
	}

	for ( int ic=0;ic<ncolmax;ic++ )
	{
		if ( BT ( plotmask, ic ) )
		{
			resultTable->insertColumns ( ncol );
			resultTable->horizontalHeader()->setLabel ( ncol,plotLbl[ic] );
			ncol++;
		}
	}
	if ( ext!=0 )
	{
		memcpy ( &ext,ext_in,sizeof ( bool ) *4 );
		int i;
		for ( i=0;i<nexpmax;i++ )
		{
			if ( !ext[i] ) continue;
			ncolext++;
		}
		if ( ncolext!=0 )
		{
			resultTable->setNumCols ( ncol+ncolext );
			for ( i=0;i<nexpmax;i++ )
			{
				if ( !ext[i] ) continue;
				resultTable->horizontalHeader()->setLabel ( ncol+i,exprBox[i]->currentText() );
			}
		}
	}

	resultTable->setNumRows ( nrow_hdr ); //<<<<<<<<<<<<<
	nrow=nr+nrow_hdr;

	int k=0;
	for ( int ic=0;ic<ncolmax;ic++ )
	{
		if ( BT ( plotmask, ic ) )
		{
			resultTable->setText ( 1,k,uLbl[ic] );
			k++;
		}
	}
	for ( int ic=0;ic<nexpmax;ic++ )
	{
		if ( !ext[ic] ) continue;
		resultTable->setText ( 1,ncol+ic,"N/A" );
	}
	resultTable->verticalHeader()->setLabel ( 0,"plot" );
	resultTable->verticalHeader()->setLabel ( 1,"unit" );

	connect ( resultTable->verticalHeader(), SIGNAL ( sectionClicked ( int ) ), this, SLOT ( clearPlotSelect ( int ) ) );
}

void resultWindowCls::startPoll ( void )
{
	initCTI();
	nrmax=5000+nrow_hdr;
	timerid=startTimer ( 200 );//<<<<<<<<<<<<
}

void resultWindowCls::setTable()
{
	QStringList::Iterator it=sl->begin();
	resultTable->setNumRows ( nrow>nrmax ? nrmax: nrow );
	int ir=nrow_hdr-1;
	for ( ;; )
	{
		ir++;
		resultTable->verticalHeader()->setLabel ( ir,QString::number ( ir-nrow_hdr+1 ) );
		for ( int ic=0;ic<ncol+ncolext;ic++ )
		{
			resultTable->setText ( ir,ic,* ( it++ ) );
		}
		if ( ir+1 == nrow ) break;
		if ( ir+1 == nrmax )
		{
			KGuiItem yes ( "Yes for all" );
			KGuiItem no ( "Yes" );
			switch ( KMessageBox::questionYesNoCancel ( this,
			         "Too Large Table!\n"
			         +QString::number ( ir-nrow_hdr+1 ) +" of "+QString::number ( nrow-nrow_hdr )
			         +"("+QString::number ( float ( ir-nrow_hdr+1 ) /float ( nrow-nrow_hdr ) *100. ) +"%) read\n"
			         +"Continue?",
			         "kinemaTable: Warning!",yes,no ) )
			{
				case KMessageBox::Yes:
					resultTable->setNumRows ( nrow );
					break;
				case KMessageBox::No:
					nrmax*=2; //<<<<<<<<<<<<
					resultTable->setNumRows ( nrow>nrmax ? nrmax: nrow );
					break;
				default:
					nrow=nrmax;
					goto done;
			}
		}
	}
done:
	adjTable();
	msgLbl->setPaletteForegroundColor ( "Green" );
	msgLbl->setText ( "<b> Ready" );
	emit done();
}

void resultWindowCls::timerEvent ( QTimerEvent *e )
{
	if ( *ndone>=0 )
	{
		msgLbl->setPaletteForegroundColor ( "blue" );
		msgLbl->setText ( "<b>Calculating... "
		                  +QString::number ( *ndone ) +" of "+QString::number ( nrow-nrow_hdr ) +" done." );
		return;
	}
	killTimer ( timerid );
	msgLbl->setText ( "<b> Loading table... Please wait" );
	setTable();
}

void resultWindowCls::initResultDescBox ( QString reaction, double incidentEnergy, double incidentMomentum, double Ex, double QValue, double beta, double gamma, double m1, double m2, double m3, double m4, QString fmt )
{
	QString s,t;
	resultDescBox->clear();
	setCaption ( "Results: "+reaction+", incident "+QString::number ( incidentEnergy ) +" MeV" );
	plotLbl=reaction+" K1="+s.sprintf ( fmt, incidentEnergy );
	plotLbl+=" MeV Ex="+s.sprintf ( fmt, Ex ) +" MeV";
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
	pal.setColor ( QColorGroup::ButtonText,"blue" );
	resultTable->setPalette ( pal );
	resultTable->horizontalHeader()->setPalette ( palorig );
	resultTable->verticalHeader()->setPalette ( palorig );
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		cti[ic] = new QComboTableItem ( resultTable,NULL );
		cti[ic]->setStringList ( s );
		resultTable->setItem ( 0,ic,cti[ic] );
	}
	for ( int ir=1;ir<nrow;ir++ )
	{
		resultTable->setRowReadOnly ( ir,true );
	}
}

void resultWindowCls::saveasSlot()
{
	QString f= KFileDialog::getSaveFileName ( *homedir,
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
	if ( file.exists() )
	{
		if ( KMessageBox::questionYesNo ( this,
		                                  "File already exists:\n"+f+"\n Replace it?",
		                                  "kinemaTable: Warning!" )
		        !=KMessageBox::Yes ) return;
	}

	if ( file.open ( IO_WriteOnly ) )
	{
		QTextStream stream ( &file );

		stream << "# " <<resultDescBox->text().replace ( "\n","\n# " );
		stream << "\n#\n#";

		int ic;  // section no

		for ( ic=0;ic<ncol;ic++ )
		{
			if ( ic!=0 ) stream << delim;
			stream << resultTable->horizontalHeader()->label ( ic );
		}
		stream << "\n#";
		for ( int ir=1;ir<nrow;ir++ ) //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		{
			for ( ic=0;ic<ncol;ic++ )
			{
				if ( ic!=0 ) stream << delim;
				stream << resultTable->text ( ir,ic );
			}
			stream << "\n";
		}
		file.close();
		msgLbl->setPaletteForegroundColor ( "blue" );
		msgLbl->setText ( "<b> saved as "+f );
	}
	else
	{
		QMessageBox::critical ( this, "relkinema: ", "Cannot open file!\n"+f, tr ( "&ok" ) );
		msgLbl->setPaletteForegroundColor ( "red" );
		msgLbl->setText ( "<b> saving "+f+" failed!" );
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
	QComboTableItem *c;

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
		KMessageBox::sorry ( this, "No x-data selected","error" );
		return;
	}
	switch ( k )
	{
		case 0:
			c= ( QComboTableItem* ) resultTable->item ( 0,0 );
			c->setCurrentItem ( 1 );
			c= ( QComboTableItem* ) resultTable->item ( 0,1 );
			c->setCurrentItem ( 2 );
			cols[0]=0;
			cols[1]=1;
			k=2;
			break;
		case 1:
			c= ( QComboTableItem* ) resultTable->item ( 0,0 );
			c->setCurrentItem ( 2 );
			cols[1]=0;
			k=2;
			break;
	}
	kinemaPlotCls *win = new kinemaPlotCls ( NULL,NULL, Qt::WDestructiveClose, resultTable,
	        homedir, &plotLbl, cols[0], k-1, &cols[1], nrow_hdr, font() );
	win->resize ( plot_width_def, plot_height_def );
	win->setCaption ( this->caption().replace ( "Results:","Plots: " ) );
	connect ( win,SIGNAL ( done() ),this,SLOT ( plotDone() ) );
	nplots++;
	exitBut->setEnabled ( false );
	win->show();
}

void resultWindowCls::changeFontSize ( int d )
{
	int const min_pointsize=4;
	int const max_pointsize=32;
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
		for ( int ic=0;ic<resultTable->numCols();ic++ )
		{
			delete cti[ic];
		}
		e->accept();
	}
	else
	{
		e->ignore();
	}
}

int resultWindowCls::countCTI ( int i, int *liid )
{
	QComboTableItem *c;
	int s=0;
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		c= ( QComboTableItem* ) resultTable->item ( 0,ic );
		if ( c->currentItem() ==i )
		{
			s++;
			if ( liid!=NULL ) *liid=ic;
		}
	}
	return s;
}

void resultWindowCls::clearPlotSelect ( int s )
{
/*	if ( s/=0 ) return;
	for ( int ic=0;ic<resultTable->numCols();ic++ )
	{
		QComboTableItem *c;
		c= ( QComboTableItem* ) resultTable->item ( 0,ic );
 		c->setCurrentItem ( 0 );
	}*/
}


void resultWindowCls::rtSlot ( int r, int c )
{
}

#include "resultwindowcls.moc"
