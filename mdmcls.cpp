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
#include "mdmcls.h"
#include "config.h"
#include <stdlib.h>

#include <qlistbox.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qwidgetstack.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qprocess.h>

#include <kfiledialog.h>
#include <kurllabel.h>

int const MU_EV  = 0;
int const MU_KEV = 1;
int const MU_MEV = 2;
int const MU_GEV = 3;
int const MU_AMU = 4;

mdmCls::mdmCls ( QWidget *parent, const char *name, WFlags wf )
		:massDataManager ( parent, name, wf )
{
	appBut->setEnabled ( false );
	discardOk=0;
	currentA=0;
	currentFile="";
	currentFileExists=false;
	timer=0;
	blink=false;
	QPalette pal = EBox->palette();
	pal.setColor ( QColorGroup::Highlight, qRgb ( 205,234,255 ) );
	pal.setColor ( QColorGroup::HighlightedText, qRgb ( 0,71,171 ) );
	EBox->setPalette ( pal );
}

void mdmCls::openMassData ( int A )
{
	QString f;
	f=massdata+"/A";
	if ( A!=0 )
	{
		f+=QString::number ( A );
	}
	f+=".dat";
	EBox->clear();

	QStringList EList;
	EList.clear();
	MList.clear();
	currentFile=f;

	QFile file ( f );
	if ( file.open ( IO_ReadOnly ) )
	{
		QTextStream stream ( &file );
		QString line;
		while ( !stream.atEnd() )
		{
			line = stream.readLine().simplifyWhiteSpace();
			EList += line.section ( " ",0,0 );
			MList += line.section ( " ",1,1 );
		}
		file.close();
		setReady ( true );
		EBox->insertStringList ( EList );
		mess ( "Data file opened: "+f );
		currentFileExists=true;
	}
	else
	{
		currentFileExists=false;
		mess ( "Failed to open data file:"+f,"red" );
		setReady ( false );
	}
}

double mdmCls::readMass ( double m )
{
	if ( ABox->value() ==0 )
	{
// return in MeV
		switch ( MUnitBox->currentItem() )
		{
			case MU_EV:
				return m*1.0e-6;
				break;
			case MU_KEV:
				return m*1.0e-3;
				break;
			case MU_MEV:
				return m;
				break;
			case MU_GEV:
				return m*1.0e3;
				break;
			case MU_AMU:
				return m*AMU;
				break;
		}
	}
	else
	{

// return in keV Excess
		if ( showExcessBox->isChecked() )
		{
			switch ( MUnitBox->currentItem() )
			{
				case MU_EV:
					return m*1.0e-3;
					break;
				case MU_KEV:
					return m;
					break;
				case MU_MEV:
					return m*1.0e3;
					break;
				case MU_GEV:
					return m*1.0e6;
					break;
				case MU_AMU:
					return m*AMU*1.0e3;
					break;
			}
		}
		else
		{
			double m0=AMU*ABox->value() *1000.; // in keV
			switch ( MUnitBox->currentItem() )
			{
				case MU_EV:
					return m*1.0e-3-m0;
					break;
				case MU_KEV:
					return m-m0;
					break;
				case MU_MEV:
					return m*1.0e3-m0;
					break;
				case MU_GEV:
					return m*1.0e6-m0;
					break;
				case MU_AMU:
					return m*AMU*1.0e3-m0;
					break;
			}
		}
	}
	return 0;
}

void mdmCls::showMass ( QString m )
{
	bool ok;
	double mass=m.toDouble ( &ok );
	double m0;

	if ( !ok ) return;

	if ( ABox->value() ==0 )
	{
// m is in MeV
		switch ( MUnitBox->currentItem() )
		{
			case MU_EV:
				mass*=1000000.;
				break;
			case MU_KEV:
				mass*=1000.;
				break;
			case MU_MEV:
				break;
			case MU_GEV:
				mass*=1./1000.;
				break;
			case MU_AMU:
				mass*=1./AMU;
				break;
		}
	}
	else
	{
// m is in keV Excess
		if ( showExcessBox->isChecked() )
		{

			switch ( MUnitBox->currentItem() )
			{
				case MU_EV:
					mass=mass*1.0e3;
					break;
				case MU_KEV:
					break;
				case MU_MEV:
					mass=mass*1.0e-3;
					break;
				case MU_GEV:
					mass=mass*1.0e-6;
					break;
				case MU_AMU:
					mass= ( mass*1.0e-3 ) /AMU;
					break;
			}
		}
		else
		{

			m0=AMU*ABox->value(); // in MeV
			switch ( MUnitBox->currentItem() )
			{
				case MU_EV:
					mass=m0*1.0e6+mass*1.0e3;
					break;
				case MU_KEV:
					mass=m0*1.0e3+mass;
					break;
				case MU_MEV:
					mass=m0+mass*1.0e-3;
					break;
				case MU_GEV:
					mass=m0*1.0e-3+mass*1.0e-6;
					break;
				case MU_AMU:
					mass= ( m0+mass*1.0e-3 ) /AMU;
					break;
			}
		}
	}
	massBox->blockSignals ( true );
	massBox->setText ( QString::number ( mass, 'g',9 ) );
	massBox->blockSignals ( false );
	rawLbl->setText ( m );
}

void mdmCls::EBoxSlot()
{
	if ( EBox->count() <=0 || EBox->currentItem()<0) return;
	QStringList::Iterator it = MList.at ( EBox->currentItem() );
	if ( it!=NULL )
	{
		showMass ( *it );
		EeditBox->blockSignals ( true );
		EeditBox->setText ( EBox->currentText() );
		EeditBox->blockSignals ( false );
		if ( currentA>0 )
		{
			int z=getZ ( EBox->currentText().simplifyWhiteSpace() );
			if ( z>0 ) ZLbl->setText ( QString::number ( z ) );
		}
		else
		{
			ZLbl->setText ( "N/A" );
		}
	}
}

void mdmCls::warnDiscard()
{
	warn ( "Discard Changes? Proceed if ok." );
	discardOk=-1;
	ABox->blockSignals ( true );
	ABox->setValue ( currentA );
	ABox->blockSignals ( false );
}

void mdmCls::changeASlot()
{
	if ( appBut->isEnabled() &&discardOk>=0 )
	{
		warnDiscard();
		return;
	}
	discardOk=0;

	currentA=ABox->value();
	showExcessBox->setEnabled ( currentA!=0 );
	openMassData ( currentA );
	EeditBox->clear();
	massBox->clear();
	appBut->setEnabled ( false );
}

void mdmCls::changeUnitSlot()
{
	EBoxSlot();
}

void mdmCls::setReady ( bool on )
{
	EBox->setEnabled ( on );
	massBox->setEnabled ( on );
	EeditBox->setEnabled ( on );
	delBut->setEnabled ( on );
	ZLbl->clear();
}

void mdmCls::delSlot()
{
	if ( EBox->count() <=0 ) return;
	int i=EBox->currentItem();
	QStringList::Iterator it = MList.at ( i );
	MList.remove ( *it );
	EBox->removeItem ( i );
	EBox->setSelected ( EBox->currentItem(),true );
	appBut->setEnabled ( true ); // changes made
	if ( EBox->count() ==0 ) setReady ( false );
}

void mdmCls::neweSlot()
{
	if ( !currentFileExists ) setReady ( true );
	MList.append ( "0" );
	EBox->insertItem ( "New" );
	EBox->setSelected ( EBox->count()-1,true );
	appBut->setEnabled ( true );
}

void mdmCls::massChangedSlot()
{
	appBut->setEnabled ( true );
	bool ok;
	double v=massBox->text().toDouble ( &ok );
	if ( !ok ) return;
	double m=readMass ( v );
	rawLbl->setText ( QString::number ( m, 'g',9 ) );

	int i=EBox->currentItem();
	QStringList::Iterator it = MList.at ( i );
	*it=QString::number ( m,'g',9 );
}

void mdmCls::EChangedSlot()
{
	EeditBox->blockSignals ( true );
	EeditBox->setText ( EeditBox->text().simplifyWhiteSpace() );
	EeditBox->blockSignals ( false );
	appBut->setEnabled ( true );
	EBox->changeItem ( EeditBox->text(),EBox->currentItem() );
}

void mdmCls::appSlot()
{
	QTextStream ns ( NULL );
	int i=applyChanges ( &ns,true );
	if ( i>=0 )
	{
		mess ( "Invalid Entry Found","red" );
		EBox->setSelected ( i,true );
		return;
	}
	QFile file ( currentFile );
	if ( file.open ( IO_WriteOnly ) )
	{
		QTextStream stream ( &file );
		applyChanges ( &stream,false );
		file.close();
		mess ( "Changes saved: "+currentFile,"blue" );
		appBut->setEnabled ( false );
	}
	else
	{
		mess ( "Error during write: "+currentFile,"red" );
	}
}

int mdmCls::applyChanges ( QTextStream *s, bool checkValue )
{
	int i=0;
	QString line;
	bool ok;
	for ( QStringList::Iterator it = MList.begin(); it != MList.end(); ++it )
	{
		if ( checkValue )
		{
			if ( EBox->text ( i ).length() ==0 ) return i;
			( *it ).toDouble ( &ok );
			if ( !ok ) return i;
		}
		else
		{
			line=EBox->text ( i ).leftJustify ( 9,' ' );
			*s << line << *it << endl;
		}
		i++;
	}
	return -1;
}

void mdmCls::mess ( QString m, QString col )
{
	if ( timer!=0 )
	{
		killTimer ( timer );
		timer=0;
	}
	msgLbl->setPaletteForegroundColor ( col );
	msgLbl->setText ( m );
}

void mdmCls::createMassDataSlot()
{
	if ( appBut->isEnabled() &&discardOk>=0 )
	{
		warnDiscard();
		return;
	}
	wStack->raiseWidget ( 1 );
	createMassDataBut->setEnabled ( false );
	editDataBut->setEnabled ( true );
	msgLbl->clear();
	if ( massDataFileBox->text().length() ==0 )
	{
		mess ( "Please specify atomic mass data file" );
	}
}

void mdmCls::editDataSlot()
{
	wStack->raiseWidget ( 0 );
	createMassDataBut->setEnabled ( true );
	editDataBut->setEnabled ( false );
	if ( currentFileExists )
	{
		mess ( "Data file opened: "+currentFile );
	}
	else
	{
		mess ( "Failed to open data file:"+currentFile,"red" );
	}
}

void mdmCls::openMassDataFileSlot()
{
	QString s = KFileDialog::getOpenFileName ( home,
	            "*",
	            this,
	            tr ( "Selects Mass Data File" ) );
	if ( s.length() ==0 ) return;
	massDataFileBox->setText ( s );
}

void mdmCls::openMassDataDirSlot()
{
	QString	s=KFileDialog::getExistingDirectory ( home,this,tr ( "Choose a Mass Data Directory" ) );
	if ( s.length() ==0 ) return;
	massDataDirBox->setText ( s );
}

void mdmCls::extMassDataSlot()
{
	QString str;
	QString fin=massDataFileBox->text();
	if ( fin.length() ==0 ) return;
	QString foutd=massDataDirBox->text();
	if ( foutd.length() ==0 )
	{
		foutd=home+"/massdata";
		massDataDirBox->setText ( foutd );
	}
	QDir d;
	if ( !QFile::exists ( foutd ) )
	{
		if ( !d.mkdir ( foutd ) )
		{
			QMessageBox::critical ( this, "relkinema: ", "Cannot create directory:\n"+foutd, tr ( "&ok" ) );
			return;
		}
	}
	QFileInfo fi ( foutd );
	if ( !fi.isDir() )
	{
		warn ( "Not a direcotry: "+foutd );
		return;
	}
	else if ( !fi.isWritable() )
	{
		warn ( "Not writable: "+foutd );
		return;
	}
	d=foutd;
	if ( d.isRelative() )
	{
		massDataDirBox->setText ( d.absPath() );
	}

	QFile file ( fin );
	if ( file.open ( IO_ReadOnly ) )
	{
		mess ( "Extracting mass data..." );
		QFile ofile;
		QTextStream stream ( &file );
		QTextStream ostream;
		QString line;
		QString fout;
		int NsubZ,N,Z,A;
		QString EL,Decay,m;
		double MassExcess;
		bool start=false;
		bool opened=false;
		while ( !stream.atEnd() )
		{
			line = stream.readLine();
			NsubZ= line.mid ( 1,3 ).toInt();
			N    = line.mid ( 4,5 ).toInt();
			Z    = line.mid ( 9,5 ).toInt();
			A    = line.mid ( 14,5 ).toInt();
			if ( !start && (NsubZ==1&&N==1&&Z==0&&A==1 )){
				 start=true;
			}else{
				continue;
			}
			EL   = line.mid ( 20,3 );
			Decay = line.mid ( 23,4 );
			m=line.mid ( 29,13 ).replace ( "#","." );
			MassExcess=m.toDouble();

			if ( line.at ( 0 ) =='0' )
			{
				if ( opened ) ofile.close();
				fout=foutd+"/A"+line.mid ( 14,5 ).simplifyWhiteSpace() +".dat";
				ofile.setName ( fout );
				opened=ofile.open ( IO_WriteOnly );
				if ( opened ) ostream.setDevice ( &ofile );
			}
			if ( opened )
			{
				if ( useFormalBox->isChecked() &&Z>=Zmin_new && Z<=Zmax )
				{
					EL=Element[Z-1];
				}
				ostream << EL.leftJustify ( 9,' ' ) << m.simplifyWhiteSpace() << endl;
			}
		}
		if ( opened ) ofile.close();
		file.close();

		ofile.setName ( foutd+"/A.dat" );
		ofile.open ( IO_WriteOnly );
		ostream.setDevice ( &ofile );
		ostream << "g        " << QString::number ( 0.,'g',9 ) << endl;
		ostream << "e        " << QString::number ( Me,'g',9 ) << endl;
		ostream << "n        " << QString::number ( Mn,'g',9 ) << endl;
		ostream << "p        " << QString::number ( Mp,'g',9 ) << endl;
		ostream << "d        " << QString::number ( Md,'g',9 ) << endl;
		ostream << "t        " << QString::number ( Mt,'g',9 ) << endl;
		ostream << "a        " << QString::number ( Ma,'g',9 ) << endl;
		ofile.close();
		if ( start )
		{
			mess ( "Creating mass data done","blue" );
		}
		else
		{
			warn ( "Failed to create mass data: invalid file" );
		}
	}
	else
	{
		warn ( "Failed to read: "+fin );
	}
}

void mdmCls::warn ( QString mess )
{
	warnmess=mess;
	msgLbl->setPaletteForegroundColor ( "red" );
	msgLbl->setText ( "<b>"+warnmess+"</b>" );
	timer=startTimer ( 600 );
}

void mdmCls::timerEvent ( QTimerEvent *e )
{
	if ( !blink )
	{
		msgLbl->clear();
	}
	else
	{
		msgLbl->setPaletteForegroundColor ( "red" );
		msgLbl->setText ( "<b>"+warnmess+"</b>" );
	}
	blink=!blink;
}

void mdmCls::keyPressEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Q:
		case Qt::Key_Escape:
			close();
			break;
	}
}

void mdmCls::stopTimer()
{
	if ( timer!=0 )
	{
		killTimer ( timer );
		timer=0;
	}
}

void mdmCls::massDataFileSlot()
{
	stopTimer();
	QFileInfo f ( massDataFileBox->text() );
	extMassDataBut->setEnabled ( f.exists() &&f.isFile() &&f.size() >0 );
}

void mdmCls::massDataDirSlot()
{
	stopTimer();
}

void mdmCls::gotoamdcSlot()
{
	QProcess p;
	p.addArgument ( wb );
	p.addArgument ( gotoamdc->text() );
	p.start();
}

int mdmCls::getZ_from_List ( QString n, const char **el )
{
	int z=0;
	unsigned int lin=strlen ( n.latin1() );
	while ( el[z]!=0 )
	{
		if ( strlen ( el[z] ) ==lin )
		{
#if defined(HAVE_STRNSTR) && (HAVE_STRNSTR == 1)
			if ( strnstr ( el[z],n.latin1(),lin ) !=NULL ) return z+1;
#else
			if ( strstr ( el[z],n.latin1() ) !=NULL ) return z+1;
#endif
		}
		z++;
	}
	return -1;
}

int mdmCls::getZ ( QString n )
{
	int z;

	z=getZ_from_List ( n, Element );
	if ( z==-1 )
	{
		z=getZ_from_List ( n, newElement );
		if ( z!=-1 ) z+=Zmin_new-1;
	}

	return z;
}

int mdmCls::getParticleAZ ( QString n, int *a, int *z )
{
	unsigned int lin=strlen ( n.latin1() );
	int i=0;
	while ( Particle[i]!=0 )
	{
		if ( lin==strlen ( Particle[i] ) )
		{
#if defined(HAVE_STRNSTR) && (HAVE_STRNSTR == 1)
			if ( strnstr ( Particle[i],n.latin1(),lin ) !=NULL )
#else
			if ( strstr ( Particle[i],n.latin1() ) !=NULL )
#endif
			{
				*a=-ParticleA[i]; // particle A is negative
				*z=ParticleZ[i];
				return i;
			}
		}
		i++;
	}
	return -1;
}

void mdmCls::closeEvent ( QCloseEvent *e )
{
	if ( appBut->isEnabled() &&discardOk>=0 )
	{
		warnDiscard();
		e->ignore();
	}
	else
	{
		e->accept();
	}
}

#include "mdmcls.moc"
