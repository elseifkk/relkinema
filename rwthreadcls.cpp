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

#include "rwthreadcls.h"

#include <qscrollbar.h>
#include <qtable.h>

void rwThreadCls::run()
{
	rk->thetaBar->blockSignals ( true );
	int ic=rk->thetaBar->value();
	QString s;
	int k=0;
	for ( int ir=irmin;ir<=irmax;ir++ )
	{
		k++;
		rk->thetaBar->setValue ( ir );
		rk->thetaBarSlot();
		rw->resultTable->setText ( k,col_th3,  s.sprintf ( fmt, rk->th3*au ) );
		rw->resultTable->setText ( k,col_th3c, s.sprintf ( fmt, rk->th3c*au ) );
		rw->resultTable->setText ( k,col_q,    s.sprintf ( fmt, rk->theq ) );
		rw->resultTable->setText ( k,col_K3,   s.sprintf ( fmt, rk->K3*eu ) );
		rw->resultTable->setText ( k,col_p3,   s.sprintf ( fmt, rk->p3*eu ) );
		rw->resultTable->setText ( k,col_J3,   s.sprintf ( fmt, rk->J3 ) );
		rw->resultTable->setText ( k,col_ks,   s.sprintf ( fmt, rk->kinemaShift ) );
		rw->resultTable->setText ( k,col_th4,  s.sprintf ( fmt, rk->th4*au ) );
		rw->resultTable->setText ( k,col_th4c, s.sprintf ( fmt, rk->th4c*au ) );
		rw->resultTable->setText ( k,col_K4,   s.sprintf ( fmt, rk->K4*eu ) );
		rw->resultTable->setText ( k,col_p4,   s.sprintf ( fmt, rk->p4*eu ) );
		rw->resultTable->setText ( k,col_J4,   s.sprintf ( fmt, rk->J4 ) );
		rw->resultTable->setText ( k,col_fk,   s.sprintf ( fmt, rk->factorK ) );
		rw->resultTable->verticalHeader()->setLabel ( k,QString::number ( ir ) );
	}
	if ( col_first!=0 ) rw->resultTable->swapColumns ( 0,col_first,true );
	rk->thetaBar->blockSignals ( false );
	rk->thetaBar->setValue ( ic );
	emit done ( rw );
}


#include "rwthreadcls.moc"