/*
 * FILE: cursesdsp.c
 *
 *     Curses interface for GNU Shogi
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
 */

/* request *snprintf prototypes */
#define _POSIX_C_SOURCE 200112L

#include "gnushogi.h"

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#include <curses.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_FILIO_H
/* Definition of FIONREAD */
#include <sys/filio.h>
#endif

#if HAVE_ERRNO_H
/* Definition of errno(). */
#include <errno.h>
#endif

#define FLUSH_SCANW fflush(stdout), scanw

#define MARGIN (5)
#define TAB (58)

#define VIR_C(s)  ((flag.reverse) ? (NO_COLS - 1 - column(s)) : column(s))
#define VIR_R(s)  ((flag.reverse) ? (NO_ROWS - 1 - row(s)) : row(s))

/****************************************
 * forward declarations
 ****************************************/

/* FIXME: change this name, puh-leeze! */
static void UpdateCatched(void);
static void DrawPiece(short sq);
static void ShowScore(short score);
static void Curses_UpdateDisplay(short f, short t, short redraw, short isspec);
static void Curses_Die(int sig);
static void Curses_ShowSidetoMove(void);

/****************************************
 * Trivial output functions.
 ****************************************/

static void
ClearEoln(void)
{
    clrtoeol();
    refresh();
}


static void
Curses_ClearScreen(void)
{
    clear();
    refresh();
}


static void
gotoXY(short x, short y)
{
    move(y - 1, x - 1);
}


static void
ClearMessage(void)
{
    gotoXY(TAB, 6);
    ClearEoln();
}

static void
Curses_ShowCurrentMove(short pnt, short f, short t)
{
    algbr(f, t, 0);
    gotoXY(TAB, 7);
    printw("(%2d) %5s ", pnt, mvstr[0]);
}


static void
Curses_ShowDepth(char ch)
{
    gotoXY(TAB, 4);
    printw("Depth= %d%c ", Sdepth, ch);
    ClearEoln();
}


static void
Curses_ShowGameType(void)
{
    if (flag.post)
    {
        gotoXY(TAB, 20);
        printw("%c vs. %c", GameType[black], GameType[white]);
    }
}


static void
ShowHeader(void)
{
    gotoXY(TAB, 2);
    printw("GNU Shogi %s", PACKAGE_VERSION);
}


static void
Curses_ShowLine(unsigned short *bstline)
{
}


static void
_vprintw(const char *format, va_list ap)
{
    static char buffer[60];
    vsnprintf(buffer, sizeof(buffer), format, ap);
    printw("%s", buffer);
}

static void
Curses_ShowMessage(char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    gotoXY(TAB, 6);
    _vprintw(format, ap);
    va_end(ap);
    ClearEoln();
}

static void
Curses_AlwaysShowMessage(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    gotoXY(TAB, 6);
    _vprintw(format, ap);
    va_end(ap);
    ClearEoln();
}


static void
Curses_Printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _vprintw(format, ap);
    va_end(ap);
}


static void
Curses_doRequestInputString(const char* fmt, char* buffer)
{
    FLUSH_SCANW(fmt, buffer);
}


static int
Curses_GetString(char* sx)
{
    fflush(stdout);
    return (getstr(sx) == ERR);
}


static void
Curses_ShowNodeCnt(long NodeCnt)
{
    gotoXY(TAB, 22);
    /* printw("Nodes = %8ld, Nodes/Sec = %5ld", NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0); */
    printw("n = %ld n/s = %ld", 
           NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0);
    ClearEoln();
}


static void
Curses_ShowPatternCount(short side, short n)
{
    if (flag.post)
    {
        gotoXY(TAB + 10 + 3 * side, 20);	  /* CHECKME */

        if (n >= 0)
            printw("%3d", n);
        else
            printw("   ");
    }
}


static void
ShowPlayers(void)
{
    gotoXY(5, ((flag.reverse) ? (5 + 2*NO_ROWS) : 2));
    printw("%s", (computer == white) ? "Computer" : "Human   ");
    gotoXY(5, ((flag.reverse) ? 2 : (5 + 2*NO_ROWS)));
    printw("%s", (computer == black) ? "Computer" : "Human   ");
}


static void
Curses_ShowPrompt(void)
{
    Curses_ShowSidetoMove();
    gotoXY(TAB, 17);
    printw("Your move is? ");
    ClearEoln();
}


static void
Curses_ShowResponseTime(void)
{
    if (flag.post)
    {
        short TCC = TCcount;
        gotoXY(TAB, 21);
        printw("%ld, %d, %ld, %ld, %ld, %d",
               ResponseTime, TCC, TCleft, ExtraTime, et, flag.timeout);
        ClearEoln();
    }
}


static void
Curses_ShowResults(short score, unsigned short *bstline, char ch)
{
    unsigned char d, ply;

    if (flag.post)
    {
        Curses_ShowDepth(ch);
        ShowScore(score);
        d = 7;

        for (ply = 1; bstline[ply] > 0; ply++)
        {
            if (ply % 2 == 1)
            {
                gotoXY(TAB, ++d);
                ClearEoln();
            }

            algbr((short) bstline[ply] >> 8, 
                  (short) bstline[ply] & 0xFF, 0);
            printw("%5s ", mvstr[0]);
        }

        ClearEoln();

        while (d < 13)
        {
            gotoXY(TAB, ++d);
            ClearEoln();
        }
    }
}


static void
ShowScore(short score)
{
    gotoXY(TAB, 5);
    printw("Score= %d", score);
    ClearEoln();
}


static void
Curses_ShowSidetoMove(void)
{
    gotoXY(TAB, 14);
    printw("%2d:   %s", 1 + GameCnt / 2, ColorStr[player]);
    ClearEoln();
}


static void
Curses_ShowStage(void)
{
    gotoXY(TAB, 19);
    printw("Stage= %2d%c B= %2d W= %2d",
           stage, flag.tsume?'T':' ', balance[black], balance[white]);
    ClearEoln();
}

/****************************************
 * End of trivial output routines.
 ****************************************/

static void
Curses_Initialize(void)
{
    signal(SIGINT, Curses_Die);
    signal(SIGQUIT, Curses_Die);
    initscr();
    crmode();
}


static void
Curses_ExitShogi(void)
{ 
    if (!nolist)
        ListGame();

    gotoXY(1, 24);

    refresh();
    nocrmode();
    endwin();

    exit(0);
}


static void
Curses_Die(int sig)
{
    char s[80];

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    Curses_ShowMessage("Abort? ");
    FLUSH_SCANW("%s", s);

    if (strcmp(s, "yes") == 0)
        Curses_ExitShogi();

    signal(SIGINT, Curses_Die);
    signal(SIGQUIT, Curses_Die);
}


static void
Curses_TerminateSearch(int sig)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    if (!flag.timeout)
        flag.musttimeout = true;

    Curses_ShowMessage("Terminate Search");
    flag.bothsides = false;
    signal(SIGINT, Curses_Die);
    signal(SIGQUIT, Curses_Die);
}


static void
Curses_help(void)
{
    Curses_ClearScreen();
    printw("GNU Shogi %s command summary\n", PACKAGE_VERSION);
    printw("-------------------------------"
           "---------------------------------\n");
    printw("7g7f      move from 7g to 7f      quit      Exit Shogi\n");
    printw("S6h       move silver to 6h       beep      turn %s\n", (flag.beep) ? "OFF" : "ON");
    printw("2d2c+     move to 2c and promote  material  turn %s\n", (flag.material) ? "OFF" : "ON");
    printw("P*5e      drop pawn to 5e         easy      turn %s\n", (flag.easy) ? "OFF" : "ON");
    printw("tsume     toggle tsume mode       hash      turn %s\n", (flag.hash) ? "OFF" : "ON");
    printw("bd        redraw board            reverse   board display\n");
    printw("list      game to shogi.lst       book      turn %s used %d of %d\n", (Book) ? "OFF" : "ON", bookcount, BOOKSIZE);
    printw("undo      undo last ply           remove    take back a move\n");
    printw("edit      edit board              force     toggle manual move mode\n");
    printw("switch    sides with computer     both      computer match\n");
    printw("black     computer plays black    white     computer plays white\n");
    printw("depth     set search depth        clock     set time control\n");
    printw("post      principle variation     hint      suggest a move\n", (flag.post) ? "OFF" : "ON");
    printw("save      game to file            get       game from file\n");
    printw("random    randomize play          new       start new game\n");
    gotoXY(10, 20);
    printw("Computer: %s", ColorStr[computer]);
    gotoXY(10, 21);
    printw("Opponent: %s", ColorStr[opponent]);
    gotoXY(10, 22);
    printw("Level: %ld", MaxResponseTime/100);
    gotoXY(10, 23);
    printw("Easy mode: %s", (flag.easy) ? "ON" : "OFF");
    gotoXY(25, 23);
    printw("Tsume: %s", (flag.tsume) ? "ON" : "OFF");
    gotoXY(40, 20);
    printw("Depth: %d", MaxSearchDepth);
    gotoXY(40, 21);
    printw("Random: %s", (dither) ? "ON" : "OFF");
    gotoXY(40, 22);
    printw("Transposition table: %s", (flag.hash) ? "ON" : "OFF");
    gotoXY(40, 23);
    printw("Hit <RET> to return: ");
    gotoXY(10, 24);
    printw("Time Control %s %d moves %d sec %d add %d depth\n", (TCflag) ? "ON" : "OFF",
           TimeControl.moves[black], 
           TimeControl.clock[black] / 100, 
           OperatorTime, MaxSearchDepth);

    refresh();

#ifdef BOGUS
    fflush(stdin); /* what is this supposed to do?? */
#endif /* BOGUS */

    getchar();
    Curses_ClearScreen();
    Curses_UpdateDisplay(0, 0, 1, 0);
}


static const short x0[2] = { MARGIN + 5*NO_COLS + 4, 2 };
static const short y0[2] = { 4 + 2 * (NO_ROWS - 1), 4 };


/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, N3f will place a knight on square 3f.
 * P* will put a pawn to the captured pieces.
 */

static void
Curses_EditBoard(void)
{
    short a, c, sq, i, found;
    short r = 0;
    char s[80];

    flag.regularstart = true;
    Book = BOOKFAIL;
    Curses_ClearScreen();
    Curses_UpdateDisplay(0, 0, 1, 0);
    gotoXY(TAB, 11);
    printw(".   Exit to main\n");
    gotoXY(TAB, 12);
    printw("#   Clear board\n");
    gotoXY(TAB, 13);
    printw("c   Change sides\n");
    gotoXY(TAB, 7);
    printw("Enter piece & location: ");
    a = black;

    while(1)
    {
        gotoXY(TAB, 4);
        printw("Editing: %s", ColorStr[a]);
        gotoXY(TAB + 2, 8);
        ClearEoln();
        FLUSH_SCANW("%s", s);
        found = 0;
        ClearMessage();

        if (s[0] == '.')
            break;

        if (s[0] == '#')
        {
            for (sq = 0; sq < NO_SQUARES; sq++)
            {
                board[sq] = no_piece;
                color[sq] = neutral;
                DrawPiece(sq);
            }

            ClearCaptured();
            UpdateCatched();
            continue;
        }

        if (s[0] == 'c') {
            a = otherside[a];
            continue;
        }

        if (s[1] == '*')
        {
            for (i = NO_PIECES; i > no_piece; i--)
            {
                if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
                {
                    Captured[a][unpromoted[i]]++;
                    UpdateCatched();
                    found = 1;
                    break;
                }
            }
            if (!found)
                dsp->AlwaysShowMessage("Invalid piece type '%c'", s[0]);
            continue;
        }

        c = COL_NUM(s[1]);
        r = ROW_NUM(s[2]);

        if ((c < 0) || (c >= NO_COLS) || (r < 0) || (r >= NO_ROWS)) {
            dsp->AlwaysShowMessage("Out-of-board '%c%c'", s[1], s[2]);
            continue;
        }

        sq = locn(r, c);

        for (i = NO_PIECES; i > no_piece; i--)
        {
            if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
            {
                color[sq] = a;
                if (s[3] == '+')
                    board[sq] = promoted[i];
                else
                    board[sq] = unpromoted[i];

                found = 1;
                break;
            }
        }

        if (!found)
            dsp->AlwaysShowMessage("Invalid piece type '%c'", s[0]);

        DrawPiece(sq);
    }

    for (sq = 0; sq < NO_SQUARES; sq++)
        Mvboard[sq] = ((board[sq] != Stboard[sq]) ? 10 : 0);

    GameCnt = 0;
    Game50 = 1;
    ZeroRPT();
    Sdepth = 0;
    InitializeStats();
    Curses_ClearScreen();
    Curses_UpdateDisplay(0, 0, 1, 0);
}


static void 
UpdateCatched()
{
    short side;

    for (side = black; side <= white; side++)
    { 
        short x, y, piece, cside, k;

        cside = flag.reverse ? (side ^ 1) : side;
        x = x0[cside];
        y = y0[cside];
        k = 0;

        for (piece = pawn; piece <= king; piece++)
        {
            short n;

            if ((n = Captured[side][piece]))
            {
                gotoXY(x, y); 
                printw("%i%c", n, pxx[piece]);

                if (cside == black) 
                    y--; 
                else 
                    y++;
            }
            else
            {
                k++;
            }
        }

        while (k)
        {
            k--;
            gotoXY(x, y);
            printw("  ");

            if (cside == black) 
                y--; 
            else 
                y++;
        }
    }

    refresh();
}


static void
Curses_SearchStartStuff(short side)
{
    short i;

    signal(SIGINT, Curses_TerminateSearch);
    signal(SIGQUIT, Curses_TerminateSearch);

    for (i = 4; i < 14; i++)			  /* CHECKME */
    {
        gotoXY(TAB, i);
        ClearEoln();
    }
}


static void
Curses_OutputMove(void)
{

    Curses_UpdateDisplay(root->f, root->t, 0, (short) root->flags);
    gotoXY(TAB, 16);

    if (flag.illegal) 
    {
        printw("Illegal position.");
        return;
    }

    printw("My move is: %5s", mvstr[0]);

    if (flag.beep)
        putchar(7);

    ClearEoln();

    gotoXY(TAB, 18);

    if (root->flags & draw)
        printw("Drawn game!");
    else if (root->score == -(SCORE_LIMIT + 999))
        printw("Opponent mates!");
    else if (root->score == SCORE_LIMIT + 998)
        printw("Computer mates!");
#ifdef VERYBUGGY
    else if (root->score < -SCORE_LIMIT)
        printw("Opp: mate in %d!", SCORE_LIMIT + 999 + root->score - 1);
    else if (root->score > SCORE_LIMIT)
        printw("Comp: mate in %d!", SCORE_LIMIT + 998 - root->score - 1);
#endif /* VERYBUGGY */

    ClearEoln();

    if (flag.post)
    {
        short h, l, t;

        h = TREE;
        l = 0;
        t = TREE >> 1;

        while (l != t)
        {
            if (Tree[t].f || Tree[t].t)
                l = t;
            else
                h = t;

            t = (l + h) >> 1;
        }

        Curses_ShowNodeCnt(NodeCnt);
        gotoXY(TAB, 23);
        printw("Max Tree = %5d", t);
        ClearEoln();
    }

    Curses_ShowSidetoMove();
}


static void
Curses_UpdateClocks(void)
{
    short m, s;
    long dt;

    if (TCflag)
    {
        m = (short) ((dt = (TimeControl.clock[player] - et)) / 6000);
        s = (short) ((dt - 6000 * (long) m) / 100);
    }
    else
    {
        m = (short) ((dt = et) / 6000);
        s = (short) (et - 6000 * (long) m) / 100;
    }

    if (m < 0)
        m = 0;

    if (s < 0)
        s = 0;

    if (player == black)
        gotoXY(20, (flag.reverse) ? 2 : (5 + 2*NO_ROWS));
    else
        gotoXY(20, (flag.reverse) ? (5 + 2*NO_ROWS) : 2);

    /* printw("%d:%02d %ld  ", m, s, dt); */
    printw("%d:%02d  ", m, s); 

    if (flag.post)
        Curses_ShowNodeCnt(NodeCnt);

    refresh();
}


static void
DrawPiece(short sq)
{
    char y;
    char piece, l, r, p; 

    if (color[sq] == neutral)
    {
        l = r = ' ';
    }
    else if (flag.reverse ^ (color[sq] == black))
    {
        l = '/';
        r = '\\';
    } 
    else
    {
        l = '\\', r = '/';
    }

    piece = board[sq];

    if (is_promoted[(int)piece])
    {
        p = '+';
        y = pxx[unpromoted[(int)piece]];
    } 
    else
    {
        p = ' ';
        y = pxx[(int)piece];
    }

    gotoXY(MARGIN + 3 + 5 * VIR_C(sq), 4 + 2 * ((NO_ROWS - 1) - VIR_R(sq)));
    printw("%c%c%c%c", l, p, y, r);
}


/*
 * Curses_ShowPostnValue(): must have called ExaminePosition() first
 */
static void
Curses_ShowPostnValue(short sq)
{
    gotoXY(4 + 5 * VIR_C(sq), 5 + 2 * (7 - VIR_R(sq)));	/* CHECKME */
    (void) ScorePosition(color[sq]);

    if (color[sq] != neutral)
#if defined SAVE_SVALUE
    {
        printw("??? ");
    }
#else
    {
        printw("%3d ", svalue[sq]);
    }
#endif
    else
    {
        printw("   ");
    }
}


static void
Curses_ShowPostnValues(void)
{
    short sq, score;

    ExaminePosition(opponent);

    for (sq = 0; sq < NO_SQUARES; sq++)
        Curses_ShowPostnValue(sq);

    score = ScorePosition(opponent);
    gotoXY(TAB, 5);
    printw("S%d m%d ps%d gt%c m%d ps%d gt%c", score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);

    ClearEoln();
}


static void
Curses_UpdateDisplay(short f, short t, short redraw, short isspec)
{
    short i, sq, z;
    int j;

    if (redraw)
    {
        ShowHeader();
        ShowPlayers();

        i = 2;
        gotoXY(MARGIN, ++i);

        printw("  +");
	for (j=0; j<NO_COLS; j++)
	    printw("----+");

        while (i <= 1 + 2*NO_ROWS)
        {
            gotoXY(MARGIN, ++i);

            if (flag.reverse)
                z = (i / 2) - 1;
            else
                z = NO_ROWS + 2 - ((i + 1) / 2);

            printw("%c |", ROW_NAME(z+1));
	    for (j=0; j<NO_COLS; j++)
		printw("    |");

            gotoXY(MARGIN, ++i);

            if (i < 2 + 2*NO_ROWS)
            {
		printw("  +");
		for (j=0; j<NO_COLS; j++)
		    printw("----+");
            }
        }

	printw("  +");
	for (j=0; j<NO_COLS; j++)
	    printw("----+");

        gotoXY(MARGIN, 4 + 2*NO_ROWS);
        printw("  ");
#ifndef MINISHOGI
        if (flag.reverse)
            printw("  1    2    3    4    5    6    7    8    9");
        else
            printw("  9    8    7    6    5    4    3    2    1");
#else
        if (flag.reverse)
            printw("  1    2    3    4    5");
        else
            printw("  5    4    3    2    1");
#endif

        for (sq = 0; sq < NO_SQUARES; sq++)
            DrawPiece(sq);
    }
    else /* not redraw */
    {
        if (f < NO_SQUARES)
            DrawPiece(f);

        DrawPiece(t & 0x7f);
    }

    if ((isspec & capture) || (isspec & dropmask) || redraw)
    {
        short side;

        for (side = black; side <= white; side++)
        {
            short x, y, piece, cside, k;
            cside = flag.reverse ? (side ^ 1) : side;
            x = x0[cside];
            y = y0[cside];
            k = 0;

            for (piece = pawn; piece <= king; piece++)
            {
                short n;

                if ((n = Captured[side][piece]))
                {
                    gotoXY(x, y); 
                    printw("%i%c", n, pxx[piece]);

                    if (cside == black) y--; else y++;
                }
                else
                {
                    k++;
                }
            }

            while (k)
            {
                k--;
                gotoXY(x, y);
                printw("  ");

                if (cside == black) 
                    y--;
                else 
                    y++;
            }
        }
    }

    refresh();
}


static void
Curses_ChangeAlphaWindow(void)
{
    Curses_ShowMessage("WAwindow = ");
    FLUSH_SCANW("%hd", &WAwindow);
    Curses_ShowMessage("BAwindow = ");
    FLUSH_SCANW("%hd", &BAwindow);
}


static void
Curses_ChangeBetaWindow(void)
{
    Curses_ShowMessage("WBwindow = ");
    FLUSH_SCANW("%hd", &WBwindow);
    Curses_ShowMessage("BBwindow = ");
    FLUSH_SCANW("%hd", &BBwindow);
}


static void
Curses_GiveHint(void)
{
    char s[40];

    if (hint)
    {
        algbr((short) (hint >> 8), (short) (hint & 0xFF), 0);
        strcpy(s, "try ");
        strcat(s, mvstr[0]);
        Curses_ShowMessage(s);
    }
    else
    {
        Curses_ShowMessage("I have no idea.\n");
    }
}


static void
Curses_ChangeSearchDepth(char* sx)
{
    Curses_ShowMessage("depth = ");
    FLUSH_SCANW("%hd", &MaxSearchDepth);
    TCflag = !(MaxSearchDepth > 0);
}


static void
Curses_ChangeHashDepth(void)
{
    Curses_ShowMessage("hashdepth = ");
    FLUSH_SCANW("%hd", &HashDepth);
    Curses_ShowMessage("MoveLimit = ");
    FLUSH_SCANW("%hd", &HashMoveLimit);
}


static void
Curses_SetContempt(void)
{
    Curses_ShowMessage("contempt = ");
    FLUSH_SCANW("%hd", &contempt);
}


static void
Curses_ChangeXwindow(void)
{
    Curses_ShowMessage("xwndw= ");
    FLUSH_SCANW("%hd", &xwndw);
}


static void
Curses_SelectLevel(char *sx)
{
    int item;

    Curses_ClearScreen();
    gotoXY(32, 2);
    printw("GNU Shogi %s", PACKAGE_VERSION);
    gotoXY(20, 4);
    printw(" 1.   40 moves in   5 minutes");
    gotoXY(20, 5);
    printw(" 2.   40 moves in  15 minutes");
    gotoXY(20, 6);
    printw(" 3.   40 moves in  30 minutes");
    gotoXY(20, 7);
    printw(" 4.  all moves in  15 minutes");
    gotoXY(20, 8);
    printw(" 5.  all moves in  30 minutes");
    gotoXY(20, 9);
    printw(" 6.  all moves in  15 minutes, 30 seconds fischer clock");
    gotoXY(20, 10);
    printw(" 7.  all moves in  30 minutes, 30 seconds fischer clock");
    gotoXY(20, 11);
    printw(" 8.    1 move  in   1 minute");
    gotoXY(20, 12);
    printw(" 9.    1 move  in  15 minutes");
    gotoXY(20, 13);
    printw("10.    1 move  in  30 minutes");

    OperatorTime = 0;
    TCmoves = 40;
    TCminutes = 5;
    TCseconds = 0;

    gotoXY(20, 17);
    printw("Enter Level: ");
    refresh();
    FLUSH_SCANW("%d", &item);

    switch(item)
    {
    case 1:
        TCmoves = 40;
        TCminutes = 5;
        break;

    case 2:
        TCmoves = 40;
        TCminutes = 15;
        break;

    case 3:
        TCmoves = 40;
        TCminutes = 30;
        break;

    case 4:
        TCmoves = 80;
        TCminutes = 15;
        flag.gamein = true;
        break;

    case 5:
        TCmoves = 80;
        TCminutes = 30;
        flag.gamein = true;
        break;

    case 6:
        TCmoves = 80;
        TCminutes = 15;
        TCadd = 3000;
        flag.gamein = true;
        break;

    case 7:
        TCmoves = 80;
        TCminutes = 30;
        TCadd = 3000;
        break;

    case 8:
        TCmoves = 1;
        TCminutes = 1;
        flag.onemove = true;
        break;

    case 9:
        TCmoves = 1;
        TCminutes = 15;
        flag.onemove = true;
        break;

    case 10:
        TCmoves = 1;
        TCminutes = 30;
        flag.onemove = true;
        break;
    }

    TCflag = (TCmoves > 0);

    TimeControl.clock[black] = TimeControl.clock[white] = 0; 

    SetTimeControl();
    Curses_ClearScreen();
    Curses_UpdateDisplay(0, 0, 1, 0);
}


static void
Curses_DoDebug(void)
{
    short c, p, sq, tp, tc, tsq, score;
    char s[40];

    ExaminePosition(opponent);
    Curses_ShowMessage("Enter piece: ");
    FLUSH_SCANW("%s", s);
    c = neutral;

    if ((s[0] == 'b') || (s[0] == 'B'))
        c = black;

    if ((s[0] == 'w') || (s[0] == 'W'))
        c = white;

    for (p = king; p > no_piece; p--)
    {
        if ((s[1] == pxx[p]) || (s[1] == qxx[p]))
            break;
    }

    for (sq = 0; sq < NO_SQUARES; sq++)
    {
        tp = board[sq];
        tc = color[sq];
        board[sq] = p;
        color[sq] = c;
        tsq = PieceList[c][1];
        PieceList[c][1] = sq;
        Curses_ShowPostnValue(sq);
        PieceList[c][1] = tsq;
        board[sq] = tp;
        color[sq] = tc;
    }

    score = ScorePosition(opponent);
    gotoXY(TAB, 5);
    printw("S%d m%d ps%d gt%c m%d ps%d gt%c", score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);

    ClearEoln();
}


static void
Curses_DoTable(short table[NO_SQUARES])
{
    short  sq;
    ExaminePosition(opponent);

    for (sq = 0; sq < NO_SQUARES; sq++)
    {
        gotoXY(4 + 5 * VIR_C(sq), 5 + 2 * (7 - VIR_R(sq)));
        printw("%3d ", table[sq]);
    }
} 


static void
Curses_PollForInput(void)
{
    int  i;
    int  nchar;

    if ((i = ioctl((int) 0, FIONREAD, &nchar)))
    {
        perror("FIONREAD");
        fprintf(stderr,
                "You probably have a non-ANSI <ioctl.h>; "
                "see README. %d %d %x\n",
                i, errno, FIONREAD);
        exit(1);
    }

    if (nchar)
    {
        if (!flag.timeout)
            flag.back = true;

        flag.bothsides = false;
    }
}


static void
Curses_SetupBoard(void)
{
    Curses_ShowMessage("'setup' command is not supported in Cursesmode");
}


struct display curses_display =
{
    .ChangeAlphaWindow    = Curses_ChangeAlphaWindow,
    .ChangeBetaWindow     = Curses_ChangeBetaWindow,
    .ChangeHashDepth      = Curses_ChangeHashDepth,
    .ChangeSearchDepth    = Curses_ChangeSearchDepth,
    .ChangeXwindow        = Curses_ChangeXwindow,
    .ClearScreen          = Curses_ClearScreen,
    .DoDebug              = Curses_DoDebug,
    .DoTable              = Curses_DoTable,
    .EditBoard            = Curses_EditBoard,
    .ExitShogi            = Curses_ExitShogi,
    .GiveHint             = Curses_GiveHint,
    .Initialize           = Curses_Initialize,
    .ShowNodeCnt          = Curses_ShowNodeCnt,
    .OutputMove           = Curses_OutputMove,
    .PollForInput         = Curses_PollForInput,
    .SetContempt          = Curses_SetContempt,
    .SearchStartStuff     = Curses_SearchStartStuff,
    .SelectLevel          = Curses_SelectLevel,
    .ShowCurrentMove      = Curses_ShowCurrentMove,
    .ShowDepth            = Curses_ShowDepth,
    .ShowGameType         = Curses_ShowGameType,
    .ShowLine             = Curses_ShowLine,
    .ShowMessage          = Curses_ShowMessage,
    .AlwaysShowMessage    = Curses_AlwaysShowMessage,
    .Printf               = Curses_Printf,
    .doRequestInputString = Curses_doRequestInputString,
    .GetString            = Curses_GetString,
    .SetupBoard           = Curses_SetupBoard,
    .ShowPatternCount     = Curses_ShowPatternCount,
    .ShowPostnValue       = Curses_ShowPostnValue,
    .ShowPostnValues      = Curses_ShowPostnValues,
    .ShowPrompt           = Curses_ShowPrompt,
    .ShowResponseTime     = Curses_ShowResponseTime,
    .ShowResults          = Curses_ShowResults,
    .ShowSidetoMove       = Curses_ShowSidetoMove,
    .ShowStage            = Curses_ShowStage,
    .TerminateSearch      = Curses_TerminateSearch,
    .UpdateClocks         = Curses_UpdateClocks,
    .UpdateDisplay        = Curses_UpdateDisplay,
    .help                 = Curses_help,
};
