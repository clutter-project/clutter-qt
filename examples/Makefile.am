NULL =

noinst_PROGRAMS = \
	testqt

INCLUDES = \
	-I$(srcdir) \
	-I$(top_srcdir)

AM_CPPFLAGS = $(CLUTTER_CFLAGS) $(QT_CFLAGS)

common_deps = \
	$(top_builddir)/clutter-qt/libclutter-qt-0.8.la

common_ldadd = \
	$(top_builddir)/clutter-qt/libclutter-qt-0.8.la \
	$(CLUTTER_LIBS)	\
	$(QT_LIBS)

testqt_SOURCES = testqt.cc testqt.h moc_testqt.cc
testqt_DEPENDENCIES = $(common_deps)
testqt_LDADD = $(common_ldadd)

moc_testqt.cc: testqt.h
	moc $(DEFINES) $(INCPATH) $< -o $@

EXTRA_DIST = redhand.png

CLEANFILES = moc_testqt.cc
