/*
 * genmoves.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993 Matthias Mutz
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback
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

#include "gnushogi.h"
      
#ifdef DEBUG
#include <assert.h>
#endif

#ifdef DEBUG_EVAL
extern short debug_eval;
extern FILE *debug_eval_file;
extern short debug_moves;
#endif

short *TrP;

static struct leaf *node;
static short sqking, sqxking;
static short InCheck = false, GenerateAllMoves = false;     

static short INCscore = 0;

short deepsearchcut = true;

/*
 * Ply limits for deep search cut.
 * No moves or drops flagged with "stupid" are considered beyond ply BEYOND_STUPID.
 * Only moves or drops flagged with "kingattack" are considered beyond ply BEYOND_KINGATTACK.
 * No moves or drops flagged with "questionable" are considered beyond ply BEYOND_QUESTIONABLE.
 * Only moves or drops flagged with "tesuji" are considered beyond ply BEYOND_TESUJI.
 * No drops are considered beyond ply BEYOND_DROP.
 * Exceptions: moves or drops that prevent check or give check are always considered.
 */
                       
#if defined THINK_C || defined MSDOS
#define BEYOND_STUPID 0
#define BEYOND_KINGATTACK 4
#define BEYOND_QUESTIONABLE 6
#define BEYOND_TESUJI 6
#define BEYOND_DROP 8
#else
#define BEYOND_STUPID 0
#define BEYOND_KINGATTACK 6
#define BEYOND_QUESTIONABLE 8
#define BEYOND_TESUJI 8
#define BEYOND_DROP 10
#endif

inline
static
void
GenMakeMove (short int side,
	     short f,
	     short t,
	     short int *tempb,	/* piece at to square */
	     short int *tempc,	/* color of to square */
	     short int checkp)

/*
 * Update Arrays board[] and color[] to reflect the new board
 * position obtained after making the move pointed to by node.
 */

{
  register short int piece;

  t = t & 0x7f;
      
#ifdef DEBUG
  assert(f!=NO_SQUARES);
#endif

  if (f > NO_SQUARES )
    { 
      piece = f - NO_SQUARES;
      if ( piece > NO_PIECES ) piece -= NO_PIECES;
      board[t] = piece;
      color[t] = side;
      Captured[side][piece]--;
    }
  else
    {
      *tempb = board[t];
      *tempc = color[t];
      if ( *tempb != no_piece )
	Captured[side][unpromoted[*tempb]]++;
      piece = board[f];
      color[f] = neutral;
      board[f] = no_piece;
      color[t] = side;
      if ( checkp )
        board[t] = (node->flags & promote) ? promoted[piece] : piece;
      else
        board[t] = piece;
    } 
#ifdef DEBUG
    assert(Captured[black][0]==0 && Captured[white][0]==0);
#endif
}

static
void
GenUnmakeMove (short int side,
	       short f,
	       short t,
	       short int tempb,
	       short int tempc,
	       short int checkp)

/*
 * Take back a move.
 */

{
  register short piece;

  t = t & 0x7f;

#ifdef DEBUG
  assert(f!=NO_SQUARES);
#endif
  
  if (f > NO_SQUARES )
    { 
      piece = f - NO_SQUARES;
      if ( piece > NO_PIECES ) piece -= NO_PIECES;
      board[t] = no_piece;
      color[t] = neutral;
      Captured[side][piece]++;
    }
  else
    { 
      piece = board[t];
      color[t] = tempc;
      board[t] = tempb;
      if ( tempb != no_piece )
        Captured[side][unpromoted[tempb]]--;
      color[f] = side; 
      if ( checkp )
        board[f] = (node->flags & promote) ? unpromoted[piece] : piece;
      else
        board[f] = piece;
    }
#ifdef DEBUG
    assert(Captured[black][0]==0 && Captured[white][0]==0);
#endif
}


inline
static
void
Link (short side, short piece,
      short from, short to, short local_flag, short s) 
{        
#ifdef notdef
    if (debug_eval ) {
  	fprintf ( debug_eval_file, "link from %d to %d InCheck %d tsume %d\n",
	  from, to, InCheck, flag.tsume );
    }
#endif

    if ( *TrP == TREE ) {
#ifdef NONDSP
      printf("TREE overflow\n");
#else
      ShowMessage("TREE overflow\n");
#endif
    } else {
      node->f = from; 
      node->t = (local_flag & promote) ? (to | 0x80) : to;
      node->reply = 0;
      node->flags = local_flag;
      node->score = s;
      node->INCscore = INCscore;
      if ( GenerateAllMoves )
        {
	  (*TrP)++, node++;
        }
      else if ( InCheck || (flag.tsume && !(local_flag & capture)) )
        {
          if ( InCheck )
            {
              short tempb, tempc, sq, threat, blockable;
              GenMakeMove (side, node->f, node->t, &tempb, &tempc, true);
              sq = (from == sqking) ? to : sqking;
	      threat = SqAtakd(sq,side ^ 1, &blockable);
#ifdef DEBUG_EVAL
  	      if (debug_eval ) {
    		fprintf ( debug_eval_file, "from %d to %d threat %d\n",
		  node->f, node->t, threat );
	      }
#endif
              GenUnmakeMove (side, node->f, node->t, tempb, tempc, true);
              if ( !threat ) 
	        (*TrP)++, node++;
            }
          else if ( !(local_flag & check) && (sqxking != to ) && (board[sqxking] == king) )
            {                               
              short tempb, tempc, sq, threat, blockable;
              GenMakeMove (side, node->f, node->t, &tempb, &tempc, true);
	      if ( threat = SqAtakd(sqxking,side, &blockable) )
	        node->flags |= check;
              GenUnmakeMove (side, node->f, node->t, tempb, tempc, true);
	      if ( threat ) 
	        (*TrP)++, node++;
            }
          else
	    (*TrP)++, node++;
        }
      else
	(*TrP)++, node++;
    }
}


inline
int
PromotionPossible (short int color, short int f, short int t, short int p)
{ 
  if ( color == black ) {
    if ( f < 54 && t < 54 ) return(false);
  } else {
    if ( f > 26 && t > 26 ) return(false);
  };
 
  switch ( p ) {
    case pawn: return(true);
    case lance: return(true);
    case knight: return(true);
    case silver: return(true);
    case bishop: return(true);
    case rook: return(true);
  };
 
  return(false);
}


inline
int
NonPromotionPossible (short int color, short int f, short int t, short int p)
{
  switch ( p ) {
    case pawn : 
           if ( color == black )
             return ( t < 72 );
           else
             return ( t > 8 );
    case lance: 
           if ( color == black )
             return ( t < 72 );
           else
             return ( t > 8 );
    case knight:
           if ( color == black )
             return ( t < 63 );
           else
             return ( t > 17 );
  };

  return(true);
}


#if defined FIELDBONUS || defined DROPBONUS

inline
static 
short
field_bonus (short int ply, short int side, short int piece, short int f, short int t, 
	     short int *local_flag)

/* bonus for possible next moves */

{
  register short s, u, ptyp;
  register unsigned char *ppos, *pdir;
  register short c1, c2;
#ifdef SAVE_NEXTPOS
  short d;
#endif
      
  c1 = side;
  c2 = side ^ 1;

  s = 0;

  ptyp = ptype[side][piece];

#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,t); 
#else
  ppos = (*nextpos[ptyp])[t];
  pdir = (*nextdir[ptyp])[t];
  u = ppos[t];
#endif

  do 
    { short coloru = color[u];
      if ( piece != king && GameCnt > 40 ) {
        if ( distance(u,EnemyKing) <= 1 ) {
          /* can reach square near own king */
	  s += 2;      
	  *local_flag |= kingattack;
        } else if ( distance(u,OwnKing) <= 1 ) {
          /* can reach square near enemy king */
	  s++;
	  *local_flag |= kingattack;
	}
      }
      if (coloru == side ) {
        /* impossible next move */
#ifdef SAVE_NEXTPOS
	u = next_direction(ptyp,&d,t);
#else
        u = pdir[u];
#endif
      } else {
        /* possible next move */
        if (PromotionPossible(side,t,u,piece)) {
          /* possible promotion in next move */
	  if ( piece == pawn )
            {
	      s += 2;
#ifdef TESUJIBONUS
              if ( !InPromotionZone(side,t) ) {
                *local_flag |= tesuji; /* The dangling pawn */ 
		s++;
              }
#endif
            }
          else
	    s++; 
        }
        if (coloru == neutral) {
          /* next move to an empty square */
	  if ( u == FROMsquare ) {
             /* opponent has just left this square */
	     s++;
	  }
#ifdef SAVE_NEXTPOS
	  u = next_position(ptyp,&d,t,u);
#else
	  u = ppos[u];
#endif
        } else {
          /* attack opponents piece */
#ifdef TESUJIBONUS
          short boardu, upiece, rvupiece, rvuboard;
#endif
	  s++;
	  if ( u == TOsquare )
	    /* opponent has moved to TOsquare */
	    s++;
          if ( (boardu = board[u]) == king ) {
	      s += 20; INCscore -= 18;
	      *local_flag |= check; /* move threatens opponents king */ 
	  }
#ifdef TESUJIBONUS
          upiece = unpromoted[piece];
          rvupiece = relative_value[upiece];
          rvuboard = relative_value[unpromoted[boardu]];
          if ( upiece == pawn && Captured[side][pawn] > 1 )
	    {                
	      *local_flag |= tesuji; /* The joining pawn attack */
	      s++;
	    }     
	  if ( rvupiece <= rvuboard ) 
	    {                
	      *local_flag |= tesuji; /* The striking pawn (piece) attack */
              if ( f > NO_SQUARES )
	        s += 2;
              else
		s++;
              if ( upiece == pawn ) {
		s++;
	      }
	      if ( rvupiece == rvuboard && 
                   upiece == pawn || upiece == bishop || upiece == knight ) {
                s++; /* The opposing pawn (piece) */
		if ( upiece == pawn )
		  s++;
              }
	    }
#endif
#ifdef SAVE_NEXTPOS
	  u = next_direction(ptyp,&d,t);
#else 
	  u = pdir[u];
#endif
	}
      };
  } while (u != t);

  INCscore += s;

  return(s);
}


#endif



/* inline */ void
LinkMove (short int ply, short int f,
	  register short int t,
	  short int local_flag,
	  short int xside)

/*
 * Add a move to the tree.  Assign a bonus to order the moves as follows:
 * 1. Principle variation 2. Capture of last moved piece 3. Other captures
 * (major pieces first) 4. Killer moves 5. Tesuji drops 6. Other Moves
 * 7. Other drops. 8. Non-promoting moves
 * If the flag.tsume is set, assign a high bonus for checks.
 */

{
  register short s = 0;
  register short side, piece, mv;
  short flag_tsume, try_link = true;
  short c1, c2;

  c1 = side = xside ^ 1;
  c2 = xside;

  mv = (f << 8) | ((local_flag & promote) ? (t | 0x80) : t);

  if ( f > NO_SQUARES ) {
    piece = f - NO_SQUARES;
    if ( piece > NO_PIECES ) piece -= NO_PIECES;
  } else {
    piece = board[f];
  }

  INCscore = 0;
       
#ifdef HISTORY
  s += history[hindex(side,mv)];
#ifdef DEBUG_EVAL
  if ( debug_eval && (ply == 1 || debug_moves) )
    {
      char buf[8];
      movealgbr(mv,buf);
      fprintf(debug_eval_file, "mv=%s history=%d\n", 
        buf,history[hindex(side,mv)]); 
    }
#endif
#endif
         
  flag_tsume = flag.tsume;

  /* If we're running short of tree node, go into tsume mode. */

  if ( !(local_flag & capture) )
    if ( *TrP > TREE - 300 ) {
      /* too close to tree table limit */
      flag.tsume = true;
    }

  /* Guess strength of move and set flags. */                

  if ( piece != king && GameCnt > 40 ) {
    if ( distance(t,EnemyKing) <= 1 ) {
      /* bonus for square near enemy king */
      s += 15; INCscore += 2;
      local_flag |= kingattack; 
    } else if ( distance(t,OwnKing) <= 1 ) {
      /* bonus for square near own king */
      s += 10; INCscore++;
      local_flag |= kingattack; 
    }                                   
  }

  if ( f > NO_SQUARES )
    {
     /* bonus for drops, in order to place drops before questionable moves */
      s += 10; INCscore++;
      if (t == FROMsquare) {
        /* drop to the square the opponent has just left */
        s++; INCscore++;
      };
#if defined DROPBONUS
      s += field_bonus(ply,side,piece,f,t,&local_flag);
      if ( s == 10 && piece != pawn )
      local_flag |= questionable;
#endif
    }
  else 
    {
      /* bonus for moves (non-drops) */
      int consider_last = false;
      s += 20; 
      if (t == FROMsquare) {
        /* move to the square the opponent has just left */
        s++; INCscore++;
      }
      if (color[t] != neutral)
        {
          /* Captures */  
          s += value[board[t]] - relative_value[piece];
          if (t == TOsquare)
            /* Capture of last moved piece */ 
            s += 500; INCscore += 5;
        }  
      if ( local_flag & promote )
        {
          /* bonus for promotions */
          s++; INCscore++;
        }
      else
        {
          /* bonus for non-promotions */
          if ( PromotionPossible(side,f,t,piece) )
#ifdef TESUJIBONUS
         /* Look at non-promoting silver or knight */
            if ( piece == silver || piece == knight ) 
              {
                local_flag |= tesuji; /* Non-promotion */
                s++; 
              }
            else           
#endif        
	      {
	        consider_last = true;
                if ( piece == pawn || piece == bishop || piece == rook ) {
	   	  local_flag |= stupid;
		  INCscore -= 20;
		} else {
		  local_flag |= questionable;
		  INCscore -= 10;
		}
	      }
        }
      if ( consider_last )
	{
	  if ( local_flag & stupid )
	    s = 0;
	  else 
            s = s % 20;
	}
      else 
      	{
#if defined FIELDBONUS
          s += field_bonus(ply,side,piece,f,t,&local_flag);
#else
	  /* determine check flag */
#endif
        }  
    }

    /* check conditions for deep search cut (flag.tsume = true) */

#ifdef DEEPSEARCHCUT
  if ( !flag.tsume && deepsearchcut ) 
    {
      if ( ply > BEYOND_STUPID && (local_flag & stupid) ) {
	  try_link = flag.force || (ply == 1 && side != computer);
      } else if ( ply > BEYOND_KINGATTACK && !(local_flag & kingattack) ) {
          flag.tsume = true;
      } else if ( ply > BEYOND_QUESTIONABLE && (local_flag & questionable) ) {
          flag.tsume = true; 
      } else if ( ply > BEYOND_TESUJI && !(local_flag & tesuji) ) {
          flag.tsume = true;
      } else if ( ply > BEYOND_DROP && (f > NO_SQUARES) ) {
          flag.tsume = true;
      }
    }
#endif

  if ( try_link || GenerateAllMoves )
    Link (side, piece, 
        f, t, local_flag, s - ((SCORE_LIMIT+1000)*2));

  flag.tsume = flag_tsume;
}                                                 



short
DropPossible (short int piece, short int side, short int sq)

{ 
  short r = row(sq), possible = true;

  if ( board[sq] != no_piece )
        possible = false; 
  else if ( piece == pawn )
        { 
          if ( side == black && r == 8 )
            possible = false;
          else if ( side == white && r == 0 )
            possible = false;
          else if ( PawnCnt[side][column(sq)] )
            possible = false;
	  /* pawn drops are invalid, if they mate the opponent */
	  if ( possible )
	    {   short f, tempb, tempc;
		f = pawn + NO_SQUARES;
		if ( side == white ) f += NO_PIECES;
		GenMakeMove (side, f, sq, &tempb, &tempc, false); 
	        if ( IsCheckmate(side^1,-1,-1) )
		  possible = false;
                GenUnmakeMove (side, f, sq, tempb, tempc, false);
	    }
        }
  else if ( piece == lance )
        {
          if ( side == black && r == 8 )
            possible = false;
          else if ( side == white && r == 0 )
            possible = false;          
        }
  else if ( piece == knight )
        {
          if ( side == black && r >= 7 )
            possible = false;
          else if ( side == white && r <= 1 )
            possible = false;
        }           

  return possible;

}


static
void
SortMoves(short int ply)
{ 
  short int p;
  for (p = TrPnt[ply]; p < TrPnt[ply+1]; p++)
    pick(p,TrPnt[ply+1]-1);
}


inline
void
GenMoves (register short int ply, register short int sq, short int side, 
	  short int xside)

/*
 * Generate moves for a piece. The moves are taken from the precalulated
 * array nextpos/nextdir. If the board is free, next move is choosen from
 * nextpos else from nextdir.
 */

{
  register short u, piece, col;
  short ptyp;
#ifdef SAVE_NEXTPOS
  short d;
#else
  register unsigned char *ppos, *pdir;
#endif

  piece = board[sq];
  ptyp = ptype[side][piece];

#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,sq);
#else
  ppos = (*nextpos[ptyp])[sq];
  pdir = (*nextdir[ptyp])[sq];
  u = ppos[sq];
#endif

  do
    { short int local_flag, c;
      if ( (c = color[u]) == xside )
        local_flag = capture;
      else
        local_flag = 0;
      if ( c != side && board[u] != king ) {
        if ( PromotionPossible(color[sq],sq,u,piece) ) {
          LinkMove (ply, sq, u, local_flag | promote, xside);
          if ( NonPromotionPossible(color[sq],sq,u,piece) )
            LinkMove (ply, sq, u, local_flag, xside);
        } else {
          LinkMove (ply, sq, u, local_flag, xside);
        }
      }
      if (c == neutral)
#ifdef SAVE_NEXTPOS
        u = next_position(ptyp,&d,sq,u);
      else
        u = next_direction(ptyp,&d,sq);
#else
        u = ppos[u];
      else
        u = pdir[u];
#endif
  } while (u != sq);
}



void
MoveList (short int side, register short int ply, short int in_check)

/*
 * Fill the array Tree[] with all available moves for side to play. Array
 * TrPnt[ply] contains the index into Tree[] of the first move at a ply.
 * in_check = 0 side is not in check
 * in_check > 1 side is in check
 * in_check < 0 don't know
 * in_check -2 indicates move generation for book moves
 */

{
  register short i, xside, f, u;
  struct leaf *firstnode;
  short flag_tsume, blockable;

  xside = side ^ 1;

  flag_tsume = flag.tsume;

  sqking  = PieceList[side][0];
  sqxking = PieceList[xside][0];

  if ( in_check >= 0 )
    InCheck = in_check;
  else
    InCheck = (board[sqking] == king) ? SqAtakd(sqking,xside, &blockable) : false;

  GenerateAllMoves = (in_check == -2);

#ifdef DEBUG_EVAL
  if ( debug_eval )
    fprintf ( debug_eval_file, "%s king is %sin check\n",
	ColorStr[side],(InCheck ? "" : "not ") );
#endif

  if ( InCheck /* && (ply > 1 || side == computer) */ )
    {
	/* Own king in check */
	flag.tsume = true;
    }

  TrP = &TrPnt[ply + 1];
  *TrP = TrPnt[ply];

  firstnode = node = &Tree[*TrP];

  if (!PV)
    Swag0 = killr0[ply];
   else Swag0 = PV;
  Swag1 = killr1[ply];
  Swag2 = killr2[ply];
  Swag3 = killr3[ply];
  if (ply > 2)
    Swag4 = killr1[ply - 2]; else Swag4 = 0;

#ifdef DEBUG_EVAL
  if ( debug_eval && (ply == 1 || debug_moves) )
  {
    char bufHt[8], buf0[8], buf1[8], buf2[8], buf3[8], buf4[8];
    movealgbr(SwagHt,bufHt);
    movealgbr(Swag0,buf0);
    movealgbr(Swag1,buf1);
    movealgbr(Swag2,buf2);
    movealgbr(Swag3,buf3);
    movealgbr(Swag4,buf4);
    fprintf(debug_eval_file, "SwagHt=%s 0=%s 1=%s 2=%s 3=%s 4=%s\n", 
      bufHt, buf0, buf1, buf2, buf3, buf4); 
  }
#endif

#ifdef HISTORY
  history[hindex(side,SwagHt)] += 5000;
  history[hindex(side,Swag0)] += 2000;
  history[hindex(side,Swag1)] += 60;
  history[hindex(side,Swag2)] += 50;
  history[hindex(side,Swag3)] += 40;
  history[hindex(side,Swag4)] += 30;
#endif

  for (i = PieceCnt[side]; i >= 0; i--)
    GenMoves (ply, PieceList[side][i], side, xside);
  for (i = pawn; i < king; i++)
    if ( Captured[side][i] )
      for (u = 0; u < NO_SQUARES; u++)
        if ( DropPossible(i,side,u) )
          { short f;
            f = NO_SQUARES + i;
            if ( side == white ) f += NO_PIECES;
            LinkMove (ply, f, u, (dropmask | i), xside);
          }

#ifdef DEBUG_EVAL
  SortMoves(ply);
#endif

#ifdef HISTORY
  history[hindex(side,SwagHt)] -= 5000;
  history[hindex(side,Swag0)] -= 2000;
  history[hindex(side,Swag1)] -= 60;
  history[hindex(side,Swag2)] -= 50;
  history[hindex(side,Swag3)] -= 40;
  history[hindex(side,Swag4)] -= 30;
#endif

  SwagHt = 0;			/* SwagHt is only used once */

  if ( flag.tsume && node == firstnode )
    (*TrP)++;

  GenCnt += (TrPnt[ply+1] - TrPnt[ply]);

  flag.tsume = flag_tsume;

}

void
CaptureList (register short int side, short int ply, short int in_check)

/*
 * Fill the array Tree[] with all available captures for side to play.
 * If there is a non-promote option, discard the non-promoting move.
 * Array TrPnt[ply] contains the index into Tree[] of the first move
 * at a ply.      
 * If flag.tsume is set, only add captures (but also the non-promoting)
 * that threaten the opponents king.
 * in_check = 0 side is not in check
 * in_check > 1 side is in check
 * in_check < 0 don't know
 */

{
  register short u, sq, xside;
#ifdef SAVE_NEXTPOS
  short d;
#else
  register unsigned char *ppos, *pdir;
#endif
  short i, piece, flag_tsume, blockable;
  small_short *PL;

  xside = side ^ 1;

  TrP = &TrPnt[ply + 1];
  *TrP = TrPnt[ply];
  node = &Tree[*TrP];

  flag_tsume = flag.tsume;

  sqking = PieceList[side][0];
  sqxking = PieceList[xside][0];

  if ( in_check >= 0 )
    InCheck = in_check;
  else
    InCheck = (board[sqking] == king) ? SqAtakd(sqking,xside,&blockable) : false;

  GenerateAllMoves = (in_check == -2);

#ifdef DEBUG_EVAL
  if (debug_eval )
    fprintf ( debug_eval_file, "%s king is %sin check\n",
	ColorStr[side],(InCheck ? "" : "not ") );
#endif

  if ( InCheck && (ply > 1 || side == computer) )
    {
      /* Own king is in check */
      flag.tsume = true;
    }

  PL = PieceList[side];

  for (i = 0; i <= PieceCnt[side]; i++)
    { short ptyp;
      sq = PL[i];
      piece = board[sq];
      ptyp = ptype[side][piece];
#ifdef SAVE_NEXTPOS
      u = first_direction(ptyp,&d,sq);
#else
      ppos = (*nextpos[ptyp])[sq];
      pdir = (*nextdir[ptyp])[sq];
      u = ppos[sq];
#endif
      do
	{ 
	  if (color[u] == neutral)
	    {
#ifdef SAVE_NEXTPOS
	      u = next_position(ptyp,&d,sq,u);
#else 
	      u = ppos[u];
#endif
	    }
	  else
	    { 
	      if ( color[u] == xside && board[u] != king )
		{ 
		  short PP;
		  if ( PP = PromotionPossible(color[sq],sq,u,piece) ) {
                    Link (side, piece, 
			  sq, u, capture | promote,
                          value[board[u]]
#if !defined SAVE_SVALUE 
			    + svalue[board[u]]
#endif 
                            - relative_value[piece]);
                  } 
		  if ( !PP || flag.tsume ) {
                    Link (side, piece, 
			  sq, u, capture,
                          value[board[u]]
#if !defined SAVE_SVALUE 
			    + svalue[board[u]]
#endif 
                            - relative_value[piece]);
                  }  
	   	}
#ifdef SAVE_NEXTPOS
	      u = next_direction(ptyp,&d,sq);
#else
	      u = pdir[u];
#endif
	    }
	} while (u != sq);
    }

  flag.tsume = flag_tsume;

  SwagHt = 0;			/* SwagHt is only used once */

#ifdef DEBUG_EVAL
  SortMoves(ply);
#endif

} 



short
IsCheckmate (short int side, short int in_check, short int blockable)

/*
 * If the king is in check, try to find a move that prevents check.
 * If such a move is found, return false, otherwise return true.
 * in_check = 0: side is not in check
 * in_check > 1: side is in check
 * in_check < 0: don't know      
 * blockable > 0 && check: check can possibly prevented by a drop
 * blockable = 0 && check: check can definitely not prevented by a drop
 * blockable < 0 && check: nothing known about type of check
 */

{
  register short u, sq, xside;
#ifdef SAVE_NEXTPOS
  short d;
#else
  register unsigned char *ppos, *pdir;
#endif
  short i, piece, flag_tsume;
  small_short *PL;
  struct leaf *firstnode;
  short tempb, tempc, ksq, threat, dummy, sqking;
  short InCheck;

  xside = side ^ 1;

  sqking = PieceList[side][0];
         
  /* 
   * Checkmate is possible only if king is in check.
   */                  

  if ( in_check >= 0 )
      InCheck = in_check;
  else if ( board[sqking] == king )
      InCheck = SqAtakd(sqking,xside,&blockable);
  else
      InCheck = false;

  if ( !InCheck )
    return (false);

  /* 
   * Try to find a move, that prevents check.
   */                  

  PL = PieceList[side];

  for (i = 0; i <= PieceCnt[side]; i++)
    { short ptyp;
      sq = PL[i];
      piece = board[sq];
      ptyp = ptype[side][piece];
#ifdef SAVE_NEXTPOS
      u = first_direction(ptyp,&d,sq);
#else
      ppos = (*nextpos[ptyp])[sq];
      pdir = (*nextdir[ptyp])[sq];
      u = ppos[sq];
#endif
      do
	{ 
	  if (color[u] == neutral || color[u] == xside)
	    {          
              GenMakeMove (side, sq, u, &tempb, &tempc, false);
              ksq = (sq == sqking) ? u : sqking;
	      threat = SqAtakd(ksq,xside,&dummy);
              GenUnmakeMove (side, sq, u, tempb, tempc, false);
	      if ( !threat )
		return (false);
	    }
#ifdef SAVE_NEXTPOS
	    u = (color[u] == neutral) ? next_position(ptyp,&d,sq,u) 
				      : next_direction(ptyp,&d,sq); 
#else
	    u = (color[u] == neutral) ? ppos[u] : pdir[u]; 
#endif
	} while (u != sq);
    }
    
  /*
   * Couldn't find a move that prevents check.
   * Try to find a drop, that prevents check.
   */

  if ( blockable != 0 )
    {    
      for (piece = king-1; piece >= pawn; piece--)
	if ( Captured[side][piece] )
	  {
	    for (u = 0; u < NO_SQUARES; u++)
	      if ( DropPossible(piece,side,u) )
		{ short f;
		  f = NO_SQUARES + piece;
		  if ( side == white ) f += NO_PIECES;
		  GenMakeMove (side, f, u, &tempb, &tempc, false); 
	          threat = SqAtakd(sqking,xside,&dummy);
                  GenUnmakeMove (side, f, u, tempb, tempc, false);
	          if ( !threat )
		    return (false);
		} 
	    /*
	     * If the piece could be dropped at any square, it is
	     * impossible for any other piece drop to prevent check.
	     * Drops are restricted for pawns, lances, and knights.
	     */
	    if ( piece > knight )
		break;
	  }
     }                  

   return (true);

} 


