/*
 * FILE: dspwrappers.c
 *
 *     Wrapper functions which call analogous functions in rawdsp.c
 *     or cursesdsp.c depending on the interface.
 *
 * ----------------------------------------------------------------------
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
 * Copyright (c) 2008, 2013, 2014 Yann Dirson and the Free Software Foundation
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988, 1989, 1990 John Stanback
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with GNU Shogi; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 */

/* request *snprintf prototypes*/
#define _POSIX_C_SOURCE 200112L

#include "gnushogi.h"
#include "dspwrappers.h"
#include "rawdsp.h"
#include "cursesdsp.h"

#include <stdio.h>

#define CASE_DSP_RAW(func,args) \
  case DISPLAY_RAW:             \
  case DISPLAY_X:               \
    func args;                  \
    break

#ifdef HAVE_LIBCURSES
#define CASE_DSP_CURSES(func,args) \
  case DISPLAY_CURSES:             \
    func args;                     \
    break;
#else
#define CASE_DSP_CURSES(func,args)
#endif

#define DISPLAY_FUNC(func,argsdecl,args)        \
  void func argsdecl                            \
  {                                             \
    switch (display_type) {                     \
      CASE_DSP_RAW(Raw_ ## func, args);         \
      CASE_DSP_CURSES(Curses_ ## func, args);   \
    }                                           \
  }
#define DISPLAY_VOIDFUNC(func)                  \
  DISPLAY_FUNC(func,(void),())

#define DISPLAY_STDARGFUNC(func,argsdecl,last,args)     \
  void func argsdecl                                    \
  {                                                     \
    va_list ap;                                         \
    va_start(ap, last);                                 \
    switch (display_type) {                             \
      CASE_DSP_RAW(Raw_ ## func, args);                 \
      CASE_DSP_CURSES(Curses_ ## func, args);           \
    }                                                   \
    va_end(ap);                                         \
  }

DISPLAY_VOIDFUNC(ChangeAlphaWindow)
DISPLAY_VOIDFUNC(ChangeBetaWindow)
DISPLAY_VOIDFUNC(ChangeHashDepth)
DISPLAY_VOIDFUNC(ChangeSearchDepth)
DISPLAY_VOIDFUNC(ChangeXwindow)
DISPLAY_VOIDFUNC(ClearScreen)
DISPLAY_VOIDFUNC(DoDebug)
DISPLAY_FUNC(DoTable, (short table[NO_SQUARES]), (table))
DISPLAY_VOIDFUNC(EditBoard)
DISPLAY_VOIDFUNC(ExitShogi)
DISPLAY_VOIDFUNC(GiveHint)
DISPLAY_VOIDFUNC(Initialize)
DISPLAY_FUNC(ShowNodeCnt, (long NodeCnt), (NodeCnt))
DISPLAY_VOIDFUNC(OutputMove)
DISPLAY_VOIDFUNC(PollForInput)
DISPLAY_VOIDFUNC(SetContempt)
DISPLAY_FUNC(SearchStartStuff, (short side), (side))
DISPLAY_FUNC(SelectLevel, (char *sx), (sx))
DISPLAY_VOIDFUNC(SetupBoard)
DISPLAY_FUNC(ShowCurrentMove, (short pnt, short f, short t), (pnt, f, t))
DISPLAY_FUNC(ShowDepth, (char ch), (ch))
DISPLAY_VOIDFUNC(ShowGameType)
DISPLAY_FUNC(ShowLine, (unsigned short *bstline), (bstline))
DISPLAY_FUNC(ShowMessage, (char *s), (s))
DISPLAY_STDARGFUNC(AlwaysShowMessage, (const char *format, ...), format, (format, ap))
DISPLAY_STDARGFUNC(Printf, (const char *format, ...), format, (format, ap))
DISPLAY_FUNC(ShowPatternCount, (short side, short n), (side, n))
DISPLAY_FUNC(ShowPostnValue, (short sq), (sq))
DISPLAY_VOIDFUNC(ShowPostnValues)
DISPLAY_VOIDFUNC(ShowPrompt)
DISPLAY_VOIDFUNC(ShowResponseTime)
DISPLAY_FUNC(ShowResults, (short score, unsigned short *bstline, char ch), (score, bstline, ch))
DISPLAY_VOIDFUNC(ShowSidetoMove)
DISPLAY_VOIDFUNC(ShowStage)
DISPLAY_FUNC(TerminateSearch, (int sig), (sig))
DISPLAY_VOIDFUNC(UpdateClocks)
DISPLAY_FUNC(UpdateDisplay, (short f, short t, short redraw, short isspec), (f, t, redraw, isspec))
DISPLAY_VOIDFUNC(help)

DISPLAY_FUNC(doRequestInputString, (const char* fmt, char* buffer), (fmt, buffer))
void RequestInputString(char* buffer, unsigned bufsize)
{
    static char fmt[10];
    int ret = snprintf(fmt, sizeof(fmt), "%%%us", bufsize);
    if (ret >= sizeof(fmt)) {
        fprintf(stderr,
                "Insufficient format-buffer size in %s for bufsize=%u\n",
                __FUNCTION__, bufsize);
        exit(1);
    }
    doRequestInputString(fmt, buffer);
}

/*********/

#define CASE_DSPFUNC_RAW(func,args) \
  case DISPLAY_RAW:                 \
  case DISPLAY_X:                   \
    return (func args);             \
    break

#ifdef HAVE_LIBCURSES
#define CASE_DSPFUNC_CURSES(func,args) \
  case DISPLAY_CURSES:                 \
    return (func args);                \
    break;
#else
#define CASE_DSPFUNC_CURSES(func,args)
#endif

#define DISPLAY_INTFUNC(func,argsdecl,args)         \
  int func argsdecl                                 \
  {                                                 \
    switch (display_type) {                         \
      CASE_DSPFUNC_RAW(Raw_ ## func, args);         \
      CASE_DSPFUNC_CURSES(Curses_ ## func, args);   \
    }                                               \
    assert(0);                                      \
  }

DISPLAY_INTFUNC(GetString, (char* sx), (sx))
