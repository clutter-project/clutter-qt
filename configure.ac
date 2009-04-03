# clutter package version number, (as distinct from shared library version)
# An odd micro number indicates in-progress development, (eg. from CVS)
# An even micro number indicates a released version.
m4_define([clutter_major_version], [0])
m4_define([clutter_minor_version], [8])
m4_define([clutter_micro_version], [0])

m4_define([clutter_version],
          [clutter_major_version.clutter_minor_version.clutter_micro_version])

m4_define([clutter_api_version],
          [clutter_major_version.clutter_minor_version])

# increase the interface age for each release; if the API changes, set to 0
m4_define([clutter_interface_age], [0])
m4_define([clutter_binary_age],
          [m4_eval(100 * clutter_minor_version + clutter_micro_version)])

AC_PREREQ(2.53)

AC_INIT([clutter-qt],
        [clutter_version],
        [http://bugzilla.o-hand.com/enter_bug.cgi?product=Clutter])
AC_CONFIG_SRCDIR([clutter-qt/clutter-qt.h])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([1.9])

CLUTTER_QT_MAJOR_VERSION=clutter_major_version
CLUTTER_QT_MINOR_VERSION=clutter_minor_version
CLUTTER_QT_MICRO_VERSION=clutter_micro_version
CLUTTER_QT_VERSION=clutter_version
AC_SUBST(CLUTTER_QT_MAJOR_VERSION)
AC_SUBST(CLUTTER_QT_MINOR_VERSION)
AC_SUBST(CLUTTER_QT_MICRO_VERSION)
AC_SUBST(CLUTTER_QT_VERSION)

m4_define([lt_current],
          [m4_eval(100 * clutter_minor_version + clutter_micro_version - clutter_interface_age)])
m4_define([lt_revision], [clutter_interface_age])
m4_define([lt_age], [m4_eval(clutter_binary_age - clutter_interface_age)])
CLUTTER_LT_CURRENT=lt_current
CLUTTER_LT_REV=lt_revision
CLUTTER_LT_AGE=lt_age
CLUTTER_LT_VERSION="$CLUTTER_LT_CURRENT:$CLUTTER_LT_REV:$CLUTTER_LT_AGE"
CLUTTER_LT_LDFLAGS="-version-info $CLUTTER_LT_VERSION"

AC_SUBST(CLUTTER_LT_VERSION)
AC_SUBST(CLUTTER_LT_LDFLAGS)

dnl ========================================================================

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl ========================================================================

AC_ARG_WITH(qt_moc, AS_HELP_STRING([--with-moc=<path>],[moc for Qt4]))

dnl ========================================================================

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strcasecmp strdup])

CLUTTER_REQUIRED=0.9.0

AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)

PKG_CHECK_MODULES(QT, QtCore >= 4.2 QtGui >= 4.2 glib-2.0 clutter-0.9)
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LIBS)

dnl ========================================================================

if test -z "$with_moc" ; then
        AC_PATH_PROG(QT_MOC, moc-qt4, no)
        if test x"$QT_MOC" = xno; then
                AC_PATH_PROG(QT_MOC, moc, no)
        fi
        if test x"$QT_MOC" = xno; then
                AC_MSG_ERROR([Could not find moc])
        fi
else
        QT_MOC=$with_moc
        AC_SUBST(QT_MOC)
fi

dnl ========================================================================

if test "x$GCC" = "xyes"; then
        GCC_FLAGS="-g -Wall"
fi

AC_SUBST(GCC_FLAGS)

AC_CONFIG_FILES([
        Makefile
        clutter-qt/Makefile
        examples/Makefile
        clutter-qt.pc
])

AC_OUTPUT
