NULL =

INCLUDES = \
	-I$(srcdir) 				\
	-I$(top_srcdir) 			\
	-DG_LOG_DOMAIN=\"Clutter-Qt\" 		\
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 		\
	$(CLUTTER_DEBUG_FLAGS) 			\
	$(GCC_FLAGS) 				\
	$(CLUTTER_CFLAGS) 			\
	$(QT_CFLAGS)

lib_LTLIBRARIES = libclutter-qt-@CLUTTER_QT_API_VERSION@.la

source_cc = \
	moc_clutter-qt.cc	\
	clutter-qt.c		\
	$(NULL)

libclutter_qt_@CLUTTER_QT_API_VERSION@_la_SOURCES = $(source_cc)
libclutter_qt_@CLUTTER_QT_API_VERSION@_la_LIBADD  = $(CLUTTER_LIBS) $(QT_LIBS)
libclutter_qt_@CLUTTER_QT_API_VERSION@_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

clutterqtheadersdir = $(includedir)/clutter-@CLUTTER_QT_API_VERSION@/clutter-qt
clutterqtheaders_HEADERS = clutter-qt.h

moc_clutter-qt.cc: clutter-qt.h
	$(QT_MOC) $(DEFINES) $(INCPATH) $< -o $@

CLEANFILES = moc_clutter-qt.cc
