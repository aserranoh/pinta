AC_INIT([libpinta], [0.1], [toni.serranoh@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
PKG_PROG_PKG_CONFIG
AC_PROG_CC
AM_PROG_AR
LT_INIT

AC_ARG_ENABLE([drm],
 [AS_HELP_STRING([--enable-drm], [Compile support for DRM frontend])],
 [],
 [enable_drm=yes])
AS_CASE(["$enable_drm"],
 [yes], [PKG_CHECK_MODULES([libdrm], [libdrm])
   PKG_CHECK_MODULES([gbm], [gbm])],
 [no], [],
 [AC_MSG_ERROR([bad value "$enable_drm" for --enable-drm])])
AM_CONDITIONAL([USE_DRM], [test "$enable_drm" == yes])

AC_ARG_ENABLE([x],
 [AS_HELP_STRING([--enable-x], [Compile support for X frontend])],
 [],
 [enable_x=yes])
AS_CASE(["$enable_x"],
 [yes], [PKG_CHECK_MODULES([x11], [x11], [HAVE_X=1])],
 [no], [],
 [AC_MSG_ERROR([bad value "$enable_x" for --enable-x])])
AM_CONDITIONAL([USE_X], [test "$enable_x" == yes])

PKG_CHECK_MODULES([glesv2], [glesv2])
PKG_CHECK_MODULES([egl], [egl])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 test/Makefile
])
AC_OUTPUT
