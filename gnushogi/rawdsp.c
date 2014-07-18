/*
 * FILE: rawdsp.c
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

#include "gnushogi.h"

#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#ifndef WIN32
#include <poll.h>
#include <unistd.h>
#endif

/****************************************
 * forward declarations
 ****************************************/

static void Raw_UpdateDisplay(short f, short t, short redraw, short isspec);

/****************************************
 * Trivial output functions.
 ****************************************/

static void
Raw_ClearScreen(void)
{
    if (!XSHOGI)
        printf("\n");
}


static void
Raw_ShowPrompt(void)
{
    if (!XSHOGI)
        fputs("\nYour move is? ", stdout);
}


static void
Raw_ShowCurrentMove(short pnt, short f, short t)
{
}


static void
Raw_ShowDepth(char ch)
{
    if (!XSHOGI)
        printf("Depth= %d%c \n", Sdepth, ch);
}


static void
Raw_ShowGameType(void)
{
    if (flag.post)
        printf("%c vs. %c\n", GameType[black], GameType[white]);
}


static void
Raw_ShowLine(unsigned short *bstline)
{
    int i;

    for (i = 1; bstline[i] > 0; i++)
    {
        if ((i > 1) && (i % 8 == 1))
            printf("\n                          ");

        algbr((short)(bstline[i] >> 8), (short)(bstline[i] & 0xFF), 0);
        printf("%5s ", mvstr[0]);
    }

    printf("\n");
}


static void
Raw_ShowMessage(char *format, ...)
{
    if (XSHOGI)
        return;
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
}


static void
Raw_AlwaysShowMessage(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
}


static void
Raw_Printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}


static void
Raw_doRequestInputString(const char* fmt, char* buffer)
{
    scanf(fmt, buffer);
}


static int
Raw_GetString(char* sx)
{
    int eof = 0;
    char *nl;
    sx[0] = '\0';

    while(!eof && !sx[0])
        eof = (fgets(sx, 80, stdin) == NULL);

    /* remove any trailing newline */
    nl = strchr(sx, '\n');
    if (nl)
        nl[0] = '\0';

    return eof;
}


static void
Raw_ShowNodeCnt(long NodeCnt)
{
    printf("Nodes = %ld Nodes/sec = %ld\n",
           NodeCnt, (((et) ? ((NodeCnt * 100) / et) : 0)));
}


static void
Raw_ShowPatternCount(short side, short n)
{
    if (flag.post)
        printf("%s%s matches %d pattern(s)\n", xboard ? "# " : "" , ColorStr[side], n);
}


static void
Raw_ShowResponseTime(void)
{
}


static void
Raw_ShowResults(short score, unsigned short *bstline, char ch)
{
    if (flag.post && (xboard || !XSHOGI))
    {
        ElapsedTime(2);
        printf("%2d%c %6d %4ld %8ld  ",
               Sdepth, ch, score, et / 100, NodeCnt);
        Raw_ShowLine(bstline);
    }
}


static void
Raw_ShowSidetoMove(void)
{
}


static void
Raw_ShowStage(void)
{
    printf("stage = %d\n", stage);
    printf("balance[black] = %d balance[white] = %d\n",
           balance[black], balance[white]);
}

/****************************************
 * End of trivial output routines.
 ****************************************/

static void
Raw_Initialize(void)
{
    mycnt1 = mycnt2 = 0;

    if (XSHOGI)
    {
#ifdef WIN32
        /* needed because of inconsistency between MSVC run-time system and gcc includes */
        setbuf(stdout, NULL);
#else
#ifdef HAVE_SETVBUF
        setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#else
#  ifdef HAVE_SETLINEBUF
        setlinebuf(stdout);
#  else
#    error "Need setvbuf() or setlinebuf() to compile gnushogi!"
#  endif
#endif
#endif
        printf("GNU Shogi %s\n", PACKAGE_VERSION);
    }

    if (hard_time_limit)
    {
        if (!TCflag && (MaxResponseTime == 0))
            MaxResponseTime = 15L * 100L;
    }
}


static void
Raw_ExitShogi(void)
{
    /* CHECKME: what purpose does this next statement serve? */
    signal(SIGTERM, SIG_IGN);

    if (!nolist)
        ListGame();

    exit(0);
}


static void
Raw_TerminateSearch(int sig)
{
#ifdef INTERRUPT_TEST
    ElapsedTime(INIT_INTERRUPT_MODE);
#endif

    if (!flag.timeout)
        flag.back = true; /* previous: flag.timeout = true; */

    flag.bothsides = false;
}


static void
Raw_help(void)
{
    Raw_ClearScreen();
    printf("GNU Shogi %s command summary\n", PACKAGE_VERSION);
    printf("----------------------------------------------------------------\n");
    printf("7g7f      move from 7g to 7f      quit      Exit Shogi\n");
    printf("S6h       move silver to 6h       beep      turn %s\n",
           (flag.beep) ? "OFF" : "ON");
    printf("2d2c+     move to 2c and promote  material  turn %s\n",
           (flag.material) ? "OFF" : "ON");
    printf("P*5e      drop pawn to 5e         easy      turn %s\n",
           (flag.easy) ? "OFF" : "ON");
    printf("tsume     toggle tsume mode       hash      turn %s\n",
           (flag.hash) ? "OFF" : "ON");
    printf("bd        redraw board            reverse   board display\n");
    printf("list      game to shogi.lst       book      turn %s used %d of %d\n",
           (Book) ? "OFF" : "ON", bookcount, booksize);
    printf("undo      undo last ply           remove    take back a move\n");
    printf("edit      edit board              force     toggle manual move mode\n");
    printf("switch    sides with computer     both      computer match\n");
    printf("black     computer plays black    white     computer plays white\n");
    printf("sd        set search depth        clock     set time control\n");
    printf("post      principle variation     hint      suggest a move\n");
    printf("save      game to file            get       game from file\n");
    printf("xsave     pos. to xshogi file     xget      pos. from xshogi file\n");
    printf("random    randomize play          new       start new game\n");
    printf("setup                             first     \n");
    printf("go        computer plays now      material  turn %s\n",
           flag.material ? "OFF" : "ON");
    printf("level     time control            gamein    \n");
    printf("time      set engine clock        otime     set opponent clock\n");
    printf("Awindow                           Bwindow     \n");
    printf("rcptr     turn %3s                bsave     book save\n",
           flag.rcptr ? "OFF" : "ON ");
    printf("hashdepth                         hard      turn easy OFF\n");
    printf("contempt                          xwndw                  \n");
    printf("rv        turn %3s                coords    turn %s\n",
           flag.rv ? "OFF" : "ON ", flag.coords ? "OFF" : "ON");
    printf("stars     turn %3s                moves                  \n",
           flag.stars ? "OFF" : "ON ");
    printf("test                              p                      \n");
    printf("debug                             depth     alias for 'sd'\n");
    printf("----------------------------------------------------------------\n");
    printf("Computer: %-12s Opponent:            %s\n",
           ColorStr[computer], ColorStr[opponent]);
    printf("Depth:    %-12d Response time:       %ld sec\n",
           MaxSearchDepth, MaxResponseTime/100);
    printf("Random:   %-12s Easy mode:           %s\n",
           (dither) ? "ON" : "OFF", (flag.easy) ? "ON" : "OFF");
    printf("Beep:     %-12s Transposition file:  %s\n",
           (flag.beep) ? "ON" : "OFF", (flag.hash) ? "ON" : "OFF");
    printf("Tsume:    %-12s Force:               %s\n",
           (flag.tsume) ? "ON" : "OFF", (flag.force) ? "ON" : "OFF");
    printf("Time Control %s %d moves %ld sec %d add %d depth\n",
           (TCflag) ? "ON" : "OFF",
           TimeControl.moves[black], TimeControl.clock[black] / 100,
           TCadd/100, MaxSearchDepth);
}


/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, Nf3 will place a knight on square f3.
 */
static void
Raw_EditBoard(void)
{
    short a, r, c, sq, i, found;
    char s[80];

    flag.regularstart = true;
    Book = BOOKFAIL;
    Raw_ClearScreen();
    Raw_UpdateDisplay(0, 0, 1, 0);
    printf(".   Exit to main\n");
    printf("#   Clear board\n");
    printf("c   Change sides\n");
    printf("enter piece & location:\n");

    a = black;

    while(1)
    {
        scanf("%s", s);
        found = 0;

        if (s[0] == '.')
            break;

        if (s[0] == '#')
        {
            for (sq = 0; sq < NO_SQUARES; sq++)
            {
                board[sq] = no_piece;
                color[sq] = neutral;
            }

            ClearCaptured();
            continue;
        }

        if (s[0] == 'c') {
            a = otherside[a];
            continue;
        }

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
            if (!found)
                printf("# Invalid piece type '%c'\n", s[0]);
            continue;
        }

        c = COL_NUM(s[1]);
        r = ROW_NUM(s[2]);

        if ((c < 0) || (c >= NO_COLS) || (r < 0) || (r >= NO_ROWS)) {
            printf("# Out-of-board position '%c%c'\n", s[1], s[2]);
            continue;
        }

        sq = locn(r, c);

        for (i = no_piece; i <= king; i++)
        {
            if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
            {
                color[sq] = a;
                if (s[3] == '+')
                    board[sq] = promoted[i];
                else
                    board[sq] = i;

                found = 1;
                break;
            }
        }

        if (!found)
            printf("# Invalid piece type '%c'\n", s[0]);
    }

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
static void
Raw_SetupBoard(void)
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
    fputs("Setup successful\n", stdout);
}


static void
Raw_SearchStartStuff(short side)
{
    if (flag.post)
    {
        printf("\nMove# %d    Target = %ld    Clock: %ld\n",
               GameCnt/2 + 1,
               ResponseTime, TimeControl.clock[side]);
    }
}


static void
Raw_OutputMove(void)
{
    if (flag.illegal)
    {
        printf("Illegal position.\n");
        return;
    }

    if (mvstr[0][0] == '\0')
        goto nomove;

    mycnt1++;
    if (XSHOGI && xboard) /* xboard: print move in XBoard format, with 'move' prefix */
        printf("move %s\n", mvstr[0]);
    else if (XSHOGI)
        /* add remaining time in milliseconds to xshogi */
        printf("%d. ... %s %ld\n", mycnt1, mvstr[0],
               (TimeControl.clock[player] - et) * 10);
    else
        printf("%d. ... %s\n", mycnt1, mvstr[0]);

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

        printf("Gen %ld Node %ld Tree %d Eval %ld Rate %ld EC %d/%d RS hi %ld lo %ld \n", GenCnt, NodeCnt, t, EvalNodes,
               (et > 100) ? (NodeCnt / (et / 100)) : 0,
               EADD, EGET, reminus, replus);

        printf("Hin/Hout/Tcol/Coll/Fin/Fout = %ld/%ld/%ld/%ld/%ld/%ld\n",
               HashAdd, HashCnt, THashCol, HashCol, FHashCnt, FHashAdd);
    }

    Raw_UpdateDisplay(root->f, root->t, 0, root->flags);

    if (!XSHOGI)
    {
        printf("My move is: %5s\n", mvstr[0]);

        if (flag.beep)
            printf("%c", 7);
    }

 summary:
    if (root->flags & draw)
        fputs("Drawn game!\n", stdout);
    else if (root->score == -(SCORE_LIMIT + 999))
        printf("%s mates!\n", ColorStr[opponent]);
    else if (root->score == (SCORE_LIMIT + 998))
        printf("%s mates!\n", ColorStr[computer]);
#ifdef VERYBUGGY
    else if (!XSHOGI && (root->score < -SCORE_LIMIT))
        printf("%s%s has a forced mate in %d moves!\n", xboard ? "# " : "",
               ColorStr[opponent], SCORE_LIMIT + 999 + root->score - 1);
    else if (!XSHOGI && (root->score > SCORE_LIMIT))
        printf("%s%s has a forced mate in %d moves!\n", xboard ? "# " : "",
               ColorStr[computer], SCORE_LIMIT + 998 - root->score - 1);
#endif /* VERYBUGGY */
}


static void
Raw_UpdateClocks(void)
{
}


static void
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
        printf("Black %d:%02d  White %d:%02d\n", r, c, l, m);
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


static void
Raw_ChangeAlphaWindow(void)
{
    printf("WAwindow: ");
    scanf("%hd", &WAwindow);
    printf("BAwindow: ");
    scanf("%hd", &BAwindow);
}


static void
Raw_ChangeBetaWindow(void)
{
    printf("WBwindow: ");
    scanf("%hd", &WBwindow);
    printf("BBwindow: ");
    scanf("%hd", &BBwindow);
}


static void
Raw_GiveHint(void)
{
    if (hint)
    {
        algbr((short) (hint >> 8), (short) (hint & 0xFF), 0);
        printf("Hint: %s\n", mvstr[0]);
    }
    else
        fputs("I have no idea.\n", stdout);
}


static void
Raw_SelectLevel(char *sx)
{
    /* FIXME: NO_SQUARES is nonsense here */
    char T[NO_SQUARES + 1], *p;

    strncpy(T, sx, NO_SQUARES);
    T[NO_SQUARES] = '\0';


    if (!xboard) {
        /* if line empty, ask for input */
        if (!T[0])
        {
            fputs("Enter #moves #minutes: ", stdout);
            fgets(T, NO_SQUARES + 1, stdin);
        }
    
        /* skip blackspace */
        for (p = T; *p == ' '; p++) ;
    
        /* could be moves or a fischer clock */
        if (*p == 'f')
        {
            /* its a fischer clock game */
            char *q;
            p++;
            TCminutes = (short)strtol(p, &q, 10);
            TCadd = (short)strtol(q, NULL, 10) *100;
            TCseconds = 0;
            TCmoves = 50;
        }
        else
        {
            /* regular game */
            char *q;
            TCadd = 0;
            TCmoves = (short)strtol(p, &q, 10);
            TCminutes = (short)strtol(q, &q, 10);
    
            if (*q == ':')
                TCseconds = (short)strtol(q + 1, (char **) NULL, 10);
            else
                TCseconds = 0;
    
    #ifdef OPERATORTIME
            fputs("Operator time (hundredths) = ", stdout);
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
    } else {
        int min, sec=0, inc, mps;
        /* parse regular "level MPS TC INC" command of WB protocol */
        if (sscanf(sx, "%d %d %d", &mps, &min, &inc) != 3)
            sscanf(sx, "%d %d:%d %d", &mps, &min, &sec, &inc);
        TCminutes = min; TCseconds = sec;
        TCadd = inc*100; TCmoves = mps ? mps : 50;
        MaxResponseTime = 0; TCflag = true;
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


static void
Raw_ChangeSearchDepth(char *sx)
{
    char buf[80+1];
    strncpy(buf, sx, 80); buf[80] = '\0';
    /* if line empty, ask for input */
    if (!buf[0]) {
        printf("depth = ");
        fgets(buf, 80+1, stdin);
    }
    sscanf(buf, "%hd", &MaxSearchDepth);
    TCflag = !(MaxSearchDepth > 0);
}


static void
Raw_ChangeHashDepth(void)
{
    printf("hashdepth = ");
    scanf("%hd", &HashDepth);
    printf("MoveLimit = ");
    scanf("%hd", &HashMoveLimit);
}


static void
Raw_SetContempt(void)
{
    printf("contempt = ");
    scanf("%hd", &contempt);
}


static void
Raw_ChangeXwindow(void)
{
    printf("xwndw = ");
    scanf("%hd", &xwndw);
}


/*
 * Raw_ShowPostnValue(short sq)
 * must have called ExaminePosition() first
 */
static void
Raw_ShowPostnValue(short sq)
{
    (void) ScorePosition(color[sq]);

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


static void
Raw_DoDebug(void)
{
    short c, p, sq, tp, tc, tsq, score, j, k;
    char s[40];

    ExaminePosition(opponent);
    Raw_ShowMessage("Enter piece: ");
    scanf("%s", s);
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
    printf("S%d m%d ps%d gt%c m%d ps%d gt%c", score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);
}


static void
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


static void
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
    printf("S%d m%d ps%d gt%c m%d ps%d gt%c", score,
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);
    printf("\nhung black %d hung white %d\n", hung[black], hung[white]);
}


static void
Raw_PollForInput(void)
{
#ifdef WIN32
    DWORD cnt;
    if (!PeekNamedPipe(GetStdHandle(STD_INPUT_HANDLE), NULL, 0, NULL, &cnt, NULL))
        cnt = 1;
#else
    static struct pollfd pollfds[1] = { [0] = { .fd = STDIN_FILENO,
                                                .events = POLLIN } };
    int cnt = poll(pollfds, sizeof(pollfds)/sizeof(pollfds[0]), 0);
    if (cnt < 0) {
        perror("polling standard input");
        Raw_ExitShogi();
    }
#endif
    if (cnt) { /* if anything to read, or error occured */
        if (!flag.timeout)
            flag.back = true; /* previous: flag.timeout = true; */
        flag.bothsides = false;
    }
}

struct display raw_display =
{
    .ChangeAlphaWindow    = Raw_ChangeAlphaWindow,
    .ChangeBetaWindow     = Raw_ChangeBetaWindow,
    .ChangeHashDepth      = Raw_ChangeHashDepth,
    .ChangeSearchDepth    = Raw_ChangeSearchDepth,
    .ChangeXwindow        = Raw_ChangeXwindow,
    .ClearScreen          = Raw_ClearScreen,
    .DoDebug              = Raw_DoDebug,
    .DoTable              = Raw_DoTable,
    .EditBoard            = Raw_EditBoard,
    .ExitShogi            = Raw_ExitShogi,
    .GiveHint             = Raw_GiveHint,
    .Initialize           = Raw_Initialize,
    .ShowNodeCnt          = Raw_ShowNodeCnt,
    .OutputMove           = Raw_OutputMove,
    .PollForInput         = Raw_PollForInput,
    .SetContempt          = Raw_SetContempt,
    .SearchStartStuff     = Raw_SearchStartStuff,
    .SelectLevel          = Raw_SelectLevel,
    .ShowCurrentMove      = Raw_ShowCurrentMove,
    .ShowDepth            = Raw_ShowDepth,
    .ShowGameType         = Raw_ShowGameType,
    .ShowLine             = Raw_ShowLine,
    .ShowMessage          = Raw_ShowMessage,
    .AlwaysShowMessage    = Raw_AlwaysShowMessage,
    .Printf               = Raw_Printf,
    .doRequestInputString = Raw_doRequestInputString,
    .GetString            = Raw_GetString,
    .SetupBoard           = Raw_SetupBoard,
    .ShowPatternCount     = Raw_ShowPatternCount,
    .ShowPostnValue       = Raw_ShowPostnValue,
    .ShowPostnValues      = Raw_ShowPostnValues,
    .ShowPrompt           = Raw_ShowPrompt,
    .ShowResponseTime     = Raw_ShowResponseTime,
    .ShowResults          = Raw_ShowResults,
    .ShowSidetoMove       = Raw_ShowSidetoMove,
    .ShowStage            = Raw_ShowStage,
    .TerminateSearch      = Raw_TerminateSearch,
    .UpdateClocks         = Raw_UpdateClocks,
    .UpdateDisplay        = Raw_UpdateDisplay,
    .help                 = Raw_help,
};
