# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = aboutcls.h kinemaplotcls.h mboxcls.h mdmcls.h misc.h \
	relkinemaCls.h resultwindowcls.h rkcalccls.h rkcore.h rwthreadcls.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/relkinema.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = relkinema

# the application source, library search path, and link libraries
relkinema_SOURCES = about.ui aboutcls.cpp kinemaPlot.ui kinemaplotcls.cpp \
	lineread.ui linereadcls.cpp main.cpp mboxcls.cpp mdmDlg.ui mdmcls.cpp message.ui \
	rclist.ui rclistcls.cpp relkinemaCls.cpp relkinemaDlg.ui resultWindow.ui \
	resultwindowcls.cpp rkCalcDlg.ui rkcalccls.cpp rkcore.f90 rwthreadcls.cpp
relkinema_LDFLAGS = $(KDE_RPATH) $(all_libraries)
relkinema_LDADD = $(LIB_KDEUI) -lgfortran -lkio -lfzcal

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = relkinema.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/relkinema
shellrc_DATA = relkinemaui.rc

