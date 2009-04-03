SUBDIRS = clutter-qt examples

clutter-qt-@CLUTTER_QT_API_VERSION@.pc: clutter-qt.pc
	@cp -f clutter-qt.pc clutter-qt-@CLUTTER_QT_API_VERSION@.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clutter-qt-@CLUTTER_QT_API_VERSION@.pc

CLEANFILES = clutter-qt-@CLUTTER_QT_API_VERSION@.pc

DISTCLEANFILES = clutter-qt.pc

EXTRA_DIST = clutter-qt.pc.in
