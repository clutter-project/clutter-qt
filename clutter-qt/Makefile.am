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

lib_LTLIBRARIES = libclutter-qt-0.8.la

libclutter_qt_0_8_la_SOURCES = clutter-qt.cc moc_clutter-qt.cc
libclutter_qt_0_8_la_LIBADD = $(CLUTTER_LIBS) $(QT_LIBS)
libclutter_qt_0_8_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

clutterqtheadersdir = $(includedir)/clutter-0.8/clutter-qt
clutterqtheaders_HEADERS = clutter-qt.h

moc_clutter-qt.cc: clutter-qt.h
	$(QT_MOC) $(DEFINES) $(INCPATH) $< -o $@

CLEANFILES = moc_clutter-qt.cc
