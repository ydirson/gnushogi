/*
 * FILE: rawdsp.c
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

#include <ctype.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>

#include "gnushogi.h"
#include "rawdsp.h"

unsigned short MV[MAXDEPTH];
int MSCORE;

int mycnt1, mycnt2;
char *DRAW;
extern char *InPtr;
extern short pscore[];


/****************************************
 * Trivial output functions.
 ****************************************/

void
Raw_ClearScreen(void)
{
    if (!barebones && !XSHOGI)
        printf("\n");
}


/* FIXME: change to ShowPrompt? */
void
PromptForMove(void)
{
    if (!barebones && !XSHOGI)
    {
        /* printf("\nYour move is? "); */
        printf(CP[124]);
    }
}


void
Raw_ShowCurrentMove(short pnt, short f, short t)
{
}


void
Raw_ShowDepth(char ch)
{
    if (!barebones && !XSHOGI)
    {
        printf(CP[53], Sdepth, ch);   /* Depth = %d%c */
        printf("\n");
    }
}


void
Raw_ShowGameType(void)
{
    if (flag.post)
        printf("%c vs. %c\n", GameType[black], GameType[white]);
}


void
Raw_ShowLine(unsigned short *bstline)
{
    int i;

    for (i = 1; bstline[i] > 0; i++)
    {
        if ((i > 1) && (i % 8 == 1))
            printf("\n                          ");

        algbr((short)(bstline[i] >> 8), (short)(bstline[i] & 0xFF), false);
        printf("%5s ", mvstr[0]);
    }

    printf("\n");
}


void
Raw_ShowMessage(char *s)
{
    if (!XSHOGI)
        printf("%s\n", s);
}


void
Raw_ShowPatternCount(short side, short n)
{
    if (flag.post)
        printf("%s matches %d pattern(s)\n", ColorStr[side], n);
}


void
Raw_ShowResponseTime(void)
{
}


void
Raw_ShowResults(short score, unsigned short *bstline, char ch)
{
    if (flag.post  && !XSHOGI)
    {
        ElapsedTime(2);
        printf("%2d%c %6d %4ld %8ld  ",
               Sdepth, ch, score, et / 100, NodeCnt);
        Raw_ShowLine(bstline);
    }
}


void
Raw_ShowSidetoMove(void)
{
}


void
Raw_ShowStage(void)
{
    printf("stage = %d\n", stage);
    printf("balance[black] = %d balance[white] = %d\n",
           balance[black], balance[white]);
}

/****************************************
 * End of trivial output routines.
 ****************************************/


void
Raw_Initialize(void)
{
    mycnt1 = mycnt2 = 0;

    if (XSHOGI)
    {
#ifdef HAVE_SETLINEBUF
        setlinebuf(stdout);
#else
#  ifdef HAVE_SETVBUF
        setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#  else
#    error "Need setlinebuf() or setvbuf() to compile gnushogi!"
#  endif
#endif
        printf("GNU Shogi %sp%s\n", version, patchlevel);
    }

    if (hard_time_limit)
    {
        if (!TCflag && (MaxResponseTime == 0))
            MaxResponseTime = 15L * 100L;
    }
}



void
Raw_ExitShogi(void)
{
    /* CHECKME: what purpose does this next statement serve? */
    signal(SIGTERM, SIG_IGN);

    if (!nolist)
        ListGame();
}


void
Raw_Die(int sig)
{
    char s[80];

    Raw_ShowMessage(CP[31]);        /* Abort? */
    scanf("%s", s);

    if (strcmp(s, CP[210]) == 0)    /* yes */
        Raw_ExitShogi();
}



void
Raw_TerminateSearch(int sig)
{
#ifdef INTERRUPT_TEST
    ElapsedTime(INIT_INTERRUPT_MODE);
#endif

    if (!flag.timeout)
        flag.back = true; /* previous: flag.timeout = true; */

    flag.bothsides = false;
}



void
Raw_help(void)
{
    Raw_ClearScreen();
    /* printf("SHOGI command summary\n"); */
    printf(CP[40]);
    printf("----------------------------------"
           "------------------------------\n");
    /* printf("7g7f      move from 7g to 7f      quit
     * Exit Shogi\n"); */
    printf(CP[158]);
    /* printf("S6h       move silver to 6h       beep
     * turn %s\n", (flag.beep) ? "off" : "on"); */
    printf(CP[86], (flag.beep) ? CP[92] : CP[93]);
    /* printf("2d2c+     move from 2d to 2c and promote\n"); */
    printf(CP[128], (flag.material) ? CP[92] : CP[93]);
    /* printf("P*5e      drop pawn to 5e         easy
     * turn %s\n", (flag.easy) ? "off" : "on"); */
    printf(CP[173], (flag.easy) ? CP[92] : CP[93]);
    /* printf("                                  hash
     * turn %s\n", (flag.hash) ? "off" : "on"); */
    printf(CP[174], (flag.hash) ? CP[92] : CP[93]);
    /* printf("bd        redraw board            reverse
     * board display\n"); */
    printf(CP[130]);
    /* printf("list      game to shogi.lst       book
     * turn %s used %d of %d\n", (Book) ? "off" : "on", bookcount); */
    printf(CP[170], (Book) ? CP[92] : CP[93], bookcount, booksize);
    /* printf("undo      undo last ply           remove
     * take back a move\n"); */
    printf(CP[200]);
    /* printf("edit      edit board              force
     * enter game moves\n"); */
    printf(CP[153]);
    /* printf("switch    sides with computer     both
     * computer match\n"); */
    printf(CP[194]);
    /* printf("black     computer plays black    white
     * computer plays white\n"); */
    printf(CP[202]);
    /* printf("depth     set search depth        clock
     * set time control\n"); */
    printf(CP[149]);
    /* printf("post      principle variation     hint
     * suggest a move\n"); */
    printf(CP[177]);
    /* printf("save      game to file            get
     * game from file\n"); */
    printf(CP[188]);
    printf("xsave     pos. to xshogi file     xget"
           "      pos. from xshogi file\n");
    /* printf("random    randomize play          new
     * start new game\n"); */
    printf(CP[181]);
    printf("--------------------------------"
           "--------------------------------\n");
    /* printf("Computer: %-12s Opponent:            %s\n", */
    printf(CP[46],
           ColorStr[computer], ColorStr[opponent]);
    /* printf("Depth:    %-12d Response time:       %d sec\n", */
    printf(CP[51],
           MaxSearchDepth, MaxResponseTime/100);
    /* printf("Random:   %-12s Easy mode:           %s\n", */
    printf(CP[99],
           (dither) ? CP[93] : CP[92], (flag.easy) ? CP[93] : CP[92]);
    /* printf("Beep:     %-12s Transposition file: %s\n", */
    printf(CP[36],
           (flag.beep) ? CP[93] : CP[92], (flag.hash) ? CP[93] : CP[92]);
    /* printf("Tsume:    %-12s Force:               %s\n")*/
    printf(CP[232],
           (flag.tsume) ? CP[93] : CP[92], (flag.force) ? CP[93] : CP[92]);
    /* printf("Time Control %s %d moves %d seconds %d opr %d
     * depth\n", (TCflag) ? "ON" : "OFF", */
    printf(CP[110],
           (TCflag) ? CP[93] : CP[92],
           TimeControl.moves[black], TimeControl.clock[black] / 100,
           TCadd/100, MaxSearchDepth);

    signal(SIGINT, Raw_TerminateSearch);
    signal(SIGQUIT, Raw_TerminateSearch);
}



/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, Nf3 will place a knight on square f3.
 */

void
Raw_EditBoard(void)
{
    short a, r, c, sq, i, found;
    char s[80];

    flag.regularstart = true;
    Book = BOOKFAIL;
    Raw_ClearScreen();
    Raw_UpdateDisplay(0, 0, 1, 0);
    /* printf(".   exit to main\n"); */
    printf(CP[29]);
    /* printf("#   clear board\n"); */
    printf(CP[28]);
    /* printf("c   change sides\n"); */
    printf(CP[136]);
    /* printf("enter piece & location: \n"); */
    printf(CP[155]);

    a = black;

    do
    {
        scanf("%s", s);
        found = 0;

        if (s[0] == CP[28][0])  /*#*/
        {
            for (sq = 0; sq < NO_SQUARES; sq++)
            {
                board[sq] = no_piece;
                color[sq] = neutral;
            }

            ClearCaptured();
        }

        if (s[0] == CP[136][0]) /*c*/
            a = otherside[a];

        if (s[1] == '*')
        {
            for (i = pawn; i <= king; i++)
            {
                if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
                {
                    Captured[a][i]++;
                    found = 1;
                    break;
                }
            }

            c = -1;
            r = -1;
        }
        else
        {
            c = '9' - s[1];
            r = 'i' - s[2];
        }

        if ((c >= 0) && (c < NO_COLS) && (r >= 0) && (r < NO_ROWS))
        {
            sq = locn(r, c);
            color[sq] = a;
            board[sq] = no_piece;

            for (i = no_piece; i <= king; i++)
            {
                if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
                {
                    if (s[3] == '+')
                        board[sq] = promoted[i];
                    else
                        board[sq] = i;

                    found = 1;
                    break;
                }
            }

            if (found == 0)
                color[sq] = neutral;
        }
    }
    while (s[0] != CP[29][0]);

    for (sq = 0; sq < NO_SQUARES; sq++)
        Mvboard[sq] = ((board[sq] != Stboard[sq]) ? 10 : 0);

    GameCnt = 0;
    Game50 = 1;
    ZeroRPT();
    Sdepth = 0;
    InitializeStats();
    Raw_ClearScreen();
    Raw_UpdateDisplay(0, 0, 1, 0);
}




/*
 * Set up a board position.
 * Nine lines of nine characters are used to setup the board. 9a-1a is the
 * first line. White pieces are  represented  by  uppercase characters.
 */

void
SetupBoard(void)
{
    short r, c, sq, i;
    char ch;
    char s[80];

    NewGame();

    fgets(s, 80, stdin);            /* skip "setup" command */

    for (r = NO_ROWS - 1; r >= 0; r--)
    {
        fgets(s, 80, stdin);

        for (c = 0; c <= (NO_COLS - 1); c++)
        {
            ch = s[c];
            sq = locn(r, c);
            color[sq] = neutral;
            board[sq] = no_piece;

            for (i = no_piece; i <= king; i++)
            {
                if (ch == pxx[i])
                {
                    color[sq] = white;
                    board[sq] = i;
                    break;
                }
                else if (ch == qxx[i])
                {
                    color[sq] = black;
                    board[sq] = i;
                    break;
                }
            }
        }
    }

    for (sq = 0; sq < NO_SQUARES; sq++)
        Mvboard[sq] = ((board[sq] != Stboard[sq]) ? 10 : 0);

    InitializeStats();
    Raw_ClearScreen();
    Raw_UpdateDisplay(0, 0, 1, 0);
    /* printf("Setup successful\n"); */
    printf(CP[106]);
}


void
Raw_SearchStartStuff(short side)
{
    signal(SIGINT, Raw_TerminateSearch);
    signal(SIGQUIT, Raw_TerminateSearch);

    if (flag.post)
    {
        printf(CP[123],
               GameCnt/2 + 1,
               ResponseTime, TimeControl.clock[side]);
    }
}



void
Raw_OutputMove(void)
{
    if (flag.illegal)
    {
        printf("%s\n", CP[225]);
        return;
    }

    if (mvstr[0][0] == '\0')
        goto nomove;

    if (XSHOGI)
    {
        /* add remaining time in milliseconds to xshogi */
        printf("%d. ... %s %ld\n", ++mycnt1, mvstr[0],
               (TimeControl.clock[player] - et) * 10);
    }
    else
    {
        printf("%d. ... %s\n", ++mycnt1, mvstr[0]);
    }

 nomove:
    if ((root->flags & draw) || (root->score == -(SCORE_LIMIT + 999))
        || (root->score == (SCORE_LIMIT + 998)))
        goto summary;

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

        /* printf("Nodes %ld Tree %d Eval %ld
         * Rate %ld RS high %ld low %ld\n", */
        printf(CP[89], GenCnt, NodeCnt, t, EvalNodes,
               (et > 100) ? (NodeCnt / (et / 100)) : 0,
               EADD, EGET, reminus, replus);

        /* printf("Hin/Hout/Coll/Fin/Fout =
         * %ld/%ld/%ld/%ld/%ld\n", */
        printf(CP[71],
               HashAdd, HashCnt, THashCol, HashCol, FHashCnt, FHashAdd);
    }

    Raw_UpdateDisplay(root->f, root->t, 0, root->flags);

    if (!XSHOGI)
    {
        /* printf("My move is: %s\n", mvstr[0]); */
        printf(CP[83], mvstr[0]);

        if (flag.beep)
            printf("%c", 7);
    }

 summary:
    if (root->flags & draw)
    {
        /*  printf("Drawn game!\n"); */
        printf(CP[57]);
    }
    else if (root->score == -(SCORE_LIMIT + 999))
    {
        printf("%s mates!\n", ColorStr[opponent]);
    }
    else if (root->score == (SCORE_LIMIT + 998))
    {
        printf("%s mates!\n", ColorStr[computer]);
    }
#ifdef VERYBUGGY
    else if (!barebones && (root->score < -SCORE_LIMIT))
    {
        printf("%s has a forced mate in %d moves!\n",
               ColorStr[opponent], SCORE_LIMIT + 999 + root->score - 1);
    }
    else if (!barebones && (root->score > SCORE_LIMIT))
    {
        printf("%s has a forced mate in %d moves!\n",
               ColorStr[computer], SCORE_LIMIT + 998 - root->score - 1);
    }
#endif /* VERYBUGGY */
}


void
Raw_UpdateDisplay(short f, short t, short redraw, short isspec)
{

    short r, c, l, m;

    if (redraw && !XSHOGI)
    {
        printf("\n");
        r = (short)(TimeControl.clock[black] / 6000);
        c = (short)((TimeControl.clock[black] % 6000) / 100);
        l = (short)(TimeControl.clock[white] / 6000);
        m = (short)((TimeControl.clock[white] % 6000) / 100);
        /* printf("Black %d:%02d  White %d:%02d\n", r, c, l, m); */
        printf(CP[116], r, c, l, m);
        printf("\n");

        for (r = (NO_ROWS - 1); r >= 0; r--)
        {
            for (c = 0; c <= (NO_COLS - 1); c++)
            {
                char pc;
                l = ((flag.reverse)
                     ? locn((NO_ROWS - 1) - r, (NO_COLS - 1) - c)
                     : locn(r, c));
                pc = (is_promoted[board[l]] ? '+' : ' ');

                if (color[l] == neutral)
                    printf(" -");
                else if (color[l] == black)
                    printf("%c%c", pc, qxx[board[l]]);
                else
                    printf("%c%c", pc, pxx[board[l]]);
            }

            printf("\n");
        }

        printf("\n");
        {
            short side;

            for (side = black; side <= white; side++)
            {
                short piece, c;
                printf((side == black)?"black ":"white ");

                for (piece = pawn; piece <= king; piece++)
                {
                    if ((c = Captured[side][piece]))
                        printf("%i%c ", c, pxx[piece]);
                }

                printf("\n");
            }
        }
    }
}



void
Raw_ChangeAlphaWindow(void)
{
    printf("WAwindow: ");
    scanf("%hd", &WAwindow);
    printf("BAwindow: ");
    scanf("%hd", &BAwindow);
}



void
Raw_ChangeBetaWindow(void)
{
    printf("WBwindow: ");
    scanf("%hd", &WBwindow);
    printf("BBwindow: ");
    scanf("%hd", &BBwindow);
}



void
Raw_GiveHint(void)
{
    if (hint)
    {
        algbr((short) (hint >> 8), (short) (hint & 0xFF), false);
        printf(CP[72], mvstr[0]);   /*hint*/
    }
    else
        printf(CP[223]);
}



void
Raw_SelectLevel(char *sx)
{

    char T[NO_SQUARES + 1], *p, *q;

    if ((p = strstr(sx, CP[169])) != NULL)
        p += strlen(CP[169]);
    else if ((p = strstr(sx, CP[217])) != NULL)
        p += strlen(CP[217]);

    strcat(sx, "XX");
    q = T;
    *q = '\0';

    for (; *p != 'X'; *q++ = *p++);

    *q = '\0';

    /* line empty ask for input */
    if (!T[0])
    {
        printf(CP[61]);
        fgets(T, NO_SQUARES + 1, stdin);
        strcat(T, "XX");
    }

    /* skip blackspace */
    for (p = T; *p == ' '; p++) ;

    /* could be moves or a fischer clock */
    if (*p == 'f')
    {
        /* its a fischer clock game */
        p++;
        TCminutes = (short)strtol(p, &q, 10);
        TCadd = (short)strtol(q, NULL, 10) *100;
        TCseconds = 0;
        TCmoves = 50;
    }
    else
    {
        /* regular game */
        TCadd = 0;
        TCmoves = (short)strtol(p, &q, 10);
        TCminutes = (short)strtol(q, &q, 10);

        if (*q == ':')
            TCseconds = (short)strtol(q + 1, (char **) NULL, 10);
        else
            TCseconds = 0;

#ifdef OPERATORTIME
        printf(CP[94]);
        scanf("%hd", &OperatorTime);
#endif

        if (TCmoves == 0)
        {
            TCflag = false;
            MaxResponseTime = TCminutes*60L * 100L + TCseconds * 100L;
            TCminutes = TCseconds = 0;
        }
        else
        {
            TCflag = true;
            MaxResponseTime = 0;
        }
    }

    TimeControl.clock[black] = TimeControl.clock[white] = 0;
    SetTimeControl();

    if (XSHOGI)
    {
        printf("Clocks: %ld %ld\n",
               TimeControl.clock[black] * 10,
               TimeControl.clock[white] * 10);
    }
}




void
Raw_ChangeSearchDepth(void)
{
    printf("depth = ");
    scanf("%hd", &MaxSearchDepth);
    TCflag = !(MaxSearchDepth > 0);
}




void
Raw_ChangeHashDepth(void)
{
    printf("hashdepth = ");
    scanf("%hd", &HashDepth);
    printf("MoveLimit = ");
    scanf("%hd", &HashMoveLimit);
}



void
Raw_SetContempt(void)
{
    printf("contempt = ");
    scanf("%hd", &contempt);
}



void
Raw_ChangeXwindow(void)
{
    printf("xwndw = ");
    scanf("%hd", &xwndw);
}


/*
 * Raw_ShowPostnValue(short sq)
 * must have called ExaminePosition() first
 */

void
Raw_ShowPostnValue(short sq)
{
    short score;
    score = ScorePosition(color[sq]);

    if (color[sq] != neutral)
    {
#if defined SAVE_SVALUE
        printf("???%c ", (color[sq] == white)?'b':'w');
#else
        printf("%3d%c ", svalue[sq], (color[sq] == white)?'b':'w');
#endif
    }
    else
    {
        printf(" *   ");
    }
}



void
Raw_DoDebug(void)
{
    short c, p, sq, tp, tc, tsq, score, j, k;
    char s[40];

    ExaminePosition(opponent);
    Raw_ShowMessage(CP[65]);
    scanf("%s", s);
    c = neutral;

    if ((s[0] == CP[9][0]) || (s[0] == CP[9][1]))    /* w W */
        c = black;

    if ((s[0] == CP[9][2]) || (s[0] == CP[9][3]))    /* b B */
        c = white;

    for (p = king; p > no_piece; p--)
    {
        if ((s[1] == pxx[p]) || (s[1] == qxx[p]))
            break;
    }

    if (p > no_piece)
    {
        for (j = (NO_ROWS - 1); j >= 0; j--)
        {
            for (k = 0; k < (NO_COLS); k++)
            {
                sq = j*(NO_COLS) + k;
                tp = board[sq];
                tc = color[sq];
                board[sq] = p;
                color[sq] = c;
                tsq = PieceList[c][1];
                PieceList[c][1] = sq;
                Raw_ShowPostnValue(sq);
                PieceList[c][1] = tsq;
                board[sq] = tp;
                color[sq] = tc;
            }

            printf("\n");
        }
    }

    score = ScorePosition(opponent);

    for (j = (NO_ROWS - 1); j >= 0; j--)
    {
        for (k = 0; k < (NO_COLS); k++)
        {
            sq = j*(NO_COLS) + k;

            if (color[sq] != neutral)
            {
#if defined SAVE_SVALUE
                printf("%?????%c ", (color[sq] == white)?'b':'w');
#else
                printf("%5d%c ", svalue[sq], (color[sq] == white)?'b':'w');
#endif
            }
            else
            {
                printf("    *  ");
            }
        }

        printf("\n");
    }

    printf("stage = %d\n", stage);
    printf(CP[103], score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);
}



void
Raw_DoTable(short table[NO_SQUARES])
{
    short  sq, j, k;
    ExaminePosition(opponent);

    for (j = (NO_ROWS - 1); j >= 0; j--)
    {
        for (k = 0; k < NO_COLS; k++)
        {
            sq = j*(NO_ROWS) + k;
            printf("%3d ", table[sq]);
        }

        printf("\n");
    }
}



void
Raw_ShowPostnValues(void)
{
    short sq, score, j, k;
    ExaminePosition(opponent);

    for (j = (NO_ROWS - 1); j >= 0; j--)
    {
        for (k = 0; k < NO_COLS; k++)
        {
            sq = j * NO_COLS + k;
            Raw_ShowPostnValue(sq);
        }

        printf("\n");
    }

    score = ScorePosition(opponent);
    printf(CP[103], score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);
    printf("\nhung black %d hung white %d\n", hung[black], hung[white]);
}

