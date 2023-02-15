AC_INIT([libpinta], [0.1], [toni.serranoh@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
PKG_PROG_PKG_CONFIG
AC_PROG_CXX
AM_PROG_AR
LT_INIT

PKG_CHECK_MODULES([sdl2], [sdl2])
PKG_CHECK_MODULES([glesv2], [glesv2])
PKG_CHECK_MODULES([glm], [glm])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
