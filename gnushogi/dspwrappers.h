/*
 * FILE: dspwrappers.h
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

#ifndef _DSPWRAPPERS_H_
#define _DSPWRAPPERS_H_

#include <stdarg.h>

extern void ChangeAlphaWindow(void);
extern void ChangeBetaWindow(void);
extern void ChangeHashDepth(void);
extern void ChangeSearchDepth(void);
extern void ChangeXwindow(void);
extern void ClearScreen(void);
extern void DoDebug(void);
extern void DoTable(short table[NO_SQUARES]);
extern void EditBoard(void);
extern void ExitShogi(void);
extern void GiveHint(void);
extern void Initialize(void);
extern void ShowNodeCnt(long NodeCnt);
extern void OutputMove(void);
extern void PollForInput(void);
extern void SetContempt(void);
extern void SearchStartStuff(short side);
extern void SelectLevel(char *sx);
extern void ShowCurrentMove(short pnt, short f, short t);
extern void ShowDepth(char ch);
extern void ShowGameType(void);
extern void ShowLine(unsigned short *bstline);
extern void ShowMessage(char *s);
extern void AlwaysShowMessage(const char *format, ...);
extern void Printf(const char *format, ...);
extern void RequestInputString(char* buffer, unsigned bufsize);
extern int  GetString(char* sx);
extern void SetupBoard(void);
extern void ShowPatternCount(short side, short n);
extern void ShowPostnValue(short sq);
extern void ShowPostnValues(void);
extern void ShowPrompt(void);
extern void ShowResponseTime(void);
extern void ShowResults(short score, unsigned short *bstline, char ch);
extern void ShowSidetoMove(void);
extern void ShowStage(void);
extern void TerminateSearch(int sig);
extern void UpdateClocks(void);
extern void UpdateDisplay(short f, short t, short redraw, short isspec);
extern void help(void);

#endif /* _DSPWRAPPERS_H_ */
