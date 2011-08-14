#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './message.ui'
**
** Created: 日  8月 14 19:28:31 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "message.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a mBox as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
mBox::mBox( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "mBox" );
    QFont f( font() );
    f.setFamily( "Sans Serif" );
    setFont( f ); 
    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
mBox::~mBox()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void mBox::languageChange()
{
    setCaption( tr2i18n( "message" ) );
}

#include "message.moc"
