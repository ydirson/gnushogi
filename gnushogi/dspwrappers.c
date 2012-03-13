/*
 * FILE: dspwrappers.c
 *
 *     Wrapper functions which call analogous functions in rawdsp.c
 *     or cursesdsp.c depending on the interface.
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

#include "gnushogi.h"
#include "dspwrappers.h"
#include "rawdsp.h"
#include "cursesdsp.h"


void
ChangeAlphaWindow(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ChangeAlphaWindow();
        break;

    case DISPLAY_CURSES:
        Curses_ChangeAlphaWindow();
        break;
    }
}


void
ChangeBetaWindow(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ChangeBetaWindow();
        break;

    case DISPLAY_CURSES:
        Curses_ChangeBetaWindow();
        break;
    }
}


void
ChangeHashDepth(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ChangeHashDepth();
        break;

    case DISPLAY_CURSES:
        Curses_ChangeHashDepth();
        break;
    }
}


void
ChangeSearchDepth(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ChangeSearchDepth();
        break;

    case DISPLAY_CURSES:
        Curses_ChangeSearchDepth();
        break;
    }
}


void
ChangeXwindow(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ChangeXwindow();
        break;

    case DISPLAY_CURSES:
        Curses_ChangeXwindow();
        break;
    }
}


void
ClearScreen(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ClearScreen();
        break;

    case DISPLAY_CURSES:
        Curses_ClearScreen();
        break;
    }
}


void
DoDebug(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_DoDebug();
        break;

    case DISPLAY_CURSES:
        Curses_DoDebug();
        break;
    }
}


void
DoTable(short table[NO_SQUARES])
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_DoTable(table);
        break;

    case DISPLAY_CURSES:
        Curses_DoTable(table);
        break;
    }
}


void
EditBoard(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_EditBoard();
        break;

    case DISPLAY_CURSES:
        Curses_EditBoard();
        break;
    }
}


void
ExitShogi(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ExitShogi();
        break;

    case DISPLAY_CURSES:
        Curses_ExitShogi();
        break;
    }
}


void
GiveHint(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_GiveHint();
        break;

    case DISPLAY_CURSES:
        Curses_GiveHint();
        break;
    }
}


void
Initialize(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_Initialize();
        break;

    case DISPLAY_CURSES:
        Curses_Initialize();
        break;
    }
}


void
OutputMove(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_OutputMove();
        break;

    case DISPLAY_CURSES:
        Curses_OutputMove();
        break;
    }
}


void
SetContempt(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_SetContempt();
        break;

    case DISPLAY_CURSES:
        Curses_SetContempt();
        break;
    }
}


void
SearchStartStuff(short side)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_SearchStartStuff(side);
        break;

    case DISPLAY_CURSES:
        Curses_SearchStartStuff(side);
        break;
    }
}


void
SelectLevel(char *sx)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_SelectLevel(sx);
        break;

    case DISPLAY_CURSES:
        Curses_SelectLevel(sx);
        break;
    }
}


void
ShowCurrentMove(short pnt, short f, short t)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowCurrentMove(pnt, f, t);
        break;

    case DISPLAY_CURSES:
        Curses_ShowCurrentMove(pnt, f, t);
        break;
    }
}


void
ShowDepth(char ch)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowDepth(ch);
        break;

    case DISPLAY_CURSES:
        Curses_ShowDepth(ch);
        break;
    }
}


void
ShowGameType(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowGameType();
        break;

    case DISPLAY_CURSES:
        Curses_ShowGameType();
        break;
    }
}


void
ShowLine(unsigned short *bstline)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowLine(bstline);
        break;

    case DISPLAY_CURSES:
        Curses_ShowLine(bstline);
        break;
    }
}


void
ShowMessage(char *s)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowMessage(s);
        break;

    case DISPLAY_CURSES:
        Curses_ShowMessage(s);
        break;
    }
}


void
ShowPatternCount(short side, short n)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowPatternCount(side, n);
        break;

    case DISPLAY_CURSES:
        Curses_ShowPatternCount(side, n);
        break;
    }
}


void
ShowPostnValue(short sq)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowPostnValue(sq);
        break;

    case DISPLAY_CURSES:
        Curses_ShowPostnValue(sq);
        break;
    }
}


void
ShowPostnValues(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowPostnValues();
        break;

    case DISPLAY_CURSES:
        Curses_ShowPostnValues();
        break;
    }
}


void
ShowResponseTime(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowResponseTime();
        break;

    case DISPLAY_CURSES:
        Curses_ShowResponseTime();
        break;
    }
}


void
ShowResults(short score, unsigned short *bstline, char ch)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowResults(score, bstline, ch);
        break;

    case DISPLAY_CURSES:
        Curses_ShowResults(score, bstline, ch);
        break;
    }
}


void
ShowSidetoMove(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowSidetoMove();
        break;

    case DISPLAY_CURSES:
        Curses_ShowSidetoMove();
        break;
    }
}


void
ShowStage(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_ShowStage();
        break;

    case DISPLAY_CURSES:
        Curses_ShowStage();
        break;
    }
}


void
TerminateSearch(int sig)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_TerminateSearch(sig);
        break;

    case DISPLAY_CURSES:
        Curses_TerminateSearch(sig);
        break;
    }
}


void
UpdateDisplay(short f, short t, short redraw, short isspec)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_UpdateDisplay(f, t, redraw, isspec);
        break;

    case DISPLAY_CURSES:
        Curses_UpdateDisplay(f, t, redraw, isspec);
        break;
    }
}


void
help(void)
{
    switch (display_type)
    {
    case DISPLAY_RAW:
    case DISPLAY_X:
        Raw_help();
        break;

    case DISPLAY_CURSES:
        Curses_help();
        break;
    }
}

