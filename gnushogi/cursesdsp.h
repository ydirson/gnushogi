/*
 * FILE: cursesdsp.h
 *
 *     Curses interface for GNU Shogi.
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

#ifndef _CURSESDSP_H_
#define _CURSESDSP_H_

#include "gnushogi.h"

/* The following are common to rawdsp.h and cursesdsp.h */

void Curses_ChangeAlphaWindow(void);
void Curses_ChangeBetaWindow(void);
void Curses_ChangeHashDepth(void);
void Curses_ChangeSearchDepth(void);
void Curses_ChangeXwindow(void);
void Curses_ClearScreen(void);
void Curses_Die(int sig);
void Curses_DoDebug(void);
void Curses_DoTable(short table[NO_SQUARES]);
void Curses_EditBoard(void);
void Curses_ExitShogi(void);
void Curses_GiveHint(void);
void Curses_Initialize(void);
void Curses_ShowNodeCnt(long NodeCnt);
void Curses_OutputMove(void);
void Curses_PollForInput(void);
void Curses_SearchStartStuff(short side);
void Curses_SelectLevel(char *sx);
void Curses_SetContempt(void);
void Curses_ShowCurrentMove(short pnt, short f, short t);
void Curses_ShowDepth(char ch);
void Curses_ShowGameType(void);
void Curses_ShowLine(unsigned short *bstline);
void Curses_ShowMessage(char *s);
void Curses_AlwaysShowMessage(const char *format, va_list ap);
void Curses_Printf(const char *format, va_list ap);
void Curses_doRequestInputString(const char* fmt, char* buffer);
int  Curses_GetString(char* sx);
void Curses_SetupBoard(void);
void Curses_ShowPatternCount(short side, short n);
void Curses_ShowPostnValue(short sq);
void Curses_ShowPostnValues(void);
void Curses_ShowPrompt(void);
void Curses_ShowResponseTime(void);
void Curses_ShowResults(short score, unsigned short *bstline, char ch);
void Curses_ShowSidetoMove(void);
void Curses_ShowStage(void);
void Curses_TerminateSearch(int sig);
void Curses_UpdateClocks(void);
void Curses_UpdateDisplay(short f, short t, short redraw, short isspec);
void Curses_help(void);

#endif /* _CURSESDSP_H_ */
