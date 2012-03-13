/*
 * ataks.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
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
 * the Free Software Foundation.
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


#if defined DEBUG

void 
ataks (short int side, long int *a)
/*
 * Fill array atak[][] with info about ataks to a square.  Bits 16-31 are set
 * if the piece (king..pawn) ataks the square.  Bits 0-15 contain a count of
 * total ataks to the square.
 */  
                                                            
{
  register short u, sq;
  long int c;
#ifdef SAVE_NEXTPOS
  short d;
#else
  register unsigned char *ppos, *pdir;
#endif
  short i, piece; 
  small_short *PL;

  array_zero (a, NO_SQUARES * sizeof (a[0]));

  PL = PieceList[side];
  for (i = PieceCnt[side]; i >= 0; i--)
    { short ptyp;
      sq = PL[i];
      piece = board[sq];
      ptyp = ptype[side][piece];
      c = control[piece];
#ifdef SAVE_NEXTPOS
      u = first_direction(ptyp,&d,sq);
#else
      ppos = (*nextpos[ptyp])[sq];
      pdir = (*nextdir[ptyp])[sq];
      u = ppos[sq];
#endif
      do {
          a[u] = ((a[u]+1) | c);
#ifdef SAVE_NEXTPOS
          u = ((color[u] == neutral) ? next_position(ptyp,&d,sq,u)
				     : next_direction(ptyp,&d,sq));
#else
          u = ((color[u] == neutral) ? ppos[u] : pdir[u]);
#endif
      } while (u != sq);
   }
}

#endif


#if defined DEBUG || defined DEBUG_EVAL

void
debug_ataks (FILE *D, long *atk)
{              
	short l,c,i;         
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

#endif


#define CHECK_DISTANCE


int
SqAtakd (short int square, short int side, short int *blockable)

/*
 * See if any piece with color 'side' ataks sq.
 * *blockable == attack could be blocked by drop  
 */

{
#ifdef SAVE_NEXTPOS
  short d;
#else
  register unsigned char *ppos, *pdir;
#endif
  register short u, ptyp;

  if ( MatchSignature(threats_signature[side]) ) {
#ifdef DEBUG  
    short i,n, sq;
    long int a[NO_SQUARES];
    ataks(side,a);
    for ( i = 0, n = -1; i < NO_SQUARES; i++ ) 
      if (a[i] != atak[side][i]) {
    	n = i; printf("atak #check error on square %d\n",i);
      }
    if ( n >= 0 ) {
      debug_ataks (stdout, a);
      debug_ataks (stdout, atak[side]);
      debug_position (stdout);
      printf("%d pieces\n",PieceCnt[side]);
      for ( i = PieceCnt[side]; i>= 0; i-- ) {
        short sq, piece;
        sq = PieceList[side][i];
        piece = board[sq];
        printf("square %d is %d with piece %d\n", i, sq, piece);
      }
      printf("hashkey = %ld hashbd = %ld\n",hashkey,hashbd);
      assert(a[n] == atak[side][n]);
    }
#endif  
#ifdef notdef
    printf("atak array for %s available for SqAtakd!\n",ColorStr[side]);
#endif
    *blockable = true; /* don't know */
    return(Anyatak(side,square));
  }

 /*
  * First check neigboured squares,
  * then check Knights.
  * then check Bishops,
  * last check Rooks,
  */                                         

  *blockable = false;          

  /* try a capture from direct neighboured squares */

  ptyp = ptype[black][king];
#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,square);
#else
  pdir = (*nextdir[ptyp])[square];
  u = pdir[square];
#endif
  do
    {
      if (color[u] == side)
	/* can piece reach square in one step ? */
#ifdef CHECK_DISTANCE
        if ( piece_distance(side,board[u],u,square) == 1 )
	  return(true);
#else   
	{
	  short v;
	  short ptypv = ptype[side][board[u]];
#ifdef SAVE_NEXTPOS
	  short dv;
	  v = first_direction(ptypv,&dv,u);
#else 
	  unsigned char *qdir;
	  qdir = (*nextdir[ptypv])[u];
	  v = qdir[u];
#endif 
	  do
	    {
	      if (v == square)
		return (true);
#ifdef SAVE_NEXTPOS
	      v = next_direction(ptypv,&dv,u);
#else
	      v = qdir[v];
#endif
	  } while (v != u);
	}
#endif
#ifdef SAVE_NEXTPOS
      u = next_direction(ptyp,&d,square);
#else
      u = pdir[u];
#endif
  } while (u != square);

  /* try a knight capture (using xside's knight moves) */

  ptyp = ptype[side ^ 1][knight];
#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,square);
#else
  pdir = (*nextdir[ptyp])[square];
  u = pdir[square];
#endif
  do
    {
      if (color[u] == side && board[u] == knight)
	return (true);
#ifdef SAVE_NEXTPOS
      u = next_direction(ptyp,&d,square);
#else
      u = pdir[u];
#endif
  } while (u != square);

  *blockable = true;

  /* try a (promoted) bishop capture */

  ptyp = ptype[black][bishop];
#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,square);
#else
  ppos = (*nextpos[ptyp])[square];
  pdir = (*nextdir[ptyp])[square];
  u = ppos[square];
#endif
  do
    {
      if (color[u] == neutral)
#ifdef SAVE_NEXTPOS
	u = next_position(ptyp,&d,square,u);
#else
	u = ppos[u];
#endif
      else
	{
	  if (color[u] == side && (unpromoted[board[u]] == bishop))
	    return (true);
#ifdef SAVE_NEXTPOS
	  u = next_direction(ptyp,&d,square);
#else
	  u = pdir[u];
#endif
	}
  } while (u != square);

  /* try a (promoted) rook capture */

  ptyp = ptype[black][rook];
#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,square);
#else
  ppos = (*nextpos[ptyp])[square];
  pdir = (*nextdir[ptyp])[square];
  u = ppos[square];
#endif
  do
    {
      if (color[u] == neutral)
#ifdef SAVE_NEXTPOS
	u = next_position(ptyp,&d,square,u);
#else
	u = ppos[u];
#endif
      else
	{
	  if (color[u] == side && (unpromoted[board[u]] == rook))
	    return (true);
#ifdef SAVE_NEXTPOS
	  u = next_direction(ptyp,&d,square);
#else
	  u = pdir[u];
#endif
	}
  } while (u != square);

  /* try a lance capture (using xside's lance moves) */

  ptyp = ptype[side ^ 1][lance];
#ifdef SAVE_NEXTPOS
  u = first_direction(ptyp,&d,square);
#else
  ppos = (*nextpos[ptyp])[square];
  u = ppos[square];
#endif
  do
    {
      if (color[u] == neutral)
#ifdef SAVE_NEXTPOS
	u = next_position(ptyp,&d,square,u);
#else
	u = ppos[u];
#endif
      else
	{
	  if (color[u] == side && (board[u] == lance))
	    return (true);
	  u = square;
	}
  } while (u != square);
                     
  return (false);

}
