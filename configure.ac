# ------------------------------------------------------------
# GNU shogi and xshogi configuration script.
# ------------------------------------------------------------

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([gnushogi],[1.4.0+],[https://savannah.gnu.org/bugs/?group=gnushogi])
AC_CONFIG_SRCDIR([version.h])
AC_CONFIG_HEADER(config.h)


#
# Check for C compiler.
#

echo
AC_MSG_CHECKING(for C compiler)
echo

AC_PROG_CC


#
# Check for parser generators.
#

echo
AC_MSG_CHECKING([for yacc/bison, lex/flex, and install])
echo

AC_PROG_YACC
AC_PROG_LEX
AC_PROG_INSTALL


#
# Check for X windows.
#

echo
AC_PATH_XTRA

#
# Check for ncurses.
#

AC_ARG_WITH([curses],
  [AS_HELP_STRING([--with-curses],
    [enable curses UI])],
  [],
  [with_curses=check])

LIBCURSES=
AS_IF([test "x$with_curses" != xno],
  [AC_CHECK_LIB([curses], [clrtoeol],
    [AC_SUBST([LIBCURSES], ["-lcurses"])
     AC_DEFINE([HAVE_LIBCURSES], [1],
               [Define if you have lib])
    ],
    [AC_MSG_FAILURE(
       [--with-curses was given, but test for curses failed])],
    [-ltermcap])])

#
# Check for other libraries.
#

AC_CHECK_LIB(m, pow)
dnl Replace `main' with a function in -ltermcap:
AC_CHECK_LIB(termcap, tgoto)


#
# C compiler warnings.
#

if [[ $ac_cv_c_compiler_gnu = yes ]]
then
WARNINGS="-Wall -Wno-implicit-int -Wstrict-prototypes -ansi -pedantic"
CEXTRAFLAGS="-fsigned-char -funroll-loops \$(HASH)"
# For profiling targets:
CEXTRAFLAGS2="-fsigned-char -pg -fprofile-arcs -ftest-coverage \$(HASH)"
else
# Who knows what warnings your compiler uses?
WARNINGS=
CEXTRAFLAGS="\$(HASH)"
fi

AC_SUBST(WARNINGS)
AC_SUBST(CEXTRAFLAGS)
AC_SUBST(CEXTRAFLAGS2)


#
# Check for header files.
#

echo
AC_MSG_CHECKING(for header files)
echo

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/file.h sys/ioctl.h])
AC_CHECK_HEADERS([sys/param.h sys/time.h unistd.h])
AC_CHECK_HEADERS(errno.h)
AC_HEADER_TIME


#
# Check for typedefs, structures, and compiler characteristics.
#

echo 
AC_MSG_CHECKING(for typedefs)
echo

AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

echo 
AC_MSG_CHECKING(for compiler characteristics)
echo

AC_CHECK_SIZEOF(long)


#
# Check for library functions.
#

echo
AC_MSG_CHECKING(for library functions)
echo

AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([dup2 gethostname gettimeofday memset pow strcasecmp])
AC_CHECK_FUNCS([strchr strerror strrchr strstr strtol])
AC_CHECK_FUNCS(memcpy bcopy)
AC_CHECK_FUNCS(setlinebuf setvbuf)


#
# Set various user-definable options.
#
# Valid OPT settings:
# --with-top-part-bitmaps
#
# Other settings:
# --with-first-shogi-program=PROGNAME
# --with-second-shogi-program=PROGNAME
# --with-first-host=HOSTNAME
# --with-second-host=HOSTNAME
#

OPT=
FIRST_SHOGI_PROGRAM=gnushogi
SECOND_SHOGI_PROGRAM=gnushogi
FIRST_HOST=localhost
SECOND_HOST=localhost

AC_ARG_WITH(xshogi,
[  --with-xshogi
        Compile the xshogi graphical interface to GNU shogi (default: no)],
with_xshogi=$withval,
with_xshogi=no)

AC_ARG_WITH(top-part-bitmaps,
[  --with-top-part-bitmaps    Use top part of Kanji bitmaps only in xshogi.],
OPT=-DTOP_PART_BITMAPS)

AC_ARG_WITH(first-shogi-program,
[  --with-first-shogi-program=PROGNAME    
        Use PROGNAME as the first shogi program in xshogi (default: gnushogi)],
FIRST_SHOGI_PROGRAM=$withval)

AC_ARG_WITH(second-shogi-program,
[  --with-second-shogi-program=PROGNAME    
        Use PROGNAME as the second shogi program in xshogi (default: gnushogi)],
SECOND_SHOGI_PROGRAM=$withval)

AC_ARG_WITH(first-host,
[  --with-first-host=HOSTNAME    
        Use HOSTNAME as the machine on which the first shogi program 
        runs in xshogi (default: localhost)],
FIRST_HOST=$withval)

AC_ARG_WITH(second-host,
[  --with-second-host=HOSTNAME    
        Use HOSTNAME as the machine on which the second shogi program 
        runs in xshogi (default: localhost)],
SECOND_HOST=$withval)

AC_SUBST(OPT)
AC_SUBST(FIRST_SHOGI_PROGRAM)
AC_SUBST(SECOND_SHOGI_PROGRAM)
AC_SUBST(FIRST_HOST)
AC_SUBST(SECOND_HOST)

if [[ $with_xshogi = yes ]]
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


echo
echo outputting files...

AC_CONFIG_FILES([Makefile
                 gnushogi/Makefile
				 gnushogi/Makefile.profile
                 xshogi/Makefile])
AC_OUTPUT

