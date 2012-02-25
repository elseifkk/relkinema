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

#include "config.h"
#include "relkinemaCls.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP ( "A KDE KPart Application" );
#ifdef VERSION
static const char version[] = VERSION;
#else
static const char version[] = "unknown";
#endif
#ifdef PACKAGE
static const char package[]=PACKAGE;
#else
static const char package[]="unknown";
#endif
static KCmdLineOptions options[] =
{
	{ "c file", I18N_NOOP ( "specify config file" ), 0 },
	{ "a", I18N_NOOP ( "always on top" ), 0 },
	KCmdLineLastOption
};

int main ( int argc, char **argv )
{
	KAboutData about ( package, I18N_NOOP ( package ), version, description,
	                   KAboutData::License_GPL, "(C) 2012 Kazuaki Kumagai", 0, 0, "elseifkk@users.sf.net" );
	about.addAuthor ( "Kazuaki Kumagai", 0, "elseifkk@users.sf.net" );
	KCmdLineArgs::init ( argc, argv, &about );
	KCmdLineArgs::addCmdLineOptions ( options );
	KApplication app;
	RelKinemaCls *mainWin=NULL;

	// no session.. just start up normally
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	Qt::WFlags wf=0;
	if ( args->isSet ( "a" ) ) wf|=Qt::WStyle_StaysOnTop;

	/// @todo do something with the command line args here
	mainWin = new RelKinemaCls ( NULL,NULL,wf,VERSION,args->getOption ( "c" ) );
	app.setMainWidget ( mainWin );
	mainWin->show();
	args->clear();

// mainWin has WDestructiveClose flag by default, so it will delete itself.
	return app.exec();
}
