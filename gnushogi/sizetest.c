/*
 * FILE: sizetest.c
 *
 *     Display memory usage of GNU Shogi data structures.
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

#include <signal.h>

struct leaf  *Tree, *root;

short FROMsquare, TOsquare;

small_short ChkFlag[MAXDEPTH], CptrFlag[MAXDEPTH], TesujiFlag[MAXDEPTH];
short Pscore[MAXDEPTH], Tscore[MAXDEPTH];
small_short Pindex[NO_SQUARES];

short mtl[2], hung[2];
small_short PieceCnt[2];

struct GameRec  *GameList;

char ColorStr[2][10];

long znodes;


/*
 * In a networked enviroment gnushogi might be compiled on different hosts
 * with different random number generators; that is not acceptable if they
 * are going to share the same transposition table.
 */

unsigned long next = 1;

unsigned int
urand(void)
{
    next *= 1103515245;
    next += 12345;
    return ((unsigned int) (next >> 16) & 0xFFFF);
}


void
gsrand(unsigned int seed)
{
    next = seed;
}


#if ttblsz
struct hashentry *ttable[2];
unsigned int ttblsize;
#endif

#ifdef BINBOOK
extern char *binbookfile;
#endif

extern char *bookfile;

char savefile[128] = "";
char listfile[128] = "";

#if defined HISTORY
unsigned short *history;
#endif

short rpthash[2][256];
short TrPnt[MAXDEPTH];
small_short PieceList[2][NO_SQUARES];
small_short PawnCnt[2][NO_COLS];
small_short Captured[2][NO_PIECES];
small_short Mvboard[NO_SQUARES];

#if !defined SAVE_SVALUE
short svalue[NO_SQUARES];
#endif

struct flags flag;

short opponent, computer, WAwindow, WBwindow, BAwindow, BBwindow, dither,
    INCscore;
long ResponseTime, ExtraTime, MaxResponseTime, et, et0, time0, ft;
long GenCnt, NodeCnt, ETnodes, EvalNodes, HashCnt,
    HashAdd, FHashCnt, FHashAdd,
    HashCol, THashCol, filesz, hashmask, hashbase;
long replus, reminus;
short HashDepth = HASHDEPTH, HashMoveLimit = HASHMOVELIMIT;
short player, xwndw;
/*unsigned*/ short rehash; /* -1 is used as a flag --tpm */
short Sdepth, Game50, MaxSearchDepth;
short GameCnt = 0;
short contempt;
int Book;
struct TimeControlRec TimeControl;
int TCadd = 0;
short TCflag, TCmoves, TCminutes, TCseconds, OperatorTime;
short XCmoves[3], XCminutes[3], XCseconds[3], XC, XCmore;
const short otherside[3] = { white, black, neutral };
unsigned short hint;
short TOflag;       /* force search re-init if we backup search */

unsigned short killr0[MAXDEPTH], killr1[MAXDEPTH];
unsigned short killr2[MAXDEPTH], killr3[MAXDEPTH];
unsigned short PV, SwagHt, Swag0, Swag1, Swag2, Swag3, Swag4, sidebit;

small_short HasPiece[2][NO_PIECES];
const short kingP[3] = { 4, 76, 0 };

const long control[NO_PIECES] =
{ 0, ctlP, ctlL, ctlN, ctlS, ctlG, ctlB, ctlR,
  ctlPp, ctlLp, ctlNp, ctlSp, ctlBp, ctlRp, ctlK };

short stage, stage2;

FILE *hashfile;

unsigned int starttime;
short ahead = true, hash = true;


int timeopp[MINGAMEIN], timecomp[MINGAMEIN];
int compptr, oppptr;

void
TimeCalc()
{
    /* adjust number of moves remaining in gamein games */
    int increment = 0;
    int topsum = 0;
    int tcompsum = 0;
    int me, him;
    int i;

    /* don't do anything til you have enough numbers */
    if (GameCnt < (MINGAMEIN * 2))
        return;

    /* calculate average time in sec for last MINGAMEIN moves */
    for (i = 0; i < MINGAMEIN; i++)
    {
        tcompsum += timecomp[i];
        topsum += timeopp[i];
    }

    topsum /= (100 * MINGAMEIN);
    tcompsum /= (100 * MINGAMEIN);
    /* if I have less time than opponent add another move */
    me = TimeControl.clock[computer] / 100;
    him = TimeControl.clock[opponent] / 100;

    if (me < him)
        increment += 2;

    if (((him - me) > 60) || ((me < him) && (me < 120)))
        increment++;

    /* if I am losing more time with each move add another */
    /* if (!((me - him) > 60) && tcompsum > topsum) increment++; */

    if (tcompsum > topsum)
    {
        increment += 2;
    }
    else if ((TimeControl.moves[computer] < MINMOVES) && !increment)
    {
        /* but don't let moves go below MINMOVES */
        increment++;
    }
    else if ((me > him) && (tcompsum < topsum))
    {
        /* if I am doing really well use more time per move */
        increment = -1;
    }

    TimeControl.moves[computer] += increment;
}



int
main(int argc, char **argv)
{
    long l;
    int  n;

#if ttblsz
    l = (long)sizeof(struct hashentry);
    n = (int)((l * (ttblsz + rehash) * 2) / 1000);
    printf("ttable:\t\t%4d\tkByte\t[hashentry:%ld "
           "* (ttblsz:%d + rehash:%d) * 2]\n",
           n, l, ttblsz, rehash);
#endif

#if defined CACHE
    l = (long)sizeof(struct etable);
    n = (int)((l * (size_t)ETABLE) / 1000);
#else
    l = n = 0;
#endif

    printf("etab:\t\t%4d\tkByte\t[etable:%ld ETABLE:%d]\n",
           n, l, ETABLE);

    l = (long)sizeof(struct leaf);
    n = (int)(l * TREE / 1000);
    printf("Tree:\t\t%4d\tkByte\t[leaf:%ld * TREE:%d]\n",
           n, l, TREE);

#if defined HISTORY
    n = (int)(sizeof_history / 1000);
#else
    n = 0;
#endif

    printf("history:\t%4d\tkByte\t[unsigned short:%lu "
           "* HISTORY_SIZE:%ld]\n",
           n, sizeof(unsigned short), (long)HISTORY_SIZE);

#ifndef SAVE_NEXTPOS
    l = (long)sizeof(next_array);
    n = (int)((l * NO_PTYPE_PIECES) / 1000);

    printf("nextpos:\t%4d\tkByte\t[next_array:%ld "
           "* NO_PTYPE_PIECES:%d]\n",
           n, l, NO_PTYPE_PIECES);

    l = (long)sizeof(next_array);
    n = (int)((l * NO_PTYPE_PIECES) / 1000);
    printf("nextdir:\t%4d\tkByte\t[next_array:%ld "
           "* NO_PTYPE_PIECES:%d]\n",
           n, l, NO_PTYPE_PIECES);
#endif

#ifndef SAVE_DISTDATA
    n = (int)(sizeof(distdata_array) / 1000);
    printf("distdata:\t%4d\tkByte\n", n);
#endif

#ifndef SAVE_PTYPE_DISTDATA
    l = (long)sizeof(distdata_array);
    n = (int)((l * NO_PTYPE_PIECES) / 1000);
    printf("ptype_distdata:\t%4d\tkByte\t[distdata_array:%ld "
           "* NO_PTYPE_PIECES:%d]\n",
           n, l, NO_PTYPE_PIECES);
#endif

    l = (long)sizeof(hashcode_array);
    n = (int)(l / 1000);
    printf("hashcode:\t%4d\tkByte\t[hashval:%ld]\n",
           n, (long)sizeof(struct hashval));

    l = (long)sizeof(drop_hashcode_array);
    n = (int)(l / 1000);
    printf("drop_hashcode:\t%4d\tkByte\t[hashval:%ld]\n",
           n, (long)sizeof(struct hashval));

    l = (long)sizeof(value_array);
    n = (int)(l / 1000);
    printf("value:\t\t%4d\tkByte\n", n);

    l = (long)sizeof(fscore_array);
    n = (int)(l / 1000);
    printf("fscore:\t\t%4d\tkByte\n", n);

    return 0;
}

