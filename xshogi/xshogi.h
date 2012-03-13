/*
 * FILE: xshogi.h
 *
 *     Main header file for xshogi.
 *
 * ------------------------------------------------------------------------
 * xshogi is based on XBoard -- an Xt/Athena user interface for GNU Chess.
 *
 * Original authors:                                Dan Sears, Chris Sears
 * Enhancements (Version 2.0 and following):        Tim Mann
 * Modifications to XShogi (Version 1.0):           Matthias Mutz
 * Enhancements to XShogi (Version 1.1):            Matthias Mutz
 * Modified implementation of ISS mode for XShogi:  Matthias Mutz
 * Current maintainer:                              Michael C. Vanier
 *
 * XShogi borrows its piece bitmaps from CRANES Shogi.
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.
 * Enhancements Copyright 1992 Free Software Foundation, Inc.
 * Enhancements for XShogi Copyright 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
 *
 * The following terms apply to Digital Equipment Corporation's copyright
 * interest in XBoard:
 * ------------------------------------------------------------------------
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ------------------------------------------------------------------------
 *
 * This file is part of GNU shogi.
 *
 * GNU shogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * GNU shogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU shogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ------------------------------------------------------------------------
 *
 */

#ifndef _XSHOGI_H_
#define _XSHOGI_H_

#define BOARD_SIZE      9
#define DROP_LINES      3
#define DROP_COLS       3

#ifdef WESTERN_BITMAPS
#define LARGE_SQUARE_SIZE    32
#define MEDIUM_SQUARE_SIZE   32
#else
#define LARGE_SQUARE_SIZE    64
#define MEDIUM_SQUARE_SIZE   49
#endif

#define SMALL_SQUARE_SIZE    32
#define LINE_GAP              2
#define MAX_MOVES           512
#define MSG_SIZ             256
#define DIALOG_SIZE         256
#define MOVE_
#define MOVE_LEN             16    /* enough for "Black resigns\000"*/
#define TIME_CONTROL         "5"   /* in minutes */
#define TIME_DELAY         "1.0"   /* seconds between moves */
#define MOVES_PER_SESSION    40    /* moves per TIME_CONTROL */
#define BlackOnMove(move)    ((int) ((move) % 2) == 0)

#define BELLCHAR                '\007'
#define NULLCHAR                '\000'

#ifndef FIRST_SHOGI_PROGRAM
#define FIRST_SHOGI_PROGRAM     "gnushogi"
#endif

#ifndef SECOND_SHOGI_PROGRAM
#define SECOND_SHOGI_PROGRAM    "gnushogi"
#endif

#ifndef FIRST_HOST
#define FIRST_HOST          "localhost"
#endif

#ifndef SECOND_HOST
#define SECOND_HOST         "localhost"
#endif

#define MATCH_MODE          "False"
#define INIT_STRING         "beep\neasy\nrandom\n"
#define BLACK_STRING        "black\ngo\n"
#define WHITE_STRING        "white\ngo\n"
#define DEFAULT_SIZE        "Small"
#define LIGHT               1
#define DARK                0
#define CHAR_PIECE_COLOR    "Black"
#define ZERO_COLOR          "#000000"
#define ONE_COLOR           "#FFFFFF"
#define BLACK_PIECE_COLOR   "#FFFFD7"
#define WHITE_PIECE_COLOR   "#FFFFD7"
#define LIGHT_SQUARE_COLOR  "#EBDFB0"
#define DARK_SQUARE_COLOR   "#EBDFB0"
#define MAIN_FONT           "-*-helvetica-medium-o-normal--*-*-*-*-*-*-*-*"
#define COORD_FONT          "-*-helvetica-bold-r-normal--*-*-*-*-*-*-*-*"
#define DEFAULT_FONT   "*font: -*-helvetica-medium-r-normal--*-120-*-*-*-*-*-*"
#define BLINK_COUNT          3   
#define BORDER_X_OFFSET      3
#define BORDER_Y_OFFSET     27



typedef enum 
{
    Large, Medium, Small
} BoardSize;

typedef enum 
{
    BeginningOfGame, MachinePlaysBlack, MachinePlaysWhite, TwoMachinesPlay,
    ForceMoves, PlayFromGameFile, PauseGame, EndOfGame,
    EditPosition
} GameMode;

typedef enum 
{
    MatchFalse, MatchInit, MatchPosition, MatchOpening
} MatchMode;

typedef enum 
{
    BlackPawn,    BlackLance,   BlackKnight, BlackSilver, BlackGold, 
    BlackBishop,  BlackRook,    BlackPPawn,  BlackPLance, BlackPKnight, 
    BlackPSilver, BlackPBishop, BlackPRook,  BlackKing,
    WhitePawn,    WhiteLance,   WhiteKnight, WhiteSilver, WhiteGold, 
    WhiteBishop,  WhiteRook,    WhitePPawn,  WhitePLance, WhitePKnight, 
    WhitePSilver, WhitePBishop, WhitePRook,  WhiteKing,
    EmptySquare,
    ClearBoard,   BlackPlay,    WhitePlay /* for use on EditPosition menus */
} ShogiSquare;

typedef ShogiSquare Board[BOARD_SIZE][BOARD_SIZE];
                   
typedef int Catched[2][8];

typedef enum 
{
    BlackPromotion = 1, WhitePromotion, BlackDrop, WhiteDrop,
        NormalMove,
    BlackWins, WhiteWins, GameIsDrawn, StartGame, BadMove, Comment,
    AmbiguousMove
} ShogiMove;

typedef enum 
{
    ResetTimers, DecrementTimers, SwitchTimers, 
    ReDisplayTimers, StopTimers, StartTimers
} ClockMode;

#endif /* _XSHOGI_H_ */


