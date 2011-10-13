/***************************************************************************
 *   Copyright (C) 2011 by kazuaki kumagai                                 *
 *   elseifkk@gmai.com                                                     *
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
#include <math.h>

#include "rkcalccls.h"

#include <qtextedit.h>
#include <qcheckbox.h>

#include <kcombobox.h>
#include <kconfig.h>

bool rkCalcCls::initRKC()
{
prpnc=fzc_init();
if(prpnc==0) return false;
pcstr= ( size_t ) &cstr[0];
loadConfig(*configfile);
return true;
}

void rkCalcCls::saveConfig(QString f)
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;
	KConfigGroup histGroup ( conf, "History" );
	histGroup.writeEntry ( "formula history",formulaBox->historyItems() );
	histGroup.writeEntry ( "formula completion",formulaBox->completionObject()->items() );
	histGroup.sync();
	delete conf;
}

void rkCalcCls::loadConfig(QString f)
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;
	KConfigGroup histGroup ( conf, "History" );
	formulaBox->setHistoryItems ( histGroup.readListEntry ( "formula history" ) );
	formulaBox->completionObject()->setItems ( histGroup.readListEntry ( "formula completion" ) );
	delete conf;
}

void rkCalcCls::mess ( QString m, QColor c, bool nl )
{
	ansBox->setColor ( c );
	ansBox->append ( m );
	if(nl) ansBox->append("\n");
}

void rkCalcCls::setParameter ( const double *v, QString s )
{
	strcpy ( cstr,s.latin1() );
	int rc=fzc_regpar ( prpnc, ( size_t ) v,pcstr );
}

void rkCalcCls::enterSlot ( void )
{
	int rc;
	QString str;

	if(prpnc==0) return; // <<<<

	if ( formulaBox->currentText().isEmpty() ) return;

	if ( autoClearBox->isChecked() ) clearSlot();
	mess ( formulaBox->currentText() +" =","black",false );

	strcpy ( cstr,formulaBox->currentText().latin1() );
	rc=fzc_set_formula ( pcstr,prpnc );
	if ( rc>0 )
	{
		mess ( "Syntacs Error","red" );
	}
	else if ( rc<0 )
	{
		mess ( "ok","blue" );
	}
	else
	{
		rc=fzc_eval ( prpnc );
		if ( rc!=0 )
		{
			mess ( "Eval Error","red" );
			return;
		}
		formulaBox->addToHistory(formulaBox->currentText());
		fzc_get_strans ( prpnc,pcstr );
		str=cstr;
		mess ( str );
	}
}

void rkCalcCls::clearSlot()
{
	ansBox->clear();
}

void rkCalcCls::keyPressEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Escape:
		case Qt::Key_Q:
			e->accept();
			close();
			break;
		default:
			e->ignore();
			break;
	}
}

void rkCalcCls::closeEvent ( QCloseEvent *e )
{
	e->accept();
	saveConfig(*configfile);
}