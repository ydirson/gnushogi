/*
 * ataks.h - Header source for GNU SHOGI
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


#define CHECK_DISTANCE


inline
static
int
SqAtakd (register short int square, short int side)

/*
 * See if any piece with color 'side' ataks sq.  
 */

{
  register unsigned char *ppos, *pdir;
  register short u, ptyp;

 /*
  * First check distance 1,
  * then check Bishops,
  * then check Rooks,
  * last check Knights.
  */

  /* try a capture from distance 1 */

  ptyp = ptype[black][king];
  pdir = (*nextdir[ptyp])[square];
  u = pdir[square];
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
	  unsigned char *qdir;
	  qdir = (*nextdir[ptype[side][board[u]]])[u];
	  v = qdir[u];
	  do
	    {
	      if (v == square)
		return (true);
	      v = qdir[v];
	  } while (v != u);
	}
#endif
      u = pdir[u];
  } while (u != square);

  /* try a (promoted) bishop capture */

  ptyp = ptype[black][bishop];
  ppos = (*nextpos[ptyp])[square];
  pdir = (*nextdir[ptyp])[square];
  u = ppos[square];
  do
    {
      if (color[u] == neutral)
	u = ppos[u];
      else
	{
	  if (color[u] == side && (unpromoted[board[u]] == bishop))
	    return (true);
	  u = pdir[u];
	}
  } while (u != square);

  /* try a (promoted) rook capture */

  ptyp = ptype[black][rook];
  ppos = (*nextpos[ptyp])[square];
  pdir = (*nextdir[ptyp])[square];
  u = ppos[square];
  do
    {
      if (color[u] == neutral)
	u = ppos[u];
      else
	{
	  if (color[u] == side && (unpromoted[board[u]] == rook))
	    return (true);
	  u = pdir[u];
	}
  } while (u != square);

  /* try a lance capture (using xside's lance moves) */

  ptyp = ptype[side ^ 1][lance];
  ppos = (*nextpos[ptyp])[square];
  u = ppos[square];
  do
    {
      if (color[u] == neutral)
	u = ppos[u];
      else
	{
	  if (color[u] == side && (board[u] == lance))
	    return (true);
	  u = square;
	}
  } while (u != square);

  /* try a knight capture (using xside's knight moves) */

  ptyp = ptype[side ^ 1][knight];
  pdir = (*nextdir[ptyp])[square];
  u = pdir[square];
  do
    {
      if (color[u] == side && board[u] == knight)
	return (true);
      u = pdir[u];
  } while (u != square);

  return (false);

}
