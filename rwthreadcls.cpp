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
	rk->thetaBar->blockSignals(true);
	int ic=rk->thetaBar->value();
	for ( int ir=1;ir<=irmax;ir++ )
	{
		rk->thetaBar->setValue ( ir-1 );
		rk->changeTheta();
		rw->resultTable->setText ( ir,col_th3,  QString::number ( rk->th3*au ) );
		rw->resultTable->setText ( ir,col_th3c, QString::number ( rk->th3c*au ) );
		rw->resultTable->setText ( ir,col_q,    QString::number ( rk->theq ) );
		rw->resultTable->setText ( ir,col_K3,   QString::number ( rk->K3*eu ) );
		rw->resultTable->setText ( ir,col_p3,   QString::number ( rk->p3*eu ) );
		rw->resultTable->setText ( ir,col_J3,   QString::number ( rk->J3 ) );
		rw->resultTable->setText ( ir,col_ks,   QString::number ( rk->kinemaShift ) );
		rw->resultTable->setText ( ir,col_th4,  QString::number ( rk->th4*au ) );
		rw->resultTable->setText ( ir,col_th4c, QString::number ( rk->th4c*au ) );
		rw->resultTable->setText ( ir,col_K4,   QString::number ( rk->K4*eu ) );
		rw->resultTable->setText ( ir,col_p4,   QString::number ( rk->p4*eu ) );
		rw->resultTable->setText ( ir,col_J4,   QString::number ( rk->J4 ) );
		rw->resultTable->setText ( ir,col_fk,   QString::number ( rk->factorK ) );
		rw->resultTable->verticalHeader()->setLabel ( ir,QString::number ( ir ) );
	}
	if(col_first!=0) rw->resultTable->swapColumns(0,col_first,true);
	rk->thetaBar->blockSignals(false);
	rk->thetaBar->setValue ( ic );
	emit done(rw);
}


#include "rwthreadcls.moc"