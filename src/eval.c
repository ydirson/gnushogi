/*
 * eval.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993 Matthias Mutz
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of GNU Shogi.
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
#include "gnushogi.h"

#ifdef USE_PATTERN
#include "pattern.h"
#endif


#ifdef CACHE
etable_field *etab[2];
#endif


const small_short Stboard[NO_SQUARES] =
{lance,knight,silver,gold,king,gold,silver,knight,lance,
 0, bishop, 0, 0, 0, 0, 0, rook, 0,
 pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, 
 0, rook, 0, 0, 0, 0, 0, bishop, 0,
 lance,knight,silver,gold,king,gold,silver,knight,lance};

const small_short Stcolor[NO_SQUARES] =
{black, black, black, black, black, black, black, black, black, 
 neutral, black, neutral, neutral, neutral, neutral, neutral, black, neutral,
 black, black, black, black, black, black, black, black, black, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 white, white, white, white, white, white, white, white, white,
 neutral, white, neutral, neutral, neutral, neutral, neutral, white, neutral,
 white, white, white, white, white, white, white, white, white};

small_short board[NO_SQUARES], color[NO_SQUARES];


int EADD = 0;
int EGET = 0;
int PUTVAR = false;


#ifdef DEBUG_EVAL
extern short debug_eval;
extern FILE *debug_eval_file;
#endif


static short in_opening_stage = true;
static short behind_in_material[2] = {false, false};


/* distance to enemy king */
static const EnemyKingDistanceBonus[9] =
{0, 20, 15, 8, 5, 3, 1, 0, 0};
 
/* distance to own king */
static const OwnKingDistanceBonus[9] =
{0, 10, 4, 1, 0, 0, 0, 0, 0};

/* distance to promotion zone */
static const PromotionZoneDistanceBonus[NO_ROWS] =
{0, 0, 0, 0, 4, 8, 10, 10, 10};

/* Bishop mobility bonus indexed by # reachable squares */
static const short BMBLTY[20] =
{0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 16, 16, 16, 16};

/* Rook mobility bonus indexed by # reachable squares */
static const short RMBLTY[20] =
{0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 14, 16, 16, 16, 16};

/* Lance mobility bonus indexed by # reachable squares */
static const short LMBLTY[8] =
{0, 0, 0, 0, 1, 2, 4, 8};


/* penalty for threats to king, indexed by number of such threats */
static const short KTHRT[36] =
{0, -8, -20, -36, -52, -68, -80, -80, -80, -80, -80, -80,
 -80, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80,
 -80, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80};


#if !defined USE_PATTERN

/* Pawn positional bonus (STATIC ROOK vs. STATIC ROOK) */
static const small_short PawnAdvanceSS[NO_SQUARES] =
{  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   3, -2, 15,  4,  6,  1,  6,  8,  3, 
   0, -5, -5,  0,  0,  0,  5, 10,  5, 
   0,  0,  0,  0,  0,  0,  0,  0, 10, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

#define PawnAdvanceSR PawnAdvanceSS

/* Pawn positional bonus (RANGING ROOK vs. STATIC ROOK) */
static const small_short PawnAdvanceRS[NO_SQUARES] =
{  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   3, -2, 15,  3,  5,  0, -2, -4,  3, 
   1,  0,  8,  3,  5,  2,  2, -2, -2, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

#define PawnAdvanceRR PawnAdvanceRS

/* Lance positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short LanceAdvanceSS[NO_SQUARES] =
{ 10,  5,  5,  5,  5,  5,  5,  5, 10,
   2,  4,  4,  4,  4,  4,  4,  4,  2,
   4,  1,  1,  1,  1,  1,  1,  1,  4,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, 
  -4, -4, -4, -4, -4, -4, -4, -4, -4, 
  -6, -6, -6, -6, -6, -6, -6, -6, -6,
  -8, -8, -8, -8, -8, -8, -8, -8, -8, 
  -9, -9, -9, -9, -9, -9, -9, -9, -9, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 }; /* impossible */

#define LanceAdvanceSR LanceAdvanceSS
#define LanceAdvanceRS LanceAdvanceSS
#define LanceAdvanceRR LanceAdvanceRS

/* Knight positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short KnightAdvanceSS[NO_SQUARES] =
{-9,  4,  4,  4, 4, 4, 4, 4,-9,
 -9,  2,  2,  2, 2, 2, 2, 2,-9,
 -9,  1,-10,  1, 1, 1, 1, 1,-9,
 -9,  0,  0,  0, 0, 0, 0, 0,-9,
 -9, -5,  0, -5, 0,-5, 0,-5,-9
 -9,  0,  0,  0, 0, 0, 0, 0,-9,
 -9, -9, -9, -9,-9,-9,-9,-9,-9,
  0,  0,  0,  0, 0, 0, 0, 0, 0,  /* impossible */
  0,  0,  0,  0, 0, 0, 0, 0, 0 };/* impossible */

#define KnightAdvanceSR KnightAdvanceSS
#define KnightAdvanceRS KnightAdvanceSS
#define KnightAdvanceRR KnightAdvanceRS

/* Silver positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short SilverAdvanceSS[NO_SQUARES] =
{  0,  0,  0,  0, -6,  0,  0,  0,  0,
   0,  8,  3,  4,  0,  3,  2,  0,  0, 
   0,  0, 10,  0,  0,  6,  6,  4,  0, 
   0,  0,  0,  0,  4,  0,  4,  5,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

/* Silver positional bonus (STATIC ROOK vs. RANGING ROOK) */
static small_short SilverAdvanceSR[NO_SQUARES] =
{  0,  0,  0,  0, -6,  0,  0,  0,  0,
   0, -4, -4,  4,  0,  5,  3,  0,  0, 
   0,  0, -4, -4,  2,  8,  6,  0,  0, 
   0,  0,  0,  3,  0,  0,  4,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

/* Silver positional bonus (RANGING ROOK vs. STATIC ROOK) */
static small_short SilverAdvanceRS[NO_SQUARES] =
{ -3,  0,  0,  0,  0,  0,  0,  0, -2,
  -3, -1,  3,  3, -1, -1,  8, -1, -2, 
  -2, -2,  0,  5, -1,  9, -2,  5, -3, 
   0,  0,  5,  2,  5,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

#define SilverAdvanceRR SilverAdvanceRS 

/* Gold positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short GoldAdvanceSS[NO_SQUARES] =
{  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0, 10,  0,  6,  0,  0,  0,  0, 
   0,  0,  0,  8,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

/* Gold positional bonus (STATIC ROOK vs. RANGING ROOK) */
static small_short GoldAdvanceSR[NO_SQUARES] =
{  0,  0,  0, 10,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  6,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

/* Gold positional bonus (RANGING ROOK vs. STATIC ROOK) */
static small_short GoldAdvanceRS[NO_SQUARES] =
{ -1,  0, -1,  0, -1,  0, -1, -1, -1,
  -1,  0,  4, -1,  4, -1,  8,  0, -1, 
  -2, -1,  0, -2,  0,  6,  0, -1, -2, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0 };

#define GoldAdvanceRR GoldAdvanceRS

/* Bishop positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short BishopAdvanceSS[NO_SQUARES] =
{  3,  0,  0,  0,  0,  0,  0,  0,  3,
   0,  4,  0,  8,  0,  0,  0,  3,  0, 
   0,  0,  6,  0,  0,  0,  4,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  2,  0,  3,  0,  0,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  3,  0,  0,  0,  3,  0,  0, 
   0,  3,  0,  0,  0,  0,  0,  3,  0, 
   3,  0,  0,  0,  0,  0,  0,  0,  3 };

/* Bishop positional bonus (STATIC ROOK vs. RANGING ROOK) */
static small_short BishopAdvanceSR[NO_SQUARES] =
{  3,  0,  0,  0,  0,  0,  0,  0,  3,
   0, 10,  0,  2,  0,  0,  0,  3,  0, 
   0,  0,  2,  0,  0,  0,  4,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  2,  0,  3,  0,  0,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  3,  0,  0,  0,  3,  0,  0, 
   0,  3,  0,  0,  0,  0,  0,  3,  0, 
   3,  0,  0,  0,  0,  0,  0,  0,  3 };

/* Bishop positional bonus (RANGING ROOK vs. STATIC ROOK) */
static small_short BishopAdvanceRS[NO_SQUARES] =
{  3,  0,  0,  0,  0,  0,  0,  0,  3,
   0,  4,  0,  2,  0,  0,  0,  3,  0, 
   0,  0,  8,  0,  0,  0,  4,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  2,  0,  3,  0,  0,  0,  0, 
   0,  2,  0,  3,  0,  3,  0,  2,  0,
   0,  0,  3,  0,  0,  0,  3,  0,  0, 
   0,  3,  0,  0,  0,  0,  0,  3,  0, 
   3,  0,  0,  0,  0,  0,  0,  0,  3 };

#define BishopAdvanceRR BishopAdvanceRS

/* Rook positional bonus (STATIC ROOK vs. STATIC ROOK) */
static small_short RookAdvanceSS[NO_SQUARES] =
{  2,  0,  0,  0,  0,  0,  0,  4,  2,
   2,  0,  0,  0,  0,  0,  0,  8,  2, 
  -2,  0,  0,  0,  0,  0,  0,  2, -2, 
  -2,  0,  0,  0,  0,  0,  0,  4, -2,
  -2,  0,  0,  0,  0,  0,  0,  2, -2, 
  -2,  0,  0,  0,  0,  0,  0,  2, -2,
   4,  4,  4,  4,  4,  4,  4,  4,  4, 
   6,  6,  6,  6,  6,  6,  6,  6,  6, 
   8,  8,  8,  8,  8,  8,  8,  8,  8 };

/* Rook positional bonus (RANING ROOK vs. STATIC ROOK) */
static small_short RookAdvanceRS[NO_SQUARES] =
{  2,  6,  6,  6,  6,  0,  0,  0,  2,
   2,  4,  4,  4,  4,  0,  0,  0,  2, 
  -2,  2,  3,  3,  3,  0,  0,  0, -2, 
  -2,  4,  1,  1,  1,  0,  0,  0, -2,
  -2,  0,  2,  2,  2,  0,  0,  0, -2, 
  -2,  0,  0,  0,  0,  0,  0,  0, -2,
   4,  4,  4,  4,  4,  4,  4,  4,  4, 
   6,  6,  6,  6,  6,  6,  6,  6,  6, 
   8,  8,  8,  8,  8,  8,  8,  8,  8 };

#define RookAdvanceSR RookAdvanceSS
#define RookAdvanceRR RookAdvanceRS

/* King positional bonus inopening stage (STATIC ROOK vs. STATIC ROOK) */
static const small_short KingAdvanceSS[NO_SQUARES] =
{  0, -4, 10,  0, -6, -8,-10,-12,-12,
  -2,  4,  6, -6, -8, -8,-10,-12,-12,
  -6, -6, -6, -8, -8,-10,-11,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12 };

/* King positional bonus inopening stage (STATIC ROOK vs. RANGING ROOK) */
static const small_short KingAdvanceSR[NO_SQUARES] =
{  5, -4, -4, -4, -6, -8,-10,-12,-12,
  -2,  4, 10,  6, -8, -8,-10,-12,-12,
  -6, -6, -6, -8, -8,-10,-11,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12 };

/* King positional bonus inopening stage (RANGING ROOK vs. STATIC ROOK) */
static const small_short KingAdvanceRS[NO_SQUARES] =
{-12,-12, -8, -6, -4, -8,-10,-12,-12,
 -12,-12, -8, -6, -8, 12, 16, 20, -8,
 -12,-12, -6, -8, -8,-10,  0,  0, -4,
 -12,-12,-12,-12,-12,-12,  0,  0,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12,
 -12,-12,-12,-12,-12,-12,-12,-12,-12 };
                                  
#define KingAdvanceRR KingAdvanceRS
                     
#endif /* !USE_PATTERN */


static small_short Kfield[2][NO_SQUARES];

char GameType[2] = { UNKNOWN, UNKNOWN };

#ifdef USE_PATTERN
OpeningPattern *attack_pattern[2], *castle_pattern[2];
#endif

#if defined USE_PATTERN

/*
static small_short Mpawn[2][NO_SQUARES]; 
static small_short Msilver[2][NO_SQUARES]; 
static small_short Mgold[2][NO_SQUARES]; 
static small_short Mking[2][NO_SQUARES];
static small_short Mlance[2][NO_SQUARES]; 
static small_short Mknight[2][NO_SQUARES]; 
static small_short Mbishop[2][NO_SQUARES]; 
static small_short Mrook[2][NO_SQUARES]; 
*/
static Mpiece_array Mpawn, Mlance, Mknight, Msilver, Mgold,
 		    Mbishop, Mrook, Mking;

Mpiece_array *Mpiece[NO_PIECES] =
  { NULL, &Mpawn, &Mlance, &Mknight, &Msilver, &Mgold, &Mbishop, &Mrook,
          &Mgold, &Mgold, &Mgold, &Mgold, &Mbishop, &Mrook, &Mking };

#else

static small_short Mpawn[2][NO_SQUARES]; 
static small_short Msilver[2][NO_SQUARES]; 
static small_short Mgold[2][NO_SQUARES]; 
static small_short Mking[2][NO_SQUARES];

static small_short *Mlance[2]; 
static small_short *Mknight[2]; 
static small_short *Mbishop[2]; 
static small_short *Mrook[2]; 

#endif

static short c1, c2;
static small_short *PC1, *PC2;

static long *atk1, *atk2;

static long int atak[2][NO_SQUARES];

#if defined USE_PATTERN

short ADVNCM[NO_PIECES];

#else

static short PADVNCM, SADVNCM, GADVNCM, KADVNCM;

#endif

static short R2NDCOL, EDRNK2B, PWEAKH, PAWNSHIELD;
static short NEDGE;
static short PMBLTY, BDCLOSED, PSTRONG, P2STRONG;

static short PawnBonus, BishopBonus, RookBonus;
static short KNIGHTPOST, KNIGHTSTRONG, BISHOPSTRONG, KATAK;
static short RHOPN, RHOPNX, BHCLSD, BHCLSDX, KHOPN, KHOPNX, KSFTY;

static short LHOPN, LHOPNX, LPROTECT, NDNGR, LDNGR;
static short SBEFOREG, BXCHG;
static short LXRAY, BRXRAY;
static short KINGED, KINGOD, PROMD;

static short KCASTLD, KMOVD, OPENOK, OPENWRONG;
static short ATAKD, HUNGP, HUNGX, PINVAL;

#ifdef USE_PATTERN
static short PATTACK, PCASTLE;
#endif


short pscore[2];



#if defined DEBUG8 || defined DEBUG_EVAL
                                               

void
debug_position (FILE *D)
{ 
  short r, c, l, side, piece;

  fprintf (D, "\n current board is\n\n");
  for (piece = pawn; piece <= king; piece++)
    if (c = Captured[white][piece]) 
	  fprintf(D, "%i%c ",c,pxx[piece]);
  fprintf (D, "\n");
  for (c = 0; c < NO_COLS; c++)
	fprintf (D, " %d", PawnCnt[white][c]); 
  fprintf (D, "\n\n");
  for (r = (NO_ROWS-1); r >= 0; r--)
    {
	for (c = 0; c < NO_COLS; c++)
	    {
	      l = locn (r, c);
	      if (color[l] == neutral)
		fprintf (D, " -");
	      else if (color[l] == black)
		fprintf (D, " %c", qxx[board[l]]);
	      else
		fprintf (D, " %c", pxx[board[l]]);
	    }
	fprintf (D, "\n");
    }
  fprintf (D, "\n");
  for (c = 0; c < NO_COLS; c++)
	fprintf (D, " %d", PawnCnt[black][c]); 
  fprintf (D, "\n");
  for (piece = pawn; piece <= king; piece++)
    if (c = Captured[black][piece]) 
	  fprintf(D, "%i%c ",c,pxx[piece]);
  fprintf(D, "\n");
}                                      


#if !defined SAVE_SVALUE

void
debug_svalue (FILE *D)
{ 
  short r, c;

  for (r = (NO_ROWS-1); r >= 0; r--)
    {
	for (c = 0; c < NO_COLS; c++)
	    {
	      short sq = (r * NO_COLS) + c;
	      fprintf(D,"%5d",svalue[sq]);
	    }
	fprintf (D, "\n");
    }
  fprintf (D, "\n");
}

#endif                                      

void
debug_table (FILE *D, small_short *table)
{ 
  short r, c;

  for (r = (NO_ROWS-1); r >= 0; r--)
    {
	for (c = 0; c < NO_COLS; c++)
	    {
	      short sq = (r * NO_COLS) + c;
	      fprintf(D,"%5d",table[sq]);
	    }
	fprintf (D, "\n");
    }
  fprintf (D, "\n");
}                                      

void
debug_ataks (FILE *D)
{              
  short side;
  for (side = black; side <= white; side++)
    {                 
	short l,c,i;         
	long *atk = atak[side];
	fprintf(D, "\n");
	for (l = NO_ROWS-1; l >= 0; l--) {
	  for (c = 0; c < NO_COLS; c++) {
	    short sq = (l * NO_COLS) + c;
	    long  v = atk[sq];
	    short n = (short)(v & CNT_MASK);
	    char s[20];
	    fprintf(D,"%2d",n);
	    strcpy(s,"");
	    if ( v & ctlP  ) strcat(s,"P"); 
	    if ( v & ctlPp ) strcat(s,"+P");
	    if ( v & ctlL  ) strcat(s,"L"); 
	    if ( v & ctlLp ) strcat(s,"+L"); 
	    if ( v & ctlN  ) strcat(s,"N"); 
	    if ( v & ctlNp ) strcat(s,"+N"); 
	    if ( v & ctlS  ) strcat(s,"S"); 
	    if ( v & ctlSp ) strcat(s,"+S"); 
	    if ( v & ctlG  ) strcat(s,"G"); 
	    if ( v & ctlB  ) strcat(s,"B"); 
	    if ( v & ctlBp ) strcat(s,"+B"); 
	    if ( v & ctlR  ) strcat(s,"R"); 
	    if ( v & ctlRp ) strcat(s,"+R"); 
	    if ( v & ctlK  ) strcat(s,"K");
	    fprintf(D,s);
	    for (i = strlen(s); i < 5; i++)
		fprintf(D," ");
	    fprintf(D," "); 
	  }	                          
	  fprintf(D,"\n");
	}
	fprintf(D, "\n");
    }
}

#endif



/* ............    POSITIONAL EVALUATION ROUTINES    ............ */

/*
 * Inputs are:
 * mtl[side]  - value of all material
 * hung[side] - count of hung pieces
 * Tscore[ply] - search tree score for ply ply
 * Pscore[ply] - positional score for ply ply
 * INCscore    - bonus score or penalty for certain moves
 * Sdepth - search goal depth
 * xwndw - evaluation window about alpha/beta
 * EWNDW - second evaluation window about alpha/beta
 * ChkFlag[ply]- checking piece at level ply or 0 if no check
 * TesujiFlag[ply]- 1 if tesuji move at level ply or 0 if no tesuji
 * PC1[column] - # of my pawns in this column
 * PC2[column] - # of opponents pawns in column
 * PieceCnt[side] - just what it says
 */



int
evaluate (register short int side,
	  register short int ply,
	  register short int alpha,
	  register short int beta,
	  short int INCscore,
	  short int *InChk)	/* output Check flag */

/*
 * Compute an estimate of the score by adding the positional score from the
 * previous ply to the material difference. If this score falls inside a
 * window which is 180 points wider than the alpha-beta window (or within a
 * 50 point window during quiescence search) call ScorePosition() to
 * determine a score, otherwise return the estimated score. 
 * "side" has to moved.
 */

{
    register short evflag, xside;
    short s, sq, blockable;

    xside = side ^ 1;
    s = -Pscore[ply - 1] + mtl[side] - mtl[xside] - INCscore;

#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf (debug_eval_file, "estimated score at ply %d:%d, -%d+%d-%d-%d\n",
	ply, s, Pscore[ply-1], mtl[side], mtl[xside], INCscore );
#endif

    hung[black] = hung[white] = 0;

    /* should we use the estimete or score the position */
     
#if !defined OLD_EVALUATE
    if ( ply == 1 ||
	(ply == Sdepth) ||
	(ply > Sdepth && (s >= (alpha - 30) && s <= (beta + 30))) ||
#else
    if ( ply <= Sdepth ||
	((ply == Sdepth + 1 || ply == (Sdepth + 2)) && (s > (alpha - xwndw) && s < (beta + xwndw))) ||
	(ply > (Sdepth + 2) && s >= (alpha - EWNDW) && s <= (beta + EWNDW)) ||
#endif
#ifdef CACHE
    	(CheckEETable (side)) )
#endif   

      { short sq;
	/* score the position */
#ifdef DEBUG_EVAL
        if ( debug_eval )
          fprintf (debug_eval_file, "do NOT use the estimete\n");
#endif
	ataks (side, atak[side]);
	if (Anyatak (side, sq=PieceList[xside][0]) && board[sq] == king) {
	  *InChk = (board[sq=PieceList[side][0]] == king) ? SqAtakd (sq, xside, &blockable) : false;
	  return ((SCORE_LIMIT+1001) - ply);
	}
	ataks (xside, atak[xside]);
	*InChk = (board[sq=PieceList[side][0]] == king) ? Anyatak (xside, sq) : false;
#ifndef BAREBONES 
	EvalNodes++;
#endif    
#if !defined OLD_EVALUATE
	if ( ply > 4 )       
#endif
	  PUTVAR=true;
	s = ScorePosition (side);
	PUTVAR = false;
      }
    else
      {
	/* use the estimate but look at check */
	short sq;
#ifdef DEBUG_EVAL
        if ( debug_eval )
          fprintf (debug_eval_file, "use the estimete\n");
#endif
	*InChk = (board[sq=PieceList[side][0]] == king) ? SqAtakd (sq, xside, &blockable) : false; 
	if ( SqAtakd (sq=PieceList[xside][0], side, &blockable) && board[sq] == king ) {
	  return ((SCORE_LIMIT+1001) - ply);
	}
      }

    Pscore[ply] = s - mtl[side] + mtl[xside];
    ChkFlag[ply - 1] = ((*InChk) ? Pindex[TOsquare] : 0);
    return (s);
}

inline
int
BRLscan (register short int sq, short int *mob)

/*
 * Find (promoted) Bishop, (promoted) Rook, and Lance mobility, XRAY attacks, and pins. 
 * Let BRL be the bishop, rook, or lance.
 * Let P be the first piece (no king and no pawn) in a direction and let Q be the second
 * piece in the same direction. If Q is an unprotected opponent's piece with 
 * bigger relative value than BRL, there is a pin if P is an opponent's piece and
 * there is an XRAY attack if P is an own piece.
 * Increment the hung[] array if a pin is found.
 */
{
#ifdef SAVE_NEXTPOS
    short d;
#else
    register unsigned char *ppos, *pdir;
#endif
    register short s, mobx;
    register short u, pin, ptyp;
    short piece, rvalue;
    small_short *Kf;
    mobx = s = 0;
    Kf = Kfield[c1];
    piece = board[sq];

    rvalue = relative_value[piece];
    ptyp = ptype[c1][unpromoted[piece]];
#ifdef SAVE_NEXTPOS
    u = first_direction(ptyp,&d,sq);
#else
    ppos = (*nextpos[ptyp])[sq];
    pdir = (*nextdir[ptyp])[sq];
    u = ppos[sq];                         
#endif
    pin = -1;			/* start new direction */
    do
      {
	  s += Kf[u];
#ifdef DEBUG_EVAL
	  if ( debug_eval && Kf[u] != 0 )
	    fprintf(debug_eval_file,"add %d for controlling square near enemy king\n",Kf[u]);
#endif
	  if (color[u] == neutral)
	    {
#ifdef SAVE_NEXTPOS
		short xu, dd = d;
		xu = next_position(ptyp,&d,sq,u);
		if ( xu == next_direction(ptyp,&dd,sq) )
		    pin = -1;	/* oops new direction */
		u = xu;
#else
		if (ppos[u] == pdir[u])
		    pin = -1;	/* oops new direction */
		u = ppos[u];
#endif
		mobx++;
	    }
	  else 
	    {   /* there is a piece in current direction */
		if (pin < 0)
		    {   /* it's the first piece in the current direction */
			if (piece == lance && color[u] == c1 )
			  {   
			    if ( board[u] == pawn )
			      {
				s += LPROTECT;
#ifdef DEBUG_EVAL
				if ( debug_eval )
				  fprintf(debug_eval_file,"adding %d for lance protection\n",LPROTECT);
#endif
  			      }
  			    else if ( in_opening_stage )
  			      {
				s -= 2*LPROTECT;
#ifdef DEBUG_EVAL
				if ( debug_eval )
				  fprintf(debug_eval_file,"adding %d for lance protection\n",-2*LPROTECT);
#endif
  			      }
			  }
			else if ( piece == bishop && board[u] == pawn )
			  {
			      if ( color[u] == c1 )
				{
				  s += 2*BHCLSD; 
#ifdef DEBUG_EVAL
				  if (debug_eval )
				    fprintf(debug_eval_file,"add %d for own pawn in bishops direction\n",2*BHCLSD);
#endif
				}
			      else
				{
				  s += 2*BHCLSDX;
#ifdef DEBUG_EVAL
				  if (debug_eval )
				    fprintf(debug_eval_file,"add %d for opponents pawn in bishops direction\n",2*BHCLSDX);
#endif
				}
			  }
			if (board[u] == pawn || board[u] == king)
#ifdef SAVE_NEXTPOS
			    u = next_direction(ptyp,&d,sq);
#else
			    u = pdir[u];
#endif
			else
			  {
#ifdef SAVE_NEXTPOS
			      short xu, dd = d;
			      xu = next_position(ptyp,&d,sq,u);
			      if ( xu != next_direction(ptyp,&dd,sq) )
				  pin = u;	/* not on the edge and on to find a pin */
			      u = xu;
#else
			      if (ppos[u] != pdir[u])
				  pin = u;	/* not on the edge and on to find a pin */
			      u = ppos[u];
#endif
			  }
		    }
		else
		    {
			/* it's the second piece in the current direction */
			if ( piece == bishop && board[u] == pawn )
			  {
			      if ( color[u] == c1 )
				{
				  s += BHCLSD; 
#ifdef DEBUG_EVAL
				  if (debug_eval )
				    fprintf(debug_eval_file,"add %d for own pawn in bishops (2) direction\n",BHCLSD);
#endif
				}
			      else
				{
				  s += BHCLSDX;
#ifdef DEBUG_EVAL
				  if (debug_eval )
				    fprintf(debug_eval_file,"add %d for opponents pawn in bishops (2) direction\n",BHCLSDX);
#endif
				}
			  }
			if ((color[u] == c2) && 
	                    (relative_value[board[u]] > rvalue || atk2[u] == 0))
			  {
			      if (color[pin] == c2)
				{
				    s += PINVAL;
#ifdef DEBUG_EVAL
				    if ( debug_eval )
				      fprintf(debug_eval_file,"adding %d for pin\n",PINVAL);
#endif
				    if (atk2[pin] == 0 || atk1[pin] > control[board[pin]] + 1)
					++hung[c2];
				}
			      else
				if ( piece == lance ) {
				  s += LXRAY;
#ifdef DEBUG_EVAL
				  if ( debug_eval )
				    fprintf(debug_eval_file,"lance xray: %d\n",LXRAY);
#endif
				} else {
				  s += BRXRAY; 
#ifdef DEBUG_EVAL
				  if ( debug_eval )
				    fprintf(debug_eval_file,"bishop/rook xray: %d\n",BRXRAY);
#endif
				}
			  }   
			pin = -1;	/* new direction */
#ifdef SAVE_NEXTPOS
			u = next_direction(ptyp,&d,sq);
#else
			u = pdir[u];
#endif
		    }
	    }
      }
    while (u != sq);
    *mob = mobx;
    return s;
}                             



#define ctlSG (ctlS | ctlG | ctlPp | ctlLp | ctlNp | ctlSp)


inline
short int
KingScan (register short int sq)

/*
 * Assign penalties if king can be threatened by checks, if squares near the
 * king are controlled by the enemy (especially by promoted pieces), 
 * or if there are no own generals near the king. 
 * The following must be true: 
 *    board[sq] == king, c1 == color[sq], c2 == otherside[c1]
 */


#define ScoreThreat \
	{ if (color[u] != c2)\
  	  if (atk1[u] == 0 || (atk2[u] & CNT_MASK) > 1) {\
	    ++cnt;\
	  }\
  	  else s -= 3;\
	}


{
    register short cnt;
#ifdef SAVE_NEXTPOS
    short d;
#else
    register unsigned char *ppos, *pdir;
#endif
    register short int s;
    register short u, ptyp;
    short int ok;
#ifdef DEBUG_EVAL
    short s0;
#endif

    /* Penalties, if a king can be threatened by checks. */      

    s = 0;
    cnt = 0;
    { short p;
      for ( p = pawn; p < king; p++ )
	if ( HasPiece[c2][p] || Captured[c2][p] )
          { short ptyp;
	    /* if a c1 piece can reach u from sq, 
	     * then a c2 piece can reach sq from u.
	     * That means, each u is a square, from which a 
	     * piece of type p and color c2 threats square sq.
	     */
	    ptyp = ptype[c1][p];
#ifdef SAVE_NEXTPOS
	    u = first_direction(ptyp,&d,sq);
#else
	    ppos = (*nextpos[ptyp])[sq];
	    pdir = (*nextdir[ptyp])[sq];
	    u = ppos[sq];
#endif
	    do
	      {  
		/* If a p piece can reach (controls or can drop to)  
		 * square u, then score threat. 
		 */
		if (atk2[u] & control[p])
		    ScoreThreat
		else if (Captured[c2][p] && color[u] == neutral)
		    ScoreThreat
#ifdef SAVE_NEXTPOS
		u = ((color[u] == neutral) ? next_position(ptyp,&d,sq,u)
					   : next_direction(ptyp,&d,sq));
#else
		u = ((color[u] == neutral) ? ppos[u] : pdir[u]);
#endif
	      }
	    while (u != sq);
          }
    }
    s += (KSFTY * KTHRT[cnt]) / 16;

#ifdef DEBUG_EVAL
    s0 = s;
    if (debug_eval )
       fprintf(debug_eval_file,"penalty %d for possible king threats\n",
		s0);
#endif                        

    /* Penalties, if squares near king are controlled by enemy. */

    cnt = 0;
    ok = false;
    ptyp = ptype[black][king];
#ifdef SAVE_NEXTPOS
    u = first_direction(ptyp,&d,sq);
#else
    pdir = (*nextpos[ptyp])[sq];
    u = pdir[sq];
#endif
    do
      {   
	  if ( !ok && color[u] == c1 ) 
	    {
	      short ptype_piece = ptype[c1][board[u]];
	      if ( ptype_piece == ptype_silver || ptype_piece == ptype_gold )
	        ok = true;
	    }
	  if (atk2[u] > atk1[u])
	    {
		++cnt;
		if (atk2[u] & ctlSG)
		  s -= 3;
	    }
#ifdef SAVE_NEXTPOS
	  u = next_direction(ptyp,&d,sq);
#else
	  u = pdir[u];
#endif
      }
    while (u != sq);
    if (!ok)
	s -= KSFTY;
    if (cnt > 1)
	s -= KSFTY;

#ifdef DEBUG_EVAL
    if (debug_eval )
       fprintf(debug_eval_file,"penalty %d for squares near king controlled by enemy\n",
		s-s0);
#endif                        

    return (s);
}

inline
int
trapped (register short int sq)

/*
 * See if the attacked piece has unattacked squares to move to. The following
 * must be true: c1 == color[sq] c2 == otherside[c1]
 */

{
    register short u, ptyp;
#ifdef SAVE_NEXTPOS
    short d;
#else
    register unsigned char *ppos, *pdir;
#endif
    register short int piece;
    short rvalue;

    piece = board[sq];
    rvalue = relative_value[piece];
    ptyp = ptype[c1][piece];
#ifdef SAVE_NEXTPOS
    u = first_direction(ptyp,&d,sq);
#else
    ppos = (*nextpos[ptyp])[sq];
    pdir = (*nextdir[ptyp])[sq];
    u = ppos[sq];
#endif
    do {
	if (color[u] != c1)
	    if (atk2[u] == 0 || relative_value[board[u]] >= rvalue)
	      return (false);
#ifdef SAVE_NEXTPOS
	u = ((color[u] == neutral) ? next_position(ptyp,&d,sq,u)
				   : next_direction(ptyp,&d,sq));
#else
	u = ((color[u] == neutral) ? ppos[u] : pdir[u]);
#endif
    } while (u != sq);
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"piece is trapped\n");
#endif                             
    return (true);
}


                                   
#define mirrored(sq) (NO_SQUARES-1-sq)
#define csquare(side,sq) ((side==black)?sq:mirrored(sq))
#define crow(side,sq) row(csquare(side,sq))
#define ccolumn(side,sq) column(csquare(side,sq))


inline
static short on_csquare(short side,short piece,short square)
{ short sq;
  return(board[sq=csquare(side,square)]==piece && color[sq]==side);
}
    
inline
static short on_column(short side,short piece,short c)
{ short sq;
  for (sq = c; sq < NO_SQUARES; sq+=9)
    if (on_csquare(side,piece,sq))
      return(true);
  return(false);
}

#define empty_csquare(side,square) (board[csquare(side,square)]==no_piece)

inline
static short on_left_side(short side,short piece)
{ short c;
  for (c=0; c<4; c++)
    if (on_column(side,piece,c))
      return(true);
  return(false);
}

inline
static short on_right_side(short side,short piece)
{ short c;
  for (c=5; c<NO_COLS; c++)
    if (on_column(side,piece,c))
      return(true);
  return(false);
}


/* Distance bonus */

inline
static
short AddEnemyKingDistanceBonus (short sq)
{ short s = 0;
  /* near enemy king */
  if ( !in_opening_stage || behind_in_material[c1] )
  if ( distance(sq,OwnKing) > 2 )
    s = EnemyKingDistanceBonus[distance(sq,EnemyKing)];
  return(s);
}

inline
static
short AddOwnKingDistanceBonus (short sq)
{ short s = 0;
  /* near own king */
  if ( !in_opening_stage || behind_in_material[c1] )
    s = OwnKingDistanceBonus[distance(sq,OwnKing)];
  return(s);
}

inline
static
short AddPromotionZoneDistanceBonus (short sq)
{ short s = 0;
  /* near promotion zone */
  if ( !in_opening_stage )
    s = PromotionZoneDistanceBonus[crow(c1,sq)];
  return(s);
}


static inline int
PawnValue (register short int sq, short int side)
/*
 * Calculate the positional value for a pawn on 'sq'.
 */

{
    register short s;
    short n;
    long a1, a2;

#if defined USE_PATTERN
    s = Mpawn[c1][sq] + 2*crow(c1,sq);
#else
    s = Mpawn[c1][csquare(c1,sq)] + 2*crow(c1,sq);
#endif

#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for pawn on %c%c: %d\n",
		csquare(c1,sq), cxx[column(sq)], rxx[row(sq)], s);
#endif

    s += PROMD * AddPromotionZoneDistanceBonus(sq);

    /* pawn mobility */
    {
	short u, ptyp =  ptype[c1][pawn];
#ifdef SAVE_NEXTPOS
	short d;
	u = first_direction(ptyp,&d,sq);
#else
	register unsigned char *pdir;
	pdir = (*nextdir[ptyp])[sq];
	u = pdir[sq];
#endif
	if ( u != sq )
	  if ( color[u] == neutral ) {
            s += PMBLTY;
#ifdef DEBUG_EVAL
	    if ( debug_eval )
              fprintf(debug_eval_file,"adding %d for mobility\n", PMBLTY);
#endif       
	  }
    }
    
    a1 = atk1[sq];
    a2 = atk2[sq];

    if (a2 > 0)
      {
	  if (a1 == 0 || a2 > ctlP + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif       
		if (trapped (sq)) {
		    hung[c1] += 2;
		}
		hung[c1]++;
	    }
	  else if (a2 > a1) {
	      s += ATAKD;   
#ifdef DEBUG_EVAL     
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for atacked piece\n",ATAKD);
#endif                             
	  }
      }          

    if ( a1 > 0 ) {
      s += (a1 & CNT_MASK);
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %ld for protected piece\n",(a1 & CNT_MASK));
#endif                             
    }

    if ( sq == csquare(c1,20) ) {
      s += BDCLOSED;
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %d for closed bishops diagonal\n",BDCLOSED);
#endif         
    }                    

    if ( (a1 & ((ctlR | ctlRp) | ctlL)) ) {
      s += PSTRONG;
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %d for rook/lance-supported pawn\n", PSTRONG);
#endif
    }         

    if ( in_opening_stage ) {

      if ( crow(c1,sq) == 2 ) /* pawn on 3d rank */
        if ( Mvboard[(c1==black)?(sq+36):(sq-36)] )
          {  /* opposing pawn has been moved (even column == (sq & 1)) */
	     s -= (sq & 1) ? (3*PMBLTY/2) : (2*PMBLTY);   
#ifdef DEBUG_EVAL
	     if ( debug_eval )
               fprintf(debug_eval_file,"adding %d for opposing pawn pushed\n",
		           ((sq & 1) ? -(3*PMBLTY/2) : -(2*PMBLTY)));
#endif         
          }

      if ( GameType[c1] == STATIC_ROOK && sq == csquare(c1,43) )
        if ( (atk2[csquare(c1,52)] & CNT_MASK) < 2 ) 
	  {
             s += P2STRONG;
#ifdef DEBUG_EVAL
	     if ( debug_eval )
               fprintf(debug_eval_file,"adding %d for attacking pawn on 2nd col\n",
		           P2STRONG);
#endif         
          }

      if ( GameType[c2] == STATIC_ROOK && ccolumn(c1,sq) == 1 ) 
	{
	  if ( sq == csquare(c1,28) )
	    {
              s -= P2STRONG;
#ifdef DEBUG_EVAL
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for pushed pawn on 8th file\n",
		           -P2STRONG);
#endif         
	    }
          if ( (atk1[csquare(c1,19)] & CNT_MASK) < 2 && (atk1[csquare(c1,28)] & CNT_MASK) < 2 ) 
	    {   
              s -= 2*P2STRONG;
#ifdef DEBUG_EVAL
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for weak pawn on 8th col\n",
		           -2*P2STRONG);
#endif         
            }
        }
    }

    return (s);
}


static inline int
LanceValue (register short int sq, short int side)
/*
 * Calculate the positional value for a lance on 'sq'.
 */

{
    register short s;
    register short fyle, rank;
    short mob;
    short checked_trapped = false;
    long a1, a2;

#if defined USE_PATTERN
    s = Mlance[c1][sq];
#else
    s = Mlance[c1][csquare(c1,sq)];
#endif
    
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for lance on %c%c: %d\n",
		csquare(c1,sq), cxx[column(sq)], rxx[row(sq)], s);
#endif

    s += KINGOD * AddOwnKingDistanceBonus(sq);

    s += BRLscan (sq, &mob);
    s += LMBLTY[mob];

    fyle = column(sq);
    rank = crow(c1,sq);

    if (PC1[fyle] == 0)
      {
	s += LHOPN;
#ifdef DEBUG_EVAL
	if ( debug_eval )
	  fprintf(debug_eval_file,"adding %d for no own pawn on lance file\n",LHOPN);
#endif
      }
    if (PC2[fyle] == 0)
      {
	s += LHOPNX;
#ifdef DEBUG_EVAL
	if ( debug_eval )
	  fprintf(debug_eval_file,"adding %d for no opponents pawn on lance file\n",LHOPNX);
#endif
      }

    a2 = atk2[sq];
    a1 = atk1[sq];

    if (a2 > 0)
      {
	  if (a1 == 0 || a2 > ctlL + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif              
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlL || a1 < ctlP)
	    {
	      s += ATAKD;
#ifdef DEBUG_EVAL     
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for atacked piece\n",ATAKD);
#endif       
	    }                      
      }

    if ( !checked_trapped && rank > 2 )
      {          
	if (in_opening_stage || trapped (sq))
	  {
	    s += LDNGR;
#ifdef DEBUG_EVAL     
	    if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for lance in danger\n",LDNGR);
#endif                             
	  }
	else 
	  {
	    s += LDNGR / 2;
#ifdef DEBUG_EVAL     
	    if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for lance in danger\n",LDNGR / 2);
#endif                             
	  }
      }

    if ( a1 > 0 ) {
      s += (a1 & CNT_MASK);
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %ld for protected piece\n",(a1 & CNT_MASK));
#endif             
    } 

    return (s);
}

static inline int
KnightValue (register short int sq, short int side)
/*
 * Calculate the positional value for a knight on 'sq'.
 */

{
    register short s;
    long a1, a2;    
    short checked_trapped = false;
    short c = column(sq);

#if defined USE_PATTERN
    s = Mknight[c1][sq];
#else
    s = Mknight[c1][csquare(c1,sq)];
#endif

#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for knight on %c%c: %d\n",
		csquare(c1,sq), cxx[c], rxx[row(sq)], s);
#endif

    s += PROMD * AddPromotionZoneDistanceBonus(sq);
    s += KINGOD * AddOwnKingDistanceBonus(sq);

    a2 = atk2[sq];
    a1 = atk1[sq];

    if (a2 > 0)
      {
	  if (a1 == 0 || a2 > ctlN + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif              
	        checked_trapped = true;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlN || a1 < ctlP)
	    {
	      s += ATAKD;
#ifdef DEBUG_EVAL     
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for atacked piece\n",ATAKD);
#endif                             
	    }
      }         

    if ( !checked_trapped && crow(c1,sq) > 2 )
      {          
	if (trapped (sq))
	  {
	    s += NDNGR;
#ifdef DEBUG_EVAL     
	    if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for knight in danger\n",NDNGR);
#endif                             
	  }
	else
	  {
	    s += NDNGR / 4;
#ifdef DEBUG_EVAL     
	    if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for knight in danger\n",NDNGR / 4);
#endif                             
	  }
      }
      
    if ( c == 0 || c == 8 ) {
      s += NEDGE;
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %d for knight on edge file\n",NEDGE);
#endif             
    }

    if ( a1 > 0 ) {
      s += (a1 & CNT_MASK);
#ifdef DEBUG_EVAL
      if ( debug_eval )
        fprintf(debug_eval_file,"adding %ld for protected piece\n",(a1 & CNT_MASK));
#endif             
    }
                
    return (s);
}

static inline int
SilverValue (register short int sq, short int side)
/*
 * Calculate the positional value for a silver on 'sq'.
 */

{
    register short s;
    long a1, a2;

#if defined USE_PATTERN
    s = Msilver[c1][sq];
#else
    s = Msilver[c1][csquare(c1,sq)];
#endif

    s += KINGED * AddEnemyKingDistanceBonus(sq);
    s += KINGOD * AddOwnKingDistanceBonus(sq);

    a2 = atk2[sq];
    a1 = atk1[sq];
    if (a2 > 0)
      {
	  if (a1 == 0 || a2 > ctlS + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlS || a1 < ctlP)
	      s += ATAKD;
      }
    if ( a1 > 0 )
      s += (a1 & CNT_MASK);

    if ( in_opening_stage && GameType[c1] == STATIC_ROOK )
	if ( csquare(c1,sq) == 12 )
	  {
	    short csq;
	    if ( board[csq = csquare(c1,20)] == bishop && color[csq] == c1 )
		{
		  s += 2 * OPENWRONG;
		}
          }

    /* proverb: silver before gold */
    {
	short u, ptyp = ptype[c2][pawn];
#ifdef SAVE_NEXTPOS
	short d;
	u = first_direction(ptyp,&d,sq);
#else
	register unsigned char *pdir;
	pdir = (*nextdir[ptyp])[sq];
	u = pdir[sq];
#endif
	if ( u != sq )
	  if ( color[u] == c1 && board[u] == gold )
            s += SBEFOREG;
    }
    return (s);
}

static inline int
GoldValue (register short int sq, short int side)
/*
 * Calculate the positional value for a gold on 'sq'.
 */

{
    register short s;
    long a1, a2;

#if defined USE_PATTERN
    s = Mgold[c1][sq];
#else
    s = Mgold[c1][csquare(c1,sq)];
#endif

    s += KINGED * AddEnemyKingDistanceBonus(sq);
    s += KINGOD * AddOwnKingDistanceBonus(sq);

    a2 = atk2[sq];
    a1 = atk1[sq];
    if (a2 > 0)
      {
	  if (a1 == 0 || a2 > ctlG + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlG || a1 < ctlP)
	      s += ATAKD;
      }
    if ( a1 > 0 )
      s += (a1 & CNT_MASK);

    if ( in_opening_stage &&
	 GameType[c1] == STATIC_ROOK && GameType[c2] != STATIC_ROOK )
	if ( Mvboard[csquare(c1,3)] )
	  {
		s += 2 * OPENWRONG;
          }


    return (s);
}

static inline int
BishopValue (register short int sq, short int side)
/*
 * Calculate the positional value for a bishop on 'sq'.
 */

{
    register short s;
    long a2, a1;
    short mob;

#if defined USE_PATTERN
    s = Mbishop[c1][sq];
#else
    s = Mbishop[c1][csquare(c1,sq)];
#endif

#ifdef DEBUG_EVAL
    if (debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for bishop on %c%c: %d\n",
		csquare(c1,sq), cxx[column(sq)], rxx[row(sq)], s);
#endif

    s += BRLscan (sq, &mob);
    s += BMBLTY[mob];

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlB + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif       
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlB || a1 < ctlP)
	    {
	      s += ATAKD;
#ifdef DEBUG_EVAL     
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for atacked piece\n",ATAKD);
#endif                                                                         
	    }
      }                      

    if ( in_opening_stage )
      {
	if ( GameType[c1] == RANGING_ROOK )
	  {
	    /* Bishops diagonal should not be open */
	    if ( !on_csquare(c1,pawn,30) )
	      s += OPENWRONG;
	  }
	else if ( GameType[c2] == RANGING_ROOK )
	  {
	    /* Bishops diagonal should be open */
	    if ( csquare(c1,sq) == 10 && 
		 (!empty_csquare(c1,20) || !empty_csquare(c1,30)) )
	      s += OPENWRONG;
	    else if ( csquare(c1,sq) == 20 && !empty_csquare(c1,30) ) 
	      s += OPENWRONG;
	  }
      }     
  
    return (s);
}

static inline int
RookValue (register short int sq, short int side)
/*
 * Calculate the positional value for a rook on 'sq'.
 */

{
    register short s;
    register short fyle;
    long a2, a1;
    short mob, r, c;

#if defined USE_PATTERN
    s = Mrook[c1][sq];
#else
    s = Mrook[c1][csquare(c1,sq)];
#endif

#ifdef DEBUG_EVAL
    if (debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for rook on %c%c: %d\n",
		csquare(c1,sq), cxx[column(sq)], rxx[row(sq)], s);
#endif

    s += BRLscan (sq, &mob);
    s += RMBLTY[mob];

#ifdef DEBUG_EVAL
    if ( debug_eval && (c = RMBLTY[mob]) != 0 ) {
	s += c;
	fprintf(debug_eval_file,"adding rook for mobility %d value %d\n",mob,c);
    }
#endif

    fyle = column (sq);

    if (PC1[fyle] == 0) {
	s += RHOPN;  /* no own pawn on rook file */
#ifdef DEBUG_EVAL
	if ( debug_eval )
	  fprintf(debug_eval_file,"adding %d for no own pawn on rook file\n",RHOPN);
#endif
    }
    if (PC2[fyle] == 0) {
	s += RHOPNX; /* no enemy pawn on rook file */
#ifdef DEBUG_EVAL
	if ( debug_eval )
	  fprintf(debug_eval_file,"adding %d for no enemy pawn on rook file\n",RHOPNX);
#endif                                               
    }

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlR + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif       
		if (trapped (sq)) {
		    hung[c1] += 2; 
		}
		hung[c1]++;
	    }
	  else if (a2 >= ctlR || a1 < ctlP) 
	    {
	      s += ATAKD;
#ifdef DEBUG_EVAL     
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for atacked piece\n",ATAKD);
#endif                                                                         
	    }
      }

    if ( in_opening_stage ) {
#ifdef DEBUG_EVAL
      short s0 = s;
#endif
      if ( GameType[c1] == STATIC_ROOK )
        {
	  short c = ccolumn(c1,sq);
	  /* Bonus for rook on 8th file */
          if ( c == 7 ) {
	    s += R2NDCOL;
	  }
	  /* Bonus for rook on right side, malus for rook on left side */
	  c = 4 - c;
          if ( c < 0 ) {
	    s += c + c + OPENOK;
	  } else if ( c >= 0 ) {
	    s += c + c + OPENWRONG;
	  }
        }
      else if ( GameType[c1] == RANGING_ROOK )
        {
	  /* Bonus for rook on left side and bishops diagonal closed, malus otherwise. */
          short c;
	  c = 4 - ccolumn(c1,sq);
          if ( c >= 0 ) {
	    /* Bishops diagonal should not be open */
	    if ( on_csquare(c1,pawn,30) )
	      s += OPENOK;
	    else
	      s += c + c + OPENWRONG;
          } else if ( c < 0 ) {
	    s += c + c + OPENWRONG;
	    /* Malus for king not on initial square */
	    if ( !on_csquare(side,king,4) ) {
		s += 4 * OPENWRONG;      
	    }
	  }
        }
#ifdef DEBUG_EVAL
      if ( debug_eval && (s0 != s) )
	  fprintf(debug_eval_file,"adding %d for opening\n",(s-s0));
#endif
    }

    return (s);
}

static inline int
PPawnValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted pawn on 'sq'.
 */

{
    register short s;
    long a2, a1;

    s = 0;

    s += (KINGED + 2) * AddEnemyKingDistanceBonus(sq);

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlPp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlPp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
PLanceValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted lance on 'sq'.
 */

{
    register short s;
    long a2, a1;

    s = 0;

    s += (KINGED + 1) * AddEnemyKingDistanceBonus(sq);

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlLp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlLp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
PKnightValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted knight on 'sq'.
 */

{
    register short s;
    long a2, a1;

    s = 0;

    s += (KINGED + 1 ) * AddEnemyKingDistanceBonus(sq);

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlNp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlNp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
PSilverValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted silver on 'sq'.
 */

{
    register short s;
    long a2, a1;

    s = 0;

    s += (KINGED + 1) * AddEnemyKingDistanceBonus(sq);

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlSp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlSp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
PBishopValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted bishop on 'sq'.
 */

{
    register short s;
    long a2, a1;
    short mob;

#if defined USE_PATTERN
    s = Mbishop[c1][sq];
#else
    s = Mbishop[c1][csquare(c1,sq)];
#endif
    
    if ( InPromotionZone(c1,sq) )
      s += KINGED * AddEnemyKingDistanceBonus(sq);

    s += BRLscan (sq, &mob);
    s += BMBLTY[mob];
    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlBp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlBp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
PRookValue (register short int sq, short int side)
/*
 * Calculate the positional value for a promoted rook on 'sq'.
 */

{
    register short s;
    register short fyle;
    long a2, a1;
    short mob;

#if defined USE_PATTERN
    s = Mrook[c1][sq];
#else
    s = Mrook[c1][csquare(c1,sq)];
#endif

    if ( InPromotionZone(c1,sq) )
      s += KINGED * AddEnemyKingDistanceBonus(sq);

    s += BRLscan (sq, &mob);
    s += RMBLTY[mob];
    fyle = column (sq);
    if (PC1[fyle] == 0)
	s += RHOPN;
    if (PC2[fyle] == 0)
	s += RHOPNX;
    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlRp + 1)
	    {
		s += HUNGP;
		if (trapped (sq))
		    hung[c1] += 2;
		hung[c1]++;
	    }
	  else if (a2 >= ctlRp || a1 < ctlP)
	      s += ATAKD;
      }

    return (s);
}

static inline int
KingValue (register short int sq, short int side)
/*
 * Calculate the positional value for a king on 'sq'.
 */
{
    register short s;
    register short fyle;
    long a2, a1;

#if defined USE_PATTERN
    s = Mking[c1][sq];
#else
    s = Mking[c1][csquare(c1,sq)];
#endif

#ifdef DEBUG_EVAL
    if (debug_eval )
      fprintf(debug_eval_file,"inital value [%d] for king on %c%c: %d\n",
		csquare(c1,sq), cxx[column(sq)], rxx[row(sq)], s);
#endif

    if (KSFTY > 0)
	s += KingScan (sq);

    if (Mvboard[kingP[c1]])
	s += KMOVD;

    fyle = column (sq);
    if (PC1[fyle] == 0)
      {
	s += KHOPN;
#ifdef DEBUG_EVAL
        if (debug_eval )
          fprintf(debug_eval_file,"adding %d for now own pawn on file\n",
		KHOPN);
#endif
      }
    if (PC2[fyle] == 0)
      {
	s += KHOPNX;
#ifdef DEBUG_EVAL
        if (debug_eval )
          fprintf(debug_eval_file,"adding %d for now opponents pawn on file\n",
		KHOPNX);
#endif
      }

    a2 = atk2[sq];
    if (a2 > 0)
      {
	  a1 = atk1[sq];
	  if (a1 == 0 || a2 > ctlK + 1)
	    {
		s += HUNGP;
#ifdef DEBUG_EVAL
		if ( debug_eval )
                  fprintf(debug_eval_file,"adding %d for hung\n", HUNGP);
#endif              
		++hung[c1];
	    }
	  else
	    {
	      s += ATAKD;
#ifdef DEBUG_EVAL
	      if ( debug_eval )
                fprintf(debug_eval_file,"adding %d for attacked piece\n", 
				ATAKD);
#endif              
	    }
      }  
                        
    if ( in_opening_stage ) {
      if ( GameType[c1] == STATIC_ROOK )
        {
	  /* Malus for king on right side or fifth file */
          short c;
	  c = 4 - ccolumn(c1,sq);
	  if ( c < 0 || (c == 0 && sq != kingP[c1]) ) {
	    s += c + c + OPENWRONG;
#ifdef DEBUG_EVAL
	    if ( debug_eval )
		fprintf(debug_eval_file,"add %d for king on right side (Static)\n",
				c + c + OPENWRONG);
#endif
	  }
        }
      else if ( GameType[c1] == RANGING_ROOK )
        {
	  /* Malus for king on left side or fifth file */
          short c;
	  c = 4 - ccolumn(c1,sq);
 	  if ( c > 0 || (c == 0 && sq != kingP[c1]) ) {
	    s += c + c + OPENWRONG;
#ifdef DEBUG_EVAL
	    if ( debug_eval )
		fprintf(debug_eval_file,"add %d for king on left side (Ranging)\n",
				c + c + OPENWRONG);
#endif
	  }
	  /* Malus for king moved before rook switch */
	  if ( sq != kingP[c1] )
	    {
	      if ( on_csquare(c1,rook,16) ) {
		s += 4 * OPENWRONG;         
#ifdef DEBUG_EVAL
	        if ( debug_eval )
		    fprintf(debug_eval_file,"add %d for king moved before rook switch (Ranging)\n",
				4 * OPENWRONG);
#endif
	      }
	    }
	  else
	  /* Malus for sitting king after rook switch */
	    {
	      if ( !on_csquare(c1,rook,16) ) {
	        s += 2 * OPENWRONG;
#ifdef DEBUG_EVAL
	        if ( debug_eval )
		    fprintf(debug_eval_file,"add %d for sitting king after rook switch (Ranging)\n",
				2 * OPENWRONG);
#endif
	      }
	    }
	  /* Malus for defending general moved before king switch to right side */
	  if ( ccolumn(c1,sq) < 6 )
	    if ( Mvboard[csquare(c1,5)] || Mvboard[csquare(c1,6)] ) {
		s += 2 * OPENWRONG;         
#ifdef DEBUG_EVAL
	        if ( debug_eval )
		    fprintf(debug_eval_file,
			"add %d for defending general moved before king switch (Ranging)\n",
			2 * OPENWRONG);
#endif
	    }
        }                       
    }

    return (s);
}




#ifdef USE_PATTERN
          
short
ScorePatternDistance (register short int side)

/*
 * Score distance to pattern regarding the game type which side plays.
 */

{ 
  short ds, s = 0;
  OpeningPattern *p = NULL;

#ifdef DEBUG_EVAL
  if ( debug_eval )
    fprintf(debug_eval_file,"scoring castle pattern distance for PCASTLE=%d\n",
		PCASTLE);
#endif

  if ( PCASTLE != 0 && ((p = castle_pattern[side]) != NULL) )
    {
      ds = board_to_pattern_distance(side,p,PCASTLE,GameCnt);
      if ( ds != 0) {
        s += ds;
#ifdef DEBUG_EVAL
        if ( debug_eval && ds != 0 )
          fprintf(debug_eval_file,
            "add %d for max gain of %s to reachable castle patterns %s\n", 
            ds, ColorStr[side], p->name); 
#endif
      };
    }
#ifdef DEBUG_EVAL
  else if ( debug_eval && p == NULL )
    fprintf(debug_eval_file,"no castle pattern for %s\n",ColorStr[side]);
#endif

#ifdef DEBUG_EVAL
  if ( debug_eval )
    fprintf(debug_eval_file,"scoring attack pattern distance for PATTACK=%d\n",
		PATTACK);
#endif

  if ( PATTACK != 0 && ((p = attack_pattern[side]) != NULL) )
    {            
      ds = board_to_pattern_distance(side,p,PATTACK,GameCnt);
      if ( ds != 0 ) {
        s += ds;
#ifdef DEBUG_EVAL
        if ( debug_eval && ds != 0 )
          fprintf(debug_eval_file,
             "add %d for max gain of %s to reachable attack patterns %s\n",
             ds, ColorStr[side], p->name); 
#endif
      }
    }
#ifdef DEBUG_EVAL
  else if ( debug_eval && p == NULL )
    fprintf(debug_eval_file,"no attack pattern for %s\n",ColorStr[side]);
#endif

  return(s);
}


static
void
UpdatePatterns (short int side, short int GameCnt)

/*
 * Determine castle and attack pattern which should be reached next.
 * Only patterns are considered, which have not been reached yet.
 */

{
  char s[12];
  short xside = side ^ 1;
  OpeningPattern *p;
  short i, n = 0;
  PatternSequence *sequence;

  strcpy(s,"CASTLE_?_?");
  s[7] = GameType[side];
  s[9] = GameType[xside];
  castle_pattern[side] = p = locate_opening_pattern(side,s,GameCnt);

  if ( flag.post && p )
    {
      for (i=0,sequence=p->sequence; i<p->n; i++,sequence=sequence->next_pattern)
 	if ( sequence->distance[side] != CANNOT_REACH ) n++;
    }

#if defined USE_PATTERN
  if ( p ) update_advance_bonus (side,p);
#endif

#ifdef DEBUG_EVAL
  if ( debug_eval )
    if (p != NULL )
      {
        fprintf(debug_eval_file,"castle pattern of %s is %s\n",
                 ColorStr[side], p->name);
        fprintf(debug_eval_file,"reachable patterns: ");
        for (i=0,sequence=p->sequence; i<p->n; i++,sequence=sequence->next_pattern)
	  if ( sequence->distance[side] != CANNOT_REACH )
	    fprintf(debug_eval_file,"%d ",i);
        fprintf(debug_eval_file,"\n");
      }
    else
      {
        fprintf(debug_eval_file,"no castle pattern %s for %s\n",
                 s, ColorStr[side]);
      }
#endif

  strcpy(s,"ATTACK_?_?");
  s[7] = GameType[side];
  s[9] = GameType[xside];
  attack_pattern[side] = p = locate_opening_pattern(side,s,GameCnt);
  
  if ( flag.post && p )
    {
      for (i=0,sequence=p->sequence; i<p->n; i++,sequence=sequence->next_pattern)
 	if ( sequence->distance[side] != CANNOT_REACH ) n++;
    }


#ifdef USE_PATTERN
  if ( flag.post ) 
      ShowPatternCount(side,n);
  if ( p ) update_advance_bonus (side,p);
#endif

#ifdef DEBUG_EVAL
  if ( debug_eval )
    if ( p != NULL )
      {
        fprintf(debug_eval_file,"attak pattern of %s is %s\n",
                  ColorStr[side], p->name);
        fprintf(debug_eval_file,"reachable patterns: ");
        for (i=0,sequence=p->sequence; i<p->n; i++,sequence=sequence->next_pattern)
	  if ( sequence->distance[side] != CANNOT_REACH )
	    fprintf(debug_eval_file,"%d ",i);
        fprintf(debug_eval_file,"\n");
      }
    else
      {
        fprintf(debug_eval_file,"no attak pattern %s for %s\n",
                  s, ColorStr[side]);
      }
#endif


}


#endif


short int
ScorePosition (register short int side)

/*
 * Perform normal static evaluation of board position. A score is generated
 * for each piece and these are summed to get a score for each side.
 */

{
    register short int score;
    register short sq, i, xside;
    short int s;
    short int escore;
#ifdef DEBUG_EVAL
    short int d;
#endif

    UpdateWeights ();

#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf (debug_eval_file, "side = %d, stage = %d, in_opening = %d\n",
	side, stage, in_opening_stage );
#endif

    xside = side ^ 1;
    hung[black] = hung[white] = pscore[black] = pscore[white] = 0;

#ifdef CACHE
#ifdef DEBUG_EVAL
    if (debug_eval || !ProbeEETable (side, &s))
#else
    if (!ProbeEETable (side, &s))
#endif
	{ 
#endif
	  for (c1 = black; c1 <= white; c1++)
	    {
		c2 = c1 ^ 1;
		/* atk1 is array of atacks on squares by my side */
		atk1 = atak[c1];
		/* atk2 is array of atacks on squares by other side */
		atk2 = atak[c2];
		/* same for PC1 and PC2 */
		PC1 = PawnCnt[c1];
		PC2 = PawnCnt[c2];
		for (i = PieceCnt[c1]; i >= 0; i--)
		  {
		      sq = PieceList[c1][i];
		      switch (board[sq])
			{
			case pawn:
			    s = PawnValue (sq, side);
			    break;
			case knight:
			    s = KnightValue (sq, side);
			    break;
			case bishop:
			    s = BishopValue (sq, side);
			    break;
			case rook:
			    s = RookValue (sq, side);
			    break;
			case lance:
			    s = LanceValue (sq, side);
			    break;
			case silver:
			    s = SilverValue (sq, side);
			    break;
			case gold:
			    s = GoldValue (sq, side);
			    break;
			case ppawn:
			    s = PPawnValue (sq, side);
			    break;
			case plance:
			    s = PLanceValue (sq, side);
			    break;
			case pknight:
			    s = PKnightValue (sq, side);
			    break;
			case psilver:
			    s = PSilverValue (sq, side);
			    break;
			case pbishop:
			    s = PBishopValue (sq, side);
			    break;
			case prook:
			    s = PRookValue (sq, side);
			    break;
			case king:
			    s = KingValue (sq, side);
			    break;
			default:
			    s = 0;
			    break;
			}
		      pscore[c1] += s;
#if !defined SAVE_SVALUE
		      svalue[sq] = s;
#endif
		  }
	    }
#ifdef DEBUG_EVAL 
          if ( debug_eval ) {                      
	    fprintf(debug_eval_file,"pscore[black] = %d, pscore[white] = %d\n",
		pscore[black], pscore[white]);
#if !defined SAVE_SVALUE
	    debug_svalue(debug_eval_file);
#endif
	  }
#endif
	  if (hung[side] > 1) {
#ifdef DEBUG_EVAL
	    if ( debug_eval ) {
	      fprintf(debug_eval_file,"adding %d to pscore for %d hung %d pieces.\n",
		  HUNGX, hung[side], side);
	    }
#endif
	    pscore[side] += HUNGX;
	  }
	  if (hung[xside] > 1) {
#ifdef DEBUG_EVAL
	    if ( debug_eval ) {
	      fprintf(debug_eval_file,"adding %d to pscore for %d hung %d pieces.\n",
		HUNGX, hung[xside], xside);
	    }
#endif
	    pscore[xside] += HUNGX;
	  }
                     
          for (c1=black,c2=white; c1<=white; c1++,c2--) 
	    {
#ifdef DEBUG_EVAL
	      short ps0 = pscore[c1];
#endif
	      /* Score fifth rank */
              for ( sq = 36; sq <= 44; sq++ ) 
	        if ( color[sq] == c1 || atak[c1][sq] != 0 )
		  pscore[c1]++;

#ifdef DEBUG_EVAL
	      if ( debug_eval && ps0 != pscore[c1] )
		fprintf(debug_eval_file,"adding %d for %s's control of 5th rank\n",
			  pscore[c1]-ps0, ColorStr[c1]);
	      ps0 = pscore[c1];
#endif

	      /* Score holes */
              for ( sq = ((c1==black)?0:54); sq<=((c1==black)?26:80); sq++ )
	        if ( board[sq] == no_piece && atak[c1][sq] == 0 )
		  pscore[c1]--;

#ifdef DEBUG_EVAL
	      if ( debug_eval && ps0 != pscore[c1] )
		fprintf(debug_eval_file,"adding %d for holes in %s's camp\n",
			  pscore[c1]-ps0, ColorStr[c1]);
#endif

	      /* Scoring depending whether in opening stage or not */ 

	      if ( in_opening_stage )
		{
	          /* do not exchange bishops in ranging rook openings */
		  if ( GameType[c1] == RANGING_ROOK )
		  if ( Captured[c2][bishop] ) {
		    pscore[c1] += BXCHG;
#ifdef DEBUG_EVAL   
		    if ( debug_eval )
		      fprintf(debug_eval_file,"adding %d for %s's bishop catched by opponent\n",
                           BXCHG, ColorStr[c1]);
#endif
		  }
		}
	      else
		{
		  short d1 = HasPiece[c1][silver] + HasPiece[c1][gold];
		  short d2 = Captured[c1][silver] + Captured[c1][gold];
	          pscore[c1] += (d1*2 + d2*3) * stage / 2;
#ifdef DEBUG_EVAL 
		  if ( debug_eval )
		     fprintf(debug_eval_file,"adding %d for more valuable %s generals\n",
                             (d1*2 + d2*3) * stage / 2, ColorStr[c1]);
#endif
	        }
	    }
		

#ifdef USE_PATTERN
	  /* Score pattern */
                   
	  if ( in_opening_stage ) {    
	    pscore[side]  += ScorePatternDistance (side);         
	    pscore[xside] += ScorePatternDistance (xside);         
	  }
#endif

          score = mtl[side] - mtl[xside] + pscore[side] - pscore[xside] + 10;
          if (dither)
      	    {
	      if (flag.hash)
	        gsrand (starttime + (unsigned int) hashbd);
	      score += urand () % dither;
            }

#ifdef CACHE
	  if ( PUTVAR )
	    PutInEETable(side,score);
#endif
#ifdef DEBUG_EVAL
	  if ( debug_eval )
	    fprintf (debug_eval_file, "score = %d\n", score);
#endif
	  return(score);
#ifdef CACHE
	}
#ifdef DEBUG_EVAL
      else
	{ 
	  if ( debug_eval )
	    fprintf (debug_eval_file, "Score cached!\n" );
	}
#endif
                          
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf (debug_eval_file, "s = %d\n", s);
#endif
    return (s);
#endif
}



#if !defined USE_PATTERN


#define SS 1
#define RS 2
#define SR 3
#define RR 4


static
    short int *PawnAdvance[2],*LanceAdvance[2],*KnightAdvance[2],
	*SilverAdvance[2],*GoldAdvance[2],*BishopAdvance[2],*RookAdvance[2],
	*KingAdvance[2];
 

inline
static
void
SetOpeningStrategy(short c, short s)
{ 
#ifdef DEBUG_EVAL
  if ( debug_eval )
    fprintf(debug_eval_file,"strategy for %s is ",ColorStr[c]);
#endif                  
  if ( s == RS ) {
    PawnAdvance[c] = PawnAdvanceRS;
    LanceAdvance[c] = LanceAdvanceRS;
    KnightAdvance[c] = KnightAdvanceRS;
    SilverAdvance[c] = SilverAdvanceRS;
    GoldAdvance[c] = GoldAdvanceRS;
    BishopAdvance[c] = BishopAdvanceRS;
    RookAdvance[c] = RookAdvanceRS;
    KingAdvance[c] = KingAdvanceRS;
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"Ranging vs. Static\n");
#endif                  
  } else if ( s == SR ) {
    PawnAdvance[c] = PawnAdvanceSR;
    LanceAdvance[c] = LanceAdvanceSR;
    KnightAdvance[c] = KnightAdvanceSR;
    SilverAdvance[c] = SilverAdvanceSR;
    GoldAdvance[c] = GoldAdvanceSR;
    BishopAdvance[c] = BishopAdvanceSR;
    RookAdvance[c] = RookAdvanceSR;
    KingAdvance[c] = KingAdvanceSR;
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"Static vs. Ranging\n");
#endif                  
  } else if ( s == RR ) {
    PawnAdvance[c] = PawnAdvanceRR;
    LanceAdvance[c] = LanceAdvanceRR;
    KnightAdvance[c] = KnightAdvanceRR;
    SilverAdvance[c] = SilverAdvanceRR;
    GoldAdvance[c] = GoldAdvanceRR;
    BishopAdvance[c] = BishopAdvanceRR;
    RookAdvance[c] = RookAdvanceRR;
    KingAdvance[c] = KingAdvanceRR;
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"Ranging vs. Ranging\n");
#endif                  
  } else {
    PawnAdvance[c] = PawnAdvanceSS;
    LanceAdvance[c] = LanceAdvanceSS;
    KnightAdvance[c] = KnightAdvanceSS;
    SilverAdvance[c] = SilverAdvanceSS;
    GoldAdvance[c] = GoldAdvanceSS;
    BishopAdvance[c] = BishopAdvanceSS;
    RookAdvance[c] = RookAdvanceSS;
    KingAdvance[c] = KingAdvanceSS;
#ifdef DEBUG_EVAL
    if ( debug_eval )
      fprintf(debug_eval_file,"Static vs. Static\n");
#endif                  
  }
}




#define AssignAdvance(b,w)\
{\
  SetOpeningStrategy(black,b);\
  SetOpeningStrategy(white,w);\
}                



static inline void
CopyBoard (small_short *a, small_short *b, short n, short d)
{                          
    register small_short *sqa, *sqb;
    register short i;

    if ( n == d ) {
      bcopy(a,b,NO_SQUARES*sizeof(short));
    } else
      for (sqa = a,sqb = b,i=0; i<NO_SQUARES; sqa++,sqb++,i++)
	if ( n == 1 )
	    *sqb = (*sqa) / d;
        else
	  if ( d == 1 )
	    *sqb = n * (*sqa);
	  else
	    *sqb = (n * (*sqa)) / d;
}


#endif /* USE_PATTERN */


static inline void
GuessGameType (void)
{
  /*
   * Try to determine the game type of "side".
   */

   short side, sq, StaticRook[2] = {0,0}, RangingRook[2] = {0,0};
  
   for ( side=black; side<=white; side++ ) {         

     /* static rook conditions */

     if ( on_column(side,rook,7) )
       StaticRook[side] += 3;
     if ( on_csquare(side,pawn,34) )
       StaticRook[side] += 6;
     else if ( on_csquare(side,pawn,43) )
       StaticRook[side] += 8;
     else if ( !on_column(side,pawn,7) )
       StaticRook[side] += 5;
     if ( empty_csquare(side,5) || empty_csquare(side,6) )
       StaticRook[side] += 2;
     if ( on_left_side(side,king) )
       StaticRook[side] += 2;

     /* ranging rook conditions */
 
     if ( on_left_side(side,rook) )
       RangingRook[side] += 5; 
     else if ( !on_column(side,rook,7) )
       RangingRook[side] += 3;
     if ( on_csquare(side,pawn,25) )
       RangingRook[side] += 1;
     if ( on_csquare(side,pawn,30) )
       RangingRook[side] += 1;
     else
       RangingRook[side] -= 2;
     if ( !on_right_side(side,rook) )
       RangingRook[side] += 2;
     if ( on_right_side(side,king) )
       RangingRook[side] += 2;
     if ( on_csquare(side,bishop,20) )
       if ( on_csquare(side,silver,11) || on_csquare(side,silver,12) ||
  	  on_csquare(side,silver,21) )
       RangingRook[side] += 3;
     
     if ( StaticRook[side] > 5 || RangingRook[side] > 5 )
         GameType[side] = (StaticRook[side] > RangingRook[side]) 
  				? STATIC_ROOK : RANGING_ROOK;
     else
         GameType[side] = UNKNOWN;

   }
        
#ifdef DEBUG_EVAL
#define StringOfGameType(side)\
  (GameType[side] == STATIC_ROOK ? "Static Rook" :\
  (GameType[side] == RANGING_ROOK ? "Ranging Rook" : "UNKNOWN"))

   if ( debug_eval )
      fprintf(debug_eval_file,"guessing game type: %s vs. %s\n",
	StringOfGameType(black), StringOfGameType(white));
#endif          

   if ( GameType[black] == UNKNOWN || GameType[white] == UNKNOWN )
      {
         for (side = black; side <= white; side++)
           if ( side == computer && GameType[side] == UNKNOWN ) {
  		  /*
   		   * Game type is UNKNOWN.
		   * Make a decision what type of game to play.
		   * To make computer games more interesting, make a
		   * random decision.
		   */
	           if ( !on_csquare(side,pawn,25) ) {
		     /* Play static rook if rook pawn has been pushed! */
		     GameType[side] = STATIC_ROOK;
		   } else {
		     unsigned int random = urand () % 100;
		     short d = StaticRook[side] - RangingRook[side];
		     switch ( GameType[side ^ 1] ) {
		       case STATIC_ROOK:
		 	 if ( random < 35 + d ) GameType[side] = STATIC_ROOK;
			 else if ( random < 95 ) GameType[side] = RANGING_ROOK;
			 break;
		       case RANGING_ROOK:
			 if ( random < 75 + d) GameType[side] = STATIC_ROOK;
			 else if ( random < 95 ) GameType[side] = RANGING_ROOK;
			 break;
		       default:          
			 if ( random < 33 + d) GameType[side] = STATIC_ROOK;
			 else if ( random < 66 ) GameType[side] = RANGING_ROOK;
		     }       
		   }
#ifdef DEBUG_EVAL
	     if ( debug_eval )
    	       fprintf(debug_eval_file,"decide game type: %s vs. %s\n",
		 StringOfGameType(black), StringOfGameType(white));
#endif
	   }
      }

}


#ifdef DEBUG_EVAL

#define StringOfGameType(side)\
  (GameType[side] == STATIC_ROOK ? "Static Rook" :\
  (GameType[side] == RANGING_ROOK ? "Ranging Rook" : "UNKNOWN"))

#endif


static
void
DetermineGameType (void)
{
    short int side;

    GuessGameType();

#if !defined XSHOGI
    if ( flag.post )
      ShowGameType();
#endif

#if defined USE_PATTERN

    memset ((char *)Mpawn, 0, sizeof(Mpawn));
    memset ((char *)Mlance, 0, sizeof(Mlance));
    memset ((char *)Mknight, 0, sizeof(Mknight));
    memset ((char *)Msilver, 0, sizeof(Msilver));
    memset ((char *)Mgold, 0, sizeof(Mgold));
    memset ((char *)Mbishop, 0, sizeof(Mbishop));
    memset ((char *)Mrook, 0, sizeof(Mrook));
    memset ((char *)Mking, 0, sizeof(Mking));

#else

    if ( GameType[black] == STATIC_ROOK) {
	  if ( GameType[white] == STATIC_ROOK ) {
	    /* STATIC vs. STATIC */
	    AssignAdvance(SS,SS);
	  } else if ( GameType[white] == RANGING_ROOK ) {
	    /* STATIC vs. RANGING */
	    AssignAdvance(SR,RS);
	  } else {
	    /* STATIC vs. ??? */
	    AssignAdvance(SS,SS);
	  }
    } else if ( GameType[black] == RANGING_ROOK ) {
	  if ( GameType[white] == STATIC_ROOK ) {
	    /* RANGING vs. STATIC */
	    AssignAdvance(RS,SR);
	  } else if ( GameType[white] == RANGING_ROOK ) {
	    /* RANGING vs. RANGING */
	    AssignAdvance(RR,RR);
	  } else {
	    /* RANGING vs. ???? */
	    AssignAdvance(RS,SR);
	  }
    } else {
	  if ( GameType[white] == STATIC_ROOK ) {
	    /* ???? vs. STATIC */
	    AssignAdvance(SS,SS);
	  } else if ( GameType[white] == RANGING_ROOK ) {
	    /* ???? vs. RANGING */
	    AssignAdvance(SR,RS);
	  } else {
	    /* ???? vs. ???? */
	    AssignAdvance(SS,SS);
	  }
    }

    /* Weighted advance tables. Can be updated. */

    CopyBoard (PawnAdvance[black], Mpawn[black], PADVNCM, 10);
    CopyBoard (PawnAdvance[white], Mpawn[white], PADVNCM, 10);
    CopyBoard (SilverAdvance[black], Msilver[black], SADVNCM, 10);
    CopyBoard (SilverAdvance[white], Msilver[white], SADVNCM, 10);
    CopyBoard (GoldAdvance[black], Mgold[black], GADVNCM, 10);
    CopyBoard (GoldAdvance[white], Mgold[white], GADVNCM, 10);
    CopyBoard (KingAdvance[black], Mking[black], KADVNCM, 1);
    CopyBoard (KingAdvance[white], Mking[white], KADVNCM, 1);

    /* Tables without weighted advances. Must not be updated! */

    for ( side = black; side <= white; side++ ) {
	Mlance[side]  = LanceAdvance[side];
	Mknight[side] = KnightAdvance[side];
	Mbishop[side] = BishopAdvance[side];
	Mrook[side]   = RookAdvance[side];
    }

#ifdef DEBUG_EVAL
    if ( debug_eval ) {
      fprintf(debug_eval_file,"relative pawn table weigth: %d\n",PADVNCM);
      fprintf(debug_eval_file,"relative silver table weigth: %d\n",SADVNCM);
      fprintf(debug_eval_file,"relative gold table weigth: %d\n",GADVNCM);
      fprintf(debug_eval_file,"relative king table weigth: %d\n",KADVNCM);
    }
#endif

#endif /* USE_PATTERN */

#if defined USE_PATTERN
    if ( in_opening_stage ) {
      for (side = black; side <= white; side++)
        UpdatePatterns (side, GameCnt);
    } else {
      ShowPatternCount(black,-1);
      ShowPatternCount(white,-1);
    }
#endif

}



void
ExaminePosition (void)

/*
 * This is done one time before the search is started. Set up arrays Mwpawn,
 * Mbpawn, Mknight, Mbishop, Mking which are used in the SqValue() function
 * to determine the positional value of each piece.
 */

{
    register short i, sq, msq;
    register short fyle;
    short wpadv, bpadv, z, side, pp, j, k, val, Pd, rank;

#ifdef TEST_EXAMINE
    ShowMessage("begin of ExaminePosition");
#endif

    ataks (black, atak[black]);
    ataks (white, atak[white]);

#ifdef TEST_EXAMINE
    ShowMessage("after ataks");
#endif
                      
#ifdef DEBUG_EVAL
    if ( debug_eval ) {
      debug_position (debug_eval_file);
      /* debug_ataks (debug_eval_file); */
    }
#endif

    UpdateWeights ();

#ifdef TEST_EXAMINE
    ShowMessage("after UpdateWeights");
#endif

#ifdef NOMEMSET
    { short c, p;
      for ( c = black; c <= white; c++ )
        for ( p = pawn; p <= king; p++ )
          HasPiece[c][p] = 0;
    }
#else
    memset ((char *)HasPiece, 0,(unsigned long)sizeof(HasPiece));
#endif

    for (side = black; side <= white; side++)
	for (i = PieceCnt[side]; i >= 0; i--)
	    ++HasPiece[side][board[PieceList[side][i]]];

    DetermineGameType();

    /* Build up weighted enemy-king distance tables. */

    if ( KATAK == 0 )
      {
#if defined(NOMEMSET)
#ifdef TEST_EXAMINE
	ShowMessage("just before 0 distance assignment");
#endif
	for (sq = 0; sq < NO_SQUARES; sq++)
	  Kfield[black][sq] = Kfield[white][sq] = 0;
#else
#ifdef TEST_EXAMINE
	ShowMessage("just before memset");
#endif
	memset ((char *) Kfield, 0, (unsigned long)sizeof (Kfield));
#endif /* NOMEMSET */
      }
    else
      {
#ifdef TEST_EXAMINE
	ShowMessage("just before distance assignments");
#endif
	for (sq = 0; sq < NO_SQUARES; sq++)
	  {
	    Kfield[black][sq] = (distance(sq,WhiteKing)==1) ? KATAK : 0;
	    Kfield[white][sq] = (distance(sq,BlackKing)==1) ? KATAK : 0;
	  }
      }

#ifdef DEBUG_EVAL
    if ( debug_eval )
      for (side=black; side<=white; side++)
        {
	  fprintf(debug_eval_file,"%s's pawn table\n\n",ColorStr[side]);
	  debug_table(debug_eval_file,Mpawn[side]);
	  fprintf(debug_eval_file,"%s's silver table\n\n",ColorStr[side]);
	  debug_table(debug_eval_file,Msilver[side]);
	  fprintf(debug_eval_file,"%s's gold table\n\n",ColorStr[side]);
	  debug_table(debug_eval_file,Mgold[side]);
	  fprintf(debug_eval_file,"%s's king table\n\n",ColorStr[side]);
	  debug_table(debug_eval_file,Mking[side]);
	  fprintf(debug_eval_file,"%s's distance to enemy king table\n\n",ColorStr[side]);
	  debug_table(debug_eval_file,Kfield[side]);
        }
#endif

#ifdef TEST_EXAMINE
   ShowMessage("end of ExaminePosition");
#endif

}


void
UpdateWeights (void)

/*
 * If material balance has changed, determine the values for the positional
 * evaluation terms.
 */

{
    register short s1, dio, io, os;

    behind_in_material[black] = ((mtl[white] - mtl[black]) > valueB);
    behind_in_material[white] = ((mtl[black] - mtl[white]) > valueB);
    
    /* GameCnt = number of half moves */
    
    if (GameCnt < 21)
      s1 = 0;			/*  0 <= GameCnt <= 20: stage = 0 */
    else if (GameCnt < 41)
      s1 = 1 + (GameCnt-20)/3;	/* 21 <= GameCnt <= 40: stage = 1..7 */
    else if (GameCnt < 61)
      s1 = 8 + (GameCnt-40)/5;	/* 41 <= GameCnt <= 60: stage = 8..11 */
    else
      s1 = 12 + (GameCnt-60)/7;
    
    if (s1 != stage)
      {                   
	  stage = stage2 = s1;

          os = (GameType[black] == STATIC_ROOK && GameType[white] == STATIC_ROOK) ? 10 : 8;
	  dio = (in_opening_stage = io = (s1 < os)) ? (os - s1) : 0;

#if defined USE_PATTERN
	  ADVNCM[pawn]   = io?2+s1:1; /* advanced pawn bonus increment*/
	  ADVNCM[lance]  = 1;
	  ADVNCM[knight] = 1;
	  ADVNCM[silver] = io?1+s1:1; /* advanced silver bonus increment */
	  ADVNCM[gold]   = io?1+s1:1; /* advanced gold bonus increment */
	  ADVNCM[bishop] = 1;
	  ADVNCM[rook]   = 1;
	  ADVNCM[king]   = io?1+s1:1; /* advanced king bonus increment */
#else
	  PADVNCM = io?12+s1:1;	/* advanced pawn multiplier */
	  SADVNCM = io?15+s1:1;	/* advanced silver multiplier */
	  GADVNCM = io?14+s1:1;	/* advanced gold multiplier */
	  KADVNCM = io?1+s1:0;	/* advanced king multiplier */
#endif
          PMBLTY = 5;		/* pawn mobility */
	  PSTRONG = 3; 		/* pawn supported by rook/lance */
	  BDCLOSED = -(dio*2);	/* bishops diagonal closed by pawn */
	  P2STRONG = 20;	/* attacking pawn on 2nd col is strong */

	  LHOPN = -4;		/* no own pawn on lance file ahead */
	  LHOPNX = 4;		/* no enemy pawn on lance file ahead */
	  LPROTECT = 6;		/* protect own piece with lance */
	  LDNGR = -6;		/* lance is in danger */
	  LXRAY = 8;		/* lance Xray attack on piece */

	  NDNGR = -4;		/* knight is in danger */
	  KNIGHTPOST = 2;	/* knight near enemy pieces */
	  KNIGHTSTRONG = 2;	/* occupies pawn hole */
	  NEDGE = io?-16:-8;	/* knight is on edge file */

          SBEFOREG = 2;  	/* proverb: silver before gold */

	  BISHOPSTRONG = 4;	/* occupies pawn hole */
	  BHCLSD = -3;		/* first or second piece in a bishop's direction is an own pawn */
	  BHCLSDX = -2;		/* first or second piece in a bishop's direction is an opponents pawn */                     
	  BRXRAY = 10;		/* bishop or rook Xray attack on piece */
	  BXCHG = -10;		/* bishop exchange in ranging rook opening */

	  RHOPN = 10;		/* no own pawn on rook file ahead */
	  RHOPNX = 8;           /* no enemy pawn on rook file ahead */
          R2NDCOL = dio*2;	/* rook is on 2nd column */

	  PINVAL = 15;		/* Pin */

	  KHOPN = -8;		/* no own pawn in kings file */
	  KHOPNX = KHOPN * 2;   /* no opponents pawn in kings file */
	  KCASTLD = 20;         /* king protected by castle */
	  KMOVD = 4;		/* proverb: a sitting king is a sitting duck */
	  KATAK = io ? 1 : 8;	/* B,R,L attacks near enemy king */
	  KSFTY = 		/* king safety */
 	    io ? 0 : KINGSAFETY + s1 - 20; 

          OPENOK = dio*3/2;	/* corresponding to opening pattern */
          OPENWRONG = -dio*2;	/* not correpsonding */

          KINGOD = 2+s1/4;	/* distance to own king multiplier */
          KINGED = 3+s1/4;	/* distance to enemy king multiplier */
          PROMD = 2+s1/4;	/* distance to promotion zone multiplier */

	  ATAKD = -15;		/* defender > attacker */
	  HUNGP = -14;		/* each hung piece */
	  HUNGX = -18;		/* extra for >1 hung piece */

#ifdef USE_PATTERN
	  PATTACK = dio; 	/* attack pattern distance multiplier */
	  PCASTLE = dio; 	/* castle pattern distance multiplier */
#endif

      }
} 
