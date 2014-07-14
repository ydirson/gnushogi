/*
 * FILE: init.c
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

#if defined HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#include <signal.h>

#include "pattern.h"

/****************************************
 *     A variety of global flags.
 ****************************************/

/*
 * If hard_time_limit is nonzero, exceeding the time limit means
 * losing the game.
 */

short hard_time_limit = 1;
#ifdef LIST_ON_EXIT
short nolist          = 0;  /* List the game after exit. */
#else
short nolist          = 1;  /* Don't list the game after exit. */
#endif

/*
 * The default display type can be DISPLAY_RAW, DISPLAY_CURSES,
 * or DISPLAY_X; the default is DISPLAY_X to make life easier for xshogi.
 */

display_t display_type = DISPLAY_X;

/* .... MOVE GENERATION VARIABLES AND INITIALIZATIONS .... */

#ifdef SAVE_NEXTPOS
const small_short psweep[NO_PTYPE_PIECES] =
{
    false,
#ifndef MINISHOGI
    true, false,
#endif
    false, false, true, true,
    true, true, false, false,
#ifndef MINISHOGI
    true, false,
#endif
    false, false
};
#endif

const small_short sweep[NO_PIECES] =
{
    false, false,
#ifndef MINISHOGI
    true, false,
#endif
    false, false, true, true,
    false,
#ifndef MINISHOGI
    false, false,
#endif
    false, true, true, false
};


#ifdef SAVE_DISTDATA
short
distance(short a, short b)
{
    return (short)computed_distance(a, b);
}
#else
short
distance(short a, short b)
{
    return (use_distdata
            ? (short)(*distdata)[(int)a][(int)b]
            : (short)computed_distance(a, b));
}
#endif


void
Initialize_dist(void)
{
    short a, b, d, di, ptyp;
#ifndef SAVE_DISTDATA
    for (a = 0; a < NO_SQUARES; a++)
    {
        for (b = 0; b < NO_SQUARES; b++)
        {
            d = abs(column(a) - column(b));
            di = abs(row(a) - row(b));
            (*distdata)[a][b] = (small_short)((d > di) ? d : di);
        }
    }
#endif
#ifndef SAVE_PTYPE_DISTDATA
    for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++)
    {
        for (a = 0; a < NO_SQUARES; a++)
            for (b = 0; b < NO_SQUARES; b++)
                (*ptype_distdata[ptyp])[a][b] = ptype_distance(ptyp, a, b);
    }
#endif
}


/*
 * nextpos[ptype][from-square], nextdir[ptype][from-square] gives vector
 * of positions reachable from from-square in ppos with ptype such that the
 * sequence
 *
 *     ppos = nextpos[ptype][from-square];
 *     pdir = nextdir[ptype][from-square];
 *     u = ppos[sq];
 *
 *     do
 *     {
 *         u = ppos[u];
 *
 *         if(color[u] != neutral)
 *             u = pdir[u];
 *     }
 *     while (sq != u);
 *
 * will generate the sequence of all squares reachable from sq.
 *
 * If the path is blocked u = pdir[sq] will generate the continuation of the
 * sequence in other directions.
 */


const small_short is_promoted[NO_PIECES] =
{
    false, false,
#ifndef MINISHOGI
    false, false,
#endif
    false, false, false, false,
    true,
#ifndef MINISHOGI
    true, true,
#endif
    true, true, true, false
};

/* data used to generate nextpos/nextdir */
#ifndef MINISHOGI
/* FIXME: use predefined constants ! */
#if !defined SAVE_NEXTPOS
static
#endif
const small_short direc[NO_PTYPE_PIECES][8] =
{
    {  11,   0,   0,   0,   0,   0,   0,   0 },   /*  0 ptype_pawn    */
    {  11,   0,   0,   0,   0,   0,   0,   0 },   /*  1 ptype_lance   */
    {  21,  23,   0,   0,   0,   0,   0,   0 },   /*  2 ptype_knight  */
    {  10,  11,  12, -12, -10,   0,   0,   0 },   /*  3 ptype_silver  */
    {  10,  11,  12,  -1,   1, -11,   0,   0 },   /*  4 ptype_gold    */
    {  10,  12, -12, -10,   0,   0,   0,   0 },   /*  5 ptype_bishop  */
    {  11,  -1,   1, -11,   0,   0,   0,   0 },   /*  6 ptype_rook    */
    {  10,  12, -12, -10,  11,  -1,   1, -11 },   /*  7 ptype_pbishop */
    {  11,  -1,   1, -11,  10,  12, -12, -10 },   /*  8 ptype_prook   */
    {  10,  11,  12,  -1,   1, -12, -11, -10 },   /*  9 ptype_king    */
    { -11,   0,   0,   0,   0,   0,   0,   0 },   /* 10 ptype_wpawn   */
    { -11,   0,   0,   0,   0,   0,   0,   0 },   /* 11 ptype_wlance  */
    { -21, -23,   0,   0,   0,   0,   0,   0 },   /* 12 ptype_wknight */
    { -10, -11, -12,  12,  10,   0,   0,   0 },   /* 13 ptype_wsilver */
    { -10, -11, -12,   1,  -1,  11,   0,   0 }    /* 14 ptype_wgold */
};
#else
#if !defined SAVE_NEXTPOS
static
#endif
const small_short direc[NO_PTYPE_PIECES][8] =
{
    {   7,   0,   0,   0,   0,   0,   0,   0 },   /*  0 ptype_pawn    */
    {   6,   7,   8,  -8,  -6,   0,   0,   0 },   /*  3 ptype_silver  */
    {   6,   7,   8,  -1,   1,  -7,   0,   0 },   /*  4 ptype_gold    */
    {   6,   8,  -8,  -6,   0,   0,   0,   0 },   /*  5 ptype_bishop  */
    {   7,  -1,   1,  -7,   0,   0,   0,   0 },   /*  6 ptype_rook    */
    {   6,   8,  -8,  -6,   7,  -1,   1,  -7 },   /*  7 ptype_pbishop */
    {   7,  -1,   1,  -7,   6,   8,  -8,  -6 },   /*  8 ptype_prook   */
    {   6,   7,   8,  -1,   1,  -8,  -7,  -6 },   /*  9 ptype_king    */
    {  -7,   0,   0,   0,   0,   0,   0,   0 },   /* 10 ptype_wpawn   */
    {  -6,  -7,  -8,   8,   6,   0,   0,   0 },   /* 13 ptype_wsilver */
    {  -6,  -7,  -8,   1,  -1,   7,   0,   0 }    /* 14 ptype_wgold */
};
#endif


small_short diagonal(short d)
{
  return (abs(d) == (NO_COLS+1) || abs(d) == (NO_COLS+3));
}


#ifndef MINISHOGI
/* FIXME */
static const small_short max_steps[NO_PTYPE_PIECES] =
{
    1, 8, 1, 1, 1, 8, 8, 8, 8, 1, 1, 8, 1, 1, 1
};
#else
static const small_short max_steps[NO_PTYPE_PIECES] =
{
    1, 1, 1, 4, 4, 4, 4, 1, 1, 1, 1
};
#endif

#ifndef MINISHOGI
const small_short nunmap[(NO_COLS + 2)*(NO_ROWS + 4)] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1,
    -1,  9, 10, 11, 12, 13, 14, 15, 16, 17, -1,
    -1, 18, 19, 20, 21, 22, 23, 24, 25, 26, -1,
    -1, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1,
    -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, -1,
    -1, 45, 46, 47, 48, 49, 50, 51, 52, 53, -1,
    -1, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1,
    -1, 63, 64, 65, 66, 67, 68, 69, 70, 71, -1,
    -1, 72, 73, 74, 75, 76, 77, 78, 79, 80, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


const small_short inunmap[NO_SQUARES] =
{
     23,  24,  25,  26,  27,  28,  29,  30,  31,
     34,  35,  36,  37,  38,  39,  40,  41,  42,
     45,  46,  47,  48,  49,  50,  51,  52,  53,
     56,  57,  58,  59,  60,  61,  62,  63,  64,
     67,  68,  69,  70,  71,  72,  73,  74,  75,
     78,  79,  80,  81,  82,  83,  84,  85,  86,
     89,  90,  91,  92,  93,  94,  95,  96,  97,
    100, 101, 102, 103, 104, 105, 106, 107, 108,
    111, 112, 113, 114, 115, 116, 117, 118, 119
};
#else
const small_short nunmap[(NO_COLS + 2)*(NO_ROWS + 2)] =
{
    -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4, -1,
    -1,  5,  6,  7,  8,  9, -1,
    -1, 10, 11, 12, 13, 14, -1,
    -1, 15, 16, 17, 18, 19, -1,
    -1, 20, 21, 22, 23, 24, -1,
    -1, -1, -1, -1, -1, -1, -1,
};


const small_short inunmap[NO_SQUARES] =
{
      8,   9,  10,  11,  12,
     15,  16,  17,  18,  19,
     22,  23,  24,  25,  26,
     29,  30,  31,  32,  33,
     36,  37,  38,  39,  40,
};
#endif

int InitFlag = false;


#if defined SAVE_NEXTPOS

short
next_direction(short ptyp, short *d, short sq)
{
    short delta, to, sfrom = inunmap[sq];

    do
    {
        (*d)++;
        if (*d >= 8)
            return sq;

        delta = direc[ptyp][*d];
        if (delta == 0)
            return sq;

        to = nunmap[sfrom + delta];
    }
    while (to < 0);

    return to;
}


short
next_position(short ptyp, short *d, short sq, short u)
{
    if (*d < 4 && psweep[ptyp])
    {
        short to = nunmap[inunmap[u] + direc[ptyp][*d]];

        if (to < 0)
            return next_direction(ptyp, d, sq);
        else
            return to;
    }
    else
    {
        return next_direction(ptyp, d, sq);
    }
}


short
first_direction(short ptyp, short *d, short sq)
{
    *d = -1;
    return next_direction(ptyp, d, sq);
}

#else

/*
 * This procedure pre-calculates all moves for every piece from every
 * square.  This data is stored in nextpos/nextdir and used later in the
 * move generation routines.
 */

void
Initialize_moves(void)
{
    short ptyp, po, p0, d, di, s, delta;
    unsigned char *ppos, *pdir;
    short dest[8][9];
    short sorted[9];
    short steps[8];
    short fpo = inunmap[0], tpo = 1 + inunmap[NO_SQUARES-1];

    /* pre-fill nextpos and nextdir with source position, probably so
     * (color[u] == neutral) stops to match once all moves have been seen
     */
    for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++)
    {
        for (po = 0; po < NO_SQUARES; po++)
        {
            for (p0 = 0; p0 < NO_SQUARES; p0++)
            {
                (*nextpos[ptyp])[po][p0] = (unsigned char)po;
                (*nextdir[ptyp])[po][p0] = (unsigned char)po;
            }
        }
    }

    for (ptyp = 0; ptyp < NO_PTYPE_PIECES; ptyp++)
    {
        for (po = fpo; po < tpo; po++)
        {
            if (nunmap[po] >= (small_short)0)
            {
                ppos = (*nextpos[ptyp])[nunmap[po]];
                pdir = (*nextdir[ptyp])[nunmap[po]];

                /* dest is a function of direction and steps */
                for (d = 0; d < 8; d++)
                {
                    dest[d][0] = nunmap[po];
                    delta = direc[ptyp][d];

                    if (delta != 0)
                    {
                        p0 = po;

                        for (s = 0; s < max_steps[ptyp]; s++)
                        {
                            p0 = p0 + delta;

                            /*
                             * break if (off board) or (promoted rooks
                             * wishes to move two steps diagonal) or
                             * (promoted bishops wishes to move two steps
                             * non-diagonal)
                             */
                            if ((nunmap[p0] < (small_short)0)
                                || ((ptyp == ptype_prook)
                                    && (s > 0)
                                    && diagonal(delta))
                                || ((ptyp == ptype_pbishop)
                                    && (s > 0)
                                    && !diagonal(delta)))
                                break;
                            else
                                dest[d][s] = nunmap[p0];
                        }
                    }
                    else
                    {
                        s = 0;
                    }

                    /*
                     * Sort dest in number of steps order; currently no sort
                     * is done due to compatibility with the move generation
                     * order in old gnuchess.
                     */

                    steps[d] = s;

                    for (di = d; s > 0 && di > 0; di--)
                    {
                        if (steps[sorted[di - 1]] == 0) /* should be: < s */
                            sorted[di] = sorted[di - 1];
                        else
                            break;
                    }

                    sorted[di] = d;
                }

                /*
                 * update nextpos/nextdir
                 */

                p0 = nunmap[po];
                pdir[p0] = (unsigned char)dest[sorted[0]][0];

                for (d = 0; d < 8; d++)
                {
                    for (s = 0; s < steps[sorted[d]]; s++)
                    {
                        ppos[p0] = (unsigned char)dest[sorted[d]][s];
                        p0 = dest[sorted[d]][s];

                        if (d < 7)
                            pdir[p0] = (unsigned char)dest[sorted[d + 1]][0];

                        /*
                         * else is already initialized
                         */
                    }
                }
            }
        }
    }
}

#endif



/*
 * Reset the board and other variables to start a new game.
 */

void
NewGame(void)
{
    short l, c, p, max_opening_sequence;
#ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;
#endif
    compptr = oppptr = 0;
    stage = 0;
    stage2 = -1;    /* the game is not yet started */
    flag.illegal = flag.mate = flag.quit
        = flag.reverse = flag.bothsides = flag.onemove = flag.force
        = false;
    flag.post &= xboard; /* xboard: do not alter post status on 'new' */
    flag.material = flag.coords = flag.hash = flag.easy
        = flag.beep = flag.rcptr
        = true;
    flag.stars  = flag.shade = flag.back = flag.musttimeout = false;
    flag.gamein = false;
    flag.rv     = true;

    mycnt1 = mycnt2 = 0;
    GenCnt = NodeCnt = et0 = dither =  XCmore = 0;
    znodes = ZNODES;
    WAwindow = WAWNDW;
    WBwindow = WBWNDW;
    BAwindow = BAWNDW;
    BBwindow = BBWNDW;
    xwndw = BXWNDW;

    if (!MaxSearchDepth)
        MaxSearchDepth = MAXDEPTH - 1;

    contempt = 0;
    GameCnt = 0;
    Game50 = 1;
    CptrFlag[0] = TesujiFlag[0] = false;
    hint = OPENING_HINT;
    ZeroRPT();
    GameType[0] = GameType[1] = UNKNOWN;
    Pscore[0] = Tscore[0] = (SCORE_LIMIT + 3000);
    opponent = player = black;
    computer = white;

    for (l = 0; l < TREE; l++)
        Tree[l].f = Tree[l].t = 0;

    gsrand((unsigned int) 1);

    if (!InitFlag)
    {
        for (c = black; c <= white; c++)
        {
            for (p = pawn; p <= king; p++)
            {
                for (l = 0; l < NO_SQUARES; l++)
                {
                    (*hashcode)[c][p][l].key
                         = (((unsigned long) urand()));
                    (*hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 16);
                    (*hashcode)[c][p][l].bd
                         = (((unsigned long) urand()));
                    (*hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 16);
#if SIZEOF_LONG == 8  /* 64-bit long i.e. 8 bytes */
                    (*hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 32);
                    (*hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 48);
                    (*hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 32);
                    (*hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 48);
#endif
                }
            }
        }

        for (c = black; c <= white; c++)
        {
            for (p = pawn; p <= king; p++)
            {
                for (l = 0; l < MAX_CAPTURED; l++)
                {
                    (*drop_hashcode)[c][p][l].key
                         = (((unsigned long) urand()));
                    (*drop_hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 16);
                    (*drop_hashcode)[c][p][l].bd
                         = (((unsigned long) urand()));
                    (*drop_hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 16);
#if SIZEOF_LONG == 8  /* 64-bit long i.e. 8 bytes */
                    (*drop_hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 32);
                    (*drop_hashcode)[c][p][l].key
                        += (((unsigned long) urand()) << 48);
                    (*drop_hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 32);
                    (*drop_hashcode)[c][p][l].bd
                        += (((unsigned long) urand()) << 48);
#endif
                }
            }
        }
    }

    for (l = 0; l < NO_SQUARES; l++)
    {
        board[l] = Stboard[l];
        color[l] = Stcolor[l];
        Mvboard[l] = 0;
    }

    ClearCaptured();
    dsp->ClearScreen();
    InitializeStats();

#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&tv, NULL);
    time0 = tv.tv_sec*100 + tv.tv_usec/10000;
#else
    time0 = time((long *) 0);
#endif

    /* resetting reference time */
    ElapsedTime(COMPUTE_AND_INIT_MODE);
    flag.regularstart = true;
    Book = BOOKFAIL;

    if (!InitFlag)
    {
        char sx[256];
        strcpy(sx, "level");

        if (TCflag)
            SetTimeControl();
        else if (MaxResponseTime == 0)
            dsp->SelectLevel(sx);

        dsp->UpdateDisplay(0, 0, 1, 0);
        GetOpenings();
        GetOpeningPatterns(&max_opening_sequence);

        InitFlag = true;
    }

#if ttblsz
    if (TTadd)
    {
        ZeroTTable();
        TTadd = 0;
    }
#endif /* ttblsz */

    hashbd = hashkey = 0;
    return;
}




int
InitMain(void)
{
    gsrand(starttime = ((unsigned int)time((long *)0)));    /* init urand */

#if ttblsz
    ttblsize = ttblsz;
    rehash = -1;
#endif /* ttblsz */

    if (Initialize_data() != 0)
        return 1;

    strcpy(ColorStr[0], "Black");
    strcpy(ColorStr[1], "White");

    XC = 0;
    MaxResponseTime = 0;

    if (XSHOGI)
    {
        TCmoves      = 40;
        TCminutes    = 5;
        TCseconds    = 0;
        TCadd        = 0;

        TCflag       = true;
        OperatorTime = 0;
    }
    else
    {
        TCflag       = false;
        OperatorTime = 0;
    }

    dsp->Initialize();
    Initialize_dist();
    Initialize_eval();
#if !defined SAVE_NEXTPOS
    Initialize_moves();
#endif

    NewGame();

    flag.easy = ahead;
    flag.hash = hash;

    if (xwin)
        xwndw = atoi(xwin);

#ifdef HASHFILE
    hashfile = NULL;
#endif

#if ttblsz
#ifdef HASHFILE
    hashfile = fopen(HASHFILE, RWA_ACC);

    if (hashfile)
    {
        fseek(hashfile, 0L, SEEK_END);
        filesz = ftell(hashfile) / sizeof(struct fileentry) - 1 - MAXrehash;
        hashmask = filesz >> 1;
        hashbase = hashmask + 1;
    }
#endif /* HASHFILE */
#endif /* ttblsz */

    savefile[0] = '\0';
    listfile[0] = '\0';

    return 0;
}


void
ExitMain(void)
{
#if ttblsz
#ifdef HASHFILE
    if (hashfile)
        fclose(hashfile);
#endif /* HASHFILE */
#endif /* ttblsz */

    dsp->ExitShogi();
}

