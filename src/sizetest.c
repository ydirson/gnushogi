/*
 * sizetest.c - display memory usage of GNU SHOGI data
 *
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz (GNU Shogi)
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


#include "version.h"
#include "gnushogi.h"

#include <signal.h>

#if defined THINK_C
#include <console.h>
#include <time.h>
#endif

struct leaf far *Tree, *root;

short FROMsquare, TOsquare;

small_short ChkFlag[MAXDEPTH], CptrFlag[MAXDEPTH], TesujiFlag[MAXDEPTH];
short Pscore[MAXDEPTH], Tscore[MAXDEPTH];
small_short Pindex[NO_SQUARES];

short mtl[2], hung[2];
small_short PieceCnt[2];

struct GameRec far *GameList;

char ColorStr[2][10];

long znodes;


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
unsigned short far *history;
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

small_short HasPiece[2][NO_PIECES]; 
const short kingP[3] =
{4, 76, 0}; 

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

void
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





int
main (int argc, char **argv)
{
  long l;
  int  n;

#ifdef THINK_C
  console_options.ncols = 100;
  cshow(stdout);
#endif

#if ttblsz
  l = (long)sizeof(struct hashentry);
  n = (int)((l * (ttblsz + rehash) * 2) / 1000);
  printf("ttable:\t\t%4d\tkByte\t[hashentry:%ld * (ttblsz:%d + rehash:%d) * 2]\n",
     n,l,ttblsz,rehash);
#endif

#if defined CACHE
  l = (long)sizeof(struct etable);
  n = (int)((l * (size_t)ETABLE) / 1000);
#else
  l = n = 0;
#endif
  printf("etab:\t\t%4d\tkByte\t[etable:%ld ETABLE:%d]\n",n,l,ETABLE);
             
  l = (long)sizeof(struct leaf);
  n = (int)(l * TREE / 1000);
  printf("Tree:\t\t%4d\tkByte\t[leaf:%ld * TREE:%d]\n",n,l,TREE);

#if defined HISTORY
  n = (int)(sizeof_history / 1000);
#else
  n = 0;
#endif
  printf("history:\t%4d\tkByte\t[unsigned short:%ld * HISTORY_SIZE:%ld]\n",
    n,sizeof(unsigned short),(long)HISTORY_SIZE);

#ifdef SAVE_NEXTPOS
#else
  l = (long)sizeof(next_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("nextpos:\t%4d\tkByte\t[next_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);

  l = (long)sizeof(next_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("nextdir:\t%4d\tkByte\t[next_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);
#endif

#ifdef SAVE_DISTDATA
#else
  n = (int)(sizeof(distdata_array) / 1000);
  printf("distdata:\t%4d\tkByte\n",n);
#endif

#ifdef SAVE_PTYPE_DISTDATA
#else
  l = (long)sizeof(distdata_array);
  n = (int)((l * NO_PTYPE_PIECES) / 1000);
  printf("ptype_distdata:\t%4d\tkByte\t[distdata_array:%ld * NO_PTYPE_PIECES:%d]\n",
    n,l,NO_PTYPE_PIECES);
#endif

  l = (long)sizeof(hashcode_array);
  n = (int)(l / 1000);
  printf("hashcode:\t%4d\tkByte\t[hashval:%ld]\n",
    n,(long)sizeof(struct hashval));

  l = (long)sizeof(drop_hashcode_array);
  n = (int)(l / 1000);
  printf("drop_hashcode:\t%4d\tkByte\t[hashval:%ld]\n",
    n,(long)sizeof(struct hashval));
               
  l = (long)sizeof(value_array);
  n = (int)(l / 1000);
  printf("value:\t\t%4d\tkByte\n",n);

  l = (long)sizeof(fscore_array);
  n = (int)(l / 1000);
  printf("fscore:\t\t%4d\tkByte\n",n);

}

