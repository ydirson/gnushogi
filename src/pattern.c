/*
 * pattern.c - C source for GNU SHOGI
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

#include "pattern.h"


OpeningPattern *Patterns;

#ifdef NONDSP
static long allocated_bytes;
#endif



#define is_digit(c) ((c) >= '0' && (c) <= '9')
#define is_alfa(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z')

#define eos(s) (*s == '\0' || *s == '\n')


/* skip blanks and comments in brackets */

static void skipbb(char **s) 
 { while (**s == ' ' || **s == '|' || **s == '[') { 
     if ( **s == '[' ) 
       while (**s != ']') (*s)++; 
     (*s)++; 
   } \
 }  
                        
/* skip unsigned numbers */

static void skipi(char **s)
 { 
   while ( is_digit(**s) ) 
     (*s)++;
   skipbb(s);
 }


inline
static
short
ScanPiece (char **s, small_short *side, small_short *piece, small_short *square)
{
  short isp, isw, c, r;
  
  /* determine promotion status */
  if ( **s == '+' )
    isp = true, (*s)++;
  else
    isp = false;
  /* determine side and piece */
  for (c = 0; c < NO_PIECES; c++)
    if ((isw = (**s == pxx[c])) || **s == qxx[c])
      {
	*piece = isp ? promoted[c] : unpromoted[c];
	*side  = isw;
	(*s)++; 
	break;
      } 
  if (c == NO_PIECES) 
    return(1);
  if ( **s == '*' )
    {
      /* piece is captured */ 
      (*s)++;
      *square = NO_SQUARES + *piece;
    }
  else
    {        
      /* determine column */
      for (c = 0; c < NO_COLS; c++)
        if (**s == cxx[c])
          {
	    (*s)++; 
	    break;
          }
      if (c == NO_COLS) 
        return(1);
      /* determine row */
      for (r = 0; r < NO_ROWS; r++)
        if (**s == rxx[r])
          {
	    (*s)++;
	    break;
          }
      if (r == NO_ROWS) return(1);
      /* determine square */
      *square = r*NO_COLS + c;
    }
  skipbb(s); 
  return(0);
}


short
string_to_board_color
 (char *s)

{
  skipbb(&s); 
  while ( !eos(s))
    { small_short side, piece, square;
      if ( ScanPiece(&s, &side, &piece, &square) )
	return(1);
      else
	{             
	  if ( square >= NO_SQUARES )
	    {
	      Captured[side][piece]++;
	    }
	  else
	    {
	      Pindex[square] = ++PieceCnt[side];                 
	      PieceList[side][Pindex[square]] = square;
	      color[square] = side;
	      board[square] = piece;
	    }
	}
    }
  return(0); 
}


short
string_to_patternfields
 (char *s, PatternFields *patternfields)

{
  patternfields->n = 0;
  skipbb(&s); 
  while ( !eos(s) )
    { short c, r, isp, isw;
      PatternField *field;
      field = &patternfields->field[patternfields->n];
      if ( ScanPiece(&s, &field->side, &field->piece, &field->square) )
	return(1);
      else
	if (patternfields->n++ == MAX_PATTERN) 
	  return(1);
    }
  return(0); 
}
                           

char *patternfile = PATTERNFILE;
               


static
short
ScanSequencePattern (char *s, PatternSequence **sequence)
{         
    PatternSequence *pattern;
    if ( *sequence = pattern = (PatternSequence *) malloc (sizeof(PatternSequence)) ) {
#ifdef NONDSP
      allocated_bytes += sizeof(PatternSequence);
#endif
      pattern->next_pattern = NULL;
      pattern->reachedGameCnt[black] = MAXMOVES;  
      pattern->reachedGameCnt[white] = MAXMOVES;  
      pattern->n = 0;
      skipbb(&s); /* skip blanks and comments */
      while ( is_digit(*s) )
        {                             
	  pattern->next[pattern->n] = atoi(s);
          pattern->n++;
	  skipi(&s);
        }
      if ( string_to_patternfields(s,&pattern->patternfields) )
        return(1);
      else
        return(0);
    } else {
      ShowMessage("cannot allocate pattern space...");
      return(1);
    }
}
                                       


void
GetOpeningPatterns ()

{ 
    FILE *fd;
    char s[256];
    OpeningPattern *p, *q;
    short count = 0;

#ifdef NONDSP                                      
    allocated_bytes = 0;
#endif

    if ( (fd = fopen (patternfile, "r")) == NULL )
	fd = fopen ("gnushogi.pattern", "r");       

    if ( fd != NULL )
      {
	PatternSequence **current_pattern = NULL; 
	Patterns = p = q = NULL;
	while ( fgets (s, 256, fd) != NULL )
	  {
	    if ( *s == '#' )
	      { /* comment, skip line */
	      } 
	    else if ( is_alfa(*s) )
	      {
		if ( p = (OpeningPattern *) malloc (sizeof(OpeningPattern)) )
		  { char *pn, *ps;
#ifdef NONDSP
      		    allocated_bytes += sizeof(OpeningPattern);
#endif
		    if ( Patterns == NULL )
		      Patterns = q = p;
		    else
		      q = q->next = p;
		    p->next = NULL;
		    p->n = 0;
		    p->sequence = NULL;
		    current_pattern = &p->sequence;
                    for (pn=p->name,ps=s; !eos(ps); pn++,ps++)
			*pn = *ps;
                    *pn = '\0';
		  }
		else
		  {
		    ShowMessage("cannot allocate opening sequence space...");
		    exit(1);
		  }
	      }
	    else
	      {
		if ( ScanSequencePattern(s,current_pattern) )
		  {
		    ShowMessage("error in pattern sequence...");
		    exit(1);
		  }
		else
		  {  
		    current_pattern = &(*current_pattern)->next_pattern;
		    p->n++;
		    count++;
		  }
	      }
	  }
#ifdef NONDSP
	printf("Pattern used %d entries, %ld bytes allocated.\n",
			count, allocated_bytes);
#endif  
	fclose(fd);
      }
#ifdef NONDSP
    else
      {     
        printf("no pattern file '%s'",patternfile);
      }      
#endif

}
                            



void
ShowOpeningPatterns ()

{ OpeningPattern *root = Patterns;
  while (root != NULL)
    { short i;
      PatternSequence *sequence;
      printf("Opening line: %s number of patterns %d\n",root->name,root->n);
      for (i=0,sequence = root->sequence; sequence; i++,sequence = sequence->next_pattern)
	{ short j;
	  printf("%d successor patterns:",sequence->n);
	  for (j = 0; j < sequence->n; j++) {
	    printf(" %d",sequence->next[j]);
	  }
	  printf(" pattern %d:\n",i);
          DisplayPattern(&sequence->patternfields);
	}
      root = root->next;
    }
}



short
piece_to_pattern_distance 
  (short side, short piece, short pside, PatternFields *pattern)

/*
 * Determine the minimum number of moves from the current position
 * to a specific pattern for a specific piece.
 * Consider the "side" piece of the pattern.
 * The pattern should match for "pside".
 */
{
  short nP, P[4], nB, B[4]; /* at most 4 pieces of same kind */
  short i, j, r, dd, occupied, mindd, c[4], d[4];
  
  /*
   * If pside == white, a black piece in the pattern should match
   * a white piece on board, and vice versa. Furthermore, if
   * pside == white, reversed pattern should match board.
   */

  /* special pawn handling */     

  if ( piece == pawn ) {
    mindd = occupied = 0; 
    for ( i = 0; i < pattern->n; i++ ) {
      PatternField field = pattern->field[i];
      if ( (field.side == side) && (field.piece == pawn) ) {
	short t = field.square;
        short pcol = column(t);                    
	dd = CANNOT_REACH;
        for ( j = 0; j <= PieceCnt[side ^ pside]; j++) {
	  short sq = (short)PieceList[side ^ pside][j];
	  if ( board[sq] == pawn ) {
	    if ( pside == white ) sq = NO_SQUARES - 1 - sq;
            if ( column(sq) == pcol ) {
	      dd = piece_distance (side, pawn, sq, t);
#ifdef TEST_PATTERN
              printf("update %d pawn from %d to %d is %d\n", side, sq, t, dd);
#endif                                       
	      if ( dd != CANNOT_REACH ) {
		/* Increment distance if pattern field is occupied */
		short psq, pc;
          	if ( pside == black ) {
	          psq = t;
		  pc = field.side;
		} else {
		  psq = (NO_SQUARES - 1 - t);
		  pc = ~field.side;
		}
		if ( (color[psq] == pc) && (board[psq] != pawn) ) {
#ifdef TEST_PATTERN
                  printf("square %d is occupied\n", psq);
#endif                                       
		  ++occupied;            
		}
	      }
              break;
	    } 
	  }
        }
        if ( dd == CANNOT_REACH )
	  return (CANNOT_REACH);
	else
	  mindd += dd;
      }
    }
    return (mindd + occupied);
  }

  /* 
   * Determine list of "side" "piece"s in pattern. 
   */

  for ( occupied = nP = i = 0; i < pattern->n; i++ ) {
        PatternField field = pattern->field[i];
	if ( (field.side == side) && (field.piece == piece) ) {
	  short psq, pc;
	  P[nP] = field.square;
#ifdef TEST_PATTERN
          printf("pattern %d piece %d on square %d\n",side,piece,P[nP]);
#endif
	  nP++;
	  /* Increment distance if pattern field is occupied */
          if ( pside == black ) {
	    psq = field.square;
            pc = field.side;
          } else {
	    psq = (NO_SQUARES - 1 - field.square);
	    pc = field.side ^ 1;
          }
	  if ( (color[psq] == pc) && (board[psq] != field.piece) ) {
#ifdef TEST_PATTERN
            printf("square %d is occupied\n", psq);
#endif                                       
	    ++occupied;            
	  }
	}
  }

  if ( nP == 0 )
    return (0);

#ifdef TEST_PATTERN
  printf("finding in pattern %d pieces %d of side %d\n", nP, piece, side);
#endif

  /* 
   * Determine list of "side ^ pside" "piece"s on board. 
   */

  for ( nB = 0, i = 0; i <= PieceCnt[side ^ pside]; i++ ) {
	short sq = PieceList[side ^ pside][i];
	if ( board[sq] == piece ) {
	  B[nB] = (pside == black) ? sq : (NO_SQUARES - 1 - sq);
#ifdef TEST_PATTERN
	  printf("%d piece %d on square %d\n",side,piece,B[nB]);
#endif
	  nB++;
	}
  }

#ifdef TEST_PATTERN
  printf("found on board %d pieces %d of side %d\n", nB, piece, side);
#endif

  if ( nP > nB ) {
	  return (CANNOT_REACH);
  }

  /* Determine best assignment from board piece to pattern piece */

  r = 0; c[0] = -1; mindd = CANNOT_REACH;

  while ( (r >= 0) && (mindd != 0) ) {

    if ( ++c[r] == nB ) {
	r--;
    } else {
	for ( i = 0; i < r; i++ )
	  if ( c[i] == c[r] )
	    break;
	if ( i == r ) {
	  d[r] =  piece_distance (side, piece, B[c[r]], P[r]);
#ifdef TEST_PATTERN
          printf("update d[%d] from  %d to %d is %d\n", r, B[c[r]], P[r], d[r]);
#endif
	  if ( d[r] == CANNOT_REACH ) {
	    /* r--; */
	  } else {
	    if ( ++r == nP ) {
		for (dd = i = 0; i < nP; i++)
		  dd += d[i];
		if ( (dd < mindd) || (mindd == CANNOT_REACH) ) {
		  mindd = dd;        
#ifdef TEST_PATTERN
                  printf("update min %d\n", mindd);
#endif
		}
		r--;
	    } else
		c[r] = -1;
	  }
	}
    }

  }
               
  if ( mindd == CANNOT_REACH )
    return (CANNOT_REACH);
  else
    return (mindd + occupied);

}


short
pattern_distance (short pside, PatternFields *pattern)

/*
 * Determine the minimum number of moves for the pieces from
 * the current position to reach a pattern.
 * The result is CANNOT_REACH, if there is no possible sequence
 * of moves.
 */

{
   short side, piece, d, n;

#ifdef TEST_PATTERN
   printf("\nchecking pattern for pside=%d\n\n",pside);
#endif

   for ( n = side = 0; side <= 1 && n != CANNOT_REACH; side++ )
     for ( piece = pawn; piece <= king; piece++ ) {
	d = piece_to_pattern_distance (side, piece, pside, pattern);
	if ( d == CANNOT_REACH) {
	  n = CANNOT_REACH; break;
	} else
	  n += d;
     }

#ifdef TEST_PATTERN
   printf("\ndistance to pattern is %d\n\n",n);
#endif

   return (n);
	
}


short
board_to_pattern_distance 
  (short pside, OpeningPattern *opattern, short pmplty, short GameCnt)

/*
 * Determine the maximal difference of the number of moves from the pattern 
 * to the initial position and to the current position.
 * Differences are weighted, i.e. the more closer a position is to a pattern
 * the more valuable is a move towards the pattern.
 * Patterns, which are at least "pmplty" halfmoves away, are not counted.
 */

{
   short d, dist, diff;
   short maxdiff = 0;
   PatternSequence *sequence;

   for ( sequence = opattern->sequence; sequence; sequence = sequence->next_pattern )
     if ( ((d = sequence->distance[pside]) != CANNOT_REACH) && (pmplty > d) )
       { 
         if ( (dist = pattern_distance (pside, &sequence->patternfields)) != CANNOT_REACH )
           { 
	     /* "dist" is the distance of the current board position to the pattern.
	      * "d - dist" is the difference between the current distance and the 
	      * initial distance. Compute "diff" as the weighted difference.
	      */
	     if ( (diff = (d - dist) * (pmplty - d)) > maxdiff )
		maxdiff = diff;
	     /* A reached pattern should not be considered in the future (if GameCnt >= 0) */
	     if ( dist == 0 && GameCnt >= 0)
		sequence->reachedGameCnt[pside] = GameCnt;
           }
       }
      
   return (maxdiff);
	
}



void
DisplayPattern (PatternFields *pattern)

{
  small_short pboard[NO_SQUARES], pcolor[NO_SQUARES];
  short sq, i, r, c;

  for (sq = 0; sq < NO_SQUARES; sq++)
    {
      pboard[sq] = no_piece;
      pcolor[sq] = neutral;
    }

  for (i = 0; i < pattern->n; i++)
    {
      PatternField *field = &pattern->field[i];
      pboard[field->square] = field->piece;
      pcolor[field->square] = field->side; 
    }

  for (r = NO_ROWS-1; r >= 0; r--)
    {
      for (c = 0; c < NO_COLS; c++)
        {
	  sq = r*NO_COLS + c;
	  i = pboard[sq];
	  if ( i == no_piece )
	    printf(" -");
	  else
	    printf("%c%c",is_promoted[i]?'+':' ',pcolor[sq]?pxx[i]:qxx[i]);
        }
      printf("\n");
    }

  printf("\n");

}                 


static
void
RemoveReachable (short pside, OpeningPattern *p, short n)
{
  short i,j;
  PatternSequence *sequence;

  /* Adjust to sequence pattern j */
  for (i=0,sequence=p->sequence; i<n; i++)
    sequence=sequence->next_pattern;

  /* do not perform visited link twice */
  if ( sequence->visited )
      return;
  else
      sequence->visited = true;

  /* Declare links unreachable */
  for (j=0; j<sequence->n; j++)
    RemoveReachable(pside,p,sequence->next[j]);

  /* Declare unreachable */
  sequence->distance[pside] = CANNOT_REACH;

#ifdef DEBUG_PATTERN
  printf("removing %d\n",n);
#endif

}      


OpeningPattern 
*locate_opening_pattern(short pside, char *s, short GameCnt)

{ 
   OpeningPattern *p;
   PatternSequence *sequence;
   short i, j, removed;
#ifdef DEBUG_PATTERN
   short n = 0, m = 0;
#endif    

  /* 
   * Look for opening pattern name in the list of opening patterns.
   */

   for ( p = Patterns; p != NULL; p = p->next )
     {                    
       if ( strcmp(s,p->name) == 0 )
         break;
     }

   if ( p == NULL )
     return(NULL);     

#ifdef DEBUG_PATTERN                             
   printf("%s uses opening line %s\n",ColorStr[pside],p->name);
#endif

  /*
   * Determine patterns which can be reached from the current
   * board position. Only patterns which can be reached will be
   * checked in the following search.
   */

   for ( i=0,sequence=p->sequence; sequence; i++,sequence=sequence->next_pattern )
     {
       sequence->distance[pside] = pattern_distance(pside,&sequence->patternfields);
       sequence->visited = false;
       /* Actually reached patterns need not to be observed. */
       if ( sequence->distance[pside] == 0 ) {
	 sequence->distance[pside] = CANNOT_REACH;
#ifdef DEBUG_PATTERN
         printf("pattern %d removed because reached\n",i);
#endif
       } else if ( GameCnt >= 0 && GameCnt >= sequence->reachedGameCnt[pside] ) {
	 sequence->distance[pside] = CANNOT_REACH; 
#ifdef DEBUG_PATTERN
         printf("pattern %d removed because reached at GameCnt %d below current %d\n",
			i,sequence->reachedGameCnt[pside],GameCnt);
#endif
       }
       if ( sequence->reachedGameCnt[pside] >= GameCnt )
         sequence->reachedGameCnt[pside] = MAXMOVES;
     }            
  /*
   * Remove reachable patterns from search, which are successors of
   * reachable patterns. So, only the next pattern of a pattern sequence
   * is observed. 
   */                    

   for (sequence=p->sequence; sequence; sequence=sequence->next_pattern)
     if ( sequence->distance[pside] != CANNOT_REACH ) {
       for (j=0; j<sequence->n; j++) {
#ifdef DEBUG_PATTERN
         printf("removing successors for link %d\n",sequence->next[j]);
#endif

         RemoveReachable(pside,p,sequence->next[j]);  
       }
     }

  /*
   * Look, whether there is still a reachable pattern.
   */

   for ( sequence=p->sequence; sequence; sequence=sequence->next_pattern )
     if ( sequence->distance[pside] != CANNOT_REACH )
       {
#ifdef DEBUG_PATTERN
         for (n=0,m=0,sequence=p->sequence; sequence; sequence=sequence->next_pattern,n++)
            if ( sequence->distance[pside] != CANNOT_REACH )
	      m++;
	 printf("%d reachable %s patterns out of %d patterns\n",
			m,ColorStr[pside],n);
#endif
         return(p);
       }

#ifdef DEBUG_PATTERN
   printf("all %s patterns out of %d patterns (%d reachable) removed\n",
		ColorStr[pside],n,m);
#endif

   return (NULL);
}


