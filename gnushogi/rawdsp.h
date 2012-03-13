/*
 * FILE: rawdsp.h
 *
 *     Raw text interface for GNU Shogi.
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

#ifndef _RAWDSP_H_
#define _RAWDSP_H_

/* The following are common to rawdsp.h and cursesdsp.h */

void Raw_ChangeAlphaWindow(void);
void Raw_ChangeBetaWindow(void);
void Raw_ChangeHashDepth(void);
void Raw_ChangeSearchDepth(void);
void Raw_ChangeXwindow(void);
void Raw_ClearScreen(void);
void Raw_Die(int sig);
void Raw_DoDebug(void);
void Raw_DoTable(short table[NO_SQUARES]);
void Raw_EditBoard(void);
void Raw_ExitShogi(void);
void Raw_GiveHint(void);
void Raw_Initialize(void);
void Raw_OutputMove(void);
void Raw_SearchStartStuff(short side);
void Raw_SelectLevel(char *sx);
void Raw_SetContempt(void);
void Raw_ShowCurrentMove(short pnt, short f, short t);
void Raw_ShowDepth(char ch);
void Raw_ShowGameType(void);
void Raw_ShowLine(unsigned short *bstline);
void Raw_ShowMessage(char *s);
void Raw_ShowPatternCount(short side, short n);
void Raw_ShowPostnValue(short sq);
void Raw_ShowPostnValues(void);
void Raw_ShowResponseTime(void);
void Raw_ShowResults(short score, unsigned short *bstline, char ch);
void Raw_ShowSidetoMove(void);
void Raw_ShowStage(void);
void Raw_TerminateSearch(int sig);
void Raw_UpdateDisplay(short f, short t, short redraw, short isspec);
void Raw_help(void);


/* The following are only found in rawdsp.h: */

void PromptForMove(void);
void SetupBoard(void);

#endif /* _RAWDSP_H_ */

