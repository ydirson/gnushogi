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
#include "ataks.h"


short *TrP;

static struct leaf *node;
static short sqking, sqxking;
static short InCheck = false;     


/*
 * Ply limits for deep search cut.
 * No moves or drops flagged with "stupid" are considered beyond ply BEYOND_STUPID.
 * Only moves or drops flagged with "kingattack" are considered beyond ply BEYOND_KINGATTACK.
 * No moves or drops flagged with "questionable" are considered beyond ply BEYOND_QUESTIONABLE.
 * Only moves or drops flagged with "tesuji" are considered beyond ply BEYOND_TESUJI.
 * No drops are considered beyond ply BEYOND_DROP.
 * Exceptions: moves or drops that prevent check or give check are always considered.
 */

#ifdef THINK_C
#define BEYOND_STUPID 2
#define BEYOND_KINGATTACK 4
#define BEYOND_QUESTIONABLE 6
#define BEYOND_TESUJI 6
#define BEYOND_DROP 8
#else
#define BEYOND_STUPID 2
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
	     short int *tempc)	/* color of to square */

/*
 * Update Arrays board[] and color[] to reflect the new board
 * position obtained after making the move pointed to by node.
 */

{
  register short int piece;

  t = t & 0x7f;

  if (f > NO_SQUARES )
    { 
      piece = f - NO_SQUARES;
      if ( side == white ) piece -= NO_PIECES;
      board[t] = piece;
      color[t] = side;
    }
  else
    {
      *tempb = board[t];
      *tempc = color[t];
      piece = board[f];
      color[f] = neutral;
      board[f] = no_piece;
      color[t] = side;
      board[t] = (node->flags & promote) ? promoted[piece] : piece;
    } 
}

static
void
GenUnmakeMove (short int side,
	       short f,
	       short t,
	       short int tempb,
	       short int tempc)

/*
 * Take back a move.
 */

{
  register short piece;

  t = t & 0x7f;
  
  if (f > NO_SQUARES )
    { 
      piece = f - NO_SQUARES;
      if ( piece >= NO_PIECES ) piece -= NO_PIECES;
      board[t] = no_piece;
      color[t] = neutral;
    }
  else
    { 
      piece = board[t];
      color[t] = tempc;
      board[t] = tempb;
      color[f] = side;
      board[f] = (node->flags & promote) ? unpromoted[piece] : piece;
    }
}


inline
static
void
Link (short side, short piece,
      short from, short to, short local_flag, short s) 
{        
    if ( *TrP == TREE ) {
      ShowMessage("TREE overflow\n");
    } else {
      node->f = from; 
      node->t = (local_flag & promote) ? (to | 0x80) : to;
      node->reply = 0;
      node->flags = local_flag;
      node->score = s;
      if ( flag.tsume )
        {
          if ( InCheck )
            {
              short tempb, tempc, sq, threat;
              GenMakeMove (side, node->f, node->t, &tempb, &tempc);
              sq = (from == sqking) ? to : sqking;
	      threat = SqAtakd(sq,side ^ 1);
              GenUnmakeMove (side, node->f, node->t, tempb, tempc);
              if ( !threat ) 
	        (*TrP)++, node++;
            }
          else if ( !(local_flag & check) && (sqxking != to ) && (board[sqxking] == king) )
            {                               
              short tempb, tempc, sq, threat;
              GenMakeMove (side, node->f, node->t, &tempb, &tempc);
	      if ( threat = SqAtakd(sqxking,side) )
	        local_flag |= check;
              GenUnmakeMove (side, node->f, node->t, tempb, tempc);
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
InPromotionZone (short int color, short int sq)
{
  if ( color == black ) {
    if ( sq < 54 ) return(false);
  } else {
    if ( sq > 26 ) return(false);
  };
  return(true);
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
  register short xside;

  xside = side ^ 1;

  s = 0;

  ptyp = ptype[side][piece];
  ppos = (*nextpos[ptyp])[t];
  pdir = (*nextdir[ptyp])[t];

  u = ppos[t];

  do 
    { short coloru = color[u];
      if (distance(u,WhiteKing) <= 1 || distance(u,BlackKing) <= 1 ) {
        /* move controls square near own king or enemy king */
	s += 2;
	*local_flag |= kingattack; 
      }
      if (coloru == side ) {
        /* impossible next move */
        u = pdir[u];
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
	  if ( u == FROMsquare )
             /* opponent has just left this square */
	     s++;
	  u = ppos[u];
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
              if ( upiece == pawn )
		s++;
	      if ( rvupiece == rvuboard && 
                   upiece == pawn || upiece == bishop || upiece == knight ) {
                s++; /* The opposing pawn (piece) */
		if ( upiece == pawn )
		  s++;
              }
	    }
#endif 
	  u = pdir[u];
	}
      };
  } while (u != t);

  return(s);
}


#endif



inline void
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
  register short piece;
  register short principle = false;
  short flag_tsume;

#if defined DROPBONUS || defined FIELDBONUS
  register short side;
  side = xside ^ 1;
#endif

#ifdef HISTORY
  /* Principle variation */
    { 
      unsigned short c, z, ds;
      c = (xside == black);
      z = (f << 8) | t;
      ds = history[hindex(c,z)];
      principle = (ds != 0);
      s += ds;
    }   
#endif

  if ( !flag.tsume ) {

    if (distance(t,WhiteKing) <= 1 || distance(t,BlackKing) <= 1 ) {
      /* bonus for square near own king or enemy king */
      s += 10;
      local_flag |= kingattack; 
    }

    if ( f > NO_SQUARES )
      { 
       /* bonus for drops, in order to place the before questionable moves */
        s += 10;
        if (t == FROMsquare) {
          /* drop to the square the opponent has just left */
          s++;
        };
#if defined DROPBONUS
        piece = f - NO_SQUARES; 
        if (piece > NO_PIECES) piece -= NO_PIECES;
        s += field_bonus(ply,side,piece,f,t,&local_flag);
        if ( s == 10 && piece != pawn )
  	  local_flag |= questionable;
#endif
      }
    else 
      {
        /* bonus for moves (non-drops) */
        int consider_last = false;
        piece = board[f];
        s += 20; 
        if (t == FROMsquare) {
          /* move to the square the opponent has just left */
          s++;
        }
        if (color[t] != neutral)
          {
            /* Captures */  
            s +=  value[board[t]] - relative_value[piece];
            if (t == TOsquare)
              /* Capture of last moved piece */ 
              s += 500;
          }  
        if ( local_flag & promote )
          {
            /* bonus for promotions */
            s++;
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
              if ( !principle )
  	        {
	          consider_last = true;
                  if ( piece == pawn || piece == bishop || piece == rook )
		    local_flag |= stupid;
		  else
		    local_flag |= questionable;
	        }
          }
        if ( consider_last )
	  {
	    if ( local_flag & stupid )
	      s = 0;
	    else 
              s = s % 20;
	  }
#if defined FIELDBONUS
        else 
      	  {
            s += field_bonus(ply,side,piece,f,t,&local_flag);
          }
#endif
      }

  }

  flag_tsume = flag.tsume;

  /* check conditions for deep search cut */

  if ( !flag.tsume )      
  if ( *TrP > TREE - 300 ) {
    /* too close to tree table limit */
    flag.tsume = true;
  }
#ifdef DEEPSEARCHCUT
    else if ( *TrP > TREE - 1000 ) {
    if ( ply > BEYOND_STUPID  && (local_flag & questionable) ) {
      flag.tsume = true; 
    } else if ( ply > BEYOND_KINGATTACK && !(local_flag & kingattack) ) {
      flag.tsume = true;
    } else if ( ply > BEYOND_QUESTIONABLE && (local_flag & stupid) ) {
      flag.tsume = true; 
    } else if ( ply > BEYOND_TESUJI && !(local_flag & tesuji) ) {
      flag.tsume = true;
    } else if ( ply > BEYOND_DROP && (f > NO_SQUARES) ) {
      flag.tsume = true;
    } 
  }
#endif

  Link (side, piece, 
        f, t, local_flag, s - ((SCORE_LIMIT+1000)*2));

  flag.tsume = flag_tsume;
}                                                 




inline
static
void
GenDrops (register short int ply, register short int piece, short int side,
	  short int xside)

/*
 * Generate drops for a piece.
 */

{ register short u; 

  for (u = 0; u < NO_SQUARES; u++)

    { short r = row(u), possible = true;

      if ( board[u] != no_piece )
         possible = false; 
      else if ( piece == pawn )
        { 
          if ( side == black && r == 8 )
            possible = false;
          else if ( side == white && r == 0 )
            possible = false;
          else if ( PawnCnt[side][column(u)] )
            possible = false;
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

      if ( possible )
       { short f;
         f = NO_SQUARES + piece;
         if ( side == white ) f += NO_PIECES;
         LinkMove (ply, f, u, (dropmask | piece), xside);
       }
  
    }

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
  register unsigned char *ppos, *pdir;
  short ptyp;

  TrP = &TrPnt[ply + 1];
  piece = board[sq];
  ptyp = ptype[side][piece];
  ppos = (*nextpos[ptyp])[sq];
  pdir = (*nextdir[ptyp])[sq];

  u = ppos[sq];
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
        u = ppos[u];
      else
        u = pdir[u];
  } while (u != sq);
}



void
MoveList (short int side, register short int ply)

/*
 * Fill the array Tree[] with all available moves for side to play. Array
 * TrPnt[ply] contains the index into Tree[] of the first move at a ply.
 */

{
  register short i, xside, f;
  struct leaf *firstnode;

  xside = side ^ 1;
  if ( flag.tsume )
    {
      sqking  = PieceList[side][0];
      sqxking = PieceList[xside][0];
      InCheck = (board[sqking] == king) ? SqAtakd(sqking,xside) : false;
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
      GenDrops (ply, i, side, xside);
#ifdef HISTORY
  history[hindex(side,SwagHt)] -= 5000;
  history[hindex(side,Swag0)] -= 2000;
  history[hindex(side,Swag1)] -= 60;
  history[hindex(side,Swag2)] -= 50;
  history[hindex(side,Swag3)] -= 40;
  history[hindex(side,Swag4)] -= 30;
#endif
  SwagHt = 0;			/* SwagHt is only used once */
  if ( 0 /* flag.tsume */ )
    if ( node == firstnode )
      (*TrP)++;
  GenCnt += (TrPnt[ply+1] - TrPnt[ply]);
}

void
CaptureList (register short int side, short int ply)

/*
 * Fill the array Tree[] with all available captures for side to play.
 * If there is a non-promote option, discard the non-promoting move.
 * Array TrPnt[ply] contains the index into Tree[] of the first move
 * at a ply.      
 * If flag.tsume is set, add moves (even non-promoting moves)
 * that threatens the opponents piece.
 */

{
  register short u, sq, xside;
  register unsigned char *ppos, *pdir;
  short i, piece, flag_tsume;
  small_short *PL;
  struct leaf *firstnode;

  xside = side ^ 1;

  if ( flag.tsume )
    {
      sqking = PieceList[side][0];
      sqxking = PieceList[xside][0];
      InCheck = (board[sqking] == king) ? SqAtakd(sqking,xside) : false;
    }

  TrP = &TrPnt[ply + 1];
  *TrP = TrPnt[ply];
  firstnode = node = &Tree[*TrP];

  flag_tsume = flag.tsume;

  if ( *TrP > TREE - 300 ) 
    {
      /* too close to tree table limit */
      flag.tsume = true;
    }

  PL = PieceList[side];

  for (i = 0; i <= PieceCnt[side]; i++)
    { short ptyp;
      sq = PL[i];
      piece = board[sq];
      ptyp = ptype[side][piece];
      ppos = (*nextpos[ptyp])[sq];
      pdir = (*nextdir[ptyp])[sq];
      u = ppos[sq];
      do
	{ 
	  if (color[u] == neutral)
	    { 
	      u = ppos[u];
	    }
	  else
	    { 
	      if ( color[u] == xside && board[u] != king )
		{ 
		  short PP;
		  if ( PP = PromotionPossible(color[sq],sq,u,piece) ) {
                    Link (side, piece, 
			  sq, u, capture | promote,
                          value[board[u]] + svalue[board[u]] 
                            - relative_value[piece]);
                  } 
		  if ( !PP || flag.tsume ) {
                    Link (side, piece, 
			  sq, u, capture,
                          value[board[u]] + svalue[board[u]] 
                            - relative_value[piece]);
                  }  
	   	}
	      u = pdir[u];
	    }
	} while (u != sq);
    }

  flag.tsume = flag_tsume;

  SwagHt = 0;			/* SwagHt is only used once */

} 

