/*
 * FILE: xshogi.c
 *
 *     Implementation of the X interface for GNU shogi (xshogi).
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
 * XShogi borrows some of its piece bitmaps from CRANES Shogi.
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

#include "config.h"

#ifdef X_DISPLAY_MISSING
#error You cannot compile xshogi if X windows is unavailable!
#endif

#include "sysdeps.h"

#define XBOARD_VERSION "2.0/2.1"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <time.h>
#include <pwd.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/cursorfont.h>

#include "../version.h"
#include "xshogi.h"

#define BUF_SIZE 1024
#define BOARD    1
#define MOVES    2

#include "bitmaps.h"  /* Piece bitmaps. */
#include "xshogifn.h" /* Forward declarations. */

#define off_board(x) ((x < 2) || (x > BOARD_SIZE + 1))


/**********************************************************************
 *
 * Global variables, structs etc.
 *
 **********************************************************************/

/*
 * NOTE: XShogi depends on Xt R4 or higher
 */

int xtVersion = XtSpecificationRelease;

XtIntervalId firstProgramXID = 0, secondProgramXID = 0,
    readGameXID = 0, timerXID = 0, blinkSquareXID = 0;

XtAppContext appContext;

Boolean (*fileProc) (char *name);

FILE *fromFirstProgFP, *toFirstProgFP, *fromSecondProgFP,
    *toSecondProgFP, *gameFileFP, *lastMsgFP;

int currentMove = 0, forwardMostMove = 0, backwardMostMove = 0,
    firstProgramPID = 0,
    secondProgramPID = 0, fromX = -1,
    fromY = -1, firstMove = True, flipView = False,
    xshogiDebug = True, commentUp = False, filenameUp = False,
    whitePlaysFirst = False, startedFromSetupPosition = False,
    searchTime = 0, pmFromX = -1, pmFromY = -1,
    blackFlag = False, whiteFlag = False, maybeThinking = False,
    filemodeUp = False;

int at_least_gnushogi_1_2p03 = False;

int firstSendTime = 2, secondSendTime = 2; /* 0 = don't, 1 = do,
                                              2 = test first */

MatchMode matchMode         = MatchFalse;
GameMode  gameMode          = BeginningOfGame;
GameMode  lastGameMode      = BeginningOfGame;
GameMode  pausePreviousMode = BeginningOfGame;

char moveList[MAX_MOVES][MOVE_LEN], parseList[MAX_MOVES][MOVE_LEN * 2],
    ptyname[24], *shogiDir, *programName;

char endMessage[MOVE_LEN * 4];

long blackTimeRemaining, whiteTimeRemaining, timeControl;
long timeRemaining[2][MAX_MOVES];

extern char currentMoveString[];

int updateRemotePlayer = False;

Catched catches[MAX_MOVES];

#define DIMENSION 100

Widget blackPieceMenu, whitePieceMenu, commentShell;

XSetWindowAttributes attr;

#define pawn      0
#define lance     1
#define knight    2
#define silver    3
#define gold      4
#define bishop    5
#define rook      6
#define king      7
#define no_piece  8
#define ppawn     9
#define plance   10
#define pknight  11
#define psilver  12
#define pbishop  13

#define NO_PIECES  15
#define NO_SQUARES 81
#define NO_COLS     9
#define NO_ROWS     9


char catchedIndexToChar[8] =
{
    'P', 'L', 'N', 'S', 'G', 'B', 'R', 'K'
};

ShogiSquare catchedIndexToPiece[2][8] =
{
    {
        BlackPawn, BlackLance, BlackKnight, BlackSilver, BlackGold,
        BlackBishop, BlackRook, BlackKing
    },
    {
        WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, WhiteGold,
        WhiteBishop, WhiteRook, WhiteKing
    }
};


int pieceToCatchedIndex[] =
{
    pawn, lance, knight, silver, gold, bishop, rook,
    pawn, lance, knight, silver, bishop, rook, king,
    pawn, lance, knight, silver, gold, bishop, rook,
    pawn, lance, knight, silver, bishop, rook, king,
    no_piece
};



Board boards[MAX_MOVES];
Board initialPosition =
{
    { BlackLance,  BlackKnight, BlackSilver, BlackGold, BlackKing,
      BlackGold,   BlackSilver, BlackKnight, BlackLance },
    { EmptySquare, BlackBishop, EmptySquare, EmptySquare, EmptySquare,
      EmptySquare, EmptySquare, BlackRook,   EmptySquare },
    { BlackPawn,   BlackPawn,   BlackPawn,   BlackPawn, BlackPawn,
      BlackPawn,   BlackPawn,   BlackPawn,   BlackPawn },
    { EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare,
      EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,
    { EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare,
      EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,
    { EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare,
      EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,
    { WhitePawn,   WhitePawn,   WhitePawn,   WhitePawn, WhitePawn,
      WhitePawn,   WhitePawn,   WhitePawn,   WhitePawn },
    { EmptySquare, WhiteRook,   EmptySquare, EmptySquare, EmptySquare,
      EmptySquare, EmptySquare, WhiteBishop, EmptySquare },
    { WhiteLance,  WhiteKnight, WhiteSilver, WhiteGold, WhiteKing,
      WhiteGold,   WhiteSilver, WhiteKnight, WhiteLance }
};

String gnuButtonStrings[] =
{
    "Quit",       "Load Game",      "Machine White",  "Forward",
    "Reset",      "Load Position",  "Machine Black",  "Backward",
    "Flip View",  "Save Game",      "Force Moves",    "Pause",
    "Hint",       "Save Position",  "Two Machines",   "Edit Position",
    "Challenge",  "Select Level",   "Move NOW",
};

/* must be in same order as buttonStrings! */
XtActionProc gnuButtonProcs[] =
{
    QuitProc,       LoadGameProc,      MachineWhiteProc,  ForwardProc,
    ResetProc,      LoadPositionProc,  MachineBlackProc,  BackwardProc,
    FlipViewProc,   SaveGameProc,      ForceProc,         PauseProc,
    HintProc,       SavePositionProc,  TwoMachinesProc,   EditPositionProc,
    ChallengeProc,  SelectLevelProc,   MoveNowProc,
    NULL
};


String *buttonStrings;
XtActionProc *buttonProcs;
int buttonCount;

#define PIECE_MENU_SIZE 18

String pieceMenuStrings[PIECE_MENU_SIZE] =
{
    "----",     "Pawn",          "Lance",       "Knight", "Silver",
    "Gold",     "Bishop",        "Rook",
    "PPawn",    "PLance",        "PKnight",     "PSilver",
    "PBishop",  "PRook",         "King",
    "----",     "Empty square",  "Clear board"
};

/* must be in same order as PieceMenuStrings! */
ShogiSquare pieceMenuTranslation[2][PIECE_MENU_SIZE] =
{
    {
        (ShogiSquare) 0,  BlackPawn,    BlackLance,    BlackKnight,
        BlackSilver,      BlackGold,    BlackBishop,   BlackRook,
        BlackPPawn,       BlackPLance,  BlackPKnight,  BlackPSilver,
        BlackPBishop,     BlackPRook,   BlackKing,
        (ShogiSquare) 0,  EmptySquare,  ClearBoard
    },
    {
        (ShogiSquare) 0,  WhitePawn,    WhiteLance,    WhiteKnight,
        WhiteSilver,      WhiteGold,    WhiteBishop,   WhiteRook,
        WhitePPawn,       WhitePLance,  WhitePKnight,  WhitePSilver,
        WhitePBishop,     WhitePRook,   WhiteKing,
        (ShogiSquare) 0,  EmptySquare,  ClearBoard
    },
};


typedef struct
{
    Pixel pieceColor;
    Pixel squareColor;
    Pixel charPieceColor;
    Pixel zeroColor;
    Pixel oneColor;
    Boolean westernPieceSet;
    int movesPerSession;
    String initString;
    String blackString;
    String whiteString;
    String firstShogiProgram;
    String secondShogiProgram;
    Boolean noShogiProgram;
    String firstHost;
    String secondHost;
    String reverseBigSolidBitmap;
    String reverseSmallSolidBitmap;
    String normalBigSolidBitmap;
    String normalSmallSolidBitmap;
    String reversePawnBitmap;
    String reverseLanceBitmap;
    String reverseKnightBitmap;
    String reverseSilverBitmap;
    String reverseGoldBitmap;
    String reverseRookBitmap;
    String reverseBishopBitmap;
    String reversePPawnBitmap;
    String reversePLanceBitmap;
    String reversePKnightBitmap;
    String reversePSilverBitmap;
    String reversePBishopBitmap;
    String reversePRookBitmap;
    String reverseKingBitmap;
    String normalPawnBitmap;
    String normalLanceBitmap;
    String normalKnightBitmap;
    String normalSilverBitmap;
    String normalGoldBitmap;
    String normalRookBitmap;
    String normalBishopBitmap;
    String normalPPawnBitmap;
    String normalPLanceBitmap;
    String normalPKnightBitmap;
    String normalPSilverBitmap;
    String normalPBishopBitmap;
    String normalPRookBitmap;
    String normalKingBitmap;
    String remoteShell;
    float  timeDelay;
    String timeControl;
    String gameIn;

    Boolean autoSaveGames;
    String loadGameFile;
    String loadPositionFile;
    String saveGameFile;
    String savePositionFile;
    String matchMode;
    String challengeDisplay;
    Boolean monoMode;
    Boolean debugMode;
    Boolean clockMode;
    String boardSize;
    Boolean Iconic;
    String searchTime;
    int searchDepth;
    Boolean showCoords;
    String mainFont;
    String coordFont;
    Boolean ringBellAfterMoves;
    Boolean autoCallFlag;
    int borderXoffset;
    int borderYoffset;
} AppData, *AppDataPtr;


XtResource clientResources[] =
{
    {
        "pieceColor", "pieceColor", XtRPixel, sizeof(Pixel),
        XtOffset(AppDataPtr, pieceColor), XtRString,
        PIECE_COLOR
    },
    {
        "charPieceColor", "CharPieceColor", XtRPixel, sizeof(Pixel),
        XtOffset(AppDataPtr, charPieceColor), XtRString,
        CHAR_PIECE_COLOR
    },
    {
        "oneColor", "OneColor", XtRPixel, sizeof(Pixel),
        XtOffset(AppDataPtr, oneColor), XtRString,
        ONE_COLOR
    },
    {
        "zeroColor", "ZeroColor", XtRPixel, sizeof(Pixel),
        XtOffset(AppDataPtr, zeroColor), XtRString,
        ZERO_COLOR
    },
    {
        "squareColor", "squareColor", XtRPixel,
        sizeof(Pixel), XtOffset(AppDataPtr, squareColor),
        XtRString, SQUARE_COLOR
    },
    {
        "westernPieceSet", "WesternPieceSet", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, westernPieceSet), XtRString,
        (XtPointer) False
    },
    {
        "movesPerSession", "movesPerSession", XtRInt, sizeof(int),
        XtOffset(AppDataPtr, movesPerSession), XtRImmediate,
        (XtPointer) MOVES_PER_SESSION
    },
    {
        "initString", "initString", XtRString, sizeof(String),
        XtOffset(AppDataPtr, initString), XtRString, INIT_STRING
    },
    {
        "blackString", "blackString", XtRString, sizeof(String),
        XtOffset(AppDataPtr, blackString), XtRString, BLACK_STRING
    },
    {
        "whiteString", "whiteString", XtRString, sizeof(String),
        XtOffset(AppDataPtr, whiteString), XtRString, WHITE_STRING
    },
    {
        "firstShogiProgram", "firstShogiProgram", XtRString,
        sizeof(String), XtOffset(AppDataPtr, firstShogiProgram),
        XtRString, FIRST_SHOGI_PROGRAM
    },
    {
        "secondShogiProgram", "secondShogiProgram", XtRString,
        sizeof(String), XtOffset(AppDataPtr, secondShogiProgram),
        XtRString, SECOND_SHOGI_PROGRAM
    },
    {
        "noShogiProgram", "noShogiProgram", XtRBoolean,
        sizeof(Boolean), XtOffset(AppDataPtr, noShogiProgram),
        XtRImmediate, (XtPointer) False
    },
    {
        "firstHost", "firstHost", XtRString, sizeof(String),
        XtOffset(AppDataPtr, firstHost), XtRString, FIRST_HOST
    },
    {
        "secondHost", "secondHost", XtRString, sizeof(String),
        XtOffset(AppDataPtr, secondHost), XtRString, SECOND_HOST
    },
    {
        "reversePawnBitmap", "reversePawnBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePawnBitmap),
        XtRString, NULL
    },
    {
        "reverseLanceBitmap", "reverseLanceBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseLanceBitmap),
        XtRString, NULL
    },
    {
        "reverseKnightBitmap", "reverseKnightBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseKnightBitmap),
        XtRString, NULL
    },
    {
        "reverseSilverBitmap", "reverseSilverBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseSilverBitmap),
        XtRString, NULL
    },
    {
        "reverseGoldBitmap", "reverseGoldBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseGoldBitmap),
        XtRString, NULL
    },
    {
        "reverseRookBitmap", "reverseRookBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseRookBitmap),
        XtRString, NULL
    },
    {
        "reverseBishopBitmap", "reverseBishopBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseBishopBitmap),
        XtRString, NULL
    },
    {
        "reversePPawnBitmap", "reversePPawnBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePPawnBitmap),
        XtRString, NULL
    },
    {
        "reversePLanceBitmap", "reversePLanceBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePLanceBitmap),
        XtRString, NULL
    },
    {
        "reversePKnightBitmap", "reversePKnightBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePKnightBitmap),
        XtRString, NULL
    },
    {
        "reversePSilverBitmap", "reversePSilverBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePSilverBitmap),
        XtRString, NULL
    },
    {
        "reversePRookBitmap", "reversePRookBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePRookBitmap),
        XtRString, NULL
    },
    {
        "reversePBishopBitmap", "reversePBishopBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reversePBishopBitmap),
        XtRString, NULL
    },
    {
        "reverseKingBitmap", "reverseKingBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, reverseKingBitmap),
        XtRString, NULL
    },
    {
        "normalPawnBitmap", "normalPawnBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPawnBitmap),
        XtRString, NULL
    },
    {
        "normalLanceBitmap", "normalLanceBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalLanceBitmap),
        XtRString, NULL
    },
    {
        "normalKnightBitmap", "normalKnightBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalKnightBitmap),
        XtRString, NULL
    },
    {
        "normalSilverBitmap", "normalSilverBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalSilverBitmap),
        XtRString, NULL
    },
    {
        "normalGoldBitmap", "normalGoldBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalGoldBitmap),
        XtRString, NULL
    },
    {
        "normalBishopBitmap", "normalBishopBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalBishopBitmap),
        XtRString, NULL
    },
    {
        "normalRookBitmap", "normalRookBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalRookBitmap),
        XtRString, NULL
    },
    {
        "normalPPawnBitmap", "normalPPawnBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPPawnBitmap),
        XtRString, NULL
    },
    {
        "normalPLanceBitmap", "normalPLanceBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPLanceBitmap),
        XtRString, NULL
    },
    {
        "normalPKnightBitmap", "normalPKnightBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPKnightBitmap),
        XtRString, NULL
    },
    {
        "normalPSilverBitmap", "normalPSilverBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPSilverBitmap),
        XtRString, NULL
    },
    {
        "normalPBishopBitmap", "normalPBishopBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPBishopBitmap),
        XtRString, NULL
    },
    {
        "normalPRookBitmap", "normalPRookBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalPRookBitmap),
        XtRString, NULL
    },
    {
        "normalKingBitmap", "normalKingBitmap", XtRString,
        sizeof(String), XtOffset(AppDataPtr, normalKingBitmap),
        XtRString, NULL
    },
    {
        "remoteShell", "remoteShell", XtRString, sizeof(String),
        XtOffset(AppDataPtr, remoteShell), XtRString, "rsh"
    },
    {
        "timeDelay", "timeDelay", XtRFloat, sizeof(float),
        XtOffset(AppDataPtr, timeDelay), XtRString,
        (XtPointer) TIME_DELAY
    },
    {
        "timeControl", "timeControl", XtRString, sizeof(String),
        XtOffset(AppDataPtr, timeControl), XtRString,
        (XtPointer) TIME_CONTROL
    },
    {
        "gameIn", "gameIn",
        XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, gameIn), XtRImmediate,
        (XtPointer) False
    },
    {
        "autoSaveGames", "autoSaveGames", XtRBoolean,
        sizeof(Boolean), XtOffset(AppDataPtr, autoSaveGames),
        XtRImmediate, (XtPointer) False
    },
    {
        "loadGameFile", "loadGameFile", XtRString, sizeof(String),
        XtOffset(AppDataPtr, loadGameFile), XtRString, NULL
    },
    {
        "loadPositionFile", "loadPositionFile", XtRString,
        sizeof(String), XtOffset(AppDataPtr, loadPositionFile),
        XtRString, NULL
    },
    {
        "saveGameFile", "saveGameFile", XtRString, sizeof(String),
        XtOffset(AppDataPtr, saveGameFile), XtRString, ""
    },
    {
        "savePositionFile", "savePositionFile", XtRString,
        sizeof(String), XtOffset(AppDataPtr, savePositionFile),
        XtRString, ""
    },
    {
        "challengeDisplay", "challengeDisplay", XtRString,
        sizeof(String), XtOffset(AppDataPtr, challengeDisplay),
        XtRString, NULL
    },
    {
        "matchMode", "matchMode", XtRString, sizeof(String),
        XtOffset(AppDataPtr, matchMode), XtRString, MATCH_MODE
    },
    {
        "monoMode", "monoMode", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, monoMode), XtRImmediate,
        (XtPointer) False
    },
    {
        "debugMode", "debugMode", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, debugMode), XtRImmediate,
        (XtPointer) False
    },
    {
        "Iconic", "Iconic", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, Iconic), XtRImmediate,
        (XtPointer) False
    },
    {
        "clockMode", "clockMode", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, clockMode), XtRImmediate,
        (XtPointer) True
    },
    {
        "autoCallFlag", "autoCallFlag", XtRBoolean,
        sizeof(Boolean), XtOffset(AppDataPtr, autoCallFlag),
        XtRImmediate, (XtPointer) False
    },
    {
        "boardSize", "boardSize", XtRString, sizeof(String),
        XtOffset(AppDataPtr, boardSize), XtRString, DEFAULT_SIZE
    },
    {
        "searchTime", "searchTime", XtRString, sizeof(String),
        XtOffset(AppDataPtr, searchTime), XtRString,
        (XtPointer) NULL
    },
    {
        "searchDepth", "searchDepth", XtRInt, sizeof(int),
        XtOffset(AppDataPtr, searchDepth), XtRImmediate,
        (XtPointer) 0
    },
    {
        "showCoords", "showCoords", XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, showCoords), XtRImmediate,
        (XtPointer) False
    },
    {
        "mainFont", "mainFont", XtRString, sizeof(String),
        XtOffset(AppDataPtr, mainFont), XtRString, MAIN_FONT
    },
    {
        "coordFont", "coordFont", XtRString, sizeof(String),
        XtOffset(AppDataPtr, coordFont), XtRString, COORD_FONT
    },
    {
        "ringBellAfterMoves", "ringBellAfterMoves",
        XtRBoolean, sizeof(Boolean),
        XtOffset(AppDataPtr, ringBellAfterMoves),
        XtRImmediate, (XtPointer) False
    },
    {
        "borderXoffset", "borderXoffset", XtRInt, sizeof(int),
        XtOffset(AppDataPtr, borderXoffset), XtRImmediate,
        (XtPointer) BORDER_X_OFFSET
    },
    {
        "borderYoffset", "borderYOffset", XtRInt, sizeof(int),
        XtOffset(AppDataPtr, borderYoffset), XtRImmediate,
        (XtPointer) BORDER_Y_OFFSET
    }
};


struct DisplayData
{
    AppData appData;

    Arg shellArgs[6];
    Arg boardArgs[3];
    Arg commandsArgs[7];
    Arg messageArgs[3];
    Arg timerArgs[2];
    Arg titleArgs[2];

    Pixmap reversePawnBitmap, reverseLanceBitmap,   reverseKnightBitmap,
        reverseSilverBitmap,
        reverseGoldBitmap,    reverseBishopBitmap,  reverseRookBitmap,
        reversePPawnBitmap,   reversePLanceBitmap,  reversePKnightBitmap,
        reversePSilverBitmap, reversePBishopBitmap, reversePRookBitmap,
        reverseKingBitmap,
        reverseBigSolidBitmap, reverseSmallSolidBitmap,
        normalBigSolidBitmap,  normalSmallSolidBitmap,
        normalPawnBitmap,      normalLanceBitmap,   normalKnightBitmap,
        normalSilverBitmap,    normalGoldBitmap,
        normalBishopBitmap,    normalRookBitmap,
        normalPPawnBitmap,     normalPLanceBitmap,  normalPKnightBitmap,
        normalPSilverBitmap,   normalPBishopBitmap, normalPRookBitmap,
        normalKingBitmap,
        iconPixmap;

    Display *xDisplay;
    int xScreen;
    Window xBoardWindow;

    GC squareGC, lineGC, pieceGC, oPieceGC, charPieceGC,
        squareOffBoardGC, coordGC, dropPiece;

    Font mainFontID, coordFontID;
    XFontStruct *mainFontStruct, *coordFontStruct;

    Widget shellWidget, formWidget, boardWidget,
        commandsWidget, messageWidget,
        blackTimerWidget, whiteTimerWidget,
        titleWidget, widgetList[6],
        promotionShell,
        filemodeShell, challengeWidget;

    XSegment gridSegments[(BOARD_SIZE + 1) * 2];

    Pixel timerForegroundPixel, timerBackgroundPixel;

    BoardSize boardSize;
    int squareSize;
    int black_pixel_is_zero;
    int flipView;
    int promotionUp;

    Boolean monoMode, showCoords, Iconic;
};



struct DisplayData localPlayer, remotePlayer;


typedef struct
{
    ShogiSquare piece;
    int to_x, to_y;
} PromotionMoveInfo;

static PromotionMoveInfo pmi;  /* making this global is gross */


Pixmap *pieceToReverse[2][28] =
{
    {
        &localPlayer.reversePawnBitmap,
        &localPlayer.reverseLanceBitmap,
        &localPlayer.reverseKnightBitmap,
        &localPlayer.reverseSilverBitmap,
        &localPlayer.reverseGoldBitmap,
        &localPlayer.reverseBishopBitmap,
        &localPlayer.reverseRookBitmap,
        &localPlayer.reversePPawnBitmap,
        &localPlayer.reversePLanceBitmap,
        &localPlayer.reversePKnightBitmap,
        &localPlayer.reversePSilverBitmap,
        &localPlayer.reversePBishopBitmap,
        &localPlayer.reversePRookBitmap,
        &localPlayer.reverseKingBitmap,
        &localPlayer.reversePawnBitmap,
        &localPlayer.reverseLanceBitmap,
        &localPlayer.reverseKnightBitmap,
        &localPlayer.reverseSilverBitmap,
        &localPlayer.reverseGoldBitmap,
        &localPlayer.reverseBishopBitmap,
        &localPlayer.reverseRookBitmap,
        &localPlayer.reversePPawnBitmap,
        &localPlayer.reversePLanceBitmap,
        &localPlayer.reversePKnightBitmap,
        &localPlayer.reversePSilverBitmap,
        &localPlayer.reversePBishopBitmap,
        &localPlayer.reversePRookBitmap,
        &localPlayer.reverseKingBitmap
    },
    {
        &remotePlayer.reversePawnBitmap,
        &remotePlayer.reverseLanceBitmap,
        &remotePlayer.reverseKnightBitmap,
        &remotePlayer.reverseSilverBitmap,
        &remotePlayer.reverseGoldBitmap,
        &remotePlayer.reverseBishopBitmap,
        &remotePlayer.reverseRookBitmap,
        &remotePlayer.reversePPawnBitmap,
        &remotePlayer.reversePLanceBitmap,
        &remotePlayer.reversePKnightBitmap,
        &remotePlayer.reversePSilverBitmap,
        &remotePlayer.reversePBishopBitmap,
        &remotePlayer.reversePRookBitmap,
        &remotePlayer.reverseKingBitmap,
        &remotePlayer.reversePawnBitmap,
        &remotePlayer.reverseLanceBitmap,
        &remotePlayer.reverseKnightBitmap,
        &remotePlayer.reverseSilverBitmap,
        &remotePlayer.reverseGoldBitmap,
        &remotePlayer.reverseBishopBitmap,
        &remotePlayer.reverseRookBitmap,
        &remotePlayer.reversePPawnBitmap,
        &remotePlayer.reversePLanceBitmap,
        &remotePlayer.reversePKnightBitmap,
        &remotePlayer.reversePSilverBitmap,
        &remotePlayer.reversePBishopBitmap,
        &remotePlayer.reversePRookBitmap,
        &remotePlayer.reverseKingBitmap
    }
};



Pixmap *pieceToNormal[2][28] =
{
    {
        &localPlayer.normalPawnBitmap,
        &localPlayer.normalLanceBitmap,
        &localPlayer.normalKnightBitmap,
        &localPlayer.normalSilverBitmap,
        &localPlayer.normalGoldBitmap,
        &localPlayer.normalBishopBitmap,
        &localPlayer.normalRookBitmap,
        &localPlayer.normalPPawnBitmap,
        &localPlayer.normalPLanceBitmap,
        &localPlayer.normalPKnightBitmap,
        &localPlayer.normalPSilverBitmap,
        &localPlayer.normalPBishopBitmap,
        &localPlayer.normalPRookBitmap,
        &localPlayer.normalKingBitmap,
        &localPlayer.normalPawnBitmap,
        &localPlayer.normalLanceBitmap,
        &localPlayer.normalKnightBitmap,
        &localPlayer.normalSilverBitmap,
        &localPlayer.normalGoldBitmap,
        &localPlayer.normalBishopBitmap,
        &localPlayer.normalRookBitmap,
        &localPlayer.normalPPawnBitmap,
        &localPlayer.normalPLanceBitmap,
        &localPlayer.normalPKnightBitmap,
        &localPlayer.normalPSilverBitmap,
        &localPlayer.normalPBishopBitmap,
        &localPlayer.normalPRookBitmap,
        &localPlayer.normalKingBitmap
    },
    {
        &remotePlayer.normalPawnBitmap,
        &remotePlayer.normalLanceBitmap,
        &remotePlayer.normalKnightBitmap,
        &remotePlayer.normalSilverBitmap,
        &remotePlayer.normalGoldBitmap,
        &remotePlayer.normalBishopBitmap,
        &remotePlayer.normalRookBitmap,
        &remotePlayer.normalPPawnBitmap,
        &remotePlayer.normalPLanceBitmap,
        &remotePlayer.normalPKnightBitmap,
        &remotePlayer.normalPSilverBitmap,
        &remotePlayer.normalPBishopBitmap,
        &remotePlayer.normalPRookBitmap,
        &remotePlayer.normalKingBitmap,
        &remotePlayer.normalPawnBitmap,
        &remotePlayer.normalLanceBitmap,
        &remotePlayer.normalKnightBitmap,
        &remotePlayer.normalSilverBitmap,
        &remotePlayer.normalGoldBitmap,
        &remotePlayer.normalBishopBitmap,
        &remotePlayer.normalRookBitmap,
        &remotePlayer.normalPPawnBitmap,
        &remotePlayer.normalPLanceBitmap,
        &remotePlayer.normalPKnightBitmap,
        &remotePlayer.normalPSilverBitmap,
        &remotePlayer.normalPBishopBitmap,
        &remotePlayer.normalPRookBitmap,
        &remotePlayer.normalKingBitmap
    }
};



Pixmap *pieceToReverseSolid[2][28] =
{
    {
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap
    },
    {
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap
    }
};



Pixmap *pieceToNormalSolid[2][28] =
{
    {
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap
    },
    {
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap
    }
};



int pieceIsPromoted[] =
{
    False, False, False, False, False, False, False,
    True,  True,  True,  True,  True,  True,  False,
    False, False, False, False, False, False, False,
    True,  True,  True,  True,  True,  True,  False,
    False
};


int piecePromotable[] =
{
    True,  True,  True,  True,  False, True,  True,
    False, False, False, False, False, False, False,
    True,  True,  True,  True,  False, True,  True,
    False, False, False, False, False, False, False,
    False
};


char pieceToChar[] =
{
    'P', 'L', 'N', 'S', 'G', 'B', 'R', 'P', 'L', 'N', 'S', 'B', 'R', 'K',
    'p', 'l', 'n', 's', 'g', 'b', 'r', 'p', 'l', 'n', 's', 'b', 'r', 'k',
    '.'
};


ShogiSquare pieceToPromoted[] =
{
    BlackPPawn,   BlackPLance, BlackPKnight, BlackPSilver, BlackGold,
    BlackPBishop, BlackPRook,
    BlackPPawn,   BlackPLance, BlackPKnight, BlackPSilver,
    BlackPBishop, BlackPRook,  BlackKing,
    WhitePPawn,   WhitePLance, WhitePKnight, WhitePSilver, WhiteGold,
    WhitePBishop, WhitePRook,
    WhitePPawn,   WhitePLance, WhitePKnight, WhitePSilver,
    WhitePBishop, WhitePRook,  WhiteKing
};



XrmOptionDescRec shellOptions[] =
{
    { "-pieceColor", "pieceColor", XrmoptionSepArg, NULL },
    { "-pc", "pieceColor", XrmoptionSepArg, NULL },
    { "-charPieceColor", "charPieceColor", XrmoptionSepArg, NULL },
    { "-cpc", "charPieceColor", XrmoptionSepArg, NULL },
    { "-zeroColor", "zeroColor", XrmoptionSepArg, NULL },
    { "-zc", "zeroColor", XrmoptionSepArg, NULL },
    { "-oneColor", "oneColor", XrmoptionSepArg, NULL },
    { "-oc", "oneColor", XrmoptionSepArg, NULL },
    { "-squareColor", "squareColor", XrmoptionSepArg, NULL },
    { "-sc", "squareColor", XrmoptionSepArg, NULL },
    { "-westernPieceSet", "westernPieceSet", XrmoptionSepArg, NULL },
    { "-wps", "westernPieceSet", XrmoptionSepArg, NULL },
    { "-movesPerSession", "movesPerSession", XrmoptionSepArg, NULL },
    { "-mps", "movesPerSession", XrmoptionSepArg, NULL },
    { "-firstShogiProgram", "firstShogiProgram", XrmoptionSepArg, NULL },
    { "-fsp", "firstShogiProgram", XrmoptionSepArg, NULL },
    { "-secondShogiProgram", "secondShogiProgram", XrmoptionSepArg, NULL },
    { "-ssp", "secondShogiProgram", XrmoptionSepArg, NULL },
    { "-noShogiProgram", "noShogiProgram", XrmoptionSepArg, NULL },
    { "-nsp", "noShogiProgram", XrmoptionSepArg, NULL },
    { "-firstHost", "firstHost", XrmoptionSepArg, NULL },
    { "-fh", "firstHost", XrmoptionSepArg, NULL },
    { "-secondHost", "secondHost", XrmoptionSepArg, NULL },
    { "-sh", "secondHost", XrmoptionSepArg, NULL },
    { "-reversePawnBitmap", "reversePawnBitmap", XrmoptionSepArg, NULL },
    { "-rpb", "reversePawnBitmap", XrmoptionSepArg, NULL },
    { "-reverseLanceBitmap", "reverseLanceBitmap", XrmoptionSepArg, NULL },
    { "-rlb", "reverseLanceBitmap", XrmoptionSepArg, NULL },
    { "-reverseKnightBitmap", "reverseKnightBitmap", XrmoptionSepArg, NULL },
    { "-rnb", "reverseKnightBitmap", XrmoptionSepArg, NULL },
    { "-reverseSilverBitmap", "reverseSilverBitmap", XrmoptionSepArg, NULL },
    { "-rsb", "reverseSilverBitmap", XrmoptionSepArg, NULL },
    { "-reverseGoldBitmap", "reverseGoldBitmap", XrmoptionSepArg, NULL },
    { "-rgb", "reverseGoldBitmap", XrmoptionSepArg, NULL },
    { "-reverseRookBitmap", "reverseRookBitmap", XrmoptionSepArg, NULL },
    { "-rrb", "reverseRookBitmap", XrmoptionSepArg, NULL },
    { "-reverseBishopBitmap", "reverseBishopBitmap", XrmoptionSepArg, NULL },
    { "-rbb", "reverseBishopBitmap", XrmoptionSepArg, NULL },
    { "-reversePPawnBitmap", "reversePPawnBitmap",
      XrmoptionSepArg, NULL },
    { "-rppb", "reversePPawnBitmap", XrmoptionSepArg, NULL },
    { "-reversePLanceBitmap", "reversePLanceBitmap",
      XrmoptionSepArg, NULL },
    { "-rplb", "reversePLanceBitmap", XrmoptionSepArg, NULL },
    { "-reversePKnightBitmap", "reversePKnightBitmap",
      XrmoptionSepArg, NULL },
    { "-rpnb", "reversePKnightBitmap", XrmoptionSepArg, NULL },
    { "-reversePSilverBitmap", "reversePSilverBitmap",
      XrmoptionSepArg, NULL },
    { "-rpsb", "reversePSilverBitmap", XrmoptionSepArg, NULL },
    { "-reversePRookBitmap", "reversePRookBitmap",
      XrmoptionSepArg, NULL },
    { "-rprb", "reversePRookBitmap", XrmoptionSepArg, NULL },
    { "-reversePBishopBitmap", "reversePBishopBitmap",
      XrmoptionSepArg, NULL },
    { "-rpbb", "reversePBishopBitmap", XrmoptionSepArg, NULL },
    { "-reverseKingBitmap", "reverseKingBitmap", XrmoptionSepArg, NULL },
    { "-rkb", "reverseKingBitmap", XrmoptionSepArg, NULL },
    { "-outlinePawnBitmap", "outlinePawnBitmap", XrmoptionSepArg, NULL },
    { "-opb", "normalPawnBitmap", XrmoptionSepArg, NULL },
    { "-normalLanceBitmap", "normalLanceBitmap", XrmoptionSepArg, NULL },
    { "-olb", "normalLanceBitmap", XrmoptionSepArg, NULL },
    { "-normalKnightBitmap", "normalKnightBitmap", XrmoptionSepArg, NULL },
    { "-onb", "normalKnightBitmap", XrmoptionSepArg, NULL },
    { "-normalSilverBitmap", "normalSilverBitmap", XrmoptionSepArg, NULL },
    { "-osb", "normalSilverBitmap", XrmoptionSepArg, NULL },
    { "-normalGoldBitmap", "normalGoldBitmap", XrmoptionSepArg, NULL },
    { "-ogb", "normalGoldBitmap", XrmoptionSepArg, NULL },
    { "-normalRookBitmap", "normalRookBitmap", XrmoptionSepArg, NULL },
    { "-orb", "normalRookBitmap", XrmoptionSepArg, NULL },
    { "-normalBishopBitmap", "normalBishopBitmap", XrmoptionSepArg, NULL },
    { "-obb", "normalBishopBitmap", XrmoptionSepArg, NULL },
    { "-normalPPawnBitmap", "normalPPawnBitmap", XrmoptionSepArg, NULL },
    { "-oppb", "normalPPawnBitmap", XrmoptionSepArg, NULL },
    { "-normalPLanceBitmap", "normalPLanceBitmap", XrmoptionSepArg, NULL },
    { "-oplb", "normalPLanceBitmap", XrmoptionSepArg, NULL },
    { "-normalPKnightBitmap", "normalPKnightBitmap", XrmoptionSepArg, NULL },
    { "-opnb", "normalPKnightBitmap", XrmoptionSepArg, NULL },
    { "-normalPSilverBitmap", "normalPSilverBitmap", XrmoptionSepArg, NULL },
    { "-opsb", "normalPSilverBitmap", XrmoptionSepArg, NULL },
    { "-normalPRookBitmap", "normalPRookBitmap", XrmoptionSepArg, NULL },
    { "-oprb", "normalPRookBitmap", XrmoptionSepArg, NULL },
    { "-normalPBishopBitmap", "normalPBishopBitmap", XrmoptionSepArg, NULL },
    { "-opbb", "normalPBishopBitmap", XrmoptionSepArg, NULL },
    { "-normalKingBitmap", "normalKingBitmap", XrmoptionSepArg, NULL },
    { "-okb", "outlineKingBitmap", XrmoptionSepArg, NULL },
    { "-remoteShell", "remoteShell", XrmoptionSepArg, NULL },
    { "-rsh", "remoteShell", XrmoptionSepArg, NULL },
    { "-timeDelay", "timeDelay", XrmoptionSepArg, NULL },
    { "-td", "timeDelay", XrmoptionSepArg, NULL },
    { "-timeControl", "timeControl", XrmoptionSepArg, NULL },
    { "-tc", "timeControl", XrmoptionSepArg, NULL },
    { "-gameIn", "gameIn", XrmoptionSepArg, NULL },
    { "-gi", "gameIn", XrmoptionSepArg, NULL },
    { "-loadGameFile", "loadGameFile", XrmoptionSepArg, NULL },
    { "-lgf", "loadGameFile", XrmoptionSepArg, NULL },
    { "-loadPositionFile", "loadPositionFile", XrmoptionSepArg, NULL },
    { "-lpf", "loadPositionFile", XrmoptionSepArg, NULL },
    { "-saveGameFile", "saveGameFile", XrmoptionSepArg, NULL },
    { "-sgf", "saveGameFile", XrmoptionSepArg, NULL },
    { "-savePositionFile", "savePositionFile", XrmoptionSepArg, NULL },
    { "-spf", "savePositionFile", XrmoptionSepArg, NULL },
    { "-challengeDisplay", "challengeDisplay", XrmoptionSepArg, NULL },
    { "-cd", "challengeDisplay", XrmoptionSepArg, NULL },
    { "-matchMode", "matchMode", XrmoptionSepArg, NULL },
    { "-mm", "matchMode", XrmoptionSepArg, NULL },
    { "-monoMode", "monoMode", XrmoptionSepArg, NULL },
    { "-mono", "monoMode", XrmoptionSepArg, NULL },
    { "-debugMode", "debugMode", XrmoptionSepArg, NULL },
    { "-debug", "debugMode", XrmoptionSepArg, NULL },
    { "-clockMode", "clockMode", XrmoptionSepArg, NULL },
    { "-clock", "clockMode", XrmoptionSepArg, NULL },
    { "-boardSize", "boardSize", XrmoptionSepArg, NULL },
    { "-size", "boardSize", XrmoptionSepArg, NULL },
    { "-searchTime", "searchTime", XrmoptionSepArg, NULL },
    { "-st", "searchTime", XrmoptionSepArg, NULL },
    { "-searchDepth", "searchDepth", XrmoptionSepArg, NULL },
    { "-sd", "searchDepth", XrmoptionSepArg, NULL },
    { "-showCoords", "showCoords", XrmoptionSepArg, NULL },
    { "-coords", "showCoords", XrmoptionSepArg, NULL },
    { "-iconic", "Iconic", XrmoptionNoArg, "True" }
};



XtActionsRec boardActions[] =
{
    { "DrawPosition",   (XtActionProc) DrawPosition },
    { "HandleUserMove", (XtActionProc) HandleUserMove },
    { "ResetProc",      (XtActionProc) ResetProc },
    { "ResetFileProc",  (XtActionProc) ResetFileProc },
    { "LoadGameProc",   (XtActionProc) LoadGameProc },
    { "QuitProc",       (XtActionProc) QuitProc },
    { "ForwardProc",    (XtActionProc) ForwardProc },
    { "BackwardProc",   (XtActionProc) BackwardProc },
    { "PauseProc",      (XtActionProc) PauseProc },
    { "Iconify",        (XtActionProc) Iconify },
    { "FileNameAction", (XtActionProc) FileNameAction },
    { "PieceMenuPopup", (XtActionProc) PieceMenuPopup },
    { "SetBlackToPlay", (XtActionProc) SetBlackToPlay },
    { "SetWhiteToPlay", (XtActionProc) SetWhiteToPlay }
};


char translationsTable[] =
"<Expose>: DrawPosition() \n \
<Btn1Down>: HandleUserMove() \n \
<Btn1Up>: HandleUserMove() \n \
<Btn2Down>: XawPositionSimpleMenu(menuW) PieceMenuPopup(menuW) \n \
<Btn3Down>: XawPositionSimpleMenu(menuB) PieceMenuPopup(menuB) \n \
<Key>r: ResetFileProc() ResetProc() \n \
<Key>R: ResetFileProc() ResetProc() \n \
<Key>g: LoadGameProc() \n \
<Key>G: LoadGameProc() \n \
<Key>q: QuitProc() \n \
<Key>Q: QuitProc() \n \
<Message>WM_PROTOCOLS: QuitProc() \n \
<Key>f: ForwardProc() \n \
<Key>F: ForwardProc() \n \
<Key>b: BackwardProc() \n \
<Key>B: BackwardProc() \n \
<Key>p: PauseProc() \n \
<Key>P: PauseProc() \n \
<Key>i: Iconify() \n \
<Key>I: Iconify() \n \
<Key>c: Iconify() \n \
<Key>C: Iconify() \n";


char translationsTableReduced[] =
"<Expose>: DrawPosition() \n \
<Btn1Down>: HandleUserMove() \n \
<Btn1Up>: HandleUserMove() \n \
<Message>WM_PROTOCOLS: QuitProc() \n";


char blackTranslations[] = "<BtnDown>: SetBlackToPlay()\n";
char whiteTranslations[] = "<BtnDown>: SetWhiteToPlay()\n";

String xshogiResources[] =
{
    DEFAULT_FONT,
    "*Dialog*value.translations: #override "
    "\\n <Key>Return: FileNameAction()",
    NULL
};


int global_argc;       /* number of command args */
char *global_argv[10]; /* pointers to up to 10 command args */



static struct DisplayData *player;


typedef struct
{
    char mode[2];
    char name[100];
} FileModeInfo;

static FileModeInfo fmi;


/*
 * This is a hack that allows the parser to tell the program
 * that the game it's loading has ended.
 */

int loaded_game_finished = 0;


/**********************************************************************
 *
 * End of globals.
 *
 **********************************************************************/


void
CreatePlayerWindow(void)
{
    int mainFontPxlSize, coordFontPxlSize;
    int min, sec, matched;
    XSetWindowAttributes window_attributes;
    char buf[MSG_SIZ];
    Arg args[10];
    Dimension timerWidth, boardWidth, commandsWidth, w, h;
    int local;
    int fromRemotePlayer = (player == &remotePlayer);

    player->monoMode = player->appData.monoMode;
    player->showCoords = player->appData.showCoords;

    /*
     * Parse timeControl resource.
     */

    if (player->appData.timeControl != NULL)
    {
        matched = sscanf(player->appData.timeControl, "%d:%d", &min, &sec);

        if (matched == 1)
        {
            timeControl = min * 60 * 1000;
        }
        else if (matched == 2)
        {
            timeControl = (min * 60 + sec) * 1000;
        }
        else
        {
            fprintf(stderr, "%s: bad timeControl option %s\n",
                    programName, player->appData.timeControl);
            Usage();
        }
    }

    /*
     * Parse searchTime resource
     */

    if (player->appData.searchTime != NULL)
    {
        matched = sscanf(player->appData.searchTime, "%d:%d", &min, &sec);

        if (matched == 1)
        {
            searchTime = min * 60;
        }
        else if (matched == 2)
        {
            searchTime = min * 60 + sec;
        }
        else
        {
            fprintf(stderr, "%s: bad searchTime option %s\n",
                    programName, player->appData.searchTime);
            Usage();
        }
    }

    if ((player->appData.searchTime != NULL)
        || (player->appData.searchDepth > 0)
        || player->appData.noShogiProgram)
    {
        player->appData.clockMode = False;
    }

    player->Iconic = False;
    player->boardSize = Small;
    player->squareSize = SMALL_SQUARE_SIZE;
    player->flipView = (player == &remotePlayer);
    player->promotionUp = False;

    /*
     * Determine boardSize.
     */

    if (strcasecmp(player->appData.boardSize, "Large") == 0)
    {
        player->boardSize = Large;
    }
    else if (strcasecmp(player->appData.boardSize, "Medium") == 0)
    {
        player->boardSize = Medium;
    }
    else if (strcasecmp(player->appData.boardSize, "Small") == 0)
    {
        player->boardSize = Small;
    }
    else
    {
        fprintf(stderr, "%s: bad boardSize option %s\n",
                programName, player->appData.boardSize);
        Usage();
    }

    if ((local = (player == &localPlayer)))
    {
        player->xDisplay = XtDisplay(player->shellWidget);
        player->xScreen = DefaultScreen(player->xDisplay);
    }

#undef DONT_ADJUST_BOARDSIZE
#ifndef DONT_ADJUST_BOARDSIZE
    if (((DisplayWidth(player->xDisplay, player->xScreen) < 800)
         || (DisplayHeight(player->xDisplay, player->xScreen) < 800))
        && (player->boardSize == Large))
    {
        player->boardSize = Medium;
    }
#endif

    switch (player->boardSize)
    {
    case Small:
        player->squareSize = SMALL_SQUARE_SIZE;
        mainFontPxlSize    = 11;
        coordFontPxlSize   = 10;
        break;

    case Medium:
        player->squareSize = MEDIUM_SQUARE_SIZE;
        mainFontPxlSize    = 17;
        coordFontPxlSize   = 12;
        break;

    default:
    case Large:
        player->squareSize = LARGE_SQUARE_SIZE;
        mainFontPxlSize    = 17;
        coordFontPxlSize   = 14;
        break;
    }

    /*
     * Detect if there are not enough colors are available and adapt.
     */

    if (DefaultDepth(player->xDisplay, player->xScreen) <= 2)
        player->monoMode = True;

    /*
     * Determine what fonts to use.
     */

    player->appData.mainFont
        = FindFont(player->appData.mainFont, mainFontPxlSize);
    player->mainFontID
        = XLoadFont(player->xDisplay, player->appData.mainFont);
    player->mainFontStruct
        = XQueryFont(player->xDisplay, player->mainFontID);
    player->appData.coordFont
        = FindFont(player->appData.coordFont, coordFontPxlSize);
    player->coordFontID
        = XLoadFont(player->xDisplay, player->appData.coordFont);
    player->coordFontStruct
        = XQueryFont(player->xDisplay, player->coordFontID);

    /*
     * Set default arguments.
     */

    XtSetArg(player->shellArgs[0], XtNwidth, 0);
    XtSetArg(player->shellArgs[1], XtNheight, 0);
    XtSetArg(player->shellArgs[2], XtNminWidth, 0);
    XtSetArg(player->shellArgs[3], XtNminHeight, 0);
    XtSetArg(player->shellArgs[4], XtNmaxWidth, 0);
    XtSetArg(player->shellArgs[5], XtNmaxHeight, 0);

    XtSetArg(player->boardArgs[0], XtNborderWidth, 0);
    XtSetArg(player->boardArgs[1], XtNwidth,
             LINE_GAP + (BOARD_SIZE + 4)
             * (SMALL_SQUARE_SIZE + LINE_GAP));
    XtSetArg(player->boardArgs[2], XtNheight,
             LINE_GAP + BOARD_SIZE
             * (SMALL_SQUARE_SIZE + LINE_GAP));

    XtSetArg(player->commandsArgs[0], XtNborderWidth, 0);
    XtSetArg(player->commandsArgs[1], XtNdefaultColumns, 4);
    XtSetArg(player->commandsArgs[2], XtNforceColumns, True);
    XtSetArg(player->commandsArgs[3], XtNcolumnSpacing, 12);
    XtSetArg(player->commandsArgs[4], XtNlist, (XtArgVal) buttonStrings);
    XtSetArg(player->commandsArgs[5], XtNnumberStrings, buttonCount);
    XtSetArg(player->commandsArgs[6], XtNfont, player->mainFontStruct);

    XtSetArg(player->messageArgs[0], XtNborderWidth, 0);
    XtSetArg(player->messageArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);
    XtSetArg(player->messageArgs[2], XtNlabel, (XtArgVal) "starting...");

    XtSetArg(player->timerArgs[0], XtNborderWidth, 0);
    XtSetArg(player->timerArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);

    XtSetArg(player->titleArgs[0], XtNborderWidth, 0);
    XtSetArg(player->titleArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);

    boardWidth = LINE_GAP
        + (BOARD_SIZE + 4) * (player->squareSize + LINE_GAP);

    XtSetArg(player->boardArgs[1], XtNwidth, boardWidth);
    XtSetArg(player->boardArgs[2], XtNheight,
             LINE_GAP + BOARD_SIZE * (player->squareSize + LINE_GAP));

    /*
     * widget hierarchy
     */

    player->formWidget = XtCreateManagedWidget("form",
                                               formWidgetClass,
                                               player->shellWidget, NULL, 0);

    player->widgetList[0] = player->blackTimerWidget
        = XtCreateWidget((local ? "black time:" : "rblack time:"),
                         labelWidgetClass,
                         player->formWidget, player->timerArgs,
                         XtNumber(player->timerArgs));

    XtSetArg(args[0], XtNfont, player->mainFontStruct);
    XtSetValues(player->blackTimerWidget, args, 1);

    player->widgetList[1] = player->whiteTimerWidget
        = XtCreateWidget((local ? "white time:" : "rwhite time:"),
                         labelWidgetClass,
                         player->formWidget, player->timerArgs,
                         XtNumber(player->timerArgs));

    XtSetArg(args[0], XtNfont, player->mainFontStruct);
    XtSetValues(player->whiteTimerWidget, args, 1);

    player->widgetList[2] = player->titleWidget
        = XtCreateWidget((local ? "" : "r"), labelWidgetClass,
                         player->formWidget, player->titleArgs,
                         XtNumber(player->titleArgs));

    XtSetArg(args[0], XtNfont, player->mainFontStruct);
    XtSetValues(player->titleWidget, args, 1);

    player->widgetList[3] = player->messageWidget
        = XtCreateWidget((local ? "message" : "rmessage"),
                         labelWidgetClass, player->formWidget,
                         player->messageArgs,
                         XtNumber(player->messageArgs));

    XtSetArg(args[0], XtNfont, player->mainFontStruct);
    XtSetValues(player->messageWidget, args, 1);

    player->widgetList[4] = player->commandsWidget
        = XtCreateWidget((local ? "commands" : "rcommand"),
                         listWidgetClass, player->formWidget,
                         player->commandsArgs,
                         XtNumber(player->commandsArgs));

    player->widgetList[5] = player->boardWidget
        = XtCreateWidget((local ? "board" : "rboard"),
                         widgetClass, player->formWidget,
                         player->boardArgs,
                         XtNumber(player->boardArgs));

    XtManageChildren(player->widgetList, XtNumber(player->widgetList));

    /*
     * Calculate the width of the timer labels.
     */

    XtSetArg(args[0], XtNfont, &player->mainFontStruct);
    XtGetValues(player->blackTimerWidget, args, 1);

    if (player->appData.clockMode)
    {
        timerWidth = XTextWidth(player->mainFontStruct,
                                "Black: 8:88:88 ", 15);
    }
    else
    {
        timerWidth = XTextWidth(player->mainFontStruct, "Black  ", 7);
    }

    XtSetArg(args[0], XtNwidth, timerWidth);
    XtSetValues(player->blackTimerWidget, args, 1);
    XtSetValues(player->whiteTimerWidget, args, 1);

    XtSetArg(args[0], XtNbackground, &player->timerForegroundPixel);
    XtSetArg(args[1], XtNforeground, &player->timerBackgroundPixel);
    XtGetValues(player->blackTimerWidget, args, 2);

    /*
     * Calculate the width of the name and message labels.
     */

    XtSetArg(args[0], XtNwidth, &commandsWidth);
    XtGetValues(player->commandsWidget, args, 1);
    w = ((commandsWidth > boardWidth) ? commandsWidth : boardWidth);
    XtSetArg(args[0], XtNwidth, w - timerWidth * 2 - 12);
    XtSetValues(player->titleWidget, args, 1);
    XtSetArg(args[0], XtNwidth, w - 8);
    XtSetValues(player->messageWidget, args, 1);

    /*
     * 'formWidget' uses these constraints but they are stored
     * in the children.
     */

    XtSetArg(args[0], XtNfromHoriz, player->blackTimerWidget);
    XtSetValues(player->whiteTimerWidget, args, 1);
    XtSetArg(args[0], XtNfromHoriz, player->whiteTimerWidget);
    XtSetValues(player->titleWidget, args, 1);
    XtSetArg(args[0], XtNfromVert, player->blackTimerWidget);
    XtSetValues(player->messageWidget, args, 1);
    XtSetArg(args[0], XtNfromVert, player->messageWidget);
    XtSetValues(player->commandsWidget, args, 1);
    XtSetArg(args[0], XtNfromVert, player->commandsWidget);
    XtSetValues(player->boardWidget, args, 1);

    XtRealizeWidget(player->shellWidget);

    player->xBoardWindow = XtWindow(player->boardWidget);

    /*
     * Create an icon.
     */

    player->iconPixmap =
        XCreateBitmapFromData(player->xDisplay,
                              XtWindow(player->shellWidget),
                              (char *)icon_bits, icon_width, icon_height);

    XtSetArg(args[0], XtNiconPixmap, player->iconPixmap);
    XtSetValues(player->shellWidget, args, 1);

    /*
     * Create a cursor for the board widget.
     */

    window_attributes.cursor = XCreateFontCursor(player->xDisplay, XC_hand2);
    XChangeWindowAttributes(player->xDisplay, player->xBoardWindow,
                            CWCursor, &window_attributes);

    /*
     * Inhibit shell resizing.
     */

    player->shellArgs[0].value = (XtArgVal) &w;
    player->shellArgs[1].value = (XtArgVal) &h;
    XtGetValues(player->shellWidget, player->shellArgs, 2);
    player->shellArgs[4].value = player->shellArgs[2].value = w;
    player->shellArgs[5].value = player->shellArgs[3].value = h;
    XtSetValues(player->shellWidget, &player->shellArgs[2], 4);

    /*
     * Determine value of black pixel.
     */

    player->black_pixel_is_zero =
        (XBlackPixel(player->xDisplay, player->xScreen) == 0);

    CreateGCs();
    CreateGrid();
    CreatePieces();

    if (!fromRemotePlayer)
        CreatePieceMenus();

    XtAddCallback(player->commandsWidget, XtNcallback, SelectCommand,
                  (XtPointer)fromRemotePlayer);

    if (!fromRemotePlayer)
        XtAppAddActions(appContext, boardActions, XtNumber(boardActions));

    if (fromRemotePlayer)
    {
        XtSetArg(args[0], XtNtranslations,
                 XtParseTranslationTable(translationsTableReduced));
        /* Disable key commands because often keys are pressed
           in the board window if using another talk window. */
        XtSetValues(player->boardWidget, &args[0], 1);
        XtSetValues(localPlayer.boardWidget, &args[0], 1);
    }
    else
    {
        XtSetArg(args[0], XtNtranslations,
                 XtParseTranslationTable(translationsTable));
        XtSetValues(player->boardWidget, &args[0], 1);
        XtSetArg(args[0], XtNtranslations,
                 XtParseTranslationTable(blackTranslations));
        XtSetValues(player->blackTimerWidget, &args[0], 1);
        XtSetArg(args[0], XtNtranslations,
                 XtParseTranslationTable(whiteTranslations));
        XtSetValues(player->whiteTimerWidget, &args[0], 1);
    }

    XtAddEventHandler(player->boardWidget, ExposureMask | ButtonPressMask
                      | ButtonReleaseMask | Button1MotionMask | KeyPressMask,
                      False, (XtEventHandler)EventProc,
                      (XtPointer)(player == &remotePlayer));

    sprintf(buf, "xshogi version %s, patchlevel %s based on "
            "xboard version %s",
            version, patchlevel, XBOARD_VERSION);

    /*
     * If there is to be a machine match, set it up.
     */

    if (matchMode != MatchFalse && player != &remotePlayer)
    {
        if (player->appData.noShogiProgram)
        {
            fprintf(stderr,
                    "%s: can't have a match with no shogi programs!\n",
                    programName);
            exit(1);
        }

        DisplayMessage(buf, fromRemotePlayer);
        TwoMachinesProc(NULL, NULL, NULL, NULL);
    }
    else
    {
        Reset(True);
        DisplayMessage(buf, fromRemotePlayer);
    }
}




int
main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    /*
     * Copy pointers to command line arguments and number of such pointers.
     * (argc, argv will be destroyed by XtAppInitialize)
     */

    for (global_argc = 0; global_argc < argc; global_argc++)
        global_argv[global_argc] = argv[global_argc];

    programName = strrchr(argv[0], '/');

    if (programName == NULL)
        programName = argv[0];
    else
        programName++;

    localPlayer.shellWidget
        = XtAppInitialize(&appContext, "XShogi", shellOptions,
                          XtNumber(shellOptions), &argc, argv,
                          xshogiResources, NULL, 0);

    if (argc > 1)
        Usage();

    if ((shogiDir = (char *)getenv("SHOGIDIR")) == NULL)
    {
        shogiDir = ".";
    }
    else
    {
        if (chdir(shogiDir) != 0)
        {
            fprintf(stderr, "%s: can't cd to SHOGIDIR\n",
                    programName);
            perror(shogiDir);
            exit(1);
        }
    }

    XtGetApplicationResources(localPlayer.shellWidget,
                              &localPlayer.appData, clientResources,
                              XtNumber(clientResources), NULL, 0);

    xshogiDebug = localPlayer.appData.debugMode;

    /*
     * Determine matchMode state -- poor man's resource converter.
     */

    if (strcasecmp(localPlayer.appData.matchMode, "Init") == 0)
    {
        matchMode = MatchInit;
    }
    else if (strcasecmp(localPlayer.appData.matchMode, "Position") == 0)
    {
        matchMode = MatchPosition;
    }
    else if (strcasecmp(localPlayer.appData.matchMode, "Opening") == 0)
    {
        matchMode = MatchOpening;
    }
    else if (strcasecmp(localPlayer.appData.matchMode, "False") == 0)
    {
        matchMode = MatchFalse;
    }
    else
    {
        fprintf(stderr, "%s: bad matchMode option %s\n",
                programName, localPlayer.appData.matchMode);
        Usage();
    }

    buttonStrings = gnuButtonStrings;
    buttonProcs = gnuButtonProcs;
    buttonCount = XtNumber(gnuButtonStrings);

    player = &localPlayer;

    CreatePlayerWindow();

    XtAppMainLoop(appContext);

    return 0;
}



/*
 * Find a font that matches "pattern" that is as close as
 * possible to the targetPxlSize.  Prefer fonts that are k
 * pixels smaller to fonts that are k pixels larger.  The
 * pattern must be in the X Consortium standard format,
 * e.g. "-*-helvetica-bold-r-normal--*-*-*-*-*-*-*-*".
 * The return value should be freed with XtFree when no
 * longer needed.
 */

char *
FindFont(char *pattern, int targetPxlSize)
{
    char **fonts, *p, *best;
    int i, j, nfonts, minerr, err, pxlSize;

    fonts = XListFonts(player->xDisplay, pattern, 999999, &nfonts);

    if (nfonts < 1)
    {
        fprintf(stderr, "%s: No fonts match pattern %s\n",
                programName, pattern);
        exit(1);
    }

    best = "";
    minerr = 999999;

    for (i = 0; i < nfonts; i++)
    {
        j = 0;
        p = fonts[i];

        if (*p != '-')
            continue;

        while (j < 7)
        {
            if (*p == NULLCHAR)
                break;

            if (*p++ == '-')
                j++;
        }

        if (j < 7)
            continue;

        pxlSize = atoi(p);

        if (pxlSize == targetPxlSize)
        {
            best = fonts[i];
            break;
        }

        err = pxlSize - targetPxlSize;

        if (abs(err) < abs(minerr)
            || ((minerr > 0) && (err < 0) && (-err == minerr)))
        {
            best = fonts[i];
            minerr = err;
        }
    }

    p = (char *)XtMalloc(strlen(best) + 1);
    strcpy(p, best);
    XFreeFontNames(fonts);
    return p;
}




void
CreateGCs(void)
{
    XtGCMask value_mask = GCLineWidth | GCLineStyle | GCForeground
        | GCBackground | GCFunction | GCPlaneMask;
    XGCValues gc_values;

    gc_values.plane_mask = AllPlanes;
    gc_values.line_width = LINE_GAP;
    gc_values.line_style = LineSolid;
    gc_values.function = GXcopy;

    gc_values.foreground = XBlackPixel(player->xDisplay, player->xScreen);
    gc_values.background = XBlackPixel(player->xDisplay, player->xScreen);
    player->lineGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

    gc_values.background = XWhitePixel(player->xDisplay, player->xScreen);
    player->coordGC = XtGetGC(player->shellWidget, value_mask, &gc_values);
    XSetFont(player->xDisplay, player->coordGC, player->coordFontID);

    if (player->monoMode)
    {
        gc_values.foreground
            = XWhitePixel(player->xDisplay, player->xScreen);
        gc_values.background
            = XWhitePixel(player->xDisplay, player->xScreen);

        /* empty square off board */
        player->squareOffBoardGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);

        /* empty square on board */
        player->squareGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);
    }
    else
    {
        Pixel bg;         /* background color */
        Arg   args[1];

        /* Get background color. */
        XtSetArg(args[0], XtNbackground, &bg);
        XtGetValues(player->shellWidget, args, 1);

        /* empty square off board */
        gc_values.foreground = gc_values.background = bg;
        player->squareOffBoardGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);

        /* empty square on board */
        gc_values.foreground
            = player->appData.squareColor;
        gc_values.background
            = player->appData.squareColor;
        player->squareGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);

        /* piece off board */
        gc_values.foreground
            = player->appData.pieceColor;
        gc_values.background = bg;
        player->oPieceGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);

        /* piece on board */
        gc_values.foreground
            = player->appData.pieceColor;
        gc_values.background
            = player->appData.squareColor;
        player->pieceGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);

        /* piece symbol */
        /*
         * FIXME: charPieceColor seems to have no effect;
         *    the bitmap is *always* black.
         */
        gc_values.function = (player->black_pixel_is_zero ? GXand : GXor);

        gc_values.foreground
            = player->appData.charPieceColor;
        gc_values.background
            = player->appData.charPieceColor;

        player->charPieceGC
            = XtGetGC(player->shellWidget, value_mask, &gc_values);
    }
}




void
CreatePieces(void)
{
    XSynchronize(player->xDisplay, True);   /* Work-around for xlib/xt
                                               buffering bug */

    if (player->appData.westernPieceSet)
    {
        ReadBitmap(player->appData.reverseBigSolidBitmap,
                   &player->reverseBigSolidBitmap,
                   NULL,
                   bigsolidR_bits, bigsolidR_m_bits, bigsolidR_l_bits);

        ReadBitmap(player->appData.reverseSmallSolidBitmap,
                   &player->reverseSmallSolidBitmap,
                   NULL,
                   smallsolidR_bits, smallsolidR_m_bits, smallsolidR_l_bits);

        ReadBitmap(player->appData.normalBigSolidBitmap,
                   &player->normalBigSolidBitmap,
                   NULL,
                   bigsolid_bits, bigsolid_m_bits, bigsolid_l_bits);

        ReadBitmap(player->appData.normalSmallSolidBitmap,
                   &player->normalSmallSolidBitmap,
                   NULL,
                   smallsolid_bits, smallsolid_m_bits, smallsolid_l_bits);

        ReadBitmap(player->appData.reversePawnBitmap,
                   &player->reversePawnBitmap,
                   &player->reverseSmallSolidBitmap,
                   pawnRW_bits, pawnRW_bits, pawnRW_bits);

        ReadBitmap(player->appData.reverseLanceBitmap,
                   &player->reverseLanceBitmap,
                   &player->reverseSmallSolidBitmap,
                   lanceRW_bits, lanceRW_bits, lanceRW_bits);

        ReadBitmap(player->appData.reverseKnightBitmap,
                   &player->reverseKnightBitmap,
                   &player->reverseSmallSolidBitmap,
                   knightRW_bits, knightRW_bits, knightRW_bits);

        ReadBitmap(player->appData.reverseSilverBitmap,
                   &player->reverseSilverBitmap,
                   &player->reverseBigSolidBitmap,
                   silverRW_bits, silverRW_bits, silverRW_bits);

        ReadBitmap(player->appData.reverseGoldBitmap,
                   &player->reverseGoldBitmap,
                   &player->reverseBigSolidBitmap,
                   goldRW_bits, goldRW_bits, goldRW_bits);

        ReadBitmap(player->appData.reverseRookBitmap,
                   &player->reverseRookBitmap,
                   &player->reverseBigSolidBitmap,
                   rookRW_bits, rookRW_bits, rookRW_bits);

        ReadBitmap(player->appData.reverseBishopBitmap,
                   &player->reverseBishopBitmap,
                   &player->reverseBigSolidBitmap,
                   bishopRW_bits, bishopRW_bits, bishopRW_bits);

        ReadBitmap(player->appData.reversePPawnBitmap,
                   &player->reversePPawnBitmap,
                   &player->reverseSmallSolidBitmap,
                   pawnPRW_bits, pawnPRW_bits, pawnPRW_bits);

        ReadBitmap(player->appData.reversePLanceBitmap,
                   &player->reversePLanceBitmap,
                   &player->reverseSmallSolidBitmap,
                   lancePRW_bits, lancePRW_bits, lancePRW_bits);

        ReadBitmap(player->appData.reversePKnightBitmap,
                   &player->reversePKnightBitmap,
                   &player->reverseSmallSolidBitmap,
                   knightPRW_bits, knightPRW_bits, knightPRW_bits);

        ReadBitmap(player->appData.reversePSilverBitmap,
                   &player->reversePSilverBitmap,
                   &player->reverseBigSolidBitmap,
                   silverPRW_bits, silverPRW_bits, silverPRW_bits);

        ReadBitmap(player->appData.reversePRookBitmap,
                   &player->reversePRookBitmap,
                   &player->reverseBigSolidBitmap,
                   rookPRW_bits, rookPRW_bits, rookPRW_bits);

        ReadBitmap(player->appData.reversePBishopBitmap,
                   &player->reversePBishopBitmap,
                   &player->reverseBigSolidBitmap,
                   bishopPRW_bits, bishopPRW_bits, bishopPRW_bits);

        ReadBitmap(player->appData.reverseKingBitmap,
                   &player->reverseKingBitmap,
                   &player->reverseBigSolidBitmap,
                   kingRW_bits, kingRW_bits, kingRW_bits);

        ReadBitmap(player->appData.normalPawnBitmap,
                   &player->normalPawnBitmap,
                   &player->normalSmallSolidBitmap,
                   pawnW_bits, pawnW_bits, pawnW_bits);

        ReadBitmap(player->appData.normalLanceBitmap,
                   &player->normalLanceBitmap,
                   &player->normalSmallSolidBitmap,
                   lanceW_bits, lanceW_bits, lanceW_bits);

        ReadBitmap(player->appData.normalKnightBitmap,
                   &player->normalKnightBitmap,
                   &player->normalSmallSolidBitmap,
                   knightW_bits, knightW_bits, knightW_bits);

        ReadBitmap(player->appData.normalSilverBitmap,
                   &player->normalSilverBitmap,
                   &player->normalBigSolidBitmap,
                   silverW_bits, silverW_bits, silverW_bits);

        ReadBitmap(player->appData.normalGoldBitmap,
                   &player->normalGoldBitmap,
                   &player->normalBigSolidBitmap,
                   goldW_bits, goldW_bits, goldW_bits);

        ReadBitmap(player->appData.normalRookBitmap,
                   &player->normalRookBitmap,
                   &player->normalBigSolidBitmap,
                   rookW_bits, rookW_bits, rookW_bits);

        ReadBitmap(player->appData.normalBishopBitmap,
                   &player->normalBishopBitmap,
                   &player->normalBigSolidBitmap,
                   bishopW_bits, bishopW_bits, bishopW_bits);

        ReadBitmap(player->appData.normalPPawnBitmap,
                   &player->normalPPawnBitmap,
                   &player->normalSmallSolidBitmap,
                   pawnPW_bits, pawnPW_bits, pawnPW_bits);

        ReadBitmap(player->appData.normalPLanceBitmap,
                   &player->normalPLanceBitmap,
                   &player->normalSmallSolidBitmap,
                   lancePW_bits, lancePW_bits, lancePW_bits);

        ReadBitmap(player->appData.normalPKnightBitmap,
                   &player->normalPKnightBitmap,
                   &player->normalSmallSolidBitmap,
                   knightPW_bits, knightPW_bits, knightPW_bits);

        ReadBitmap(player->appData.normalPSilverBitmap,
                   &player->normalPSilverBitmap,
                   &player->normalBigSolidBitmap,
                   silverPW_bits, silverPW_bits, silverPW_bits);

        ReadBitmap(player->appData.normalPRookBitmap,
                   &player->normalPRookBitmap,
                   &player->normalBigSolidBitmap,
                   rookPW_bits, rookPW_bits, rookPW_bits);

        ReadBitmap(player->appData.normalPBishopBitmap,
                   &player->normalPBishopBitmap,
                   &player->normalBigSolidBitmap,
                   bishopPW_bits, bishopPW_bits, bishopPW_bits);

        ReadBitmap(player->appData.normalKingBitmap,
                   &player->normalKingBitmap,
                   &player->normalBigSolidBitmap,
                   kingW_bits, kingW_bits, kingW_bits);
    }
    else
    {
        ReadBitmap(player->appData.reverseBigSolidBitmap,
                   &player->reverseBigSolidBitmap,
                   NULL,
                   bigsolidR_bits, bigsolidR_m_bits, bigsolidR_l_bits);

        ReadBitmap(player->appData.reverseSmallSolidBitmap,
                   &player->reverseSmallSolidBitmap,
                   NULL,
                   smallsolidR_bits, smallsolidR_m_bits, smallsolidR_l_bits);

        ReadBitmap(player->appData.normalBigSolidBitmap,
                   &player->normalBigSolidBitmap,
                   NULL,
                   bigsolid_bits, bigsolid_m_bits, bigsolid_l_bits);

        ReadBitmap(player->appData.normalSmallSolidBitmap,
                   &player->normalSmallSolidBitmap,
                   NULL,
                   smallsolid_bits, smallsolid_m_bits, smallsolid_l_bits);

        ReadBitmap(player->appData.reversePawnBitmap,
                   &player->reversePawnBitmap,
                   &player->reverseSmallSolidBitmap,
                   pawnR_bits, pawnR_m_bits, pawnR_l_bits);

        ReadBitmap(player->appData.reverseLanceBitmap,
                   &player->reverseLanceBitmap,
                   &player->reverseSmallSolidBitmap,
                   lanceR_bits, lanceR_m_bits, lanceR_l_bits);

        ReadBitmap(player->appData.reverseKnightBitmap,
                   &player->reverseKnightBitmap,
                   &player->reverseSmallSolidBitmap,
                   knightR_bits, knightR_m_bits, knightR_l_bits);

        ReadBitmap(player->appData.reverseSilverBitmap,
                   &player->reverseSilverBitmap,
                   &player->reverseBigSolidBitmap,
                   silverR_bits, silverR_m_bits, silverR_l_bits);

        ReadBitmap(player->appData.reverseGoldBitmap,
                   &player->reverseGoldBitmap,
                   &player->reverseBigSolidBitmap,
                   goldR_bits, goldR_m_bits, goldR_l_bits);

        ReadBitmap(player->appData.reverseRookBitmap,
                   &player->reverseRookBitmap,
                   &player->reverseBigSolidBitmap,
                   rookR_bits, rookR_m_bits, rookR_l_bits);

        ReadBitmap(player->appData.reverseBishopBitmap,
                   &player->reverseBishopBitmap,
                   &player->reverseBigSolidBitmap,
                   bishopR_bits, bishopR_m_bits, bishopR_l_bits);

        ReadBitmap(player->appData.reversePPawnBitmap,
                   &player->reversePPawnBitmap,
                   &player->reverseSmallSolidBitmap,
                   pawnPR_bits, pawnPR_m_bits, pawnPR_l_bits);

        ReadBitmap(player->appData.reversePLanceBitmap,
                   &player->reversePLanceBitmap,
                   &player->reverseSmallSolidBitmap,
                   lancePR_bits, lancePR_m_bits, lancePR_l_bits);

        ReadBitmap(player->appData.reversePKnightBitmap,
                   &player->reversePKnightBitmap,
                   &player->reverseSmallSolidBitmap,
                   knightPR_bits, knightPR_m_bits, knightPR_l_bits);

        ReadBitmap(player->appData.reversePSilverBitmap,
                   &player->reversePSilverBitmap,
                   &player->reverseBigSolidBitmap,
                   silverPR_bits, silverPR_m_bits, silverPR_l_bits);

        ReadBitmap(player->appData.reversePRookBitmap,
                   &player->reversePRookBitmap,
                   &player->reverseBigSolidBitmap,
                   rookPR_bits, rookPR_m_bits, rookPR_l_bits);

        ReadBitmap(player->appData.reversePBishopBitmap,
                   &player->reversePBishopBitmap,
                   &player->reverseBigSolidBitmap,
                   bishopPR_bits, bishopPR_m_bits, bishopPR_l_bits);

        ReadBitmap(player->appData.reverseKingBitmap,
                   &player->reverseKingBitmap,
                   &player->reverseBigSolidBitmap,
                   kingR_bits, kingR_m_bits, kingR_l_bits);

        ReadBitmap(player->appData.normalPawnBitmap,
                   &player->normalPawnBitmap,
                   &player->normalSmallSolidBitmap,
                   pawn_bits, pawn_m_bits, pawn_l_bits);

        ReadBitmap(player->appData.normalLanceBitmap,
                   &player->normalLanceBitmap,
                   &player->normalSmallSolidBitmap,
                   lance_bits, lance_m_bits, lance_l_bits);

        ReadBitmap(player->appData.normalKnightBitmap,
                   &player->normalKnightBitmap,
                   &player->normalSmallSolidBitmap,
                   knight_bits, knight_m_bits, knight_l_bits);

        ReadBitmap(player->appData.normalSilverBitmap,
                   &player->normalSilverBitmap,
                   &player->normalBigSolidBitmap,
                   silver_bits, silver_m_bits, silver_l_bits);

        ReadBitmap(player->appData.normalGoldBitmap,
                   &player->normalGoldBitmap,
                   &player->normalBigSolidBitmap,
                   gold_bits, gold_m_bits, gold_l_bits);

        ReadBitmap(player->appData.normalRookBitmap,
                   &player->normalRookBitmap,
                   &player->normalBigSolidBitmap,
                   rook_bits, rook_m_bits, rook_l_bits);

        ReadBitmap(player->appData.normalBishopBitmap,
                   &player->normalBishopBitmap,
                   &player->normalBigSolidBitmap,
                   bishop_bits, bishop_m_bits, bishop_l_bits);

        ReadBitmap(player->appData.normalPPawnBitmap,
                   &player->normalPPawnBitmap,
                   &player->normalSmallSolidBitmap,
                   pawnP_bits, pawnP_m_bits, pawnP_l_bits);

        ReadBitmap(player->appData.normalPLanceBitmap,
                   &player->normalPLanceBitmap,
                   &player->normalSmallSolidBitmap,
                   lanceP_bits, lanceP_m_bits, lanceP_l_bits);

        ReadBitmap(player->appData.normalPKnightBitmap,
                   &player->normalPKnightBitmap,
                   &player->normalSmallSolidBitmap,
                   knightP_bits, knightP_m_bits, knightP_l_bits);

        ReadBitmap(player->appData.normalPSilverBitmap,
                   &player->normalPSilverBitmap,
                   &player->normalBigSolidBitmap,
                   silverP_bits, silverP_m_bits, silverP_l_bits);

        ReadBitmap(player->appData.normalPRookBitmap,
                   &player->normalPRookBitmap,
                   &player->normalBigSolidBitmap,
                   rookP_bits, rookP_m_bits, rookP_l_bits);

        ReadBitmap(player->appData.normalPBishopBitmap,
                   &player->normalPBishopBitmap,
                   &player->normalBigSolidBitmap,
                   bishopP_bits, bishopP_m_bits, bishopP_l_bits);

        ReadBitmap(player->appData.normalKingBitmap,
                   &player->normalKingBitmap,
                   &player->normalBigSolidBitmap,
                   king_bits, king_m_bits, king_l_bits);

    }

    XSynchronize(player->xDisplay, False);  /* Work-around for xlib/xt
                                               buffering bug */
}




int
ReadBitmapFile(Display *display, Drawable d, char *filename,
               unsigned int *width_return,
               unsigned int *height_return,
               Pixmap *bitmap_return,
               int *x_hot_return, int *y_hot_return)
{
    int n;

    if ((n = XReadBitmapFile(display, d, filename,
                             width_return, height_return,
                             bitmap_return, x_hot_return, y_hot_return))
        != BitmapSuccess)
    {
        return n;
    }
    else
    {
        /* transform a 1 plane pixmap to a k plane pixmap */
        return BitmapSuccess;
    }
}




/*
 * Create the X pixmap from .xbm file bitmap data.  This may
 * have to be revised considerably.
 */

void
ReadBitmap(String name, Pixmap *pm, Pixmap *qm,
           unsigned char *small_bits, 
		   unsigned char *medium_bits, 
		   unsigned char *large_bits)
{
    int x_hot, y_hot;
    unsigned int w, h;

    if ((name == NULL)
        || (ReadBitmapFile(player->xDisplay, player->xBoardWindow, name,
                          &w, &h, pm, &x_hot, &y_hot) != BitmapSuccess)
        || (w != player->squareSize)
        || (h != player->squareSize))
    {
        unsigned long fg, bg;
        unsigned int depth;

        depth = DisplayPlanes(player->xDisplay, player->xScreen);

        if (player->monoMode)
        {
            fg = XBlackPixel(player->xDisplay, player->xScreen);
            bg = XWhitePixel(player->xDisplay, player->xScreen);
        }
        else if (qm == NULL)
        {
            fg = player->appData.oneColor;
            bg = player->appData.zeroColor;
        }
        else
        {
            fg = (player->black_pixel_is_zero ? 0 : ~0);
            bg = (player->black_pixel_is_zero ? ~0 : 0);
        }

        switch (player->boardSize)
        {
        case Large:
            *pm = XCreatePixmapFromBitmapData(player->xDisplay,
                                              player->xBoardWindow,
                                              (char *)large_bits,
                                              player->squareSize,
                                              player->squareSize,
                                              fg, bg, depth);
            break;

        case Medium:
            *pm = XCreatePixmapFromBitmapData(player->xDisplay,
                                              player->xBoardWindow,
                                              (char *)medium_bits,
                                              player->squareSize,
                                              player->squareSize,
                                              fg, bg, depth);
            break;

        case Small:
            *pm = XCreatePixmapFromBitmapData(player->xDisplay,
                                              player->xBoardWindow,
                                              (char *)small_bits,
                                              player->squareSize,
                                              player->squareSize,
                                              fg, bg, depth);
            break;
        }
    }
}




void
CreateGrid(void)
{
    int i, offset;

    offset = 2 * (player->squareSize + LINE_GAP);

    for (i = 0; i < BOARD_SIZE + 1; i++)
    {
        player->gridSegments[i].x1 = offset;
        player->gridSegments[i + BOARD_SIZE + 1].y1 = 0;
        player->gridSegments[i].y1 = player->gridSegments[i].y2
            = LINE_GAP / 2 + (i * (player->squareSize + LINE_GAP));
        player->gridSegments[i].x2 = LINE_GAP + BOARD_SIZE *
            (player->squareSize + LINE_GAP) + offset;
        player->gridSegments[i + BOARD_SIZE + 1].x1
            = player->gridSegments[i + BOARD_SIZE + 1].x2 = LINE_GAP / 2
            + (i * (player->squareSize + LINE_GAP)) + offset;
        player->gridSegments[i + BOARD_SIZE + 1].y2
            = BOARD_SIZE * (player->squareSize + LINE_GAP);
    }
}




void
CreatePieceMenus(void)
{
    int i;
    Widget entry;
    Arg args[1];
    ShogiSquare selection;

    XtSetArg(args[0], XtNlabel, "Black");
    blackPieceMenu = XtCreatePopupShell("menuW", simpleMenuWidgetClass,
                                        localPlayer.boardWidget, args, 1);

    for (i = 0; i < PIECE_MENU_SIZE; i++)
    {
        String item = pieceMenuStrings[i];

        if (strcmp(item, "----") == 0)
        {
            entry = XtCreateManagedWidget(item, smeLineObjectClass,
                                          blackPieceMenu, NULL, 0);
        }
        else
        {
            entry = XtCreateManagedWidget(item, smeBSBObjectClass,
                                          blackPieceMenu, NULL, 0);
            selection = pieceMenuTranslation[0][i];
            XtAddCallback(entry, XtNcallback,
                          (XtCallbackProc)PieceMenuSelect,
                          (XtPointer)selection);

            if (selection == BlackPawn)
            {
                XtSetArg(args[0], XtNpopupOnEntry, entry);
                XtSetValues(blackPieceMenu, args, 1);
            }
        }
    }

    XtSetArg(args[0], XtNlabel, "White");
    whitePieceMenu = XtCreatePopupShell("menuB", simpleMenuWidgetClass,
                                        localPlayer.boardWidget, args, 1);

    for (i = 0; i < PIECE_MENU_SIZE; i++)
    {
        String item = pieceMenuStrings[i];

        if (strcmp(item, "----") == 0)
        {
            entry = XtCreateManagedWidget(item, smeLineObjectClass,
                                          whitePieceMenu, NULL, 0);
        }
        else
        {
            entry = XtCreateManagedWidget(item, smeBSBObjectClass,
                                          whitePieceMenu, NULL, 0);
            selection = pieceMenuTranslation[1][i];
            XtAddCallback(entry, XtNcallback,
                          (XtCallbackProc)PieceMenuSelect,
                          (XtPointer)selection);

            if (selection == WhitePawn)
            {
                XtSetArg(args[0], XtNpopupOnEntry, entry);
                XtSetValues(whitePieceMenu, args, 1);
            }
        }
    }

    XtRegisterGrabAction(PieceMenuPopup, True,
                         (unsigned)(ButtonPressMask|ButtonReleaseMask),
                         GrabModeAsync, GrabModeAsync);
}




void
PieceMenuPopup(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    if (event->type != ButtonPress)
        return;

    if (gameMode != EditPosition)
        return;

    if (((pmFromX = EventToXSquare(event->xbutton.x)) < 1)
        || (pmFromX > BOARD_SIZE + 2)
        || ((pmFromY = EventToSquare(event->xbutton.y)) < 0))
    {
        pmFromX = pmFromY = -1;
        return;
    }

    if (localPlayer.flipView)
        pmFromX = BOARD_SIZE + 3 - pmFromX;
    else
        pmFromY = BOARD_SIZE - 1 - pmFromY;

    XtPopupSpringLoaded(XtNameToWidget(localPlayer.boardWidget, params[0]));
}




static void
PieceMenuSelect(Widget w, ShogiSquare piece, char *junk)
{
    if ((pmFromX < 0) || (pmFromY < 0))
        return;

    if (off_board(pmFromX))
    {
        int i, c;
        switch (piece)
        {
        case ClearBoard:
            break;

        case BlackPlay:
            break;

        case WhitePlay:
            break;

        default:
            i = pieceToCatchedIndex[piece];
            c = (piece >= WhitePawn);
            catches[0][c][i]++;
            DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
            XSync(localPlayer.xDisplay, False);
            return;
        }
    }

    pmFromX -= 2;

    switch (piece)
    {
    case ClearBoard:
        for (pmFromY = 0; pmFromY < BOARD_SIZE; pmFromY++)
            for (pmFromX = 0; pmFromX < BOARD_SIZE; pmFromX++)
                boards[0][pmFromY][pmFromX] = EmptySquare;

        ClearCatches(catches[0]);
        DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
        break;

    case BlackPlay:  /* not currently on menu */
        SetBlackToPlay();
        break;

    case WhitePlay:  /* not currently on menu */
        SetWhiteToPlay();
        break;

    default:
        boards[0][pmFromY][pmFromX] = piece;
        DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
        break;
    }

    XSync(localPlayer.xDisplay, False);
}




static void
SetBlackToPlay(void)
{
    int saveCM;

    if (gameMode != EditPosition)
        return;

    whitePlaysFirst = False;
    saveCM = currentMove;
    currentMove = 0;  /* kludge */
    DisplayClocks(ReDisplayTimers);
    currentMove = saveCM;
}




static void
SetWhiteToPlay(void)
{
    int saveCM;

    if (gameMode != EditPosition)
        return;

    whitePlaysFirst = True;
    saveCM = currentMove;
    currentMove = 1;  /* kludge */
    DisplayClocks(ReDisplayTimers);
    currentMove = saveCM;
}




/*
 * If the user selects on a border boundary or off the board, return failure.
 * Otherwise map the event coordinate to the square.
 */

int
EventToSquare(int x)
{
    if (x < LINE_GAP)
        return -1;

    x -= LINE_GAP;

    if ((x % (player->squareSize + LINE_GAP)) >= player->squareSize)
        return -1;

    x /= (player->squareSize + LINE_GAP);

    if (x >= BOARD_SIZE)
        return -1;

    return x;
}




int
EventToXSquare(int x)
{
    if (x < LINE_GAP)
        return -1;

    x -= LINE_GAP;

    if ((x % (player->squareSize + LINE_GAP)) >= player->squareSize)
        return -1;

    x /= (player->squareSize + LINE_GAP);

    if (x >= BOARD_SIZE + 4)
        return -1;

    return x;
}




ShogiSquare
CharToPiece(int c, int p)
{
    if (p)
    {
        switch (c)
        {
        default:
        case '.':   return EmptySquare;
        case 'P':   return BlackPPawn;
        case 'L':   return BlackPLance;
        case 'N':   return BlackPKnight;
        case 'S':   return BlackPSilver;
        case 'G':   return BlackGold;
        case 'R':   return BlackPRook;
        case 'B':   return BlackPBishop;
        case 'K':   return BlackKing;
        case 'p':   return WhitePPawn;
        case 'l':   return WhitePLance;
        case 'n':   return WhitePKnight;
        case 's':   return WhitePSilver;
        case 'g':   return WhiteGold;
        case 'r':   return WhitePRook;
        case 'b':   return WhitePBishop;
        case 'k':   return WhiteKing;
        }
    }
    else
    {
        switch (c)
        {
        default:
        case '.':   return EmptySquare;
        case 'P':   return BlackPawn;
        case 'L':   return BlackLance;
        case 'N':   return BlackKnight;
        case 'S':   return BlackSilver;
        case 'G':   return BlackGold;
        case 'R':   return BlackRook;
        case 'B':   return BlackBishop;
        case 'K':   return BlackKing;
        case 'p':   return WhitePawn;
        case 'l':   return WhiteLance;
        case 'n':   return WhiteKnight;
        case 's':   return WhiteSilver;
        case 'g':   return WhiteGold;
        case 'r':   return WhiteRook;
        case 'b':   return WhiteBishop;
        case 'k':   return WhiteKing;
        }
    }
}




/*
 * Convert coordinates to normal algebraic notation.
 * promoPiece must be NULLCHAR if not a promotion.
 */

ShogiMove
MakeAlg(int fromX, int fromY, int toX, int toY,
        char promoPiece, int currentBoardIndex, char *out)
{
    ShogiSquare piece;
    char *outp = out;

    if (fromX > 80)
    {
        piece = (fromX - 81);
        *outp++ = catchedIndexToChar[piece];
        *outp++ = '*';
        *outp++ = '9' - toX;
        *outp++ = 'i' - toY;
        *outp++ = NULLCHAR;
        return (BlackOnMove(forwardMostMove) ? BlackDrop : WhiteDrop);
    }
    else
    {
        *outp++ = '9' - fromX;
        *outp++ = 'i' - fromY;
        *outp++ = '9' - toX;
        *outp++ = 'i' - toY;
        *outp++ = promoPiece;
        *outp++ = NULLCHAR;

        if (promoPiece == NULLCHAR)
        {
            return NormalMove;
        }
        else
        {
            return (BlackOnMove(forwardMostMove)
                    ? BlackPromotion : WhitePromotion);
        }
    }
}




void
DrawSquare(int row, int column, ShogiSquare piece)
{
    int square_color, x, y, direction, font_ascent, font_descent;
    char string[2];
    XCharStruct overall;
    struct DisplayData *player;

    for (player = &localPlayer; True; player = &remotePlayer)
    {
        int offset, remote;

        remote = (player == &remotePlayer);
        offset = 2 * (player->squareSize + LINE_GAP);

        if (player->flipView)
        {
            x = LINE_GAP + ((BOARD_SIZE - 1) - column) *
                (player->squareSize + LINE_GAP) + offset;
            y = LINE_GAP + row * (player->squareSize + LINE_GAP);
        }
        else
        {
            x = LINE_GAP + column * (player->squareSize + LINE_GAP) + offset;
            y = LINE_GAP + ((BOARD_SIZE - 1) - row) *
                (player->squareSize + LINE_GAP);
        }

        square_color = (((column + row) % 2) ? LIGHT : DARK);

        if (piece == EmptySquare)
        {
            if (column < 0 || column >= BOARD_SIZE)
            {
                /* empty square off board */
                XFillRectangle(player->xDisplay, player->xBoardWindow,
                               player->squareOffBoardGC,
                               x, y, player->squareSize,
                               player->squareSize);
            }
            else
            {
                /* empty square on board */
                XFillRectangle(player->xDisplay,
                               player->xBoardWindow,
                               player->squareGC,
                               x, y,
                               player->squareSize,
                               player->squareSize);
            }
        }
        else if (player->monoMode) /* Draw a piece in mono mode. */
        {
            XCopyArea(player->xDisplay,
                      ((((((int)piece) < ((int)WhitePawn)))
                        ^ player->flipView)
                       ? *pieceToNormal[remote][(int)piece]
                       : *pieceToReverse[remote][(int)piece]),
                      player->xBoardWindow,
                      (player->monoMode
                       ? player->squareOffBoardGC  /* ??? FIXME? */
                       : player->pieceGC),
                      0, 0,
                      player->squareSize, player->squareSize, x, y);
        }
        else /* Draw a piece in color mode. */
        {
            if ((column < 0) || (column >= BOARD_SIZE))  /* off board */
            {
                /* draw piece background */

                XCopyPlane(player->xDisplay,
                           ((((((int)piece) < ((int)WhitePawn)))
                             ^ player->flipView)
                            ? *pieceToNormalSolid[remote][(int)piece]
                            : *pieceToReverseSolid[remote][(int)piece]),
                           player->xBoardWindow,
                           player->oPieceGC,
                           0, 0,
                           player->squareSize, player->squareSize, x, y, 1);

                /* draw piece bitmap */

                XCopyArea(player->xDisplay,
                          ((((((int)piece) < ((int)WhitePawn)))
                            ^ player->flipView)
                           ? *pieceToNormal[remote][(int)piece]
                           : *pieceToReverse[remote][(int)piece]),
                          player->xBoardWindow,
                          player->charPieceGC,
                          0, 0,
                          player->squareSize, player->squareSize, x, y);
            }
            else  /* on board */
            {
                /* draw piece background */

                XCopyPlane(player->xDisplay,
                           ((((((int)piece) < ((int)WhitePawn)))
                             ^ player->flipView)
                            ? *pieceToNormalSolid[remote][(int)piece]
                            : *pieceToReverseSolid[remote][(int)piece]),
                           player->xBoardWindow,
                           player->pieceGC,
                           0, 0,
                           player->squareSize, player->squareSize, x, y, 1);

                /* draw piece bitmap */

                XCopyArea(player->xDisplay,
                          ((((((int)piece) < ((int)WhitePawn)))
                            ^ player->flipView)
                           ? *pieceToNormal[remote][(int)piece]
                           : *pieceToReverse[remote][(int)piece]),
                          player->xBoardWindow,
                          player->charPieceGC,
                          0, 0,
                          player->squareSize, player->squareSize, x, y);
             }
        }

        string[1] = NULLCHAR;

        if (player->showCoords
            && (column >= 0) && (column < 9)
            && (row == (player->flipView ? 8 : 0)))
        {
            string[0] = '9' - column;
            XTextExtents(player->coordFontStruct, string, 1, &direction,
                         &font_ascent, &font_descent, &overall);

            if (player->monoMode)
            {
                XDrawImageString(player->xDisplay,
                                 player->xBoardWindow, player->coordGC,
                                 x + player->squareSize - overall.width - 2,
                                 y + player->squareSize - font_descent - 1,
                                 string, 1);
            }
            else
            {
                XDrawString(player->xDisplay, player->xBoardWindow,
                            player->coordGC,
                            x + player->squareSize - overall.width - 2,
                            y + player->squareSize - font_descent - 1,
                            string, 1);
            }
        }

        if (player->showCoords
            && (row >= 0) && (row < 9)
            && (column == (player->flipView ? 8 : 0)))
        {
            string[0] = 'i' - row;
            XTextExtents(player->coordFontStruct, string, 1, &direction,
                         &font_ascent, &font_descent, &overall);

            if (player->monoMode)
            {
                XDrawImageString(player->xDisplay,
                                 player->xBoardWindow, player->coordGC,
                                 x + 2, y + font_ascent + 1, string, 1);
            }
            else
            {
                XDrawString(player->xDisplay, player->xBoardWindow,
                            player->coordGC,
                            x + 2, y + font_ascent + 1, string, 1);
            }
        }

        if (!updateRemotePlayer || (player == &remotePlayer))
            break;
    }
}




void
EventProc(Widget widget, XtPointer client_data, XEvent *event)
{
    if (event->type == MappingNotify)
    {
        XRefreshKeyboardMapping((XMappingEvent *) event);
        return;
    }

    if (!XtIsRealized(widget))
        return;

    if ((event->type == ButtonPress) || (event->type == ButtonRelease))
    {
        if (event->xbutton.button != Button1)
            return;
    }

    switch (event->type)
    {
    case Expose:
        DrawPosition(widget, event, NULL, NULL);
        break;

    default:
        return;
    }
}




/*
 * event handler for redrawing the board
 */

void
DrawPosition(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    Arg args[1];
    int i, j;
    static Board lastBoard;
    static Catched lastCatches;
    static int lastBoardValid = 0;
    static int lastFlipView = 0, lastRemoteFlipView = 1;

    if (!player->Iconic)
    {
        XtSetArg(args[0], XtNiconic, False);
        XtSetValues(localPlayer.shellWidget, args, 1);
    }

    /*
     * It would be simpler to clear the window with XClearWindow()
     * but this causes a very distracting flicker.
     */

    if ((w == localPlayer.boardWidget)
        && (event == NULL)
        && lastBoardValid
        && (lastFlipView == localPlayer.flipView)
        && (!updateRemotePlayer
            || (lastRemoteFlipView == remotePlayer.flipView)))
    {
        for (i = 0; i < BOARD_SIZE; i++)
        {
            for (j = 0; j < BOARD_SIZE; j++)
            {
                if (boards[currentMove][i][j] != lastBoard[i][j])
                    DrawSquare(i, j, boards[currentMove][i][j]);
            }
        }

        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (catches[currentMove][i][j] != lastCatches[i][j])
                {
                    UpdateCatched(i, 0, False, True, currentMove);
                    break;
                }
            }
        }
    }
    else
    {
        XDrawSegments(localPlayer.xDisplay,
                      localPlayer.xBoardWindow, localPlayer.lineGC,
                      localPlayer.gridSegments, (BOARD_SIZE + 1) * 2);

        if (updateRemotePlayer)
        {
            XDrawSegments(remotePlayer.xDisplay,
                          remotePlayer.xBoardWindow, remotePlayer.lineGC,
                          remotePlayer.gridSegments, (BOARD_SIZE + 1) * 2);
        }

        for (i = 0; i < BOARD_SIZE; i++)
            for (j = 0; j < BOARD_SIZE; j++)
                DrawSquare(i, j, boards[currentMove][i][j]);

        UpdateCatched(0, 0, False, True, currentMove);
        UpdateCatched(1, 0, False, True, currentMove);
    }

    CopyBoard(lastBoard, boards[currentMove]);
    CopyCatches(lastCatches, catches[currentMove]);
    lastBoardValid = 1;
    lastFlipView = localPlayer.flipView;

    if (updateRemotePlayer)
        lastRemoteFlipView = remotePlayer.flipView;

    XSync(localPlayer.xDisplay, False);

    if (updateRemotePlayer)
        XSync(remotePlayer.xDisplay, False);
}




void
InitPosition(int redraw)
{
    currentMove = forwardMostMove = backwardMostMove = 0;
    CopyBoard(boards[0], initialPosition);
    ClearCatches(catches[0]);

    if (redraw)
        DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}




void
CopyBoard(Board to, Board from)
{
    int i, j;

    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            to[i][j] = from[i][j];
}




void
CopyCatches(Catched to, Catched from)
{
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 8; j++)
            to[i][j] = from[i][j];
}




void
SendCurrentBoard(FILE *fp)
{
    SendBoard(fp, boards[currentMove], catches[currentMove]);
}




void
SendBoard(FILE *fp, Board board, Catched catches)
{
    char message[MSG_SIZ];
    ShogiSquare *bp;
    int i, j;

    SendToProgram("edit\n", fp);
    SendToProgram("#\n", fp);

    for (i = BOARD_SIZE - 1; i >= 0; i--)
    {
        bp = &board[i][0];

        for (j = 0; j < BOARD_SIZE; j++, bp++)
        {
            if (((int) *bp) < (int)WhitePawn)
            {
                sprintf(message, "%c%c%c%s\n",
                        pieceToChar[(int) *bp],
                        '9' - j, 'i' - i,
                        (pieceIsPromoted[(int) *bp] ? "+" : ""));
                SendToProgram(message, fp);
            }
        }
    }

    for (i = 0; i <= 7; i++)
    {
        int n;

        for (n = catches[0][i]; n > 0; n--)
        {
            sprintf(message, "%c*\n",
                    catchedIndexToChar[i]);
            SendToProgram(message, fp);
        }
    }

    SendToProgram("c\n", fp);

    for (i = BOARD_SIZE - 1; i >= 0; i--)
    {
        bp = &board[i][0];

        for (j = 0; j < BOARD_SIZE; j++, bp++)
        {
            if ((((int) *bp) != ((int)EmptySquare))
                && (((int) *bp) >= ((int)WhitePawn)))
            {
                sprintf(message, "%c%c%c%s\n",
                        pieceToChar[((int) *bp) - ((int)WhitePawn)],
                        '9' - j, 'i' - i,
                        (pieceIsPromoted[(int) *bp] ? "+" : ""));
                SendToProgram(message, fp);
            }
        }
    }

    for (i = 0; i <= 7; i++)
    {
        int n;

        for (n = catches[1][i]; n > 0; n--)
        {
            sprintf(message, "%c*\n",
                    catchedIndexToChar[i]);
            SendToProgram(message, fp);
        }
    }

    SendToProgram(".\n", fp);
}




static int
PromotionPossible(int fromY, int toY, ShogiSquare piece)
{
    if (((int)piece) < ((int)WhitePawn))
    {
        if ((fromY < 6) && (toY < 6))
            return False;
    }
    else
    {
        if ((fromY > 2) && (toY > 2))
            return False;
    }

    return piecePromotable[(int)piece];

}




static void
ShowCount(int row, int column, int n)
{
    int offset = 2 * (player->squareSize + LINE_GAP);
    int x, y, direction, font_ascent, font_descent;
    char string[2];
    XCharStruct overall;
    struct DisplayData *player;

    DrawSquare(row, column, EmptySquare);

    if (n <= 1)
        return;

    for (player = &localPlayer; True; player = &remotePlayer)
    {
        if (player->flipView)
        {
            x = LINE_GAP + ((BOARD_SIZE - 1) - column) *
                (player->squareSize + LINE_GAP) + offset;
            y = LINE_GAP + row * (player->squareSize + LINE_GAP);
        }
        else
        {
            x = LINE_GAP + column * (player->squareSize + LINE_GAP) + offset;
            y = LINE_GAP + ((BOARD_SIZE - 1) - row) *
                (player->squareSize + LINE_GAP);
        }

        x -= player->squareSize / 2;

        string[1] = NULLCHAR;

        if (n > 9)
            string[0] = '*';
        else
            string[0] = '0' + n;

        XTextExtents(player->coordFontStruct, string, 1, &direction,
                     &font_ascent, &font_descent, &overall);

        if (player->monoMode)
        {
            XDrawImageString(player->xDisplay, player->xBoardWindow,
                             player->coordGC,
                             x + player->squareSize - overall.width - 2,
                             y + player->squareSize - font_descent - 1,
                             string, 1);
        }
        else
        {
            XDrawString(player->xDisplay, player->xBoardWindow,
                        player->coordGC,
                        x + player->squareSize - overall.width - 2,
                        y + player->squareSize - font_descent - 1,
                        string, 1);
        }

        if (!updateRemotePlayer || (player == &remotePlayer))
            break;
    }
}




void
UpdateCatched(int Color, int Figure, int Drop, int DropAll, int currentMove)
{
    int n, F;
    int x, y;

    /* Determine first row and column. */

    if (Color)
    {
        x = -1;
        y = BOARD_SIZE - 1;
    }
    else
    {
        x = BOARD_SIZE;
        y = 0;
    }

    if (DropAll)
        n = 0;
    else
        n = catches[currentMove][Color][Figure];

    /* Update the display for captured pieces
       if no piece of the dropped type is there (Drop && n==1)
       or if a piece type is removed (NOT Drop && n==0).
       In the other cases update only the count. */

    if (DropAll || (Drop && (n == 1)) || (!Drop && (n == 0)))
    {
        /* show all captured pieces */
        n = 0;

        for (F = pawn; F <= king; F++)
        {
            int c;

            if ((c = catches[currentMove][Color][F]) > 0)
            {
                n++;
                DrawSquare(y, x, catchedIndexToPiece[Color][F]);
                ShowCount(y, (Color ? (x - 1) : (x + 1)), c);

                if (Color)
                    y--;
                else
                    y++;
            }
        }

        if (DropAll)
        {
            for (; n < 9; n++)
            {
                DrawSquare(y, x, EmptySquare);
                ShowCount(y, (Color ? (x - 1) : (x + 1)), 0);

                if (Color)
                    y--;
                else
                    y++;
            }
        }
        else if (!Drop)
        {
            /* remove one line! */
            DrawSquare(y, x, EmptySquare);
            ShowCount(y, (Color ? (x - 1) : (x + 1)), 0);
        }
    }
    else
    {
        /* show the actual count */
        for (F = pawn; F <= Figure - 1; F++)
        {
            if (catches[currentMove][Color][F] > 0)
            {
                if (Color)
                    y--;
                else
                    y++;
            }
        }

        ShowCount(y, (Color ? (x - 1) : (x + 1)), n);
    }
}




#ifdef BLINK_COUNT

static int BlinkCount = 0;
static int BlinkRow, BlinkCol;
static ShogiSquare BlinkPiece;


void
BlinkSquareProc(void)
{
    if (BlinkCount > 0)
    {
        BlinkCount--;
        DrawSquare (BlinkRow, BlinkCol,
                    ((BlinkCount & 1) ? EmptySquare : BlinkPiece));

        if (BlinkCount > 0)
        {
            blinkSquareXID
                = XtAppAddTimeOut(appContext,
                                  150,
                                  (XtTimerCallbackProc)BlinkSquareProc,
                                  NULL);
        }
    }
    else
    {
        BlinkCount = 0;
    }
}




void
BlinkSquare(int row, int col, ShogiSquare piece)
{
    BlinkCount = 2 * BLINK_COUNT + 1;
    BlinkRow = row;
    BlinkCol = col;
    BlinkPiece = piece;
    BlinkSquareProc();
}


#endif /* BLINK_COUNT */




static int
PieceOfCatched(int color, int x, int y, int currentMove)
{
    int F, n;

    if (color)
    {
        if (x != 1)
            return (no_piece);

        y = 8 - y;
    }
    else
    {
        if (x != 11)
            return no_piece;
    }

    for (F = pawn, n = 0; F <= king; F++)
    {
        if (catches[currentMove][color][F] > 0)
        {
            if (n == y)
                return F;

            n++;
        }
    }

    return no_piece;
}




/*
 * event handler for parsing user moves
 */

void
HandleUserMove(Widget w, XEvent *event)
{
    ShogiMove move_type;
    ShogiSquare from_piece;
    int to_x, to_y, fromRemotePlayer;

    if (updateRemotePlayer)
    {
        if (((w != localPlayer.boardWidget)
             && (w != remotePlayer.boardWidget))
            || (matchMode != MatchFalse))
        {
            return;
        }

        fromRemotePlayer = (w == remotePlayer.boardWidget);
    }
    else
    {
        if ((w != localPlayer.boardWidget) || (matchMode != MatchFalse))
            return;

        fromRemotePlayer = False;
    }

    player = (fromRemotePlayer ? &remotePlayer : &localPlayer);

    if (player->promotionUp)
    {
        XtPopdown(player->promotionShell);
        XtDestroyWidget(player->promotionShell);
        player->promotionUp = False;
        fromX = fromY = -1;
    }

    switch (gameMode)
    {
    case EndOfGame:
    case PlayFromGameFile:
    case TwoMachinesPlay:
        return;

    case MachinePlaysBlack:
        if (BlackOnMove(forwardMostMove))
        {
            DisplayMessage("It is not your turn", fromRemotePlayer);
            return;
        }

        break;

    case MachinePlaysWhite:
        if (!BlackOnMove(forwardMostMove))
        {
            DisplayMessage("It is not your turn", fromRemotePlayer);
            return;
        }

        break;

    case ForceMoves:
        forwardMostMove = currentMove;
        break;

    default:
        break;
    }

    if (currentMove != forwardMostMove)
    {
        DisplayMessage("Displayed position is not current",
                       fromRemotePlayer);
        return;
    }

    switch (event->type)
    {
    case ButtonPress:
        if ((fromX >= 0) || (fromY >= 0))
            return;

        if (((fromX = EventToXSquare(event->xbutton.x)) < 1)
            || (fromX > BOARD_SIZE + 2)
            || ((fromY = EventToSquare(event->xbutton.y)) < 0))
        {
            fromX = fromY = -1;
            return;
        }

        if (player->flipView)
            fromX = BOARD_SIZE + 3 - fromX;
        else
            fromY = BOARD_SIZE - 1 - fromY;

        break;

    case ButtonRelease:
        if ((fromX < 0) || (fromY < 0))
            return;

        if (((to_x = EventToXSquare(event->xbutton.x)) < 1)
            || (to_x > BOARD_SIZE + 2)
            || ((to_y = EventToSquare(event->xbutton.y)) < 0))
        {
            if (gameMode == EditPosition && !off_board(fromX))
            {
                fromX -= 2;
                boards[0][fromY][fromX] = EmptySquare;
                DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
                XSync(localPlayer.xDisplay, False);

                if (updateRemotePlayer)
                    XSync(remotePlayer.xDisplay, False);
            }

            fromX = fromY = -1;
            return;
        }

        if (player->flipView)
            to_x = BOARD_SIZE + 3 - to_x;
        else
            to_y = BOARD_SIZE - 1 - to_y;

        if ((fromX == to_x) && (fromY == to_y))
        {
            fromX = fromY = -1;
            return;
        }

        if (gameMode == EditPosition)
        {
            ShogiSquare piece;

            if (off_board(fromX))
            {
                /* Remove a catched piece */
                int i, c;
                c = ((fromX < 5) ^ player->flipView);
                i = PieceOfCatched(c, fromX, fromY, 0);

                if (i == no_piece)
                {
                    fromX = fromY = -1;
                    return;
                }
                else
                {
                    piece = catchedIndexToPiece[c][i];
                    catches[0][c][i]--;
                }
            }
            else
            {
                /* remove piece from board field */
                fromX -= 2;
                piece = boards[0][fromY][fromX];
                boards[0][fromY][fromX] = EmptySquare;
            }

            if (!off_board(to_x))
            {
                /* drop piece to board field */
                ShogiSquare catched_piece;
                to_x -= 2;
                catched_piece = boards[0][to_y][to_x];

                if (catched_piece != EmptySquare)
                {
                    /* put piece to catched pieces */
                    int i = pieceToCatchedIndex[catched_piece];
                    int c = (catched_piece < WhitePawn);
                    catches[0][c][i]++;
                }

                /* place moved piece */
                boards[0][to_y][to_x] = piece;
            }

            fromX = fromY = -1;
            DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
            XSync(localPlayer.xDisplay, False);

            if (updateRemotePlayer)
                XSync(remotePlayer.xDisplay, False);

            return;
        }

        if (off_board(fromX))
        {
            int c     = (BlackOnMove(forwardMostMove) ? 0 : 1);
            int piece = PieceOfCatched(c, fromX, fromY, currentMove);

            if (piece == no_piece)
            {
                fromX = fromY = -1;
                return;
            }
            else
            {
                if (updateRemotePlayer
                    && (BlackOnMove(forwardMostMove) == fromRemotePlayer))
                {
                    DisplayMessage("Do not try to drop your opponent's pieces!",
                                   fromRemotePlayer);
                    fromX = fromY = -1;
                    return;
                }

                fromX = fromY = piece + 81;
                to_x -= 2;
                move_type = (BlackOnMove(forwardMostMove)
                             ? BlackDrop : WhiteDrop);
                MakeMove(&move_type, fromX, fromY, to_x, to_y);

#ifdef BLINK_COUNT
                if (updateRemotePlayer)
                    BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif

                FinishUserMove(move_type, to_x, to_y);
                break;
            }
        }
        else if (off_board(to_x))
        {
            fromX = fromY = -1;
            return;
        }
        else
        {
            fromX -= 2;
            to_x -= 2;
            from_piece = boards[currentMove][fromY][fromX];

            if ((from_piece != EmptySquare)
                && updateRemotePlayer
                && ((from_piece < WhitePawn) == fromRemotePlayer))
            {
                DisplayMessage("Do not try to move your opponent's pieces!",
                               fromRemotePlayer);
                fromX = fromY = -1;
                return;
            }

            if (PromotionPossible(fromY, to_y, from_piece))
            {
                PromotionPopUp(from_piece, to_x, to_y, fromRemotePlayer);
                return;
            }

            move_type = NormalMove;
            MakeMove(&move_type, fromX, fromY, to_x, to_y);

#ifdef BLINK_COUNT
            if (updateRemotePlayer)
                BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif

            FinishUserMove(move_type, to_x, to_y);
            break;
        }
    }
}




void
FinishUserMove(ShogiMove move_type, int to_x, int to_y)
{
    char user_move[MSG_SIZ];

    /* output move for gnushogi */
    switch (move_type)
    {
    case BlackPromotion:
    case WhitePromotion:
        sprintf(user_move, "%c%c%c%c+\n",
                '9' - fromX, 'i' - fromY, '9' - to_x, 'i' - to_y);
        break;

    case BlackDrop:
    case WhiteDrop:
        sprintf(user_move, "%c*%c%c\n",
                catchedIndexToChar[fromX - 81], '9' - to_x, 'i' - to_y);
        break;

    case NormalMove:
        sprintf(user_move, "%c%c%c%c\n",
                '9' - fromX, 'i' - fromY, '9' - to_x, 'i' - to_y);
        break;

    default:
        fprintf(stderr, "%s: internal error; bad move_type\n",
                (char *)programName);
        break;
    }

    Attention(firstProgramPID);

    if (firstSendTime)
        SendTimeRemaining(toFirstProgFP);

    SendToProgram(user_move, toFirstProgFP);
    strcpy(moveList[currentMove - 1], user_move);

    fromX = fromY = -1;

    if (gameMode == PauseGame)
    {
        /* a user move restarts a paused game*/
        PauseProc(NULL, NULL, NULL, NULL);
    }

    switch (gameMode)
    {
    case ForceMoves:
        break;

    case BeginningOfGame:
        if (localPlayer.appData.noShogiProgram)
            lastGameMode = gameMode = ForceMoves;
        else
            lastGameMode = gameMode = MachinePlaysWhite;

        ModeHighlight();
        break;

    case MachinePlaysWhite:
    case MachinePlaysBlack:
    default:
        break;
    }
}




/* Simple parser for moves from gnushogi. */
void
ParseMachineMove(char *machine_move, ShogiMove *move_type,
                 int *from_x, int *from_y, int *to_x, int *to_y)
{
#define no_digit(c) (c < '0' || c > '9')
    {
        if (no_digit(machine_move[0]))
        {
            switch (machine_move[0])
            {
            case 'P':
                *from_x = 81;
                break;

            case 'L':
                *from_x = 82;
                break;

            case 'N':
                *from_x = 83;
                break;

            case 'S':
                *from_x = 84;
                break;

            case 'G':
                *from_x = 85;
                break;

            case 'B':
                *from_x = 86;
                break;

            case 'R':
                *from_x = 87;
                break;

            case 'K':
                *from_x = 88;
                break;

            default:
                *from_x = -1;
            }

            *from_y = *from_x;
            *to_x   = '9' - machine_move[2];
            *to_y   = 'i' - machine_move[3];
        }
        else
        {
            *from_x = '9' - machine_move[0] ;
            *from_y = 'i' - machine_move[1];
            *to_x   = '9' - machine_move[2];
            *to_y   = 'i' - machine_move[3];

            switch (machine_move[4])
            {
            case '+':
                *move_type = (BlackOnMove(forwardMostMove)
                    ? BlackPromotion : WhitePromotion);
                break;

            default:
                *move_type = NormalMove;
                break;
            }
        }
    }
}




void
SkipString(char **mpr)
{
    while (**mpr == ' ')
        (*mpr)++;

    while ((**mpr != ' ') && (**mpr != NULLCHAR) && (**mpr != '\n'))
        (*mpr)++;

    while (**mpr == ' ')
        (*mpr)++;
}




void
HandleMachineMove(char *message, FILE *fp)
{
    char machine_move[MSG_SIZ], buf1[MSG_SIZ], buf2[MSG_SIZ];
    int from_x, from_y, to_x, to_y;
    ShogiMove move_type;
    char *mpr;

#ifdef SYNCHTIME
    long time_remaining;
#endif

    maybeThinking = False;

    if (strncmp(message, "warning:", 8) == 0)
    {
        DisplayMessage(message, False);

        if (updateRemotePlayer)
            DisplayMessage(message, True);

        return;
    }

    /*
     * If shogi program startup fails, exit with an error message.
     * Attempts to recover here are futile.
     */

    if ((strstr(message, "unknown host") != NULL)
        || (strstr(message, "No remote directory") != NULL)
        || (strstr(message, "not found") != NULL)
        || (strstr(message, "No such file") != NULL)
        || (strstr(message, "Permission denied") != NULL))
    {
        fprintf(stderr,
                "%s: failed to start shogi program %s on %s: %s\n",
                programName,
                ((fp == fromFirstProgFP)
                 ? localPlayer.appData.firstShogiProgram
                 : localPlayer.appData.secondShogiProgram),
                ((fp == fromFirstProgFP)
                 ? localPlayer.appData.firstHost
                 : localPlayer.appData.secondHost),
                message);
        ShutdownShogiPrograms(message);
        exit(1);
    }

    /*
     * If the move is illegal, cancel it and redraw the board.
     */

    if (strncmp(message, "Illegal move", 12) == 0)
    {
        if (fp == fromFirstProgFP && firstSendTime == 2)
        {
            /* First program doesn't have the "time" command */
            firstSendTime = 0;
            return;
        }
        else if (fp == fromSecondProgFP && secondSendTime == 2)
        {
            /* Second program doesn't have the "time" command */
            secondSendTime = 0;
            return;
        }

        if (forwardMostMove <= backwardMostMove)
            return;

        if (gameMode == PauseGame)
            PauseProc(NULL, NULL, NULL, NULL);

        if (gameMode == PlayFromGameFile)
        {
            /* Stop reading this game file */
            gameMode = ForceMoves;
            ModeHighlight();
        }

        currentMove = --forwardMostMove;

        if ((gameMode == PlayFromGameFile)
            || (gameMode == ForceMoves))
            DisplayClocks(ReDisplayTimers);
        else
            DisplayClocks(SwitchTimers);

        sprintf(buf1, "Illegal move: %s", parseList[currentMove]);
        DisplayMessage(buf1, False);

        if (updateRemotePlayer)
            DisplayMessage(buf1, True);

#ifdef BLINK_COUNT
        /*
         * Disable blinking of the target square.
         */

        if (BlinkCount > 0)
        {
            /* If BlinkCount is even, the piece is currently displayed. */
            if (!(BlinkCount & 1))
                DrawSquare (BlinkRow, BlinkCol, EmptySquare);

            /* BlinkCount = 0 will force the next blink timeout
             * to do nothing. */
            BlinkCount = 0;
        }
#endif

        DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);

        XSync(localPlayer.xDisplay, False);

        if (updateRemotePlayer)
            XSync(remotePlayer.xDisplay, False);

        return;
    }

    if (strstr(message, "GNU Shogi") != NULL)
    {
        at_least_gnushogi_1_2p03 = True;
        return;
    }

    if (strncmp(message, "Hint:", 5) == 0)
    {
        char promoPiece;
        sscanf(message, "Hint: %s", machine_move);
        ParseMachineMove(machine_move, &move_type,
                         &from_x, &from_y, &to_x, &to_y);

        if (move_type == WhitePromotion || move_type == BlackPromotion)
            promoPiece = '+';
        else
            promoPiece = NULLCHAR;

        move_type = MakeAlg(from_x, from_y, to_x, to_y, promoPiece,
                            currentMove, buf1);
        sprintf(buf2, "Hint: %s", buf1);
        DisplayMessage(buf2, False);

        if (updateRemotePlayer)
            DisplayMessage(buf2, True);

        return;
    }

    if (strncmp(message, "Clocks:", 7) == 0)
    {
        sscanf(message, "Clocks: %ld %ld",
               &blackTimeRemaining, &whiteTimeRemaining);
        DisplayClocks(ReDisplayTimers);

        return;
    }

    /*
     * win, lose or draw
     */

    if (strncmp(message, "Black", 5) == 0)
    {
        ShutdownShogiPrograms("Black wins");
        return;
    }
    else if (strncmp(message, "White", 5) == 0)
    {
        ShutdownShogiPrograms("White wins");
        return;
    }
    else if (strncmp(message, "Repetition", 10) == 0)
    {
        ShutdownShogiPrograms("Repetition");
        return;
    }
    else if (strncmp(message, "opponent mates!", 15) == 0)
    {
        switch ((gameMode == PauseGame) ? pausePreviousMode : gameMode)
        {
        case MachinePlaysWhite:
            ShutdownShogiPrograms("Black wins");
            break;

        case MachinePlaysBlack:
            ShutdownShogiPrograms("White wins");
            break;

        case TwoMachinesPlay:
            ShutdownShogiPrograms((fp == fromFirstProgFP)
                                  ? "Black wins" : "White wins");
            break;

        default:
            /* can't happen */
            break;
        }

        return;
    }
    else if (strncmp(message, "computer mates!", 15) == 0)
    {
        switch ((gameMode == PauseGame) ? pausePreviousMode : gameMode)
        {
        case MachinePlaysWhite:
            ShutdownShogiPrograms("White wins");
            break;

        case MachinePlaysBlack:
            ShutdownShogiPrograms("Black wins");
            break;

        case TwoMachinesPlay:
            ShutdownShogiPrograms((fp == fromFirstProgFP)
                                  ? "White wins" : "Black wins");
            break;

        default:
            /* can't happen */
            break;
        }

        return;
    }
    else if (strncmp(message, "Draw", 4) == 0)
    {
        ShutdownShogiPrograms("Draw");
        return;
    }

    /*
     * normal machine reply move
     */
    maybeThinking = True;

    if (strstr(message, "...") != NULL)
    {
        sscanf(message, "%s %s %s", buf1, buf2, machine_move);

#ifdef SYNCHTIME
        mpr = message;
        SkipString(&mpr); /* skip move number */
        SkipString(&mpr); /* skip ... */
        SkipString(&mpr); /* skip move */

        if ((gameMode != TwoMachinesPlay) && (gameMode != ForceMoves)
            && ((*mpr == '-') || ((*mpr >= '0') && (*mpr <= '9'))))
        {
            /* synchronize with shogi program clock */
            sscanf(mpr, "%ld", &time_remaining);

            if (xshogiDebug)
            {
                printf("from '%s' synchronize %s clock %ld\n",
                       message,
                       (BlackOnMove(forwardMostMove)
                        ? "Black's"
                        : "White's"),
                       time_remaining);
            }

            if (BlackOnMove(forwardMostMove))
                blackTimeRemaining = time_remaining;
            else
                whiteTimeRemaining = time_remaining;
        }
#endif

        if (machine_move[0] == NULLCHAR)
            return;
    }
    else
    {
        mpr = message;

#ifdef SYNCHTIME
        if (strstr(message, "time") == NULL)
        {
            /* remaining time will be determined from move */
            SkipString(&mpr); /* skip move number */
            SkipString(&mpr); /* skip move */
        }

        if ((gameMode != TwoMachinesPlay) && (gameMode != ForceMoves)
            && ((*mpr == '-') || ((*mpr >= '0') && (*mpr <= '9'))))
        {
            /* synchronize with shogi program clock */
            sscanf(mpr, "%ld", &time_remaining);

            if (xshogiDebug)
            {
                printf("from '%s' synchronize %s clock %ld\n",
                       message,
                       ((!BlackOnMove(forwardMostMove))
                        ? "Black's" : "White's"),
                       time_remaining);
            }

            if (!BlackOnMove(forwardMostMove))
                blackTimeRemaining = time_remaining;
            else
                whiteTimeRemaining = time_remaining;
        }
        else
#endif

            if (xshogiDebug)
                printf("ignore noise: '%s'\n", message);

        return; /* ignore noise */
    }

    strcpy(moveList[forwardMostMove], machine_move);

    ParseMachineMove(machine_move, &move_type, &from_x, &from_y,
                     &to_x, &to_y);

    if (gameMode != PauseGame)
        currentMove = forwardMostMove;  /* display latest move */

    MakeMove(&move_type, from_x, from_y, to_x, to_y);

#ifdef BLINK_COUNT
    if (gameMode != TwoMachinesPlay)
        BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif

    if ((gameMode != PauseGame) && localPlayer.appData.ringBellAfterMoves)
        putc(BELLCHAR, stderr);

    if ((gameMode == TwoMachinesPlay)
        || ((gameMode == PauseGame)
            && (pausePreviousMode == TwoMachinesPlay)))
    {
        strcat(machine_move, "\n");

        if (BlackOnMove(forwardMostMove))
        {
            Attention(secondProgramPID);

            if (secondSendTime)
                SendTimeRemaining(toSecondProgFP);

            SendToProgram(machine_move, toSecondProgFP);

            if (firstMove)
            {
                firstMove = False;
                SendToProgram(localPlayer.appData.blackString,
                              toSecondProgFP);
            }
        }
        else
        {
            Attention(firstProgramPID);

            if (firstSendTime)
                SendTimeRemaining(toFirstProgFP);

            SendToProgram(machine_move, toFirstProgFP);

            if (firstMove)
            {
                firstMove = False;
                SendToProgram(localPlayer.appData.blackString,
                              toFirstProgFP);
            }
        }
    }
}




void
ReadGameFile(void)
{
    for (;;)
    {
        if (!ReadGameFileProc())
            return;

        if (matchMode == MatchOpening)
            continue;

        readGameXID
            = XtAppAddTimeOut(appContext,
                              (int)(1000 * localPlayer.appData.timeDelay),
                              (XtTimerCallbackProc) ReadGameFile, NULL);
        break;
    }
}



/*
 * FIXME: there is a naming inconsistency: here ReadGameFileProc() is
 * called by ReadGameFile() while in other places XXXProc() calls XXX().
 */

int
ReadGameFileProc(void)
{
    ShogiMove move_type;
    char move[MSG_SIZ], buf[MSG_SIZ];

    if (gameFileFP == NULL)
        return (int)False;

    if (gameMode == PauseGame)
        return True;

    if (gameMode != PlayFromGameFile)
    {
        fclose(gameFileFP);
        gameFileFP = NULL;
        return (int)False;
    }

    if (commentUp)
    {
        XtPopdown(commentShell);
        XtDestroyWidget(commentShell);
        commentUp = False;
    }

    fgets(move, MSG_SIZ, gameFileFP);
    move[strlen(move) - 1] = NULLCHAR;
    sprintf(buf, "# %s game file", programName);

    if (strncmp(move, buf, strlen(buf)))
    {
        strcat(move, ": no xshogi game file");
        DisplayMessage(move, False);
        return (int)False;
    }

    DisplayName(move);
    rewind(gameFileFP);

    parseGameFile();

    move_type = (ShogiMove)0;

    lastGameMode = gameMode;
    gameMode = ForceMoves;
    ModeHighlight();

    if (!loaded_game_finished)
        DisplayMessage("End of game file", False);

    if (readGameXID != 0)
    {
        XtRemoveTimeOut(readGameXID);
        readGameXID = 0;
    }

    fclose(gameFileFP);
    gameFileFP = NULL;

    return (int)False;
}




/*
 * Apply a move to the given board.  Oddity: move_type is ignored on input
 * unless the move is seen to be a pawn promotion, in which case move_type
 * tells us what to promote to.
 */

void
ApplyMove(ShogiMove *move_type, int from_x, int from_y,
          int to_x, int to_y, int currentMove)
{
    ShogiSquare piece, cpiece;
    char pieceChar;
    int  i, c;

    if (from_x > 80)
    {
        i = from_x - 81;
        c = (BlackOnMove(currentMove) ? 1 : 0);
        cpiece = catchedIndexToPiece[c][i];
        boards[currentMove][to_y][to_x] = cpiece;
        catches[currentMove][c][i]--;
    }
    else if (PromotionPossible(from_y, to_y,
                               piece = boards[currentMove][from_y][from_x]))
    {
        cpiece = boards[currentMove][to_y][to_x];

        if (cpiece != EmptySquare)
        {
            i = pieceToCatchedIndex[cpiece];
            c = (cpiece < WhitePawn);
            catches[currentMove][c][i]++;
        }

        if (*move_type == NormalMove)
        {
            boards[currentMove][to_y][to_x] = piece;
        }
        else
        {
            boards[currentMove][to_y][to_x] = piece = pieceToPromoted[piece];
            pieceChar = '+';
        }

        boards[currentMove][from_y][from_x] = EmptySquare;
    }
    else
    {
        ShogiSquare piece = boards[currentMove][to_y][to_x];

        if (piece != EmptySquare)
        {
            i = pieceToCatchedIndex[piece];
            c = (piece < WhitePawn);
            catches[currentMove][c][i]++;
        }

        *move_type = NormalMove;
        boards[currentMove][to_y][to_x] =
            boards[currentMove][from_y][from_x];
        boards[currentMove][from_y][from_x] = EmptySquare;
    }
}




/*
 * MakeMove() displays moves.  If they are illegal, GNU shogi will detect
 * this and send an Illegal move message.  XShogi will then retract the move.
 * The clockMode False case is tricky because it displays the player on move.
 */

void
MakeMove(ShogiMove *move_type, int from_x, int from_y, int to_x, int to_y)
{
    char message[MSG_SIZ], movestr[MSG_SIZ];
    char promoPiece = NULLCHAR;

    forwardMostMove++;

    CopyBoard(boards[forwardMostMove], boards[forwardMostMove - 1]);
    CopyCatches(catches[forwardMostMove], catches[forwardMostMove - 1]);

    ApplyMove(move_type, from_x, from_y, to_x, to_y, forwardMostMove);

    endMessage[0] = NULLCHAR;

    timeRemaining[0][forwardMostMove] = blackTimeRemaining;
    timeRemaining[1][forwardMostMove] = whiteTimeRemaining;

    if ((gameMode == PauseGame) && (pausePreviousMode != PlayFromGameFile))
        return;

    currentMove = forwardMostMove;

    if (gameMode == PlayFromGameFile)
    {
        sprintf(message, "%d. %s%s",
                ((currentMove + 1) / 2),
                (BlackOnMove(currentMove) ? "... " : ""),
                currentMoveString);
        strcpy(parseList[currentMove - 1], currentMoveString);
    }
    else
    {
        if ((*move_type == WhitePromotion) || (*move_type == BlackPromotion))
            promoPiece = '+';
        else
            promoPiece = NULLCHAR;

        MakeAlg(from_x, from_y, to_x, to_y, promoPiece,
                currentMove - 1, movestr);
        sprintf(message, "%d. %s%s",
                ((currentMove + 1) / 2),
                (BlackOnMove(currentMove) ? "... " : ""),
                movestr);
        strcpy(parseList[currentMove - 1], movestr);
    }

    DisplayMessage(message, False);

    if ((gameMode == PlayFromGameFile) || (gameMode == ForceMoves)
        || ((gameMode == PauseGame)
            && (pausePreviousMode == PlayFromGameFile)))
    {
        DisplayClocks(ReDisplayTimers);
    }
    else
    {
        DisplayClocks(SwitchTimers);
    }

    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);

    XSync(localPlayer.xDisplay, False);

    if (updateRemotePlayer)
    {
        DisplayMessage(message, True);
        XSync(remotePlayer.xDisplay, False);
    }
}




void
InitShogiProgram(char *host_name, char *program_name, int *pid,
                 FILE **to, FILE **from, XtIntervalId *xid, int *sendTime)
{
    char  arg_buf[10];
    char *arg1, *arg2;
    int   to_prog[2], from_prog[2];
    FILE *from_fp, *to_fp;
    int   dummy_source;
    XtInputId dummy_id;

    if (localPlayer.appData.noShogiProgram)
        return;

    signal(SIGPIPE, CatchPipeSignal);
    pipe(to_prog);
    pipe(from_prog);

    if ((*pid = fork()) == 0)
    {
        signal(SIGPIPE, CatchPipeSignal);

        dup2(to_prog[0], 0);
        dup2(from_prog[1], 1);
        close(to_prog[0]);
        close(to_prog[1]);
        close(from_prog[0]);
        close(from_prog[1]);
        dup2(1, fileno(stderr));    /* force stderr to the pipe */

        if (localPlayer.appData.searchTime != NULL)
        {
            sprintf(arg_buf, "%d", searchTime);
            arg1 = arg_buf;
            arg2 = (char *)NULL;
        }
        else if (localPlayer.appData.searchDepth > 0)
        {
            sprintf(arg_buf, "%d", localPlayer.appData.searchDepth);
            arg1 = "1";
            arg2 = "9999";
        }
        else
        {
            sprintf(arg_buf, "%d", localPlayer.appData.movesPerSession);
            arg1 = arg_buf;
            arg2 = localPlayer.appData.timeControl;
        }

        if (strcmp(host_name, "localhost") == 0)
        {
            execlp(program_name, program_name, arg1, arg2,
                   (char *)NULL);
        }
        else
        {
            execlp(localPlayer.appData.remoteShell,
                   localPlayer.appData.remoteShell,
                   host_name, program_name, arg1, arg2,
                   (char *)NULL);
        }

        perror(program_name);
        exit(1);
    }

    close(to_prog[0]);
    close(from_prog[1]);

    *from = from_fp = fdopen(from_prog[0], "r");
    *to   = to_fp   = fdopen(to_prog[1],   "w");
    setbuf(from_fp, NULL);
    setbuf(to_fp,   NULL);

    ReceiveFromProgram(from_fp, &dummy_source, &dummy_id); /* "GNU Shogi"*/

    if (!at_least_gnushogi_1_2p03)
    {
        fprintf(stderr, "you must have at least gnushogi-1.2p03\n");
        exit(1);
    }

    if (*pid == 0)
        return;

    *xid = XtAppAddInput(appContext, fileno(from_fp),
                         (XtPointer)XtInputReadMask,
                         (XtInputCallbackProc)ReceiveFromProgram,
                         (XtPointer)from_fp);

    SendToProgram(localPlayer.appData.initString, *to);

    if (localPlayer.appData.gameIn)
        SendToProgram("gamein\n", *to);

    SendSearchDepth(*to);

    if (*sendTime == 2)
    {
        /* Does program have "time" command? */
        char buf[MSG_SIZ];

        sprintf(buf, "time %ld\n", blackTimeRemaining / 10);
        SendToProgram(buf, to_fp);
        ReceiveFromProgram(from_fp, &dummy_source, &dummy_id);

        if (*sendTime == 2)
        {
            *sendTime = 1;  /* yes! */
            sprintf(buf, "otime %ld\n", whiteTimeRemaining / 10);
            SendToProgram(buf, to_fp);
            ReceiveFromProgram(from_fp, &dummy_source, &dummy_id);
        }
    }
}




void
ShutdownShogiPrograms(char *why)
{
    lastGameMode = gameMode;
    gameMode = EndOfGame;
    ModeHighlight();
    CopyBoard(boards[currentMove + 1], boards[currentMove]);
    CopyCatches(catches[currentMove + 1], catches[currentMove]);
    strncpy(parseList[currentMove], why, MOVE_LEN);
    parseList[currentMove][MOVE_LEN - 1] = NULLCHAR;
    currentMove++;
    DisplayMessage(why, False);

    if (readGameXID != 0)
        XtRemoveTimeOut(readGameXID);

    readGameXID = 0;

    if (firstProgramPID != 0)
    {
        fclose(fromFirstProgFP);
        fclose(toFirstProgFP);
        fromFirstProgFP = toFirstProgFP = NULL;

        if (kill(firstProgramPID, SIGTERM) == 0)
            WAIT0;
    }

    firstProgramPID = 0;

    if (firstProgramXID != 0)
        XtRemoveInput(firstProgramXID);

    firstProgramXID = 0;

    if (secondProgramPID != 0)
    {
        fclose(fromSecondProgFP);
        fclose(toSecondProgFP);
        fromSecondProgFP = toSecondProgFP = NULL;

        if (kill(secondProgramPID, SIGTERM) == 0)
            WAIT0;
    }

    secondProgramPID = 0;

    if (secondProgramXID != 0)
        XtRemoveInput(secondProgramXID);

    secondProgramXID = 0;

    DisplayClocks(StopTimers);

    if (matchMode != MatchFalse)
    {
        if (localPlayer.appData.saveGameFile[0] != NULLCHAR)
            SaveGame(localPlayer.appData.saveGameFile);

        exit(0);
    }
}




void
CommentPopUp(char *label)
{
    Arg args[2];
    Position x, y;
    Dimension bw_width, pw_width;

    if (commentUp)
    {
        XtPopdown(commentShell);
        XtDestroyWidget(commentShell);
        commentUp = False;
    }

    DisplayMessage("Comment", False);

    XtSetArg(args[0], XtNwidth, &bw_width);
    XtGetValues(localPlayer.formWidget, args, 1);

    XtSetArg(args[0], XtNresizable, True);
    XtSetArg(args[1], XtNwidth, bw_width - 8);

    commentShell = XtCreatePopupShell("Comment",
                                      transientShellWidgetClass,
                                      localPlayer.commandsWidget, args, 2);

    XtSetArg(args[0], XtNlabel, label);

    (void)XtCreateManagedWidget("commentLabel", labelWidgetClass,
                                commentShell, args, 1);

    XtRealizeWidget(commentShell);

    XtSetArg(args[0], XtNwidth, &pw_width);
    XtGetValues(commentShell, args, 1);

    XtTranslateCoords(localPlayer.shellWidget,
                      (bw_width - pw_width) / 2, -50, &x, &y);

    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(commentShell, args, 2);

    XtPopup(commentShell, XtGrabNone);
    commentUp = True;
}




void
FileNamePopUp(char *label, Boolean (*proc) (char *))
{
    Arg args[2];
    Widget popup, dialog;
    Position x, y;
    Dimension bw_width, pw_width;

    fileProc = proc;

    XtSetArg(args[0], XtNwidth, &bw_width);
    XtGetValues(localPlayer.boardWidget, args, 1);

    XtSetArg(args[0], XtNresizable, True);
    XtSetArg(args[1], XtNwidth, DIALOG_SIZE);

    popup = XtCreatePopupShell("File Name Prompt",
                               transientShellWidgetClass,
                               localPlayer.commandsWidget, args, 2);

    XtSetArg(args[0], XtNlabel, label);
    XtSetArg(args[1], XtNvalue, "");

    dialog = XtCreateManagedWidget("dialog", dialogWidgetClass,
                                   popup, args, 2);

    XawDialogAddButton(dialog, "ok", FileNameCallback, (XtPointer) dialog);
    XawDialogAddButton(dialog, "cancel", FileNameCallback,
                       (XtPointer) dialog);

    XtRealizeWidget(popup);

    XtSetArg(args[0], XtNwidth, &pw_width);
    XtGetValues(popup, args, 1);

    XtTranslateCoords(localPlayer.boardWidget,
                      (bw_width - pw_width) / 2, 10, &x, &y);

    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(popup, args, 2);

    XtPopup(popup, XtGrabExclusive);
    filenameUp = True;

    XtSetKeyboardFocus(localPlayer.shellWidget, popup);
}




void
FileNameCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    String name;
    Arg args[1];

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    if (strcmp(name, "cancel") == 0)
    {
        XtPopdown(w = XtParent(XtParent(w)));
        XtDestroyWidget(w);
        filenameUp = False;
        ModeHighlight();
        return;
    }

    FileNameAction(w, NULL, NULL, NULL);
}




void
FileNameAction(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    char buf[MSG_SIZ];
    String name;

    name = XawDialogGetValueString(w = XtParent(w));

    if ((name != NULL) && (*name != NULLCHAR))
    {
        strcpy(buf, name);
        XtPopdown(w = XtParent(w));
        XtDestroyWidget(w);
        filenameUp = False;
        (*fileProc)(buf);  /* I can't see a way not
                              to use a global here */
        ModeHighlight();
        return;
    }

    XtPopdown(w = XtParent(w));
    XtDestroyWidget(w);
    filenameUp = False;
    ModeHighlight();
}




void
PromotionPopUp(ShogiSquare piece, int to_x, int to_y, int fromRemotePlayer)
{
    Arg args[2];
    Widget dialog;
    Position x, y;
    Dimension bw_width, bw_height, pw_width, pw_height;

    player = (fromRemotePlayer ? &remotePlayer : &localPlayer);

    pmi.piece = piece;
    pmi.to_x = to_x;
    pmi.to_y = to_y;

    XtSetArg(args[0], XtNwidth, &bw_width);
    XtSetArg(args[1], XtNheight, &bw_height);
    XtGetValues(player->boardWidget, args, 2);

    XtSetArg(args[0], XtNresizable, True);

    player->promotionShell
        = XtCreatePopupShell("Promotion",
                             transientShellWidgetClass,
                             player->commandsWidget, args, 1);

    XtSetArg(args[0], XtNlabel, "Promote piece?");
    dialog = XtCreateManagedWidget("promotion", dialogWidgetClass,
                                   player->promotionShell, args, 1);

    XawDialogAddButton(dialog, "Yes", PromotionCallback,
                       (XtPointer) dialog);
    XawDialogAddButton(dialog, "No", PromotionCallback,
                       (XtPointer) dialog);
    XawDialogAddButton(dialog, "cancel", PromotionCallback,
                       (XtPointer) dialog);

    XtRealizeWidget(player->promotionShell);

    XtSetArg(args[0], XtNwidth, &pw_width);
    XtSetArg(args[1], XtNheight, &pw_height);
    XtGetValues(player->promotionShell, args, 2);

    XtTranslateCoords(player->boardWidget,
                      ((bw_width - pw_width) / 2),
                      (LINE_GAP
                       + player->squareSize / 3
                       + (((piece == BlackPawn) ^ (player->flipView))
                          ? 0
                          : (6 * (player->squareSize + LINE_GAP)))),
                      &x, &y);

    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(player->promotionShell, args, 2);

    XtPopup(player->promotionShell, XtGrabNone);

    player->promotionUp = True;
}




void
PromotionCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    String name;
    Arg args[1];
    ShogiMove move_type;
    struct DisplayData *player;

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    w = XtParent(XtParent(w));
    player = ((w == remotePlayer.promotionShell)
              ? &remotePlayer : &localPlayer);
    XtPopdown(w);
    XtDestroyWidget(w);
    player->promotionUp = False;

    if (fromX == -1)
        return;

    if (strcmp(name, "Yes") == 0)
    {
        if ((int)pmi.piece < (int)WhitePawn)
            move_type = BlackPromotion;
        else
            move_type = WhitePromotion;
    }
    else if (strcmp(name, "No") == 0)
    {
        move_type = NormalMove;
    }
    else /* strcmp(name, "cancel") == 0 */
    {
        fromX = fromY = -1;
        return;
    }

    MakeMove(&move_type, fromX, fromY, pmi.to_x, pmi.to_y);

#ifdef BLINK_COUNT
    if (updateRemotePlayer)
    {
        BlinkSquare(pmi.to_y, pmi.to_x,
                    boards[currentMove][pmi.to_y][pmi.to_x]);
    }
#endif

    FinishUserMove(move_type, pmi.to_x, pmi.to_y);
}




void
FileModePopUp(char *name)
{
    Arg args[2];
    Widget dialog;
    Position x, y;
    Dimension bw_width, bw_height, pw_width, pw_height;

    struct DisplayData *player = &localPlayer;

    strcpy(fmi.name, name);

    XtSetArg(args[0], XtNwidth, &bw_width);
    XtSetArg(args[1], XtNheight, &bw_height);
    XtGetValues(player->boardWidget, args, 2);

    XtSetArg(args[0], XtNresizable, True);
    player->filemodeShell
        = XtCreatePopupShell("FileMode",
                             transientShellWidgetClass,
                             player->commandsWidget, args, 1);

    XtSetArg(args[0], XtNlabel, "Append to existing file?");
    dialog = XtCreateManagedWidget("filemode", dialogWidgetClass,
                                   player->filemodeShell, args, 1);

    XawDialogAddButton(dialog, "Yes", FileModeCallback,
                       (XtPointer) dialog);
    XawDialogAddButton(dialog, "No", FileModeCallback,
                       (XtPointer) dialog);
    XawDialogAddButton(dialog, "cancel", FileModeCallback,
                       (XtPointer) dialog);

    XtRealizeWidget(player->filemodeShell);

    XtSetArg(args[0], XtNwidth, &pw_width);
    XtSetArg(args[1], XtNheight, &pw_height);
    XtGetValues(player->filemodeShell, args, 2);

    XtTranslateCoords(player->boardWidget, (bw_width - pw_width) / 2,
                      LINE_GAP + player->squareSize/3 +
                      (6*(player->squareSize + LINE_GAP)),
                      &x, &y);

    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(player->filemodeShell, args, 2);

    XtPopup(player->filemodeShell, XtGrabNone);

    filemodeUp = True;
}




void
FileModeCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    String name;
    Arg args[1];

    XtSetArg(args[0], XtNlabel, &name);
    XtGetValues(w, args, 1);

    XtPopdown(w = XtParent(XtParent(w)));
    XtDestroyWidget(w);

    if (strcmp(name, "Yes") == 0)
    {
        strcpy(fmi.mode, "a");
    }
    else if (strcmp(name, "No") == 0)
    {
        strcpy(fmi.mode, "w");
    }
    else /* strcmp(name, "cancel") == 0 */
    {
        filemodeUp = False;
        return;
    }

    XtPopdown(localPlayer.filemodeShell);
    XtDestroyWidget(localPlayer.filemodeShell);

    SaveGame(fmi.name);

    filemodeUp = False;
}




void
SelectCommand(Widget w, XtPointer client_data, XtPointer call_data)
{
    Cardinal fromRemotePlayer = (Cardinal)client_data;

    XawListReturnStruct *list_return = XawListShowCurrent(w);

    player = fromRemotePlayer ? &remotePlayer : &localPlayer;

    fromX = fromY = -1;

    if (player->promotionUp)
    {
        XtPopdown(player->promotionShell);
        XtDestroyWidget(player->promotionShell);
        player->promotionUp = False;
    }

    (*buttonProcs[list_return->list_index])
        (w, NULL, NULL, &fromRemotePlayer);

    if (!filenameUp)
        ModeHighlight();
}




void
HighlightProcButton(XtActionProc proc)
{
    int i = 0;

    if (proc == NULL)
    {
        XawListUnhighlight(localPlayer.commandsWidget);

        if (updateRemotePlayer)
            XawListUnhighlight(remotePlayer.commandsWidget);

        return;
    }

    for (;;)
    {
        if (buttonProcs[i] == NULL)
        {
            XawListUnhighlight(localPlayer.commandsWidget);

            if (updateRemotePlayer)
                XawListUnhighlight(remotePlayer.commandsWidget);

            return;
        }

        if (buttonProcs[i] == proc)
        {
            XawListHighlight(localPlayer.commandsWidget, i);

            if (updateRemotePlayer)
                XawListHighlight(remotePlayer.commandsWidget, i);

            return;
        }

        i++;
    }
}




void
ModeHighlight(void)
{
    switch (gameMode)
    {
    case BeginningOfGame:
        if (localPlayer.appData.noShogiProgram)
            HighlightProcButton(ForceProc);
        else
            HighlightProcButton(MachineBlackProc);

        break;

    case MachinePlaysBlack:
        HighlightProcButton(MachineBlackProc);
        break;

    case MachinePlaysWhite:
        HighlightProcButton(MachineWhiteProc);
        break;

    case TwoMachinesPlay:
        HighlightProcButton(TwoMachinesProc);
        break;

    case ForceMoves:
        HighlightProcButton(ForceProc);

        break;

    case PlayFromGameFile:
        HighlightProcButton(LoadGameProc);
        break;

    case PauseGame:
        HighlightProcButton(PauseProc);
        break;

    case EditPosition:
        HighlightProcButton(EditPositionProc);
        break;

    case EndOfGame:
    default:
        HighlightProcButton(NULL);
        break;
    }
}




/*
 * Button procedures
 */

void
QuitRemotePlayerProc(void)
{
    /* This should be modified... */
    XCloseDisplay(remotePlayer.xDisplay);
    /* XtDestroyWidget(remotePlayer.shellWidget); */
    updateRemotePlayer = False;
    DisplayMessage("Remote player has pressed Quit", False);
    fromX = fromY = -1;
}



void
QuitProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    if (updateRemotePlayer)
        QuitRemotePlayerProc();

    ShutdownShogiPrograms("Quitting");
    exit(0);
}



void
LoadGameProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (fromRemotePlayer)
    {
        DisplayMessage("only opponent may load game", fromRemotePlayer);
        return;
    }

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first.", False);
        return;
    }

    if (localPlayer.appData.loadGameFile == NULL)
        FileNamePopUp("Game file name?", LoadGame);
    else
        (void) LoadGame(localPlayer.appData.loadGameFile);
}




Boolean
LoadGame(char *name)
{
    char buf[MSG_SIZ];

    loaded_game_finished = 0;

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first", False);
        return (int)False;
    }

    if (localPlayer.appData.loadGameFile != name)
    {
        if (localPlayer.appData.loadGameFile)
            XtFree(localPlayer.appData.loadGameFile);

        localPlayer.appData.loadGameFile = XtMalloc(strlen(name) + 1);
        strcpy(localPlayer.appData.loadGameFile, name);
    }

    if ((gameFileFP = fopen(name, "r")) == NULL)
    {
        sprintf(buf, "Can't open %s", name);
        DisplayMessage(buf, False);
        XtFree(localPlayer.appData.loadGameFile);
        localPlayer.appData.loadGameFile = NULL;
        return (int)False;
    }

    lastGameMode = gameMode = PlayFromGameFile;
    ModeHighlight();
    InitPosition(True);
    DisplayClocks(StopTimers);

    if (firstProgramXID == 0)
    {
        InitShogiProgram(localPlayer.appData.firstHost,
                         localPlayer.appData.firstShogiProgram,
                         &firstProgramPID, &toFirstProgFP,
                         &fromFirstProgFP, &firstProgramXID,
                         &firstSendTime);
    }

    SendToProgram(localPlayer.appData.initString, toFirstProgFP);
    SendSearchDepth(toFirstProgFP);
    SendToProgram("force\n", toFirstProgFP);

    currentMove = forwardMostMove = backwardMostMove = 0;

    ReadGameFile();

    return True;
}




/*
 * Restart the shogi program and feed it all the moves made so far.
 * Used when the user wants to back up from end of game, when gnushogi
 * has already exited.  Assumes gameMode == EndOfGame.
 */

void
ResurrectShogiProgram(void)
{
    char buf[MSG_SIZ];
    int i;

    if (currentMove > 0)
        currentMove--;  /* delete "Black wins" or the like */

    InitShogiProgram(localPlayer.appData.firstHost,
                     localPlayer.appData.firstShogiProgram,
                     &firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
                     &firstProgramXID, &firstSendTime);

    SendToProgram(localPlayer.appData.initString, toFirstProgFP);
    SendSearchDepth(toFirstProgFP);
    SendToProgram("force\n", toFirstProgFP);
    gameMode = lastGameMode = ForceMoves;
    ModeHighlight();

    i = (whitePlaysFirst ? 1 : 0);

    if (startedFromSetupPosition)
        SendBoard(toFirstProgFP, boards[i], catches[i]);

    for (; i < currentMove; i++)
    {
        strcpy(buf, moveList[i]);
        SendToProgram(buf, toFirstProgFP);
    }

    if (!firstSendTime)
    {
        /* can't tell gnushogi what its clock should read,
           so we bow to its notion. */
        DisplayClocks(ResetTimers);
        timeRemaining[0][currentMove] = blackTimeRemaining;
        timeRemaining[1][currentMove] = whiteTimeRemaining;
    }
}




void
MachineWhiteProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("no machine moves in challenge mode",
                       fromRemotePlayer);

        return;
    }

    if (gameMode == PauseGame)
        PauseProc(w, event, prms, nprms);

    if (gameMode == PlayFromGameFile)
        ForceProc(w, event, prms, nprms);

    if (gameMode == EditPosition)
        EditPositionDone();

    if ((gameMode == EndOfGame)
        || (gameMode == PlayFromGameFile)
        || (gameMode == TwoMachinesPlay)
        || localPlayer.appData.noShogiProgram
        || (gameMode == MachinePlaysWhite))
    {
        return;
    }

    if (BlackOnMove((gameMode == ForceMoves)
                    ? currentMove
                    : forwardMostMove))
    {
        DisplayMessage("It is not White's turn.", False);
        return;
    }

    if (gameMode == ForceMoves)
        forwardMostMove = currentMove;

    lastGameMode = gameMode = MachinePlaysWhite;
    ModeHighlight();
    SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);
    DisplayClocks(StartTimers);
}




void
MachineBlackProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("no machine moves in challenge mode",
                       fromRemotePlayer);
        return;
    }

    if (gameMode == PauseGame)
        PauseProc(w, event, prms, nprms);

    if (gameMode == PlayFromGameFile)
        ForceProc(w, event, prms, nprms);

    if (gameMode == EditPosition)
        EditPositionDone();

    if ((gameMode == EndOfGame)
        || (gameMode == PlayFromGameFile)
        || (gameMode == TwoMachinesPlay)
        || localPlayer.appData.noShogiProgram
        || (gameMode == MachinePlaysBlack))
    {
        return;
    }

    if (!BlackOnMove((gameMode == ForceMoves)
                     ? currentMove
                     : forwardMostMove))
    {
        DisplayMessage("It is not Black's turn.", False);
        return;
    }

    if (gameMode == ForceMoves)
        forwardMostMove = currentMove;

    lastGameMode = gameMode = MachinePlaysBlack;
    ModeHighlight();
    SendToProgram(localPlayer.appData.blackString, toFirstProgFP);
    DisplayClocks(StartTimers);
}




void
ForwardProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    char buf[MSG_SIZ];
    int target;
    unsigned int state;

    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("Forward button disabled.", fromRemotePlayer);
        return;
    }

    if ((gameMode == EndOfGame) || (gameMode == EditPosition))
        return;

    if (gameMode == PlayFromGameFile)
        PauseProc(w, event, prms, nprms);

    if (currentMove >= forwardMostMove)
        return;

    if (event == NULL)
    {
        /* Kludge */
        Window root, child;
        int root_x, root_y;
        int win_x, win_y;
        XQueryPointer(localPlayer.xDisplay, localPlayer.xBoardWindow,
                      &root, &child, &root_x, &root_y,
                      &win_x, &win_y, &state);
    }
    else
    {
        state = event->xkey.state;
    }

    if (state & ShiftMask)
        target = forwardMostMove;
    else
        target = currentMove + 1;

    if (gameMode == ForceMoves)
    {
        while (currentMove < target)
        {
            strcpy(buf, moveList[currentMove++]);
            SendToProgram(buf, toFirstProgFP);
        }
    }
    else
    {
        currentMove = target;
    }

    if (gameMode == ForceMoves)
    {
        blackTimeRemaining = timeRemaining[0][currentMove];
        whiteTimeRemaining = timeRemaining[1][currentMove];
    }

    DisplayClocks(ReDisplayTimers);
    DisplayMove(currentMove - 1);
    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}




void
ResetFileProc(void)
{
    char *buf = "";

    if (updateRemotePlayer)
        return;

    if (localPlayer.appData.loadGameFile)
        XtFree(localPlayer.appData.loadGameFile);

    if (localPlayer.appData.loadPositionFile)
        XtFree(localPlayer.appData.loadPositionFile);

    localPlayer.appData.loadGameFile
        = localPlayer.appData.loadPositionFile = NULL;
    DisplayName(buf);

    if (gameFileFP != NULL)
    {
        fclose(gameFileFP);
        gameFileFP = NULL;
    }
}




void
ResetChallenge(void)
{
    char *buf = "";

    if (localPlayer.appData.challengeDisplay)
        XtFree(localPlayer.appData.challengeDisplay);

    localPlayer.appData.challengeDisplay = NULL;
    DisplayName(buf);
}




void
ResetProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (fromRemotePlayer)
    {
        DisplayMessage("Only your opponent may reset the game.",
                       fromRemotePlayer);
        return;
    }

    Reset(True);
}




void
Reset(int redraw)  /* Boolean */
{
    ResetFileProc();
    ResetChallenge();

    localPlayer.flipView = False;
    remotePlayer.flipView = True;
    startedFromSetupPosition = whitePlaysFirst = False;
    matchMode = MatchFalse;
    firstMove = True;
    blackFlag = whiteFlag = False;
    maybeThinking = False;

    endMessage[0] = NULLCHAR;

    ShutdownShogiPrograms("");
    lastGameMode = gameMode = BeginningOfGame;
    ModeHighlight();
    InitPosition(redraw);
    DisplayClocks(ResetTimers);
    timeRemaining[0][0] = blackTimeRemaining;
    timeRemaining[1][0] = whiteTimeRemaining;
    InitShogiProgram(localPlayer.appData.firstHost,
                     localPlayer.appData.firstShogiProgram,
                     &firstProgramPID, &toFirstProgFP,
                     &fromFirstProgFP, &firstProgramXID,
                     &firstSendTime);

    if (commentUp)
    {
        XtPopdown(commentShell);
        XtDestroyWidget(commentShell);
        commentUp = False;
    }

    if (localPlayer.promotionUp)
    {
        XtPopdown(localPlayer.promotionShell);
        XtDestroyWidget(localPlayer.promotionShell);
        localPlayer.promotionUp = False;
    }

    if (updateRemotePlayer && remotePlayer.promotionUp)
    {
        XtPopdown(remotePlayer.promotionShell);
        XtDestroyWidget(remotePlayer.promotionShell);
        remotePlayer.promotionUp = False;
    }
}




void
ClearCatches(int (*catches)[8])
{
    int c, p;

    for (c = 0; c <= 1; c++)
        for (p = 0; p <= 7; p++)
            catches[c][p] = 0;
}




Boolean
Challenge(char *name)
{
    char buf[MSG_SIZ];
    int argc;
    char **argv;
    XrmDatabase database;

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first.", False);
        return (int)False;
    }

    if (localPlayer.appData.challengeDisplay != name)
    {
        if (localPlayer.appData.challengeDisplay)
            XtFree(localPlayer.appData.challengeDisplay);

        localPlayer.appData.challengeDisplay = XtMalloc(strlen(name) + 1);
        strcpy(localPlayer.appData.challengeDisplay, name);
    }

    sprintf(buf, "trying to connect to %s.....", name);
    DisplayMessage(buf, False);

    argc = global_argc;
    argv = global_argv;

    if ((remotePlayer.xDisplay
         = XtOpenDisplay(appContext, name, "XShogi",
                         "XShogi", 0, 0, &argc, argv)) == NULL)
    {
        sprintf(buf, "Can't open display %s", name);
        DisplayMessage(buf, False);
        XtFree(localPlayer.appData.challengeDisplay);
        localPlayer.appData.challengeDisplay = NULL;
        return (int)False;
    }

    DisplayMessage("connected! creating remote window...", False);

    remotePlayer.xScreen = DefaultScreen(remotePlayer.xDisplay);

    remotePlayer.shellWidget
        = XtAppCreateShell(NULL, "XShogi",
                           applicationShellWidgetClass,
                           remotePlayer.xDisplay, NULL, 0);

    database = XtDatabase(remotePlayer.xDisplay);

    XrmParseCommand(&database,
                    shellOptions, XtNumber(shellOptions),
                    "XShogi", &argc, argv);

    XtGetApplicationResources(remotePlayer.shellWidget,
                              &remotePlayer.appData, clientResources,
                              XtNumber(clientResources), NULL, 0);

    player = &remotePlayer;

    CreatePlayerWindow();

    updateRemotePlayer = True;

    DisplayName("REMOTE");
    DrawPosition(remotePlayer.boardWidget, NULL, NULL, NULL);
    DisplayClocks(ReDisplayTimers);

    DisplayMessage("ready to play", False);
    DisplayMessage("ready to play", True);

    return True;
}




void
ChallengeProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("You are already in challenge mode.",
                       fromRemotePlayer);
        return;
    }

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first.", False);
        return;
    }

    if (localPlayer.appData.challengeDisplay == NULL)
        FileNamePopUp("Challenge display?", Challenge);
    else
        (void) Challenge(localPlayer.appData.challengeDisplay);
}




Boolean
SelectLevel(char *command)
{
    char buf[MSG_SIZ];

    sprintf(buf, "level %s\n", command);
    SendToProgram(buf, toFirstProgFP);

    return True;
}




void
SelectLevelProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    if ((BlackOnMove(forwardMostMove) && (gameMode == MachinePlaysBlack))
        || (!BlackOnMove(forwardMostMove) && (gameMode == MachinePlaysWhite)))
    {
        DisplayMessage("Wait until your turn.", False);
    }
    else
    {
        FileNamePopUp("#moves #minutes", SelectLevel);
    }
}




void
MoveNowProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    if ((!BlackOnMove(forwardMostMove) && (gameMode == MachinePlaysBlack))
        || (BlackOnMove(forwardMostMove) && (gameMode == MachinePlaysWhite)))
    {
        DisplayMessage("Wait until machine's turn.", False);
    }
    else
    {
        Attention(firstProgramPID);
    }
}




void
LoadPositionProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (fromRemotePlayer)
    {
        DisplayMessage("only opponent may load position", fromRemotePlayer);
        return;
    }

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first.", False);
        return;
    }

    FileNamePopUp("Position file name?", LoadPosition);
}




Boolean
LoadPosition(char *name)
{
    char *p, line[MSG_SIZ], buf[MSG_SIZ];
    Board initial_position;
    Catched initial_catches;
    FILE *fp;
    int i, j;

    if (gameMode != BeginningOfGame)
    {
        DisplayMessage("Press Reset first.", False);
        return False;
    }

    if (localPlayer.appData.loadPositionFile != name)
    {
        if (localPlayer.appData.loadPositionFile)
            XtFree(localPlayer.appData.loadPositionFile);

        localPlayer.appData.loadPositionFile = XtMalloc(strlen(name) + 1);
        strcpy(localPlayer.appData.loadPositionFile, name);
    }

    if ((fp = fopen(name, "r")) == NULL)
    {
        sprintf(buf, "Can't open %s", name);
        DisplayMessage(buf, False);
        XtFree(localPlayer.appData.loadPositionFile);
        localPlayer.appData.loadPositionFile = NULL;
        return False;
    }

    lastGameMode = gameMode = ForceMoves;
    ModeHighlight();
    startedFromSetupPosition = True;

    if (firstProgramXID == 0)
    {
        InitShogiProgram(localPlayer.appData.firstHost,
                         localPlayer.appData.firstShogiProgram,
                         &firstProgramPID, &toFirstProgFP,
                         &fromFirstProgFP, &firstProgramXID,
                         &firstSendTime);
    }

    /*
     * Check and skip header information in position file.
     */

    fgets(line, MSG_SIZ, fp);
    line[strlen(line) - 1] = NULLCHAR;
    sprintf(buf, "# %s position file", programName);

    if (strncmp(line, buf, strlen(buf)))
    {
        strcat(line, ": no xshogi position file");
        DisplayMessage(line, False);
        return False;
    }

    DisplayName(line);
    fgets(line, MSG_SIZ, fp); /* skip opponents */

    for (i = BOARD_SIZE - 1; i >= 0; i--)
    {
        fgets(line, MSG_SIZ, fp);

        for (p = line, j = 0; j < BOARD_SIZE; p++)
        {
            int promoted = False;  /* CHECKME: is this valid? */

            if (*p == '+')
                promoted = True;

            if (*p == ' ')
                promoted = False;

            p++;
            initial_position[i][j++] = CharToPiece(*p, promoted);
        }
    }

    {
        int color;

        for (color = 0; color <= 1; color++)
        {
            fscanf(fp, "%i%i%i%i%i%i%i%i\n",
                   &initial_catches[color][pawn],
                   &initial_catches[color][lance],
                   &initial_catches[color][knight],
                   &initial_catches[color][silver],
                   &initial_catches[color][gold],
                   &initial_catches[color][bishop],
                   &initial_catches[color][rook],
                   &initial_catches[color][king]);
        }
    }

    whitePlaysFirst = False;

    if (!feof(fp))
    {
        fgets(line, MSG_SIZ, fp);

        if (strncmp(line, "white", strlen("white")) == 0)
            whitePlaysFirst = True;
    }

    fclose(fp);

    if (whitePlaysFirst)
    {
        CopyBoard(boards[0], initial_position);
        CopyCatches(catches[0], initial_catches);
        strcpy(moveList[0], " ...\n");
        strcpy(parseList[0], " ...\n");
        currentMove = forwardMostMove = backwardMostMove = 1;
        CopyBoard(boards[1], initial_position);
        CopyCatches(catches[1], initial_catches);
        SendToProgram("white\n", toFirstProgFP);
        SendToProgram("force\n", toFirstProgFP);
        SendCurrentBoard(toFirstProgFP);
        DisplayMessage("White to play", False);
    }
    else
    {
        currentMove = forwardMostMove = backwardMostMove = 0;
        CopyBoard(boards[0], initial_position);
        CopyCatches(catches[0], initial_catches);
        SendCurrentBoard(toFirstProgFP);
        SendToProgram("force\n", toFirstProgFP);
        DisplayMessage("Black to play", False);
    }

    DisplayClocks(ResetTimers);
    timeRemaining[0][1] = blackTimeRemaining;
    timeRemaining[1][1] = whiteTimeRemaining;

    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
    return True;
}




void
EditPositionProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("Edit button disabled", fromRemotePlayer);
        return;
    }

    if (gameMode == EditPosition)
        return;

    ForceProc(w, event, prms, nprms);

    if (gameMode != ForceMoves)
        return;

    DisplayName("<-- Press to set side to play next");
    DisplayMessage("Mouse: 1=drag, 2=black, 3=white", False);

    lastGameMode = gameMode = EditPosition;
    ModeHighlight();

    if (currentMove > 0)
        CopyBoard(boards[0], boards[currentMove]);

    whitePlaysFirst = !BlackOnMove(forwardMostMove);
    currentMove = forwardMostMove = backwardMostMove = 0;
}




void
EditPositionDone(void)
{
    startedFromSetupPosition = True;
    SendToProgram(localPlayer.appData.initString, toFirstProgFP);
    SendSearchDepth(toFirstProgFP);

    if (whitePlaysFirst)
    {
        strcpy(moveList[0], " ...\n");
        strcpy(parseList[0], " ...\n");
        currentMove = forwardMostMove = backwardMostMove = 1;
        CopyBoard(boards[1], boards[0]);
        CopyCatches(catches[1], catches[0]);
        SendToProgram("force\n", toFirstProgFP);
        SendCurrentBoard(toFirstProgFP);
        DisplayName(" ");
        DisplayMessage("White to play", False);
    }
    else
    {
        currentMove = forwardMostMove = backwardMostMove = 0;
        SendCurrentBoard(toFirstProgFP);
        SendToProgram("force\n", toFirstProgFP);
        DisplayName(" ");
        DisplayMessage("Black to play", False);
    }

    lastGameMode = gameMode = ForceMoves;
}



/*
 * FUNCTION
 *     BackwardProc
 *
 * DESCRIPTION
 *     This function executes when undoing a move.
 *     FIXME: this function is totally hosed!!!
 *
 * ARGUMENTS
 *     Widget w
 *     XEvent *event
 *     String *prms
 *     Cardinal *nprms
 *
 * RETURN VALUE
 *     void
 *
 */

void
BackwardProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int target;
    unsigned int state;

    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("Backward button disabled", fromRemotePlayer);
        return;
    }

    /*
     * Why do we need this here?
     */

    ForceProc(w, event, prms, nprms);

    if ((currentMove <= backwardMostMove) || (gameMode == EditPosition))
        return;

    if (gameMode == EndOfGame)
        ResurrectShogiProgram();

    if (gameMode == PlayFromGameFile)
        PauseProc(w, event, prms, nprms);

    if (event == NULL)
    {
        /* Kludge */
        Window root, child;
        int root_x, root_y;
        int win_x, win_y;

        XQueryPointer(localPlayer.xDisplay, localPlayer.xBoardWindow,
                      &root, &child, &root_x, &root_y,
                      &win_x, &win_y, &state);
    }
    else
    {
        state = event->xkey.state;
    }

    if (state & ShiftMask)
    {
        target = backwardMostMove;
    }
    else
    {
        target = currentMove - 1;
    }

    if (gameMode == ForceMoves)
    {
        Attention(firstProgramPID);

        while (currentMove > target)
        {
            SendToProgram("undo\n", toFirstProgFP);
            currentMove--;
        }
    }
    else
    {
        currentMove = target;
    }

    if (gameMode == ForceMoves)
    {
        whiteTimeRemaining = timeRemaining[0][currentMove];
        blackTimeRemaining = timeRemaining[1][currentMove];
    }

    DisplayClocks(ReDisplayTimers);
    DisplayMove(currentMove - 1);
    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}




void
FlipViewProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    struct DisplayData *player = (*nprms ? &remotePlayer : &localPlayer);

    player->flipView = !player->flipView;
    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}




void
SaveGameProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    char def[MSG_SIZ];

    int fromRemotePlayer = *nprms;

    if (fromRemotePlayer)
    {
        DisplayMessage("only opponent may save game", fromRemotePlayer);
        return;
    }

    def[0] = NULLCHAR;

    FileNamePopUp("Filename for saved game?", SaveGame);
}




Boolean
SaveGame(char *name)
{
    char buf[MSG_SIZ];
    int i, len, move = 0;
    time_t tm;

    if (!filemodeUp) /* if called via FileModeCallback avoid recursion */
    {
        if ((gameFileFP = fopen(name, "r")) == NULL)
        {
            strcpy(fmi.mode, "w");
        }
        else
        {
            fclose(gameFileFP);
            FileModePopUp(name);
            return False; /* CHECKME: what should the return value be? */
        }
    }

    if ((gameFileFP = fopen(name, fmi.mode)) == NULL)
    {
        sprintf(buf, "Can't open %s (mode %s)", name, fmi.mode);
        DisplayMessage(buf, False);
        return False;
    }

    tm = time((time_t *) NULL);

    fprintf(gameFileFP, "# %s game file -- %s", programName, ctime(&tm));
    PrintOpponents(gameFileFP);

    for (i = 0; i < currentMove;)
    {
        if ((i % 5) == 0)
            fprintf(gameFileFP, "\n");

        fprintf(gameFileFP, "%d. %s ", ++move, parseList[i++]);

        if (i >= currentMove)
        {
            fprintf(gameFileFP, "\n");
            break;
        }

        if ((len = strlen(parseList[i])) == 0)
            break;

        fprintf(gameFileFP, "%s ", parseList[i++]);
    }

    fprintf(gameFileFP, "\n");

    fclose(gameFileFP);
    gameFileFP = NULL;

    return True;
}




void
SwitchProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    if (localPlayer.appData.noShogiProgram)
        return;

    switch (gameMode)
    {
    default:
        return;

    case MachinePlaysBlack:
        if (BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        lastGameMode = gameMode = MachinePlaysWhite;
        ModeHighlight();
        break;

    case BeginningOfGame:

    case MachinePlaysWhite:
        if (!BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        if (forwardMostMove == 0)
        {
            MachineBlackProc(w, event, prms, nprms);
            return;
        }

        lastGameMode = gameMode = MachinePlaysBlack;
        ModeHighlight();
        break;
    }

    Attention(firstProgramPID);
    SendToProgram("switch\n", toFirstProgFP);
}




void
ForceProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int i;

    switch (gameMode)
    {
    case MachinePlaysBlack:
        if (BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        Attention(firstProgramPID);
        SendToProgram("force\n", toFirstProgFP);
        break;

    case MachinePlaysWhite:
        if (!BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        Attention(firstProgramPID);
        SendToProgram("force\n", toFirstProgFP);
        break;

    case BeginningOfGame:
        SendToProgram("force\n", toFirstProgFP);
        break;

    case PlayFromGameFile:
        if (readGameXID != 0)
        {
            XtRemoveTimeOut(readGameXID);
            readGameXID = 0;
        }

        if (gameFileFP != NULL)
        {
            fclose(gameFileFP);
            gameFileFP = NULL;
        }

        break;

    case EndOfGame:
        ResurrectShogiProgram();
        break;

    case EditPosition:
        EditPositionDone();
        break;

    case TwoMachinesPlay:
        ShutdownShogiPrograms("");
        ResurrectShogiProgram();
        return;

    default:
        return;
    }

    if ((gameMode == MachinePlaysWhite)
        || (gameMode == MachinePlaysBlack)
        || (gameMode == TwoMachinesPlay)
        || (gameMode == PlayFromGameFile))
    {
        i = forwardMostMove;

        while (i > currentMove)
        {
            SendToProgram("undo\n", toFirstProgFP);
            i--;
        }

        blackTimeRemaining = timeRemaining[0][currentMove];
        whiteTimeRemaining = timeRemaining[1][currentMove];

        if (whiteFlag || blackFlag)
        {
            whiteFlag = blackFlag = 0;
        }

        DisplayTitle("");
    }

    lastGameMode = gameMode = ForceMoves;
    ModeHighlight();
    DisplayClocks(StopTimers);
}



void
HintProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("no hints in challenge mode", fromRemotePlayer);
        return;
    }

    if (localPlayer.appData.noShogiProgram)
        return;

    switch (gameMode)
    {
    case MachinePlaysBlack:
        if (BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        break;

    case BeginningOfGame:
    case MachinePlaysWhite:
        if (!BlackOnMove(forwardMostMove))
        {
            DisplayMessage("Wait until your turn", False);
            return;
        }

        break;

    default:
        DisplayMessage("No hint available", False);
        return;
    }

    Attention(firstProgramPID);
    SendToProgram("hint\n", toFirstProgFP);
}




void
PrintPosition(FILE *fp, int move)
{
    int i, j, color;

    for (i = BOARD_SIZE - 1; i >= 0; i--)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (pieceIsPromoted[(int)boards[currentMove][i][j]])
                fprintf(fp, "%c", '+');
            else
                fprintf(fp, "%c", ' ');

            fprintf(fp, "%c",
                    pieceToChar[(int)boards[currentMove][i][j]]);

            if (j == BOARD_SIZE - 1)
                fputc('\n', fp);
        }
    }

    for (color = 0; color <= 1; color++)
    {
        fprintf(fp, "%i %i %i %i %i %i %i %i\n",
                catches[currentMove][color][pawn],
                catches[currentMove][color][lance],
                catches[currentMove][color][knight],
                catches[currentMove][color][silver],
                catches[currentMove][color][gold],
                catches[currentMove][color][bishop],
                catches[currentMove][color][rook],
                catches[currentMove][color][king]);
    }

    if ((gameMode == EditPosition)
        ? !whitePlaysFirst
        : BlackOnMove(forwardMostMove))
    {
        fprintf(fp, "black to play\n");
    }
    else
    {
        fprintf(fp, "white to play\n");
    }
}




void
PrintOpponents(FILE *fp)
{
    char host_name[MSG_SIZ];

#ifdef HAVE_GETHOSTNAME
    gethostname(host_name, MSG_SIZ);
#else
    strncpy(host_name, "hostname not available", MSG_SIZ);
#endif

    switch (lastGameMode)
    {
    case MachinePlaysWhite:
        fprintf(fp, "# %s@%s vs. %s@%s\n",
                localPlayer.appData.firstShogiProgram,
                localPlayer.appData.firstHost,
                getpwuid(getuid())->pw_name,
                host_name);
        break;

    case MachinePlaysBlack:
        fprintf(fp, "# %s@%s vs. %s@%s\n",
                getpwuid(getuid())->pw_name,
                host_name,
                localPlayer.appData.firstShogiProgram,
                localPlayer.appData.firstHost);
        break;

    case TwoMachinesPlay:
        fprintf(fp, "# %s@%s vs. %s@%s\n",
                localPlayer.appData.secondShogiProgram,
                localPlayer.appData.secondHost,
                localPlayer.appData.firstShogiProgram,
                localPlayer.appData.firstHost);
        break;

    default:
        fprintf(fp, "#\n");
        break;
    }
}




void
SavePositionProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    char def[MSG_SIZ];

    int fromRemotePlayer = *nprms;

    if (fromRemotePlayer)
    {
        DisplayMessage("only opponent may save game", fromRemotePlayer);
        return;
    }

    def[0] = NULLCHAR;

    FileNamePopUp("Filename for saved position?", SavePosition);
}




Boolean
SavePosition(char *name)
{
    char buf[MSG_SIZ];
    FILE *fp;
    time_t tm;

    if ((fp = fopen(name, "w")) == NULL)
    {
        sprintf(buf, "Can't open %s", name);
        DisplayMessage(buf, False);
        return False;
    }

    tm = time((time_t *) NULL);

    fprintf(fp, "# %s position file -- %s", programName, ctime(&tm));
    PrintOpponents(fp);
    PrintPosition(fp, currentMove);
    fclose(fp);

    return True;
}




void
TwoMachinesProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    int i;
    MatchMode matchKind;

    int fromRemotePlayer = *nprms;

    if (updateRemotePlayer)
    {
        DisplayMessage("no machine moves in challenge mode",
                       fromRemotePlayer);
        return;
    }

    if (gameMode == PauseGame)
        PauseProc(w, event, prms, nprms);

    if (gameMode == PlayFromGameFile)
        ForceProc(w, event, prms, nprms);

    if ((gameMode == EndOfGame)
        || (gameMode == TwoMachinesPlay)
        || localPlayer.appData.noShogiProgram)
    {
        return;
    }

    if (matchMode == MatchFalse)
    {
        switch (gameMode)
        {
        case PauseGame:
        case PlayFromGameFile:
            return;

        case MachinePlaysBlack:
        case MachinePlaysWhite:
            ForceProc(w, event, prms, nprms);

            if (gameMode != ForceMoves)
                return;

            matchKind = MatchOpening;
            break;

        case ForceMoves:
            matchKind = MatchOpening;
            break;

        case EditPosition:
            EditPositionDone();
            matchKind = MatchPosition;
            break;

        case BeginningOfGame:
        default:
            matchKind = MatchInit;
            break;
        }
    }
    else
    {
        matchKind = matchMode;
    }

    forwardMostMove = currentMove;

    localPlayer.flipView = False;
    remotePlayer.flipView = True;
    firstMove = False;
    DisplayClocks(ResetTimers);
    DisplayClocks(StartTimers);

    switch (matchKind)
    {
    case MatchOpening:
        if (firstProgramXID == 0)
        {
            if (localPlayer.appData.loadGameFile == NULL)
            {
                DisplayMessage("Select game file first", False);
                return;
            }

            InitShogiProgram(localPlayer.appData.firstHost,
                             localPlayer.appData.firstShogiProgram,
                             &firstProgramPID, &toFirstProgFP,
                             &fromFirstProgFP, &firstProgramXID,
                             &firstSendTime);

            if (!LoadGame(localPlayer.appData.loadGameFile))
            {
                ShutdownShogiPrograms("Bad game file");
                return;
            }

            DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
        }

        InitShogiProgram(localPlayer.appData.secondHost,
                         localPlayer.appData.secondShogiProgram,
                         &secondProgramPID, &toSecondProgFP,
                         &fromSecondProgFP, &secondProgramXID,
                         &secondSendTime);

        if (startedFromSetupPosition)
        {
            if (whitePlaysFirst)
            {
                i = 1;
                SendToProgram("force\n", toSecondProgFP);
                SendBoard(toSecondProgFP, boards[i], catches[i]);
            }
            else
            {
                i = 0;
                SendBoard(toSecondProgFP, boards[i], catches[i]);
                SendToProgram("force\n", toSecondProgFP);
            }
        }
        else
        {
            i = 0;
            SendToProgram("force\n", toSecondProgFP);
        }

        for (i = backwardMostMove; i < forwardMostMove; i++)
            SendToProgram(moveList[i], toSecondProgFP);

        lastGameMode = gameMode = TwoMachinesPlay;
        ModeHighlight();
        firstMove = True;

        if (BlackOnMove(forwardMostMove))
            SendToProgram(localPlayer.appData.blackString, toSecondProgFP);
        else
            SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);

        break;

    case MatchPosition:
        if (firstProgramXID == 0)
        {
            if (localPlayer.appData.loadPositionFile == NULL)
            {
                DisplayMessage("Select position file first", False);
                return;
            }

            InitShogiProgram(localPlayer.appData.firstHost,
                             localPlayer.appData.firstShogiProgram,
                             &firstProgramPID, &toFirstProgFP,
                             &fromFirstProgFP, &firstProgramXID,
                             &firstSendTime);

            if (!LoadPosition(localPlayer.appData.loadPositionFile))
                return;
        }

        InitShogiProgram(localPlayer.appData.secondHost,
                         localPlayer.appData.secondShogiProgram,
                         &secondProgramPID, &toSecondProgFP,
                         &fromSecondProgFP, &secondProgramXID,
                         &secondSendTime);

        if (whitePlaysFirst)
            SendToProgram("force\n", toSecondProgFP);

        SendCurrentBoard(toSecondProgFP);
        lastGameMode = gameMode = TwoMachinesPlay;
        ModeHighlight();
        firstMove = True;

        if (BlackOnMove(forwardMostMove))
            SendToProgram(localPlayer.appData.blackString, toSecondProgFP);
        else
            SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);

        break;

    case MatchInit:
        InitPosition(True);

        if (firstProgramXID == 0)
        {
            InitShogiProgram(localPlayer.appData.firstHost,
                             localPlayer.appData.firstShogiProgram,
                             &firstProgramPID, &toFirstProgFP,
                             &fromFirstProgFP, &firstProgramXID,
                             &firstSendTime);
        }

        InitShogiProgram(localPlayer.appData.secondHost,
                         localPlayer.appData.secondShogiProgram,
                         &secondProgramPID, &toSecondProgFP,
                         &fromSecondProgFP, &secondProgramXID,
                         &secondSendTime);

        lastGameMode = gameMode = TwoMachinesPlay;
        ModeHighlight();
        SendToProgram(localPlayer.appData.blackString, toSecondProgFP);

    default:
        break;
    }

    if (!firstSendTime || !secondSendTime)
    {
        DisplayClocks(ResetTimers);
        timeRemaining[0][forwardMostMove] = blackTimeRemaining;
        timeRemaining[1][forwardMostMove] = whiteTimeRemaining;
    }

    DisplayClocks(StartTimers);
}




void
PauseProc(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    static GameMode previous_mode = PauseGame;

    switch (gameMode)
    {
    case ForceMoves:
    case EndOfGame:
    case EditPosition:
    default:
        return;

    case PauseGame:
        gameMode = previous_mode;
        ModeHighlight();
        previous_mode = PauseGame;
        DisplayClocks(StartTimers);
        DisplayMessage("", False);

        if (updateRemotePlayer)
            DisplayMessage("", True);
        break;

    case PlayFromGameFile:
        if (readGameXID == 0)
        {
            readGameXID =
                XtAppAddTimeOut(appContext,
                                (int)(1000 * localPlayer.appData.timeDelay),
                                (XtTimerCallbackProc) ReadGameFile, NULL);
        }
        else
        {
            XtRemoveTimeOut(readGameXID);
            readGameXID = 0;
        }

        DisplayMessage("Pausing", False);

        if (updateRemotePlayer)
            DisplayMessage("Pausing", True);

        break;

    case BeginningOfGame:
    case MachinePlaysBlack:
    case MachinePlaysWhite:
    case TwoMachinesPlay:
        if (forwardMostMove == 0)   /* Don't pause if no one has moved. */
            return;

        if (((gameMode == MachinePlaysWhite)
             && !BlackOnMove(forwardMostMove))
            || ((gameMode == MachinePlaysBlack) &&
                BlackOnMove(forwardMostMove)))
        {
            DisplayClocks(StopTimers);
        }

        previous_mode = gameMode;
        gameMode = PauseGame;
        ModeHighlight();
        DisplayClocks(StopTimers);
        DisplayMessage("Pausing", False);

        if (updateRemotePlayer)
            DisplayMessage("Pausing", True);

        break;
    }
}




void
Iconify(Widget w, XEvent *event, String *prms, Cardinal *nprms)
{
    Arg args[1];

    fromX = fromY = -1;

    XtSetArg(args[0], XtNiconic, True);
    XtSetValues(localPlayer.shellWidget, args, 1);
}




void
SendToProgram(char *message, FILE *fp)
{
    if (fp == NULL)
        return;

    lastMsgFP = fp;

    if (xshogiDebug)
    {
        fprintf(stderr, "Sending to %s: %s\n",
                ((fp == toFirstProgFP) ? "first" : "second"), message);
    }

    if (message[strlen(message) - 1] != '\n')
        fprintf(fp, "\n%s\n", message);
    else
        fputs(message, fp);

    fflush(fp);
}




void
ReceiveFromProgram(FILE *fp, int *source, XtInputId *id)
{
    char message[MSG_SIZ], *end_str, *number, *name;

    if (fgets(message, MSG_SIZ, fp) == NULL)
    {
        if (fp == fromFirstProgFP)
        {
            number = "first";
            name = localPlayer.appData.firstShogiProgram;
        }
        else if (fp == fromSecondProgFP)
        {
            number = "second";
            name = localPlayer.appData.secondShogiProgram;
        }
        else
        {
            return;
        }

        if (ferror(fp) == 0)
        {
            sprintf(message, "%s shogi program (%s) exited unexpectedly",
                    number, name);
            fprintf(stderr, "%s: %s\n", programName, message);
        }
        else
        {
            sprintf(message,
                    "error reading from %s shogi program (%s): %s",
                    number, name, strerror(ferror(fp)));
            fprintf(stderr, "%s: %s\n", programName, message);
        }

        return;
    }

    if ((end_str = (char *)strchr(message, '\r')) != NULL)
        *end_str = NULLCHAR;

    if ((end_str = (char *)strchr(message, '\n')) != NULL)
        *end_str = NULLCHAR;

    if (xshogiDebug || localPlayer.appData.debugMode)
    {
        fprintf(stderr, "Received from %s: %s\n",
                ((fp == fromFirstProgFP) ? "first" : "second"), message);
    }

    HandleMachineMove(message, fp);
}




void
SendSearchDepth(FILE *fp)
{
    char message[MSG_SIZ];

    if (localPlayer.appData.searchDepth <= 0)
        return;

    sprintf(message, "depth\n%d\nhelp\n", localPlayer.appData.searchDepth);
    /* Note kludge: "help" command forces gnushogi to print
     * out something that ends with a newline. */
    SendToProgram(message, fp);
}




void
DisplayMessage(char *message, int toRemotePlayer)
{
    Arg arg;

    XtSetArg(arg, XtNlabel, message);

    if (!toRemotePlayer)
        XtSetValues(localPlayer.messageWidget, &arg, 1);

    if (updateRemotePlayer && toRemotePlayer)
        XtSetValues(remotePlayer.messageWidget, &arg, 1);
}




void
DisplayName(char *name)
{
    Arg arg;

    XtSetArg(arg, XtNlabel, name);
    XtSetValues(localPlayer.titleWidget, &arg, 1);

    if (updateRemotePlayer)
        XtSetValues(remotePlayer.titleWidget, &arg, 1);
}




void SendTimeRemaining(FILE *fp)
{
    char message[MSG_SIZ];
    long comtime, opptime;

    if (BlackOnMove(forwardMostMove) == (fp == toFirstProgFP))
    {
        comtime = blackTimeRemaining;
        opptime = whiteTimeRemaining;
    }
    else
    {
        comtime = whiteTimeRemaining;
        opptime = blackTimeRemaining;
    }

    if (comtime <= 0)
        comtime = 1000;

    if (opptime <= 0)
        opptime = 1000;

    sprintf(message, "time %ld\n",  comtime / 10);
    SendToProgram(message, fp);
    sprintf(message, "otime %ld\n", opptime / 10);
    SendToProgram(message, fp);
}




void DisplayMove(int moveNumber)
{
    char message[MSG_SIZ];

    if (moveNumber < 0)
    {
        if (moveNumber == forwardMostMove - 1)
            DisplayMessage(endMessage, False);
        else
            DisplayMessage("", False);
    }
    else
    {
        sprintf(message, "%d. %s%s  %s",
                (moveNumber / 2 + 1),
                (BlackOnMove(moveNumber) ? "" : "... "),
                parseList[moveNumber],
                (moveNumber == (forwardMostMove - 1)) ? endMessage : "");
        DisplayMessage(message, False);
    }
}




void DisplayTitle(char *title)
{
    Arg arg;

    XtSetArg(arg, XtNlabel, title);
    XtSetValues(localPlayer.titleWidget, &arg, 1);
}




/* CHECKME: does this work?
 * This routine sends a SIGINT (^C interrupt) to gnushogi to awaken it
 * if it might be busy thinking on our time.  This normally isn't needed,
 * but is useful on systems where the FIONREAD ioctl doesn't work since
 * on those systems the gnushogi feature that lets you interrupt its thinking
 * just by typing a command does not work.
 *
 * In the future, similar code could be used to stop gnushogi and make
 * it move immediately when it is thinking about its own move; this could
 * be useful if we want to make Backward or ForceMoves work while gnushogi
 * is thinking.
 */

void
Attention(int pid)
{
#if !defined(FIONREAD)
    if (localPlayer.appData.noShogiProgram || (pid == 0))
        return;

    switch (gameMode)
    {
    case MachinePlaysBlack:
    case MachinePlaysWhite:
    case TwoMachinesPlay:
        if ((forwardMostMove > backwardMostMove + 1) && maybeThinking)
        {
            if (xshogiDebug || localPlayer.appData.debugMode)
            {
                fprintf(stderr, "Sending SIGINT to %s\n",
                        ((pid == firstProgramPID) ? "first" : "second"));
            }

            (void)kill(pid, SIGINT); /* stop it thinking */
        }
        break;

    default:
        break;  /* CHECKME: is this OK? */
    }
#endif /* !defined(FIONREAD) */
}




void
CheckFlags(void)
{
    if (blackTimeRemaining <= 0)
    {
        if (!blackFlag)
        {
            blackFlag = True;

            if (whiteFlag)
                DisplayName("  Both flags have fallen");
            else
                DisplayName("  Black's flag has fallen");
        }
    }

    if (whiteTimeRemaining <= 0)
    {
        if (!whiteFlag)
        {
            whiteFlag = True;

            if (blackFlag)
                DisplayName("  Both flags have fallen");
            else
                DisplayName("  White's flag has fallen");
        }
    }
}




void
CheckTimeControl(void)
{
    if (!localPlayer.appData.clockMode)
        return;

    if (forwardMostMove == 0)
        return;

    /*
     * Add time to clocks when time control is achieved.
     */

    if ((forwardMostMove % (localPlayer.appData.movesPerSession * 2)) == 0)
    {
        blackTimeRemaining += timeControl;
        whiteTimeRemaining += timeControl;
    }
}




void
DisplayLabels(void)
{
    DisplayTimerLabel(localPlayer.blackTimerWidget, "Black",
                      blackTimeRemaining);
    DisplayTimerLabel(localPlayer.whiteTimerWidget, "White",
                      whiteTimeRemaining);

    if (updateRemotePlayer)
    {
        DisplayTimerLabel(remotePlayer.blackTimerWidget, "Black",
                          blackTimeRemaining);
        DisplayTimerLabel(remotePlayer.whiteTimerWidget, "White",
                          whiteTimeRemaining);
    }
}




#ifdef HAVE_GETTIMEOFDAY
static struct timeval tickStartTV;
static int tickLength;

int
PartialTickLength(void)
{
    struct timeval tv;
    int ptl;

    gettimeofday(&tv, NULL);
    ptl = ((tv.tv_sec - tickStartTV.tv_sec) * 1000000 +
           (tv.tv_usec - tickStartTV.tv_usec) + 500) / 1000;

    if (ptl > tickLength)
        ptl = tickLength;

    return ptl;
}
#else /* !HAVE_GETTIMEOFDAY */
#define tickLength 1000
#endif /* HAVE_GETTIMEOFDAY */




/*
 * DisplayClocks manages the game clocks.
 *
 * In tournament play, white starts the clock and then black makes a move.
 * We give the human user a slight advantage if he is playing black---the
 * clocks don't run until he makes his first move, so it takes zero time.
 * Also, DisplayClocks doesn't account for network lag so it could get out
 * of sync with GNU Shogi's clock -- but then, referees are always right.
 */

void
DisplayClocks(int clock_mode)
{
    long timeRemaining;

    switch (clock_mode)
    {
    case ResetTimers:
        /* Stop clocks and reset to a fresh time control */
        if (timerXID != 0)
        {
            XtRemoveTimeOut(timerXID);
            timerXID = 0;
        }

        blackTimeRemaining = timeControl;
        whiteTimeRemaining = timeControl;

        if (blackFlag || whiteFlag)
        {
            DisplayName("");
            blackFlag = whiteFlag = False;
        }

        DisplayLabels();
        break;

    case DecrementTimers:
        /* Decrement running clock to next 1-second boundary */
        if (gameMode == PauseGame)
            return;

        timerXID = 0;

        if (!localPlayer.appData.clockMode)
            return;

        if (BlackOnMove(forwardMostMove))
        {
            timeRemaining = (blackTimeRemaining -= tickLength);
        }
        else
        {
            timeRemaining = (whiteTimeRemaining -= tickLength);
        }

        DisplayLabels();
        CheckFlags();

#ifdef HAVE_GETTIMEOFDAY
        tickLength = (((timeRemaining <= 1000) && (timeRemaining > 0))
                      ? 100 : 1000);
        gettimeofday(&tickStartTV, NULL);
#endif /* HAVE_GETTIMEOFDAY */

        timerXID =
            XtAppAddTimeOut(appContext, tickLength,
                            (XtTimerCallbackProc) DisplayClocks,
                            (XtPointer) DecrementTimers);
        break;

    case SwitchTimers:
        /* A player has just moved, so stop the previously running
           clock and start the other one. */

        if (timerXID != 0)
        {
            XtRemoveTimeOut(timerXID);
            timerXID = 0;

#ifdef HAVE_GETTIMEOFDAY
            if (localPlayer.appData.clockMode)
            {
                if (BlackOnMove(forwardMostMove))
                    whiteTimeRemaining -= PartialTickLength();
                else
                    blackTimeRemaining -= PartialTickLength();
                CheckFlags();
            }
#endif /* HAVE_GETTIMEOFDAY */
        }

        CheckTimeControl();
        DisplayLabels();

        if (!localPlayer.appData.clockMode)
            return;

        if ((gameMode == PauseGame)
            && ((pausePreviousMode == MachinePlaysBlack)
                || (pausePreviousMode == MachinePlaysWhite)))
        {
            return;
        }

        timeRemaining = (BlackOnMove(forwardMostMove)
                         ? blackTimeRemaining : whiteTimeRemaining);

#ifdef HAVE_GETTIMEOFDAY
        tickLength = (((timeRemaining <= 1000) && (timeRemaining > 0))
                      ? (((timeRemaining - 1) % 100) + 1)
                      : (((timeRemaining - 1) % 1000) + 1));

        if (tickLength <= 0)
            tickLength += 1000;

        gettimeofday(&tickStartTV, NULL);

#endif /* HAVE_GETTIMEOFDAY */
        timerXID =
            XtAppAddTimeOut(appContext, tickLength,
                            (XtTimerCallbackProc) DisplayClocks,
                            (XtPointer) DecrementTimers);
        break;

    case ReDisplayTimers:
        /* Display current clock values */
        DisplayLabels();
        break;

    case StopTimers:
        /* Stop both clocks */
        if (timerXID == 0)
            return;

        XtRemoveTimeOut(timerXID);
        timerXID = 0;

        if (!localPlayer.appData.clockMode)
            return;

#ifdef HAVE_GETTIMEOFDAY
        if (BlackOnMove(forwardMostMove))
            blackTimeRemaining -= PartialTickLength();
        else
            whiteTimeRemaining -= PartialTickLength();
        CheckFlags();
        DisplayLabels();
#endif /* HAVE_GETTIMEOFDAY */
        break;

    case StartTimers:
        /* Start clock of player on move, if not already running. */
        if (timerXID != 0)
            return;

        DisplayLabels();

        if (!localPlayer.appData.clockMode)
            return;

        timeRemaining = (BlackOnMove(forwardMostMove)
                         ? blackTimeRemaining : whiteTimeRemaining);

        if (timeRemaining == 0)
            return;

#ifdef HAVE_GETTIMEOFDAY
        tickLength = (((timeRemaining <= 1000) && (timeRemaining > 0))
                      ? (((timeRemaining - 1) % 100) + 1)
                      : (((timeRemaining - 1) % 1000) + 1));

        if (tickLength <= 0)
            tickLength += 1000;

        gettimeofday(&tickStartTV, NULL);
#endif /* HAVE_GETTIMEOFDAY */

        timerXID =
            XtAppAddTimeOut(appContext, tickLength,
                            (XtTimerCallbackProc) DisplayClocks,
                            (XtPointer)DecrementTimers);
        break;
    }
}




void
DisplayTimerLabel(Widget w, char *color, long int timer)
{
    char buf[MSG_SIZ];
    Arg args[3];
    struct DisplayData *player;

    player = (((w == localPlayer.blackTimerWidget)
               || (w == localPlayer.whiteTimerWidget))
              ? &localPlayer : &remotePlayer);

    if (localPlayer.appData.clockMode)
    {
        sprintf(buf, "%s: %s", color, TimeString(timer));
        XtSetArg(args[0], XtNlabel, buf);
    }
    else
    {
        XtSetArg(args[0], XtNlabel, color);
    }

    if (((color[0] == 'W') && BlackOnMove(forwardMostMove))
        || ((color[0] == 'B') && !BlackOnMove(forwardMostMove)))
    {
        XtSetArg(args[1], XtNbackground, player->timerForegroundPixel);
        XtSetArg(args[2], XtNforeground, player->timerBackgroundPixel);
    }
    else
    {
        XtSetArg(args[1], XtNbackground, player->timerBackgroundPixel);
        XtSetArg(args[2], XtNforeground, player->timerForegroundPixel);
    }

    XtSetValues(w, args, 3);
}




char *
TimeString(long tm)
{
    int second, minute, hour, day;
    char *sign = "";
    static char buf[32];

    if ((tm > 0) && (tm <= 900))
    {
        /* convert milliseconds to tenths, rounding up */
        sprintf(buf, " 0.%1ld ", (tm + 99) / 100);
        return buf;
    }

    /* convert milliseconds to seconds, rounding up */
    tm = (tm + 999) / 1000;

    if (tm < 0)
    {
        sign = "-";
        tm = -tm;
    }

    if (tm >= (60 * 60 * 24))
    {
        day = (int)(tm / (60 * 60 * 24));
        tm -= day * 60 * 60 * 24;
    }
    else
    {
        day = 0;
    }

    if (tm >= (60 * 60))
    {
        hour = (int)(tm / (60 * 60));
        tm -= hour * 60 * 60;
    }
    else
    {
        hour = 0;
    }

    if (tm >= 60)
    {
        minute = (int)(tm / 60);
        tm -= minute * 60;
    }
    else
    {
        minute = 0;
    }

    second = tm % 60;

    if (day > 0)
    {
        sprintf(buf, " %s%d:%02d:%02d:%02d ",
                sign, day, hour, minute, second);
    }
    else if (hour > 0)
    {
        sprintf(buf, " %s%d:%02d:%02d ",
                sign, hour, minute, second);
    }
    else
    {
        sprintf(buf, " %s%2d:%02d ",
                sign, minute, second);
    }

    return buf;
}




void
Usage(void)
{
    fprintf(stderr, "Usage: %s\n", programName);
    fprintf(stderr, "\tstandard Xt options\n");
    fprintf(stderr, "\t-iconic\n");
    fprintf(stderr, "\t-tc or -timeControl minutes[:seconds]\n");
    fprintf(stderr, "\t-gi or -gameIn (True | False)\n");
    fprintf(stderr, "\t-mps or -movesPerSession moves\n");
    fprintf(stderr, "\t-st or -searchTime minutes[:seconds]\n");
    fprintf(stderr, "\t-sd or -searchDepth number\n");
    fprintf(stderr, "\t-clock or -clockMode (True | False)\n");
    fprintf(stderr, "\t-td or -timeDelay seconds\n");

    fprintf(stderr, "\t-nsp or -noShogiProgram (True | False)\n");
    fprintf(stderr, "\t-fsp or -firstShogiProgram program_name\n");
    fprintf(stderr, "\t-ssp or -secondShogiProgram program_name\n");
    fprintf(stderr, "\t-fh or -firstHost host_name\n");
    fprintf(stderr, "\t-sh or -secondHost host_name\n");
    fprintf(stderr, "\t-rsh or -remoteShell shell_name\n");
    fprintf(stderr,
            "\t-mm or -matchMode (False | Init | Position | Opening)\n");
    fprintf(stderr, "\t-lgf or -loadGameFile file_name\n");
    fprintf(stderr, "\t-lpf or -loadPositionFile file_name\n");
    fprintf(stderr, "\t-sgf or -saveGameFile file_name\n");
    fprintf(stderr, "\t-spf or -savePositionFile file_name\n");
    fprintf(stderr, "\t-size or -boardSize (Large | Medium | Small)\n");
    fprintf(stderr, "\t-coords or -showCoords (True | False)\n");
    fprintf(stderr, "\t-mono or -monoMode (True | False)\n");
    fprintf(stderr, "\t-pc or -pieceColor color\n");
    fprintf(stderr, "\t-sc  or -squareColor color\n");
    fprintf(stderr, "\t-wps or -westernPieceSet (True | False)\n");
    fprintf(stderr, "\t-debug or -debugMode (True | False)\n");
    exit(2);
}



void
CatchPipeSignal(int dummy)
{
    char message[MSG_SIZ];

    sprintf(message,
            "%s shogi program (%s) exited unexpectedly",
            ((lastMsgFP == toFirstProgFP) ? "first" : "second"),
            ((lastMsgFP == toFirstProgFP)
             ? localPlayer.appData.firstShogiProgram
             : localPlayer.appData.secondShogiProgram));
    fprintf(stderr, "%s: %s\n", programName, message);
    ShutdownShogiPrograms(message);
    return;
}

