/*
 * ataks.c - C source for GNU SHOGI
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
  register unsigned char *ppos, *pdir;
  short i, piece; 
  small_short *PL;

#ifdef NOMEMSET
  for (u = NO_SQUARES; u; a[--u] = 0) ;
#else
  memset ((char *) a, 0, NO_SQUARES * sizeof (a[0]));
#endif /* NOMEMSET */
  PL = PieceList[side];
  for (i = PieceCnt[side]; i >= 0; i--)
    { short ptyp;
      sq = PL[i];
      piece = board[sq];
      ptyp = ptype[side][piece];
      c = control[piece];
      if (sweep[piece])
	{
	  ppos = (*nextpos[ptyp])[sq];
	  pdir = (*nextdir[ptyp])[sq];
	  u = ppos[sq];
	  do
	    {
	      a[u] = ((a[u]+1) | c);
	      u = ((color[u] == neutral) ? ppos[u] : pdir[u]);
	  } while (u != sq);
	}
      else
	{
	  pdir = (*nextdir[ptyp])[sq];
	  u = pdir[sq];		/* follow captures thread for pawns */
	  do
	    {
	      a[u] = ((a[u]+1) | c);
	      u = pdir[u];
	  } while (u != sq);
	}
    }
}
