/*
 * FILE: cursesdsp.h
 *
 *     Curses interface for GNU Shogi.
 *
 * ----------------------------------------------------------------------
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
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
 * Free Software Foundation; either version 1, or (at your option) any
 * later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with GNU Shogi; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------
 *
 */

#ifndef _CURSESDSP_H_
#define _CURSESDSP_H_

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
void Curses_OutputMove(void);
void Curses_SearchStartStuff(short side);
void Curses_SelectLevel(char *sx);
void Curses_SetContempt(void);
void Curses_ShowCurrentMove(short pnt, short f, short t);
void Curses_ShowDepth(char ch);
void Curses_ShowGameType(void);
void Curses_ShowLine(unsigned short *bstline);
void Curses_ShowMessage(char *s);
void Curses_ShowPatternCount(short side, short n);
void Curses_ShowPostnValue(short sq);
void Curses_ShowPostnValues(void);
void Curses_ShowResponseTime(void);
void Curses_ShowResults(short score, unsigned short *bstline, char ch);
void Curses_ShowSidetoMove(void);
void Curses_ShowStage(void);
void Curses_TerminateSearch(int sig);
void Curses_UpdateDisplay(short f, short t, short redraw, short isspec);
void Curses_help(void);


/* The following are only found in cursesdsp.h: */

void ClearEoln(void);
void ClearMessage(void);
void DrawPiece(short sq);
void ShowHeader(void);
void ShowNodeCnt(long NodeCnt);
void ShowPlayers(void);
void ShowPrompt(void);
void ShowScore(short score);
void UpdateClocks(void);
void gotoXY(short x, short y);

#endif /* _CURSESDSP_H_ */

