/***************************************************************************
 *   Copyright (C) 2011-2012 by Kazuaki Kumagai                            *
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
#include <stdio.h>
#include <math.h>

#include "rkcalccls.h"

#include <qtextedit.h>
#include <qcheckbox.h>
#include <qtextstream.h>

#include <kcombobox.h>
#include <kconfig.h>

bool rkCalcCls::initRKC()
{
	loadConfig ( *configfile );
	return true;
}

void rkCalcCls::saveConfig ( QString f )
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;
	KConfigGroup histGroup ( conf, "History" );
	histGroup.writeEntry ( "formula history",formulaBox->historyItems() );
	histGroup.writeEntry ( "formula completion",formulaBox->completionObject()->items() );
	histGroup.sync();
	delete conf;
}

void rkCalcCls::loadConfig ( QString f )
{
	KConfig *conf = new KConfig ( f );
	if ( conf==NULL ) return;
	KConfigGroup histGroup ( conf, "History" );
	formulaBox->setHistoryItems ( histGroup.readListEntry ( "formula history" ) );
	formulaBox->completionObject()->setItems ( histGroup.readListEntry ( "formula completion" ) );
	delete conf;
}

void rkCalcCls::mess ( QString m, QColor c, bool ap )
{
	ansBox->setColor ( c );
	if ( ap )
	{
		ansBox->insertAt ( m+"\n",nextpara,0 );
		nextpara++;
	}
	else
	{
		ansBox->insertAt ( m+"\n",0,0 );
		nextpara=1;
	}
}

void rkCalcCls::enterSlot ( void )
{
	int rc;
	QString str;
	char cstr[LEN_FZCSTR_MAX];
	size_t pcstr= ( size_t ) &cstr[0];

	if ( formulaBox->currentText().isEmpty() ) return;
	strcpy ( cstr,formulaBox->currentText().latin1() );

	if ( autoClearBox->isChecked() ) clearSlot();

	switch ( fzc_proc_com ( pfzc, pcstr ) )
	{
		case FZCCID_INV:
			mess ( "Invalid command\n","red", false );
			return;
		case FZCCID_DONE:
			mess ( "ok\n","blue", false );
			return;
	}

	mess ( QString(cstr) +" =","black", false );
	rc=fzc_set_formula ( pfzc, pcstr );
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
		rc=fzc_eval ( pfzc );
		if ( rc!=0 )
		{
			mess ( "Eval Error","red" );
		}
		else
		{
			formulaBox->addToHistory ( formulaBox->currentText() );
			fzc_get_strans ( pfzc, pcstr );
			str=cstr;
			mess ( str );
		}
	}
	mess ( "" );
}

void rkCalcCls::clearSlot()
{
	ansBox->clear();
	formulaBox->clear();
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

void rkCalcCls::clearHistSlot()
{
	formulaBox->clearHistory();
	formulaBox->completionObject()->clear();
}

void rkCalcCls::closeEvent ( QCloseEvent *e )
{
	e->accept();
	saveConfig ( *configfile );
}
