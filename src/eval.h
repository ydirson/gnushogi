/*
 * eval.h - C source for GNU SHOGI
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
 
#define NO_STAGES 100
#define NO_FEATURES 31


     struct signature
     {
       unsigned long hashbd;
       unsigned long hashkey;
     } ; 

#define MatchSignature(s) (s.hashbd == hashbd && s.hashkey == hashkey)
#define CopySignature(s) {s.hashbd = hashbd; s.hashkey = hashkey;}      

typedef short value_array[NO_STAGES][NO_PIECES];
typedef small_short fscore_array[NO_STAGES][NO_FEATURES][2];

     extern value_array far *value;
     extern fscore_array far *fscore;

     extern void threats (short int side); 

     extern long int atak[2][NO_SQUARES];
     extern small_short sseed[NO_SQUARES];
 
     extern struct signature threats_signature[2];

     extern small_short starget[2][NO_SQUARES];
     extern small_short sloose[NO_SQUARES];
     extern small_short shole[NO_SQUARES];
     extern small_short shung[NO_SQUARES];

     extern struct signature squares_signature;
