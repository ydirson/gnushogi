/*
 * FILE: cursesdsp.c
 *
 *     Curses interface for GNU Shogi
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
 * GNU Shogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Shogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <ctype.h>
#include <signal.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#include <curses.h>

#include "gnushogi.h"
#include "cursesdsp.h"

#define FLUSH_SCANW fflush(stdout), scanw

int mycnt1, mycnt2;

#define TAB (58)

#define VIR_C(s)  ((flag.reverse) ? (8 - column(s)) : column(s))
#define VIR_R(s)  ((flag.reverse) ? (8 - row(s)) : row(s))

unsigned short MV[MAXDEPTH];
int MSCORE;
char *DRAW;

/* Forward declarations. */
/* FIXME: change this name, puh-leeze! */

static void UpdateCatched();


/****************************************
 * Trivial output functions.
 ****************************************/

void
ClearEoln(void)
{
    clrtoeol();
    refresh();
}


void
Curses_ClearScreen(void)
{
    clear();
    refresh();
}


void
ClearMessage(void)
{
    gotoXY(TAB, 6);
    ClearEoln();
}



void
gotoXY(short x, short y)
{
    move(y - 1, x - 1);
}


void
Curses_ShowCurrentMove(short pnt, short f, short t)
{
    algbr(f, t, false);
    gotoXY(TAB, 7);
    printw("(%2d) %5s ", pnt, mvstr[0]);
}


void
Curses_ShowDepth(char ch)
{
    gotoXY(TAB, 4);
    printw(CP[53], Sdepth, ch);   /* Depth = %d%c */
    ClearEoln();
}


void
Curses_ShowGameType(void)
{
    if (flag.post)
    {
        gotoXY(TAB, 20);
        printw("%c vs. %c", GameType[black], GameType[white]);
    }
}


void
ShowHeader(void)
{
    gotoXY(TAB, 2);
    printw(CP[69], version, patchlevel);
}


void
Curses_ShowLine(unsigned short *bstline)
{
}


void
Curses_ShowMessage(char *s)
{
    gotoXY(TAB, 6);
    printw("%s", s);
    ClearEoln();
}


void
ShowNodeCnt(long NodeCnt)
{
    gotoXY(TAB, 22);
    /* printw(CP[90], NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0); */
    printw("n = %ld n/s = %ld", 
           NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0);
    ClearEoln();
}


void
Curses_ShowPatternCount(short side, short n)
{
    if (flag.post)
    {
        gotoXY(TAB + 10 + 3 * side, 20);

        if (n >= 0)
            printw("%3d", n);
        else
            printw("   ");
    }
}


void
ShowPlayers(void)
{
    gotoXY(5, ((flag.reverse) ? 23 : 2));
    printw("%s", (computer == white) ? CP[218] : CP[74]);
    gotoXY(5, ((flag.reverse) ? 2 : 23));
    printw("%s", (computer == black) ? CP[218] : CP[74]);
}


void
ShowPrompt(void)
{
    gotoXY(TAB, 17);
    printw(CP[121]);     /* Your move is? */
    ClearEoln();
}


void
Curses_ShowResponseTime(void)
{
    if (flag.post)
    {
        short TCC = TCcount;
        gotoXY(TAB, 21);
        /* printw("RT = %ld TCC = %d TCL = %ld EX = %ld ET = %ld TO = %d",
           ResponseTime, TCC, TCleft, ExtraTime, et, flag.timeout); */
        printw("%ld, %d, %ld, %ld, %ld, %d",
               ResponseTime, TCC, TCleft, ExtraTime, et, flag.timeout);
        ClearEoln();
    }
}


void
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
                  (short) bstline[ply] & 0xFF, false);
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


void
ShowScore(short score)
{
    gotoXY(TAB, 5);
    printw(CP[104], score);
    ClearEoln();
}


void
Curses_ShowSidetoMove(void)
{
    gotoXY(TAB, 14);
    printw("%2d:   %s", 1 + GameCnt / 2, ColorStr[player]);
    ClearEoln();
}


void
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


void
Curses_Initialize(void)
{
    signal(SIGINT, Curses_Die);
    signal(SIGQUIT, Curses_Die);
    initscr();
    crmode();
}


void
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



void
Curses_Die(int sig)
{
    char s[80];

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    Curses_ShowMessage(CP[31]);     /* Abort? */
    FLUSH_SCANW("%s", s);

    if (strcmp(s, CP[210]) == 0) /* yes */
        Curses_ExitShogi();

    signal(SIGINT, Curses_Die);
    signal(SIGQUIT, Curses_Die);
}



void
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



void
Curses_help(void)
{
    Curses_ClearScreen();
    /* printw("GNU Shogi ??p? command summary\n"); */
    printw(CP[40], version, patchlevel);
    printw("-------------------------------"
           "---------------------------------\n");
    /* printw("7g7f      move from 7g to 7f      quit      
     * Exit Shogi\n"); */
    printw(CP[158]);
    /* printw("S6h       move silver to 6h       beep      
     * turn %s\n", (flag.beep) ? "off" : "on"); */
    printw(CP[86], (flag.beep) ? CP[92] : CP[93]);
    /* printw("2d2c+     move to 2c and promote\n"); */
    printw(CP[128], (flag.material) ? CP[92] : CP[93]);
    /* printw("P*5e      drop a pawn to 5e       easy      
     * turn %s\n", (flag.easy) ? "off" : "on"); */
    printw(CP[173], (flag.easy) ? CP[92] : CP[93]);
    /* printw("                                  hash      
     * turn %s\n", (flag.hash) ? "off" : "on"); */
    printw(CP[174], (flag.hash) ? CP[92] : CP[93]);
    /* printw("bd        redraw board            reverse   
     * board display\n"); */
    printw(CP[130]);
    /* printw("list      game to shogi.lst       book      
     * turn %s used %d of %d\n", (Book) ? "off" : "on", book
      count, booksize); */
    printw(CP[170], (Book) ? CP[92] : CP[93], bookcount, BOOKSIZE);
    /* printw("undo      undo last ply           remove    
     * take back a move\n"); */
    printw(CP[200]);
    /* printw("edit      edit board              force     
     * enter game moves\n"); */
    printw(CP[153]);
    /* printw("switch    sides with computer     both      
     * computer match\n"); */
    printw(CP[194]);
    /* printw("black     computer plays black    white     
     * computer plays white\n"); */
    printw(CP[202]);
    /* printw("depth     set search depth        clock     
     * set time control\n"); */
    printw(CP[149]);
    /* printw("hint      suggest a move         post      
     * turn %s principle variation\n", (flag.post) ? "off" :
     * "on"); */
    printw(CP[177], (flag.post) ? CP[92] : CP[93]);
    /* printw("save      game to file            get       
     * game from file\n"); */
    printw(CP[188]);
    /* printw("random    randomize play          new       
     * start new game\n"); */
    printw(CP[181]);
    gotoXY(10, 20);
    printw(CP[47], ColorStr[computer]);
    gotoXY(10, 21);
    printw(CP[97], ColorStr[opponent]);
    gotoXY(10, 22);
    printw(CP[79], MaxResponseTime/100);
    gotoXY(10, 23);
    printw(CP[59], (flag.easy) ? CP[93] : CP[92]);
    gotoXY(25, 23);
    printw(CP[231], (flag.tsume) ? CP[93] : CP[92]);
    gotoXY(40, 20);
    printw(CP[52], MaxSearchDepth);
    gotoXY(40, 21);
    printw(CP[100], (dither) ? CP[93] : CP[92]);
    gotoXY(40, 22);
    printw(CP[112], (flag.hash) ? CP[93] : CP[92]);
    gotoXY(40, 23);
    printw(CP[73]);
    gotoXY(10, 24);
    printw(CP[110], (TCflag) ? CP[93] : CP[92],
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


static const short x0[2] = { 54, 2 };
static const short y0[2] = { 20, 4 };


/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, N3f will place a knight on square 3f.
 * P* will put a pawn to the captured pieces.
 */

void
Curses_EditBoard(void)
{
    short a, c, sq, i;
    short r = 0;
    char s[80];

    flag.regularstart = true;
    Book = BOOKFAIL;
    Curses_ClearScreen();
    Curses_UpdateDisplay(0, 0, 1, 0);
    gotoXY(TAB, 3);
    printw(CP[29]);
    gotoXY(TAB, 4);
    printw(CP[28]);
    gotoXY(TAB, 5);
    printw(CP[136]);
    gotoXY(TAB, 7);
    printw(CP[64]);
    a = black;

    do
    {
        gotoXY(TAB, 6);
        printw(CP[60], ColorStr[a]); /* Editing %s */
        gotoXY(TAB + 24, 7);
        ClearEoln();
        FLUSH_SCANW("%s", s);

        if (s[0] == CP[28][0])    /* # */
        {
            for (sq = 0; sq < NO_SQUARES; sq++)
            {
                board[sq] = no_piece;
                color[sq] = neutral;
                DrawPiece(sq);
            }

            ClearCaptured();
            UpdateCatched();
        }

        if (s[0] == CP[136][0])   /* c */
            a = otherside[a];

        if (s[1] == '*')
        {
            for (i = NO_PIECES; i > no_piece; i--)
            {
                if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
                    break;
            }

            Captured[a][unpromoted[i]]++;
            UpdateCatched();
            c = -1;
        }
        else
        {
            c = '9' - s[1];
            r = 'i' - s[2];
        }

        if ((c >= 0) && (c < NO_COLS) && (r >= 0) && (r < NO_ROWS))
        {
            sq = locn(r, c);

            for (i = NO_PIECES; i > no_piece; i--)
            {
                if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
                    break;
            }

            if (s[3] == '+')
                i = promoted[i];
            else
                i = unpromoted[i];

            board[sq] = i;
            color[sq] = ((board[sq] == no_piece) ? neutral : a);
            DrawPiece(sq);
        }
    }
    while (s[0] != CP[29][0]);  /* . */

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



void
Curses_SearchStartStuff(short side)
{
    short i;

    signal(SIGINT, Curses_TerminateSearch);
    signal(SIGQUIT, Curses_TerminateSearch);

    for (i = 4; i < 14; i++)
    {
        gotoXY(TAB, i);
        ClearEoln();
    }
}



void
Curses_OutputMove(void)
{

    Curses_UpdateDisplay(root->f, root->t, 0, (short) root->flags);
    gotoXY(TAB, 16);

    if (flag.illegal) 
    {
        printw(CP[225]);
        return;
    }

    printw(CP[84], mvstr[0]);    /* My move is %s */

    if (flag.beep)
        putchar(7);

    ClearEoln();

    gotoXY(TAB, 18);

    if (root->flags & draw)
        printw(CP[58]);
    else if (root->score == -(SCORE_LIMIT + 999))
        printw(CP[95]);
    else if (root->score == SCORE_LIMIT + 998)
        printw(CP[44]);
#ifdef VERYBUGGY
    else if (root->score < -SCORE_LIMIT)
        printw(CP[96], SCORE_LIMIT + 999 + root->score - 1);
    else if (root->score > SCORE_LIMIT)
        printw(CP[45], SCORE_LIMIT + 998 - root->score - 1);
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

        ShowNodeCnt(NodeCnt);
        gotoXY(TAB, 23);
        printw(CP[81], t); /* Max Tree= */
        ClearEoln();
    }

    Curses_ShowSidetoMove();
}



void
UpdateClocks(void)
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
        gotoXY(20, (flag.reverse) ? 2 : 23);
    else
        gotoXY(20, (flag.reverse) ? 23 : 2);

    /* printw("%d:%02d %ld  ", m, s, dt); */
    printw("%d:%02d  ", m, s); 

    if (flag.post)
        ShowNodeCnt(NodeCnt);

    refresh();
}



void
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

    gotoXY(8 + 5 * VIR_C(sq), 4 + 2 * (8 - VIR_R(sq)));
    printw("%c%c%c%c", l, p, y, r);
}




/*
 * Curses_ShowPostnValue(): must have called ExaminePosition() first
 */

void
Curses_ShowPostnValue(short sq)
{
    short score;

    gotoXY(4 + 5 * VIR_C(sq), 5 + 2 * (7 - VIR_R(sq)));
    score = ScorePosition(color[sq]);

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



void
Curses_ShowPostnValues(void)
{
    short sq, score;

    ExaminePosition(opponent);

    for (sq = 0; sq < NO_SQUARES; sq++)
        Curses_ShowPostnValue(sq);

    score = ScorePosition(opponent);
    gotoXY(TAB, 5);
    printw(CP[103], score, 
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);

    ClearEoln();
}



void
Curses_UpdateDisplay(short f, short t, short redraw, short isspec)
{
    short i, sq, z;

    if (redraw)
    {
        ShowHeader();
        ShowPlayers();

        i = 2;
        gotoXY(3, ++i);

        printw("    +----+----+----+----+----+----+----+----+----+");

        while (i < 20)
        {
            gotoXY(1, ++i);

            if (flag.reverse)
                z = (i / 2) - 1;
            else
                z = 11 - ((i + 1) / 2);

            printw("    %c |    |    |    |    |    |"
                   "    |    |    |    |", 'a' + 9 - z);

            gotoXY(3, ++i);

            if (i < 20)
            {
                printw("    +----+----+----+----+----+----+----+----+----+");
            }
        }

        printw("    +----+----+----+----+----+----+----+----+----+");

        gotoXY(3, 22);
        printw("    ");

        if (flag.reverse)
            printw(CP[16]);
        else
            printw(CP[15]);

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


extern char *InPtr;


void
Curses_ChangeAlphaWindow(void)
{
    Curses_ShowMessage(CP[114]);
    FLUSH_SCANW("%hd", &WAwindow);
    Curses_ShowMessage(CP[34]);
    FLUSH_SCANW("%hd", &BAwindow);
}



void
Curses_ChangeBetaWindow(void)
{
    Curses_ShowMessage(CP[115]);
    FLUSH_SCANW("%hd", &WBwindow);
    Curses_ShowMessage(CP[35]);
    FLUSH_SCANW("%hd", &BBwindow);
}



void
Curses_GiveHint(void)
{
    char s[40];

    if (hint)
    {
        algbr((short) (hint >> 8), (short) (hint & 0xFF), false);
        strcpy(s, CP[198]);  /* try */
        strcat(s, mvstr[0]);
        Curses_ShowMessage(s);
    }
    else
    {
        Curses_ShowMessage(CP[223]);
    }
}



void
Curses_ChangeSearchDepth(void)
{
    Curses_ShowMessage(CP[150]);
    FLUSH_SCANW("%hd", &MaxSearchDepth);
    TCflag = !(MaxSearchDepth > 0);
}


void
Curses_ChangeHashDepth(void)
{
    Curses_ShowMessage(CP[163]);
    FLUSH_SCANW("%hd", &HashDepth);
    Curses_ShowMessage(CP[82]);
    FLUSH_SCANW("%hd", &HashMoveLimit);
}


void
Curses_SetContempt(void)
{
    Curses_ShowMessage(CP[142]);
    FLUSH_SCANW("%hd", &contempt);
}



void
Curses_ChangeXwindow(void)
{
    Curses_ShowMessage(CP[208]);
    FLUSH_SCANW("%hd", &xwndw);
}



void
Curses_SelectLevel(char *sx)
{
    int item;

    Curses_ClearScreen();
    gotoXY(32, 2);
    printw(CP[41], version, patchlevel);
    gotoXY(20, 4);
    printw(CP[18]);
    gotoXY(20, 5);
    printw(CP[19]);
    gotoXY(20, 6);
    printw(CP[20]);
    gotoXY(20, 7);
    printw(CP[21]);
    gotoXY(20, 8);
    printw(CP[22]);
    gotoXY(20, 9);
    printw(CP[23]);
    gotoXY(20, 10);
    printw(CP[24]);
    gotoXY(20, 11);
    printw(CP[25]);
    gotoXY(20, 12);
    printw(CP[26]);
    gotoXY(20, 13);
    printw(CP[27]);

    OperatorTime = 0;
    TCmoves = 40;
    TCminutes = 5;
    TCseconds = 0;

    gotoXY(20, 17);
    printw(CP[62]);
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



void
Curses_DoDebug(void)
{
    short c, p, sq, tp, tc, tsq, score;
    char s[40];

    ExaminePosition(opponent);
    Curses_ShowMessage(CP[65]);
    FLUSH_SCANW("%s", s);
    c = neutral;

    if ((s[0] == CP[9][0]) || (s[0] == CP[9][1])) /* b B */
        c = black;

    if ((s[0] == CP[9][2]) || (s[0] == CP[9][3])) /* w W */
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
    printw(CP[103], score, 
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);

    ClearEoln();
}



void
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



