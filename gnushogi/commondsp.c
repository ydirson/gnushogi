/*
 * FILE: commondsp.c
 *
 *     Common display routines for GNU Shogi.
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

/* request *snprintf prototypes */
#define _POSIX_C_SOURCE 200112L

#include "gnushogi.h"

#include <stdio.h>

#if defined HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#include <ctype.h>
#include <signal.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>

char mvstr[4][6];
int mycnt1, mycnt2;
static char *InPtr;
struct display *dsp = &raw_display;

short xboard = false;

#if defined(BOOKTEST)

void
movealgbr(short m, char *s)
{
    unsigned int f, t;
    short piece = 0, flag = 0;

    if (m == 0)
    {
        strcpy(s, "none");
        return;
    }

    f = (m >> 8) & 0x7f;
    t = m & 0xff;

    if (f > NO_SQUARES)
    {
        piece = f - NO_SQUARES;

        if (piece > NO_PIECES)
            piece -= NO_PIECES;

        flag = (dropmask | piece);
    }

    if (t & 0x80)
    {
        flag |= promote;
        t &= 0x7f;
    }

    if (flag & dropmask)
    {
        *s = pxx[piece];
        s++;
        *s = '*';
        s++;
        *s = COL_NAME(column(t));
        s++;
        *s = ROW_NAME(row(t));
        s++;
    }
    else
    {
        *s = COL_NAME(column(f));
        s++;
        *s = ROW_NAME(row(f));
        s++;
        *s = COL_NAME(column(t));
        s++;
        *s = ROW_NAME(row(t));
        s++;

        if (flag & promote)
        {
            *s = '+';
            s++;
        }
    }

    if (m & 0x8000)
    {
        *s = '?';
        s++;
    }

    *s = '\0';
}

#endif /* BOOKTEST */


/*
 * Generate move strings in different formats.
 *
 * INPUT:
 * - f					piece to be moved
 *   - 0 < f < NO_SQUARES				source square
 *   - NO_SQUARES <= f NO_SQUARES + 2*NO_PIECES		dropped piece modulo NO_PIECES
 * - t & 0x7f				target square
 * - t & 0x80				promotion flag
 * - flag
 *   - if flag & dropmask, piece type encoded in flag & pmask
 *
 * FIXME: that makes 2 ways to specify drops and promotions, why ?
 *
 * OUTPUT:
 * - GLOBAL mvstr
 */

void
algbr(short f, short t, short flags)
{
    if (f > NO_SQUARES)
    {
        short piece;

        piece = f - NO_SQUARES;

        if (f > (NO_SQUARES + NO_PIECES))
            piece -= NO_PIECES;

        flags = (dropmask | piece);
    }

    if ((t & 0x80) != 0)
    {
        flags |= promote;
        t &= 0x7f;
    }

    if ((f == t) && ((f != 0) || (t != 0)))
    {
        if (!XSHOGI) {
            dsp->Printf("error in algbr: FROM=TO=%d, flags=0x%4x\n", t, flags);
        }

        mvstr[0][0] = mvstr[1][0] = mvstr[2][0] = mvstr[3][0] = '\0';
    }
    else if ((flags & dropmask) != 0)
    {
        short piece = flags & pmask;

        mvstr[0][0] = pxx[piece];
        mvstr[0][1] = xboard ? '@' : '*';
        mvstr[0][2] = COL_NAME(column(t));
        mvstr[0][3] = ROW_NAME(row(t));
        mvstr[0][4] = '\0';
        strcpy(mvstr[1], mvstr[0]);
        strcpy(mvstr[2], mvstr[0]);
        strcpy(mvstr[3], mvstr[0]);
    }
    else if ((f != 0) || (t != 0))
    {
        /* pure coordinates notation */
        mvstr[0][0] = COL_NAME(column(f));
        mvstr[0][1] = ROW_NAME(row(f));
        mvstr[0][2] = COL_NAME(column(t));
        mvstr[0][3] = ROW_NAME(row(t));
        mvstr[0][4] = '\0';

        /* algebraic notation without disambiguation */
        mvstr[1][0] = pxx[board[f]];
        mvstr[1][1] = mvstr[0][2];    /* to column */
        mvstr[1][2] = mvstr[0][3];    /* to row */
        mvstr[1][3] = '\0';

        /* algebraic notation with row disambiguation */
        mvstr[2][0] = mvstr[1][0];
        mvstr[2][1] = mvstr[0][1];
        mvstr[2][2] = mvstr[0][2];    /* to column */
        mvstr[2][3] = mvstr[0][3];    /* to row */
        mvstr[2][4] = '\0';

        /* algebraic notation with column disambiguation */
        strcpy(mvstr[3], mvstr[2]);
        mvstr[3][1] = mvstr[0][0];

        if (flags & promote)
        {
            strcat(mvstr[0], "+");
            strcat(mvstr[1], "+");
            strcat(mvstr[2], "+");
            strcat(mvstr[3], "+");
        }
    }
    else
    {
        mvstr[0][0] = mvstr[1][0] = mvstr[2][0] = mvstr[3][0] = '\0';
    }
}


/*
 * Compare the string 's' to the list of legal moves available for the
 * opponent. If a match is found, make the move on the board.
 */

int
VerifyMove(char *s, VerifyMove_mode iop, unsigned short *mv)
{
    static short pnt, tempb, tempc, tempsf, tempst, cnt;
    static struct leaf xnode;
    struct leaf  *node;
    short i, l, local_flags;
    char buffer[60];

    /* check and remove quality flags */
    for (i = local_flags = 0, l = strlen(s); i < l; i++)
    {
        switch(s[i])
        {
        case '?':
            local_flags |= badmove;
            s[i] = '\0';
            break;

        case '!':
            local_flags |= goodmove;
            s[i] = '\0';
            break;

#ifdef EASY_OPENINGS
        case '~':
            local_flags |= difficult;
            s[i] = '\0';
            break;
#endif
        }
    }

    *mv = 0;

    if (iop == UNMAKE_MODE)
    {
        UnmakeMove(opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
        return false;
    }

    cnt = 0;

    if (iop == VERIFY_AND_MAKE_MODE)
        generate_move_flags = true;

    MoveList(opponent, 2, -1, true);
    generate_move_flags = false;
    pnt = TrPnt[2];
    if(s[4] == '=') s[4] = '\0'; /* deferral is implied */

    while (pnt < TrPnt[3])
    {
        node = &Tree[pnt++];
        algbr(node->f, node->t, (short) node->flags);

        if ((strcmp(s, mvstr[0]) == 0)
            || (strcmp(s, mvstr[1]) == 0)
            || (strcmp(s, mvstr[2]) == 0)
            || (strcmp(s, mvstr[3]) == 0))
        {
            cnt++;
            xnode = *node;
        }
    }

    if ((cnt == 1) && (xnode.score > DONTUSE))
    {
        short blocked;

        MakeMove(opponent, &xnode, &tempb, &tempc,
                 &tempsf, &tempst, &INCscore);

        if (SqAttacked(PieceList[opponent][0], computer, &blocked))
        {
            UnmakeMove(opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
            dsp->AlwaysShowMessage("Illegal move (in check): %s", s);
            return false;
        }
        else
        {
            if (iop == VERIFY_AND_TRY_MODE)
                return true;

            dsp->UpdateDisplay(xnode.f, xnode.t, 0, (short) xnode.flags);
            GameList[GameCnt].depth = GameList[GameCnt].score = 0;
            GameList[GameCnt].nodes = 0;
            ElapsedTime(COMPUTE_AND_INIT_MODE);
            GameList[GameCnt].time = (short) (et + 50)/100;
            GameList[GameCnt].flags |= local_flags;

            if (TCflag)
            {
                TimeControl.clock[opponent] -= et;
                timeopp[oppptr] = et;
                --TimeControl.moves[opponent];
            }

            *mv = (xnode.f << 8) | xnode.t;
            algbr(xnode.f, xnode.t, 0);

            /* in force mode, check for mate conditions */
            if (flag.force)
            {
                if (IsCheckmate(opponent ^ 1, -1, -1))
                {
                    char buf[20];

                    sprintf(buf, "%s mates!\n", ColorStr[opponent]);
                    dsp->ShowMessage(buf);
                    flag.mate = true;
                }
            }

            return true;
        }
    }

    dsp->AlwaysShowMessage("Illegal move (no match): %s", s);

    if (!XSHOGI && (cnt > 1))
    {
        sprintf(buffer, "Ambiguous Move %s!", s);
        dsp->ShowMessage(buffer);
    }

    return false;
}


static int
parser(char *f, short *fpiece)
{
    int c1, r1, c2, r2;
    short i, p = false;

    if (*f == '+')
        f++, p = true;

    for (i = 1, *fpiece = no_piece; i < NO_PIECES; i++)
    {
        if (f[0] == pxx[i] || f[0] == qxx[i])
        {
            *fpiece = (p ? promoted[i] : unpromoted[i]);
            break;
        }
    }

    if (f[1] == '*' || f[1] == '\'')
    {
        c2 = COL_NUM(f[2]);
        r2 = ROW_NUM(f[3]);

        return ((NO_SQUARES + *fpiece) << 8) | locn(r2, c2);
    }
    else
    {
        c1 = COL_NUM(f[1]);
        r1 = ROW_NUM(f[2]);
        c2 = COL_NUM(f[3]);
        r2 = ROW_NUM(f[4]);
        p = (f[5] == '+') ? 0x80 : 0;

        return (locn(r1, c1) << 8) | locn(r2, c2) | p;
    }
}


void
skip()
{
    while (*InPtr != ' ')
        InPtr++;

    while (*InPtr == ' ')
        InPtr++;
}


void
skipb()
{
    while (*InPtr == ' ')
        InPtr++;
}


void RequestInputString(char* buffer, unsigned bufsize)
{
    static char fmt[10];
    int ret = snprintf(fmt, sizeof(fmt), "%%%us", bufsize);
    if (ret < 0 ) {
        perror("RequestInputString snprintf");
        exit(1);
    }
    if (ret >= sizeof(fmt)) {
        fprintf(stderr,
                "Insufficient format-buffer size in %s for bufsize=%u\n",
                __FUNCTION__, bufsize);
        exit(1);
    }
    dsp->doRequestInputString(fmt, buffer);
}


static void
GetGame(void)
{
    FILE *fd;
    char fname[256], *p;
    int c, i, j;
    short sq;
    short side, isp;

    if (savefile[0]) {
        strcpy(fname, savefile);
    } else {
        dsp->ShowMessage("Enter file name: ");
        RequestInputString(fname, sizeof(fname)-1);
    }

    if (fname[0] == '\0')
        strcpy(fname, "shogi.000");

    if ((fd = fopen(fname, "r")) != NULL)
    {
        NewGame();
        fgets(fname, 256, fd);
        computer = opponent = black;
        InPtr = fname;
        skip();

        if (*InPtr == 'c')
            computer = white;
        else
            opponent = white;

        /* FIXME: write a skipn() function so that we can get
         * 3 skips by doing skipn(3) */
        skip();
        skip();
        skip();
        Game50 = atoi(InPtr);
        skip();
        flag.force = (*InPtr == 'f');
        fgets(fname, 256, fd); /* empty */
        fgets(fname, 256, fd);
        InPtr = &fname[11];
        skipb();
        TCflag = atoi(InPtr);
        skip();
        InPtr += 14;
        skipb();
        OperatorTime = atoi(InPtr);
        fgets(fname, 256, fd);
        InPtr = &fname[11];
        skipb();
        TimeControl.clock[black] = atol(InPtr);
        skip();
        skip();
        TimeControl.moves[black] = atoi(InPtr);
        fgets(fname, 256, fd);
        InPtr = &fname[11];
        skipb();
        TimeControl.clock[white] = atol(InPtr);
        skip();
        skip();
        TimeControl.moves[white] = atoi(InPtr);
        fgets(fname, 256, fd); /* empty */

        for (i = NO_ROWS - 1; i > -1; i--)
        {
            fgets(fname, 256, fd);
            p = &fname[2];
            InPtr = &fname[23];

            for (j = 0; j < NO_COLS; j++)
            {
                sq = i * NO_COLS + j;
                isp = (*p == '+');
                p++;

                if (*p == '-')
                {
                    board[sq] = no_piece;
                    color[sq] = neutral;
                }
                else
                {
                    for (c = 0; c < NO_PIECES; c++)
                    {
                        if (*p == pxx[c])
                        {
                            if (isp)
                                board[sq] = promoted[c];
                            else
                                board[sq] = unpromoted[c];

                            color[sq] = white;
                        }
                    }

                    for (c = 0; c < NO_PIECES; c++)
                    {
                        if (*p == qxx[c])
                        {
                            if (isp)
                                board[sq] = promoted[c];
                            else
                                board[sq] = unpromoted[c];

                            color[sq] = black;
                        }
                    }
                }

                p++;
                Mvboard[sq] = atoi(InPtr);
                skip();
            }
        }

        fgets(fname, 256, fd);  /* empty */
        fgets(fname, 256, fd);  /* 9 8 7 ... */
        fgets(fname, 256, fd);  /* empty */
        fgets(fname, 256, fd);  /* p l n ... */
        ClearCaptured();

        for (side = 0; side <= 1; side++)
        {
            fgets(fname, 256, fd);
            InPtr = fname;
            skip();
            skipb();
            Captured[side][pawn] = atoi(InPtr);
            skip();
#ifndef MINISHOGI
            Captured[side][lance] = atoi(InPtr);
            skip();
            Captured[side][knight] = atoi(InPtr);
            skip();
#endif
            Captured[side][silver] = atoi(InPtr);
            skip();
            Captured[side][gold] = atoi(InPtr);
            skip();
            Captured[side][bishop] = atoi(InPtr);
            skip();
            Captured[side][rook] = atoi(InPtr);
            skip();
            Captured[side][king] = atoi(InPtr);
        }

        GameCnt = 0;
        flag.regularstart = true;
        Book = BOOKFAIL;
        fgets(fname, 256, fd); /* empty */
        fgets(fname, 256, fd);   /*  move score ... */

        while (fgets(fname, 256, fd))
        {
            struct GameRec  *g;
            int side = computer;

            side = side ^ 1;
            ++GameCnt;
            InPtr = fname;
            skipb();
            g = &GameList[GameCnt];
            g->gmove = parser(InPtr, &g->fpiece);
            skip();
            g->score = atoi(InPtr);
            skip();
            g->depth = atoi(InPtr);
            skip();
            g->nodes = atol(InPtr);
            skip();
            g->time = atol(InPtr);
            skip();
            g->flags = c = atoi(InPtr);
            skip();
            g->hashkey = strtol(InPtr, (char **) NULL, 16);
            skip();
            g->hashbd = strtol(InPtr, (char **) NULL, 16);

            if (c & capture)
            {
                short i, piece;

                skip();

                for (piece = no_piece, i = 0; i < NO_PIECES; i++)
                {
                    if (pxx[i] == *InPtr)
                    {
                        piece = i;
                        break;
                    }
                }

                skip();
                g->color = ((*InPtr == 'W') ? white : black);
                skip();
                g->piece = (*InPtr == '+'
                            ? promoted[piece]
                            : unpromoted[piece]);
            }
            else
            {
                g->color = neutral;
                g->piece = no_piece;
            }
        }

        if (TimeControl.clock[black] > 0)
            TCflag = true;

        fclose(fd);
    }

    ZeroRPT();
    InitializeStats();
    dsp->UpdateDisplay(0, 0, 1, 0);
    Sdepth = 0;
    hint = 0;
}


static void
SaveGame(void)
{
    FILE *fd;
    char fname[256];
    short sq, i, c, f, t;
    char p;
    short side, piece;
    char empty[2] = "\n";

    if (savefile[0]) {
        strcpy(fname, savefile);
    } else {
        dsp->ShowMessage("Enter file name: ");
        RequestInputString(fname, sizeof(fname)-1);
    }

    if (fname[0] == '\0')
        strcpy(fname, "shogi.000");

    if ((fd = fopen(fname, "w")) != NULL)
    {
        char *b, *w;
        b = w = "Human   ";

        if (computer == white)
            w = "computer";

        if (computer == black)
            b = "computer";

        fprintf(fd, "White %s Black %s %d %s\n", w, b, Game50,
                flag.force ? "force" : "");
        fputs(empty, fd);
        fprintf(fd, "TimeControl %d Operator Time %d\n", TCflag, OperatorTime);
        fprintf(fd, "Black Clock %ld Moves %d\nWhite Clock %ld Moves %d\n",
                TimeControl.clock[black], TimeControl.moves[black],
                TimeControl.clock[white], TimeControl.moves[white]);
        fputs(empty, fd);

        for (i = NO_ROWS - 1; i > -1; i--)
        {
            fprintf(fd, "%c ", ROW_NAME(i));

            for (c = 0; c < NO_COLS; c++)
            {
                sq = i * NO_COLS + c;
                piece = board[sq];
                p = is_promoted[piece] ? '+' : ' ';
                fprintf(fd, "%c", p);

                switch(color[sq])
                {
                case white:
                    p = pxx[piece];
                    break;

                case black:
                    p = qxx[piece];
                    break;

                default:
                    p = '-';
                }

                fprintf(fd, "%c", p);
            }

            fprintf(fd, "  ");

            for (f = i * NO_COLS; f < i * NO_COLS + NO_ROWS; f++)
                fprintf(fd, " %d", Mvboard[f]);

            fprintf(fd, "\n");
        }

        fputs(empty, fd);
#ifndef MINISHOGI
        fprintf(fd, "   9 8 7 6 5 4 3 2 1\n");
        fputs(empty, fd);
        fprintf(fd, "   p  l  n  s  g  b  r  k\n");
#else
        fprintf(fd, "   5 4 3 2 1\n");
        fputs(empty, fd);
        fprintf(fd, "   p  s  g  b  r  k\n");
#endif

        for (side = 0; side <= 1; side++)
        {
            fprintf(fd, "%c", (side == black) ? 'B' : 'W');
            fprintf(fd, " %2d", Captured[side][pawn]);
#ifndef MINISHOGI
            fprintf(fd, " %2d", Captured[side][lance]);
            fprintf(fd, " %2d", Captured[side][knight]);
#endif
            fprintf(fd, " %2d", Captured[side][silver]);
            fprintf(fd, " %2d", Captured[side][gold]);
            fprintf(fd, " %2d", Captured[side][bishop]);
            fprintf(fd, " %2d", Captured[side][rook]);
            fprintf(fd, " %2d", Captured[side][king]);
            fprintf(fd, "\n");
        }

        fputs(empty, fd);
        fputs("  move   score depth   nodes   time flags                         capture\n", fd);

        for (i = 1; i <= GameCnt; i++)
        {
            struct GameRec  *g = &GameList[i];

            f = g->gmove >> 8;
            t = (g->gmove & 0xFF);
            algbr(f, t, g->flags);

            fprintf(fd, "%c%c%-5s %6d %5d %7ld %6ld %5d  0x%08lx 0x%08lx",
                    ((f > NO_SQUARES)
                     ? ' '
                     : (is_promoted[g->fpiece] ? '+' : ' ')),
                    pxx[g->fpiece],
                    ((f > NO_SQUARES) ? &mvstr[0][1] : mvstr[0]),
                    g->score, g->depth,
                    g->nodes, g->time, g->flags,
                    g->hashkey, g->hashbd);

            if (g->piece != no_piece)
            {
                fprintf(fd, "  %c %s %c\n",
                        pxx[g->piece], ColorStr[g->color],
                        (is_promoted[g->piece] ? '+' : ' '));
            }
            else
            {
                fprintf(fd, "\n");
            }
        }

        fclose(fd);

        dsp->ShowMessage("Game saved");
    }
    else
    {
        dsp->ShowMessage("Could not open file");
    }
}


/*
 * GetXGame, SaveXGame and BookGame used to only be defined if
 * xshogi wasn't defined -- wonder why?
 */

static void
GetXGame(void)
{
    FILE *fd;
    char fname[256], *p;
    int c, i, j;
    short sq;
    short side, isp;

    dsp->ShowMessage("Enter file name: ");
    RequestInputString(fname, sizeof(fname)-1);

    if (fname[0] == '\0')
        strcpy(fname, "xshogi.position.read");

    if ((fd = fopen(fname, "r")) != NULL)
    {
        NewGame();
        flag.regularstart = false;
        Book = false;

        /* xshogi position file ... */
        fgets(fname, 256, fd);

#ifdef notdef
        fname[6] = '\0';

        if (strcmp(fname, "xshogi"))
            return;
#endif

        /* -- empty line -- */
        fgets(fname, 256, fd);
        /* -- empty line -- */
        fgets(fname, 256, fd);

        for (i = NO_ROWS - 1; i > -1; i--)
        {
            fgets(fname, 256, fd);
            p = fname;

            for (j = 0; j < NO_COLS; j++)
            {
                sq = i * NO_COLS + j;
                isp = (*p == '+');
                p++;

                if (*p == '.')
                {
                    board[sq] = no_piece;
                    color[sq] = neutral;
                }
                else
                {
                    for (c = 0; c < NO_PIECES; c++)
                    {
                        if (*p == qxx[c])
                        {
                            if (isp)
                                board[sq] = promoted[c];
                            else
                                board[sq] = unpromoted[c];

                            color[sq] = white;
                        }
                    }

                    for (c = 0; c < NO_PIECES; c++)
                    {
                        if (*p == pxx[c])
                        {
                            if (isp)
                                board[sq] = promoted[c];
                            else
                                board[sq] = unpromoted[c];

                            color[sq] = black;
                        }
                    }
                }

                p++;
            }
        }

        ClearCaptured();

        for (side = 0; side <= 1; side++)
        {
            fgets(fname, 256, fd);
            InPtr = fname;
            Captured[side][pawn]   = atoi(InPtr);
            skip();
#ifndef MINISHOGI
            Captured[side][lance]  = atoi(InPtr);
            skip();
            Captured[side][knight] = atoi(InPtr);
            skip();
#endif
            Captured[side][silver] = atoi(InPtr);
            skip();
            Captured[side][gold]   = atoi(InPtr);
            skip();
            Captured[side][bishop] = atoi(InPtr);
            skip();
            Captured[side][rook]   = atoi(InPtr);
            skip();
            Captured[side][king]   = atoi(InPtr);
        }

        if (fgets(fname, 256, fd) != NULL && strncmp(fname, "white", 5) == 0)
        {
            computer = black;
            opponent = white;
            xwndw = BXWNDW;
        }

        fclose(fd);
    }

    Game50 = 1;
    ZeroRPT();
    InitializeStats();
    dsp->UpdateDisplay(0, 0, 1, 0);
    Sdepth = 0;
    hint = 0;
}


static void
SaveXGame(void)
{
    FILE *fd;
    char fname[256], *p;
    int i, j;
    short sq, piece;
    short side, isp;

    dsp->ShowMessage("Enter file name: ");
    RequestInputString(fname, sizeof(fname)-1);

    if (fname[0] == '\0')
        strcpy(fname, "xshogi.position.read");

    if ((fd = fopen(fname, "w")) != NULL)
    {
        fputs("# xshogi position file -- \n", fd);
        fputs("\n", fd);
        fputs("\n", fd);

        for (i = NO_ROWS - 1; i > -1; i--)
        {
            p = fname;

            for (j = 0; j < NO_COLS; j++)
            {
                sq = i * NO_COLS + j;
                piece = board[sq];
                isp = is_promoted[piece];
                *p = (isp ? '+' : ' ');
                p++;

                if (piece == no_piece)
                    *p = '.';
                else if (color[sq] == white)
                    *p = qxx[piece];
                else
                    *p = pxx[piece];

                p++;
            }

            *p++ = '\n';
            *p++ = '\0';
            fputs(fname, fd);
        }

        for (side = 0; side <= 1; side++)
        {
            sprintf(fname,
#ifndef MINISHOGI
		    "%d %d %d %d %d %d %d %d\n",
#else
		    "%d %d %d %d %d %d\n",
#endif
                    Captured[side][pawn],
#ifndef MINISHOGI
                    Captured[side][lance],
                    Captured[side][knight],
#endif
                    Captured[side][silver],
                    Captured[side][gold],
                    Captured[side][bishop],
                    Captured[side][rook],
                    Captured[side][king]);

            fputs(fname, fd);
        }

        if (computer == black)
            fputs("white to play\n", fd);
        else
            fputs("black to play\n", fd);

        fclose(fd);
    }
}


static void
BookSave(void)
{
    FILE *fd;
    char fname[256], sflags[4];
    short i, j, f, t;

    if (savefile[0]) {
        strcpy(fname, savefile);
    } else {
        /* Enter file name */
        dsp->ShowMessage("Enter file name: ");
        RequestInputString(fname, sizeof(fname)-1);
    }

    if (fname[0] == '\0') {
        dsp->AlwaysShowMessage("aborting book save");
        return;
    }

    if ((fd = fopen(fname, "a")) != NULL)
    {
        fprintf(fd, "#\n");

        for (i = 1; i <= GameCnt; i++)
        {
            struct GameRec  *g = &GameList[i];
            char mvnr[20], mvs[20];

            if (i % 2)
                sprintf(mvnr, "%d.", (i + 1)/2);
            else
                strcpy(mvnr, "");

            f = g->gmove >> 8;
            t = (g->gmove & 0xFF);
            algbr(f, t, g->flags);
            j = 0;

            /* determine move quality string */
            if (g->flags & goodmove)
                sflags[j++] = '!';

            if (g->flags & badmove)
                sflags[j++] = '?';

#ifdef EASY_OPENINGS
            if (g->flags & difficult)
                sflags[j++] = '~';
#endif

            sflags[j] = '\0';

            /* determine move string */
            if (f > NO_SQUARES)
            {
                sprintf(mvs, "%s%s ", &mvstr[0][1], sflags);
            }
            else
            {
                sprintf(mvs, "%c%c%c%c%c%s%s ",
                        mvstr[0][0], mvstr[0][1],
                        (g->flags & capture) ? 'x' : '-',
                        mvstr[0][2], mvstr[0][3],
                        (mvstr[0][4] == '+') ? "+" : "",
                        sflags);
            }

            fprintf(fd, "%s%s%c%s",
                    mvnr,
                    (f > NO_SQUARES
                     ? ""
                     : (is_promoted[g->fpiece] ? "+" : "")),
                    pxx[g->fpiece],
                    mvs);

            if ((i % 10) == 0)
                fprintf(fd, "\n");
        }

        if ((i % 10) != 1)
            fprintf(fd, "\n");

        fclose(fd);

        dsp->ShowMessage("Game saved");
    }
    else
    {
        dsp->ShowMessage("Could not open file");
    }
}


void
ListGame(void)
{
    FILE *fd;
    short i, f, t;
    time_t when;
    char fname[256], dbuf[256];

    if (listfile[0])
    {
        strcpy(fname, listfile);
    }
    else
    {
        time(&when);
        strncpy(dbuf, ctime(&when), 20);
        dbuf[7]  = '\0';
        dbuf[10] = '\0';
        dbuf[13] = '\0';
        dbuf[16] = '\0';
        dbuf[19] = '\0';

        /* use format "CL.Jan01-020304B" when
           date is Jan 1
           time is 02:03:04
           program played white */

        sprintf(fname, "CL.%s%s-%s%s%s%c",
                dbuf + 4, dbuf + 8, dbuf + 11, dbuf + 14,
                dbuf + 17, ColorStr[computer][0]);

        /* replace space padding with 0 */
        for (i = 0; fname[i] != '\0'; i++)
        {
            if (fname[i] == ' ')
                fname[i] = '0';
        }
    }

    fd = fopen(fname, "w");

    if (!fd)
    {
        printf("Open failure for file: %s", fname);
        exit(1);
    }

    fprintf(fd, "gnushogi %s game\n", PACKAGE_VERSION);
    fputs("         score  depth   nodes  time         ", fd);
    fputs("         score  depth   nodes  time\n", fd);

    for (i = 1; i <= GameCnt; i++)
    {
        f = GameList[i].gmove >> 8;
        t = (GameList[i].gmove & 0xFF);
        algbr(f, t, GameList[i].flags);

        if (GameList[i].flags & book)
        {
            fprintf(fd, "%c%c%-5s  %5d    Book%7ld %5ld",
                    ((f > NO_SQUARES)
                     ? ' '
                     : (is_promoted[GameList[i].fpiece] ? '+' : ' ')),
                    pxx[GameList[i].fpiece],
                    ((f > NO_SQUARES)
                     ? &mvstr[0][1] : mvstr[0]),
                    GameList[i].score,
                    GameList[i].nodes,
                    GameList[i].time);
        }
        else
        {
            fprintf(fd, "%c%c%-5s  %5d     %2d %7ld %5ld",
                    (f > NO_SQUARES
                     ? ' '
                     : (is_promoted[GameList[i].fpiece] ? '+' : ' ')),
                    pxx[GameList[i].fpiece],
                    (f > NO_SQUARES ? &mvstr[0][1] : mvstr[0]),
                    GameList[i].score, GameList[i].depth,
                    GameList[i].nodes, GameList[i].time);
        }

        if ((i % 2) == 0)
        {
            fprintf(fd, "\n");
        }
        else
        {
            fprintf(fd, "         ");
        }
    }

    fprintf(fd, "\n\n");

    if (GameList[GameCnt].flags & draw)
    {
        fprintf(fd, "Draw %s\n", DRAW);

        if (DRAW == DRAW_REPETITION)
        {
            short j;

            fprintf(fd, "repetition by positions ");

            for (j = GameCnt - 1; j >= Game50; j -= 2)
            {
                if (GameList[j].hashkey == hashkey &&
                    GameList[j].hashbd == hashbd)
                    fprintf(fd, "%d ", j);
            }

            fprintf(fd, "\n");
        }
    }
    else if (GameList[GameCnt].score == -(SCORE_LIMIT + 999))
    {
        fprintf(fd, "%s\n", ColorStr[player ]);
    }
    else if (GameList[GameCnt].score == (SCORE_LIMIT + 998))
    {
        fprintf(fd, "%s\n", ColorStr[player ^ 1]);
    }

    fclose(fd);
}


static void
FlagMove(char c)
{
    switch(c)
    {
    case '?' :
        GameList[GameCnt].flags |= badmove;
        break;

    case '!' :
        GameList[GameCnt].flags |= goodmove;
        break;

#ifdef EASY_OPENINGS
    case '~' :
        GameList[GameCnt].flags |= difficult;
        break;
#endif
    }
}


/*
 * Undo the most recent half-move.
 */

static void
Undo(void)
{
    short f, t;

    f = GameList[GameCnt].gmove >> 8;
    t = GameList[GameCnt].gmove & 0x7F;

    if (f > NO_SQUARES)
    {
        /* the move was a drop */
        Captured[color[t]][board[t]]++;
        board[t] = no_piece;
        color[t] = neutral;
        Mvboard[t]--;
    }
    else
    {
        if (GameList[GameCnt].flags & promote)
            board[f] = unpromoted[board[t]];
        else
            board[f] = board[t];

        color[f] = color[t];
        board[t] = GameList[GameCnt].piece;
        color[t] = GameList[GameCnt].color;

        if (board[t] != no_piece)
            Captured[color[f]][unpromoted[board[t]]]--;

        if (color[t] != neutral)
            Mvboard[t]--;

        Mvboard[f]--;
    }

    InitializeStats();

    if (TCflag && (TCmoves > 1))
        ++TimeControl.moves[color[f]];

    hashkey = GameList[GameCnt].hashkey;
    hashbd = GameList[GameCnt].hashbd;
    GameCnt--;
    computer = computer ^ 1;
    opponent = opponent ^ 1;
    flag.mate = false;
    Sdepth = 0;
    player = player ^ 1;
    dsp->ShowSidetoMove();
    dsp->UpdateDisplay(0, 0, 1, 0);

    if (flag.regularstart)
        Book = false;
}


static void
TestSpeed(void(*f)(short side, short ply,
                   short in_check, short blockable),
          unsigned j)
{
#ifdef test
    unsigned jj;
#endif

    unsigned i;
    long cnt, t1, t2;

#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    t1 = (tv.tv_sec*100 + (tv.tv_usec/10000));
#else
    t1 = time(0);
#endif

    for (i = 0; i < j; i++)
    {
        f(opponent, 2, -1, true);

#ifdef test
        for (jj = TrPnt[2]; i < TrPnt[3]; jj++)
        {
            if (!pick(jj, TrPnt[3] - 1))
                break;
        }
#endif
    }

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    t2 = (tv.tv_sec * 100 + (tv.tv_usec / 10000));
#else
    t2 = time(0);
#endif

    cnt = j * (TrPnt[3] - TrPnt[2]);

    if (t2 - t1)
        et = (t2 - t1);
    else
        et = 1;

    dsp->ShowNodeCnt(cnt);
}


static void
TestPSpeed(short(*f) (short side), unsigned j)
{
    unsigned i;
    long cnt, t1, t2;
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    t1 = (tv.tv_sec * 100 + (tv.tv_usec / 10000));
#else
    t1 = time(0);
#endif

    for (i = 0; i < j; i++)
        (void) f(opponent);

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    t2 = (tv.tv_sec * 100 + (tv.tv_usec / 10000));
#else
    t2 = time(0);
#endif

    cnt = j;

    if (t2 - t1)
        et = (t2 - t1);
    else
        et = 1;

    dsp->ShowNodeCnt(cnt);
}


static void
SetOppTime(char *time)
{
    int m, t;

    t = (int)strtol(time, &time, 10);

    if (*time == ':')
    {
        time++;
	/* FIXME: sec is parsed but ignored */
        (void)strtol(time, &time, 10);
    }

    m = (int)strtol(time, &time, 10);

    if (t)
        TimeControl.clock[opponent] = t;

    if (m)
        TimeControl.moves[opponent] = m;

    ElapsedTime(COMPUTE_AND_INIT_MODE);

    if (XSHOGI)
    {
        /* just to inform xshogi about availability of otime command */
        printf("otime %d %d\n", t, m);
    }
}


static void
SetMachineTime(char *time)
{
    int m, t;

    t = (int)strtol(time, &time, 10);

    if (*time == ':')
    {
        time++;
	/* FIXME: sec is parsed but ignored */
        (void)strtol(time, &time, 10);
    }

    m = (int)strtol(time, &time, 10);

    if (t)
        TimeControl.clock[computer] = t;

    if (m)
        TimeControl.moves[computer] = m;

    ElapsedTime(COMPUTE_AND_INIT_MODE);

    if (XSHOGI)
    {
        /* just to inform xshogi about availability of time command */
        printf("time %d %d\n", t, m);
    }
}


/* FIXME!  This is truly the function from hell! */

/*
 * Process the user's command. If easy mode is OFF (the computer is thinking
 * on opponents time) and the program is out of book, then make the 'hint'
 * move on the board and call SelectMove() to find a response. The user
 * terminates the search by entering a command. If the opponent does not make
 * the hint move, then set Sdepth to zero.
 */

void
InputCommand(char *command)
{
#ifdef QUIETBACKGROUND
    short have_shown_prompt = false;
#endif
    short ok, done, is_move = false;
    unsigned short mv;
    char s[80], sx[80];

    ok = flag.quit = done = false;
    player = opponent;

#if ttblsz
    if (TTadd > ttbllimit)
        ZeroTTable();
#endif

    if ((hint > 0) && !flag.easy && !flag.force)
    {
        /*
         * A hint move for the player is available.  Compute a move for the
         * opponent in background mode assuming that the hint move will be
         * selected by the player.
         */

        ft = time0; /* Save reference time for the player. */
        fflush(stdout);
        algbr((short) hint >> 8, (short) hint & 0xff, 0);
        strcpy(s, mvstr[0]);

        if (flag.post)
            dsp->GiveHint();

        /* do the hint move */
        if (VerifyMove(s, VERIFY_AND_TRY_MODE, &mv))
        {
            Sdepth = 0;

#ifdef QUIETBACKGROUND
            dsp->ShowPrompt();
            have_shown_prompt = true;
#endif /* QUIETBACKGROUND */

            /* Start computing a move until the search is interrupted. */

#ifdef INTERRUPT_TEST
            itime0 = 0;
#endif

            /* would love to put null move in here */
            /* after we make the hint move make a 2 ply search
             * with both plys our moves */
            /* think on opponents time */
            SelectMove(computer, BACKGROUND_MODE);

#ifdef INTERRUPT_TEST
            ElapsedTime(COMPUTE_INTERRUPT_MODE);

            if (itime0 == 0)
            {
                printf("searching not terminated by interrupt!\n");
            }
            else
            {
                printf("elapsed time from interrupt to "
                       "terminating search: %ld\n", it);
            }
#endif

            /* undo the hint and carry on */
            VerifyMove(s, UNMAKE_MODE, &mv);
            Sdepth = 0;
        }

        time0 = ft; /* Restore reference time for the player. */
    }

    while(!(ok || flag.quit || done))
    {
        player = opponent;

#ifdef QUIETBACKGROUND
        if (!have_shown_prompt)
        {
#endif /* QUIETBACKGROUND */

            dsp->ShowPrompt();

#ifdef QUIETBACKGROUND
        }

        have_shown_prompt = false;
#endif /* QUIETBACKGROUND */

        if (command == NULL) {
            int eof = dsp->GetString(sx);
            if (eof)
                dsp->ExitShogi();
        } else {
            strcpy(sx, command);
            done = true;
        }

        /* extract first word */
        if (sscanf(sx, "%s", s) < 1)
            continue;

        if (strcmp(s, "bd") == 0)   /* bd -- display board */
        {
            /* FIXME: Hack alert! */
            short old_xshogi = XSHOGI;

            if (old_xshogi)
                display_type = DISPLAY_RAW;

            dsp->ClearScreen();
            dsp->UpdateDisplay(0, 0, 1, 0);

            if (old_xshogi)
                display_type = DISPLAY_X;
        }
        else if (strcmp(s, "post") == 0)
        {
            flag.post = (xboard ? 1 : !flag.post);
        }
        else if (strcmp(s, "nopost") == 0)
        {
            flag.post = 0;
        }
#ifdef MINISHOGI
        else if (strcmp(s, "variant") == 0)
        {   /* only variant we play is minishogi */
            printf("setup (P.BR.S...G.+.++.+Kp.br.s...g.+.++.+k) 5x5+5_shogi rbsgk/4p/5/P4/KGSBR [-] w 0 1\n");
        }
#endif
        else if (strcmp(s, "alg") == 0 ||
                 strcmp(s, "accepted") == 0 || strcmp(s, "rejected") == 0 ||
                 strcmp(s, "variant") == 0 || strcmp(s, "computer") == 0)
        {
            /* noop */ ;
        }
        else if ((strcmp(s, "quit") == 0) ||
                 (strcmp(s, "exit") == 0))
        {
            flag.quit = true;
        }
        else if (strcmp(s, "xboard") == 0)
        {
            xboard = true;
            strcpy(ColorStr[0], "White");
            strcpy(ColorStr[1], "Black");
        }
        else if (strcmp(s, "protover") == 0)
        {
            printf("feature myname=\"GNU %s %s\" ",
#ifdef MINISHOGI
                   "MiniShogi",
#else
                   "Shogi",
#endif
                   PACKAGE_VERSION
                );
            printf("variants=\"%s\" ",
#ifdef MINISHOGI
                   "5x5+5_shogi,minishogi"
#else
                   "shogi"
#endif
                );
            printf("debug=1 setboard=0 sigint=0 done=1\n");
        }
        else if ((strcmp(s, "set") == 0) ||
                 (strcmp(s, "edit") == 0))
        {
            dsp->EditBoard();
        }
        else if (strcmp(s, "setup") == 0)
        {
            dsp->SetupBoard();
        }
        else if (strcmp(s, "first") == 0)
        {
            ok = true;
        }
        else if (strcmp(s, "go") == 0)
        {
            ok = true;
            flag.force = false;

            if (computer == black)
            {
                computer = white;
                opponent = black;
            }
            else
            {
                computer = black;
                opponent = white;
            }
        }
        else if (strcmp(s, "help") == 0)
        {
            dsp->help();
        }
        else if (strcmp(s, "material") == 0)
        {
            flag.material = !flag.material;
        }
        else if (strcmp(s, "force") == 0)
        {
            if (XSHOGI)
            {
                flag.force = true;
                flag.bothsides = false;
            }
            else
            {
                flag.force = !flag.force;
                flag.bothsides = false;
            }
        }
        else if (strcmp(s, "book") == 0)
        {
            Book = Book ? 0 : BOOKFAIL;
        }
        else if (strcmp(s, "new") == 0)
        {
            NewGame();
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (strcmp(s, "list") == 0)
        {
            ListGame();
        }
        else if (strcmp(s, "level") == 0)
        {
            dsp->SelectLevel(sx + strlen("level"));
        }
        else if (strcmp(s, "clock") == 0)
        {
            dsp->SelectLevel(sx + strlen("clock"));
        }
        else if (strcmp(s, "hash") == 0)
        {
            flag.hash = !flag.hash;
        }
        else if (strcmp(s, "gamein") == 0)
        {
            flag.gamein = !flag.gamein;
        }
        else if (strcmp(s, "beep") == 0)
        {
            flag.beep = !flag.beep;
        }
        else if (strcmp(s, "time") == 0)
        {
            SetMachineTime(sx + strlen("time"));
        }
        else if ((strcmp(s, "otime") == 0) ||
                 (xboard && (strcmp(s, "otim")) == 0))
        {
            SetOppTime(sx + strlen("otime"));
        }
        else if (strcmp(s, "Awindow") == 0)
        {
            dsp->ChangeAlphaWindow();
        }
        else if (strcmp(s, "Bwindow") == 0)
        {
            dsp->ChangeBetaWindow();
        }
        else if (strcmp(s, "rcptr") == 0)
        {
            flag.rcptr = !flag.rcptr;
        }
        else if (strcmp(s, "hint") == 0)
        {
            dsp->GiveHint();
        }
        else if (strcmp(s, "both") == 0)
        {
            flag.bothsides = !flag.bothsides;
            flag.force = false;
            Sdepth = 0;
            ElapsedTime(COMPUTE_AND_INIT_MODE);
            SelectMove(opponent, FOREGROUND_MODE);
            ok = true;
        }
        else if (strcmp(s, "reverse") == 0)
        {
            flag.reverse = !flag.reverse;
            dsp->ClearScreen();
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (strcmp(s, "switch") == 0)
        {
            computer = computer ^ 1;
            opponent = opponent ^ 1;
            xwndw = (computer == black) ? WXWNDW : BXWNDW;
            flag.force = false;
            Sdepth = 0;
            ok = true;
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (xboard ? strcmp(s, "white") == 0 : strcmp(s, "black") == 0)
        {
            computer = white;
            opponent = black;
            xwndw = WXWNDW;
            flag.force = false;
            Sdepth = 0;

            /*
             * ok = true; don't automatically start with black command
             */
        }
        else if (xboard ? strcmp(s, "black") == 0 : strcmp(s, "white") == 0)
        {
            computer = black;
            opponent = white;
            xwndw = BXWNDW;
            flag.force = false;
            Sdepth = 0;

            /*
             * ok = true; don't automatically start with white command
             */
        }
        else if (strcmp(s, "undo") == 0 && GameCnt > 0)
        {
            Undo();
        }
        else if (strcmp(s, "remove") == 0 && GameCnt > 1)
        {
            Undo();
            Undo();
        }
        /* CHECKME: are these next three correct? */
        else if (!XSHOGI && strcmp(s, "xget") == 0)
        {
            GetXGame();
        }
        else if (!XSHOGI && strcmp(s, "xsave") == 0)
        {
            SaveXGame();
        }
        else if (!XSHOGI && strcmp(s, "bsave") == 0)
        {
            BookSave();
        }
#ifdef EASY_OPENINGS
        else if ((strcmp(s, "?") == 0) ||
                 (strcmp(s, "!") == 0) ||
                 (strcmp(s, "~") == 0))
#else
        else if ((strcmp(s, "?") == 0) ||
                 (strcmp(s, "!") == 0))
#endif
        {
            FlagMove(*s);
        }
        else if (strcmp(s, "get") == 0)
        {
            GetGame();
        }
        else if (strcmp(s, "save") == 0)
        {
            SaveGame();
        }
        else if (strcmp(s, "depth") == 0)
        {
            dsp->ChangeSearchDepth(sx + strlen("depth"));
        }
        else if (strcmp(s, "sd") == 0)
        {
            dsp->ChangeSearchDepth(sx + strlen("sd"));
        }
        else if (strcmp(s, "hashdepth") == 0)
        {
            dsp->ChangeHashDepth();
        }
        else if (strcmp(s, "random") == 0)
        {
            dither = DITHER;
        }
        else if (strcmp(s, "hard") == 0)
        {
            flag.easy = false;
        }
        else if (strcmp(s, "easy") == 0)
        {
            flag.easy = !flag.easy;
        }
        else if (strcmp(s, "tsume") == 0)
        {
            flag.tsume = !flag.tsume;
        }
        else if (strcmp(s, "contempt") == 0)
        {
            dsp->SetContempt();
        }
        else if (strcmp(s, "xwndw") == 0)
        {
            dsp->ChangeXwindow();
        }
        else if (strcmp(s, "rv") == 0)
        {
            flag.rv = !flag.rv;
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (strcmp(s, "coords") == 0)
        {
            flag.coords = !flag.coords;
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (strcmp(s, "stars") == 0)
        {
            flag.stars = !flag.stars;
            dsp->UpdateDisplay(0, 0, 1, 0);
        }
        else if (!XSHOGI && strcmp(s, "moves") == 0)
        {
            short temp;

#if MAXDEPTH > 3
            if (GameCnt > 0)
            {
                extern unsigned short PrVar[MAXDEPTH];

                SwagHt = (GameList[GameCnt].gmove == PrVar[1])
                    ? PrVar[2] : 0;
            }
            else
#endif
                SwagHt = 0;

            dsp->ShowMessage("Testing MoveList Speed");
            temp = generate_move_flags;
            generate_move_flags = true;
            TestSpeed(MoveList, 1);
            generate_move_flags = temp;
            dsp->ShowMessage("Testing CaptureList Speed");
            TestSpeed(CaptureList, 1);
            dsp->ShowMessage("Testing Eval Speed");
            ExaminePosition(opponent);
            TestPSpeed(ScorePosition, 1);
        }
        else if (!XSHOGI && strcmp(s, "test") == 0)
        {
#ifdef SLOW_CPU
            dsp->ShowMessage("Testing MoveList Speed");
            TestSpeed(MoveList, 2000);
            dsp->ShowMessage("Testing CaptureList Speed");
            TestSpeed(CaptureList, 3000);
            dsp->ShowMessage("Testing Eval Speed");
            ExaminePosition(opponent);
            TestPSpeed(ScorePosition, 1500);
#else
            dsp->ShowMessage("Testing MoveList Speed");
            TestSpeed(MoveList, 20000);
            dsp->ShowMessage("Testing CaptureList Speed");
            TestSpeed(CaptureList, 30000);
            dsp->ShowMessage("Testing Eval Speed");
            ExaminePosition(opponent);
            TestPSpeed(ScorePosition, 15000);
#endif
        }
        else if (!XSHOGI && strcmp(s, "p") == 0)
        {
            dsp->ShowPostnValues();
        }
        else if (!XSHOGI && strcmp(s, "debug") == 0)
        {
            dsp->DoDebug();
        }
        else
        {
            if (flag.mate)
            {
                ok = true;
            }
            else if ((ok = VerifyMove(s, VERIFY_AND_MAKE_MODE, &mv)))
            {
                /* check for repetition */
                short rpt = repetition();

                if (rpt >= 3)
                {
                    DRAW = DRAW_REPETITION;
                    dsp->ShowMessage(DRAW);
                    GameList[GameCnt].flags |= draw;

                        flag.mate = true;
                }
                else
                {
                    is_move = true;
                }
            }

            Sdepth = 0;
        }
    }

    ElapsedTime(COMPUTE_AND_INIT_MODE);

    if (flag.force)
    {
        computer = opponent;
        opponent = computer ^ 1;
    }

    if (XSHOGI)
    {
        /* add remaining time in milliseconds for xshogi */
        if (is_move)
        {
            printf("%d. %s %ld\n",
                   ++mycnt2, s, TimeControl.clock[player] * 10);
        }
    }
}
