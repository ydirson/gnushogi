/*
 * main.c - C source for GNU SHOGI based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback (GNU Chess)
 * Copyright (c) 1992 Free Software Foundation 
 * Copyright (c) 1993 Matthias Mutz (GNU Shogi)
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


#if !defined NO_MAIN

#include "version.h"
#include "gnushogi.h"

#endif

#include <signal.h>

#if defined THINK_C
#include <console.h>
#include <time.h>
#endif

#if !defined K32SEGMENTS
#include "main_data1.c"
#include "main_data2.c"
#endif


#if defined HASGETTIMEOFDAY && defined THINK_C

#define USEC_PER_CLOCK (1000000 / CLOCKS_PER_SEC)

int gettimeofday(struct timeval *tp, void *tzp)
{ 
   long clock_count = (long)clock();
   tp->tv_sec = clock_count / CLOCKS_PER_SEC;
   tp->tv_usec = (clock_count % CLOCKS_PER_SEC) * USEC_PER_CLOCK;
   return(0);
}

#endif

/*
 * In a networked enviroment gnuchess might be compiled on different hosts
 * with different random number generators, that is not acceptable if they
 * are going to share the same transposition table.
 */
unsigned long int next = 1;

unsigned int
urand (void)
{
  next *= 1103515245;
  next += 12345;
  return ((unsigned int) (next >> 16) & 0xFFFF);
}

void
gsrand (unsigned int seed)
{
  next = seed;
}

#if ttblsz
struct hashentry huge *ttable[2];
unsigned int ttblsize;
#endif
#ifdef BINBOOK
extern char *binbookfile;
#endif
extern char *bookfile;

unsigned long hashkey, hashbd;
struct hashval hashcode[2][NO_PIECES][NO_SQUARES+(2*NO_PIECES)];

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
short svalue[NO_SQUARES];
struct flags flag;

short opponent, computer, WAwindow, WBwindow, BAwindow, BBwindow, dither,
  BADscore;
long ResponseTime, ExtraTime, MaxResponseTime, et, et0, time0, ft;
long GenCnt, NodeCnt, ETnodes, EvalNodes, HashCnt, HashAdd, FHashCnt, FHashAdd,
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
const short otherside[3] =
{white, black, neutral};
unsigned short hint;
short int TOflag;		/* force search re-init if we backup search */

unsigned short killr0[MAXDEPTH], killr1[MAXDEPTH];
unsigned short killr2[MAXDEPTH], killr3[MAXDEPTH];
unsigned short PV, SwagHt, Swag0, Swag1, Swag2, Swag3, Swag4, sidebit;

const small_short sweep[NO_PIECES] =
{false, false, true,  false, false, false, true, true,
        false, false, false, false, true,  true,  false };
small_short HasPiece[2][NO_PIECES]; 
const short kingP[3] =
{4, 76, 0};
const short value[NO_PIECES] =
{0, valueP,  valueL,  valueN,  valueS,  valueG,  valueB,  valueR,
    valuePp, valueLp, valueNp, valueSp, valueBp, valueRp, valueK}; 
const small_short relative_value[NO_PIECES] =
{0, 1,       3,       4,       7,       9,       10,      12, 
    2,       5,       6,       8,       11,      13,      14};
const long control[NO_PIECES] =
{0, ctlP, ctlL, ctlN, ctlS, ctlG, ctlB, ctlR,
    ctlPp, ctlLp, ctlNp, ctlSp, ctlBp, ctlRp, ctlK };

short stage, stage2;

FILE *hashfile;

unsigned int starttime;
short int ahead = true, hash = true;

 

#if defined XSHOGI
void
TerminateChess (int sig)
{
  ExitChess ();
}

#endif



int timeopp[MINGAMEIN], timecomp[MINGAMEIN];
int compptr, oppptr;
inline void
TimeCalc ()
{
/* adjust number of moves remaining in gamein games */
  int increment = 0;
  int topsum = 0;
  int tcompsum = 0;
  int me,him;
  int i;
/* dont do anything til you have enough numbers */
  if (GameCnt < (MINGAMEIN * 2)) return;
/* calculate average time in sec for last MINGAMEIN moves */
  for (i = 0; i < MINGAMEIN; i++)
    {
      tcompsum += timecomp[i];
      topsum += timeopp[i];
    }
  topsum /= (100 * MINGAMEIN);
  tcompsum /= (100 * MINGAMEIN);
/* if I have less time than opponent add another move */
	me = TimeControl.clock[computer]/100; 
	him = TimeControl.clock[opponent]/100;
	if(me < him) increment += 2;
	if((him - me) > 60 || (me<him && me < 120))increment++;
/* if I am losing more time with each move add another */
  /*if ( !((me - him) > 60) && tcompsum > topsum) increment++;*/
  if ( tcompsum > topsum) increment +=2;
/* but dont let moves go below MINMOVES */
  else if (TimeControl.moves[computer] < MINMOVES && !increment) increment++;
/* if I am doing really well use more time per move */
  else if (me > him && tcompsum < topsum) increment = -1;
  TimeControl.moves[computer] += increment;
}




#if !defined NO_MAIN


/* hmm.... shouldn`t main be moved to the interface routines */
int
main (int argc, char **argv)
{
  long l;
  int  n;

#ifdef THINK_C
  console_options.ncols = 100;
  cshow(stdout);
#endif

  l = (long)sizeof(struct hashentry);
  n = (int)((l * (ttblsz + rehash) * 2) / 1000);
  printf("ttable:\t\t%d\tkByte\t[hashentry:%ld * (ttblsz:%d + rehash:%d) * 2]\n",
     n,l,ttblsz,rehash);

#if defined CACHE
  l = (long)sizeof(struct etable);
  n = (int)((l * (size_t)ETABLE) / 1000);
#else
  l = n = 0;
#endif
  printf("etab:\t\t%d\tkByte\t[etable:%ld ETABLE:%d]\n",n,l,ETABLE);

  l = (long)sizeof(struct leaf);
  n = (int)(sizeof(Tree) / 1000);
  printf("Tree:\t\t%d\tkByte\t[leaf:%ld * TREE:%d]\n",n,l,TREE);

#if defined HISTORY
  n = (int)(sizeof_history / 1000);
#else
  n = 0;
#endif
  printf("history:\t%d\tkByte\n",n);

  l = (long)sizeof(next_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("nextpos:\t%d\tkByte\t[next_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);

  l = (long)sizeof(next_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("nextdir:\t%d\tkByte\t[next_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);

  n = (int)(sizeof(distdata) / 1000);
  printf("distdata:\t%d\tkByte\n",n);

  l = (long)sizeof(next_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("ptype_distdata:\t%d\tkByte\t[next_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);

  l = (long)sizeof(hashcode);
  n = (int)(l / 1000);
  printf("hashcode:\t%d\tkByte\t[hashval:%ld]\n",
    n,(long)sizeof(struct hashval));

}


#endif /* NO_MAIN */

