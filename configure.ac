# ------------------------------------------------------------
# GNU shogi and xshogi configuration script.
# ------------------------------------------------------------

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([gnushogi],[1.4.1],[https://savannah.gnu.org/bugs/?group=gnushogi])
AC_CONFIG_SRCDIR([gnushogi/gnushogi.h])
AC_CONFIG_HEADER(config.h)

##########
AC_MSG_NOTICE([C compiler])
AC_PROG_CC

##########
AC_MSG_NOTICE([yacc/bison, lex/flex, and install])
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_INSTALL

##########
AC_MSG_NOTICE([X])
AC_PATH_XTRA

##########
AC_MSG_NOTICE([libs])

# curses
AC_ARG_WITH([curses],
  [AS_HELP_STRING([--with-curses],
    [enable curses UI (default: yes if available)])],
  [],
  [with_curses=check])

LIBCURSES=
AS_IF([test "x$with_curses" != xno],
  [AC_CHECK_LIB([curses], [clrtoeol],
    [AC_SUBST([LIBCURSES], [-lcurses])
     AC_SUBST([CURSESDSP], [cursesdsp.o])
     AC_DEFINE([HAVE_LIBCURSES], [1],
               [Define if you have lib])
    ],
    [AS_IF([test "x$with_curses" = xyes],
      [AC_MSG_ERROR(
	 [--with-curses was given, but test for curses failed])])],
    [-ltermcap])])

# other libraries.
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([termcap], [tgoto])



##########
# C compiler warnings.

if [[ $ac_cv_c_compiler_gnu = yes ]]
then
WARNINGS="-Wall -Wno-implicit-int -Wstrict-prototypes -ansi -pedantic"
CEXTRAFLAGS="-fsigned-char -funroll-loops"
else
# Who knows what warnings your compiler uses?
WARNINGS=
CEXTRAFLAGS=
fi

AC_SUBST(WARNINGS)
AC_SUBST(CEXTRAFLAGS)

##########
AC_MSG_NOTICE([header files])

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/file.h sys/ioctl.h])
AC_CHECK_HEADERS([sys/param.h sys/time.h unistd.h])
AC_CHECK_HEADERS(errno.h)
AC_HEADER_TIME

##########
AC_MSG_NOTICE([typedefs])

AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

##########
AC_MSG_NOTICE([compiler characteristics])

AC_CHECK_SIZEOF(long)

##########
AC_MSG_NOTICE([library functions])

AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([dup2 gethostname gettimeofday memset pow strcasecmp])
AC_CHECK_FUNCS([strchr strerror strrchr strstr strtol])
AC_CHECK_FUNCS([memcpy bcopy])
AC_CHECK_FUNCS([setvbuf setlinebuf])


#
# Set various user-definable options.
#

AC_ARG_ENABLE([minishogi],
[  --enable-minishogi
        Build gnuminishogi instead of gnushogi (default: no)],
[AC_DEFINE([MINISHOGI], [], [Define to build gnuminishogi instead of gnushogi])
AC_SUBST([PROGNAME], [gnuminishogi])
],
[AC_SUBST([PROGNAME], [gnushogi])
])

OPT=
FIRST_SHOGI_PROGRAM=gnushogi
SECOND_SHOGI_PROGRAM=gnushogi
FIRST_HOST=localhost
SECOND_HOST=localhost

AC_ARG_ENABLE([xshogi],
AS_HELP_STRING([--enable-xshogi],
  [compile the xshogi graphical interface to GNU shogi (default: no)]),
[enable_xshogi=$enableval],
[enable_xshogi=no])

AC_ARG_WITH([top-part-bitmaps],
AS_HELP_STRING([--with-top-part-bitmaps],
  [use top part of Kanji bitmaps only in xshogi]),
[OPT=-DTOP_PART_BITMAPS])

AC_ARG_WITH([first-shogi-program],
AS_HELP_STRING([--with-first-shogi-program=PROGNAME],
  [use PROGNAME as the first shogi program in xshogi (default: gnushogi)]),
[FIRST_SHOGI_PROGRAM=$withval])

AC_ARG_WITH([second-shogi-program],
AS_HELP_STRING([--with-second-shogi-program=PROGNAME],
  [use PROGNAME as the second shogi program in xshogi (default: gnushogi)]),
[SECOND_SHOGI_PROGRAM=$withval])

AC_ARG_WITH([first-host],
AS_HELP_STRING([--with-first-host=HOSTNAME],
  [use HOSTNAME as the machine on which the first shogi program runs in xshogi (default: localhost)]),
[FIRST_HOST=$withval])

AC_ARG_WITH([second-host],
AS_HELP_STRING([--with-second-host=HOSTNAME],
  [use HOSTNAME as the machine on which the second shogi program runs in xshogi (default: localhost)]),
[SECOND_HOST=$withval])

AC_SUBST(OPT)
AC_SUBST(FIRST_SHOGI_PROGRAM)
AC_SUBST(SECOND_SHOGI_PROGRAM)
AC_SUBST(FIRST_HOST)
AC_SUBST(SECOND_HOST)

if [[ $enable_xshogi = yes ]]
then
XSHOGI=xshogi_compile
XSHOGIINSTALL=xshogi_install
XSHOGICLEAN=xshogi_clean
else
XSHOGI=
XSHOGIINSTALL=
XSHOGICLEAN=
fi

AC_SUBST(XSHOGI)
AC_SUBST(XSHOGIINSTALL)
AC_SUBST(XSHOGICLEAN)

##########
AC_CONFIG_FILES([Makefile
                 gnushogi/Makefile
                 win32/config.h
                 doc/Makefile
                 xshogi/Makefile])
AC_OUTPUT
