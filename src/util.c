/*
 * util.c - C source for GNU SHOGI
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
unsigned int TTadd = 0;
short int recycle;
extern char mvstr[4][6];

#ifdef THINK_C
#include <string.h>
#define bcopy(src,dst,len) memcpy(dst,src,len)
#endif

int
parse (FILE * fd, short unsigned int *mv, short int side, char *opening)
{
  register int c, i, r1, r2, c1, c2;
  char s[128];
  char *p;

  while ((c = getc (fd)) == ' ' || c == '\n') ;
  i = 0;
  s[0] = (char) c;
  if (c == '!')
    {
      p = opening;
      do
	{
	  *p++ = c;
	  c = getc (fd);
	  if (c == '\n' || c == EOF)
	    {
	      *p = '\0';
	      return 0;
	    }
      } while (true);
    }
  while (c != '?' && c != ' ' && c != '\t' && c != '\n' && c != EOF)
    s[++i] = (char) (c = getc (fd));
  s[++i] = '\0';
  if (c == EOF)
    return (-1);
  if (s[0] == '!' || s[0] == ';' || i < 3)
    {
      while (c != '\n' && c != EOF)
	c = getc (fd);
      return (0);
    }
  c1 = '9' - s[0];
  r1 = 'i' - s[1];
  c2 = '9' - s[2];
  r2 = 'i' - s[3];
  *mv = (locn (r1, c1) << 8) | locn (r2, c2);
  if (c == '?')
    {				/* Bad move, not for the program to play */
      *mv |= 0x8000;		/* Flag it ! */
      c = getc (fd);
    }
  return (1);
}


/*
 * The field of a hashtable is computed as follows:
 *   if sq is on board (< NO_SQUARES) the field gets the value
 *     of the piece on the square sq;
 *   if sq is off board (>= NO_SQUARES) it is a catched figure,
 *     and the field gets the number of catched pieces for
 *     each side.
 */


inline
unsigned char
CB (short sq)
{ 
  register short i = sq;
  if ( i < NO_SQUARES ) {
    return ( (color[i] == white) ? (0x80 | board[i]) : board[i] );
  } else {
    i -= NO_SQUARES;
    return ( (Captured[black][i] << 4) | Captured[white][i] );
  }
}

      

#if defined DEBUG && defined CACHE                
                          
inline
char
BDpiece(unsigned char p)
{
  unsigned short piece = p & 0x7f;
  if ( piece == no_piece )
	return '-';
  else if ( p & 0x80 )
	return qxx[piece];
  else
	return pxx[piece];
}

inline
char
BDpromoted(unsigned char p)
{
  unsigned short piece = p & 0x7f;
  if ( is_promoted[piece] )
	return '+';
  else
	return ' ';
}

void
ShowBD(unsigned char bd[])
{
  register short i;
  for ( i = 0; i < PTBLBDSIZE; i++) 
    {                   
	if ( i < NO_SQUARES )
	  { 
            printf("%c%c(%c%c) ",
               BDpromoted(bd[i]),BDpiece(bd[i]),
               BDpromoted(CB(i)),BDpiece(CB(i)));
  	    if ( i % NO_COLS == NO_COLS - 1 )
	      printf("\n");
	  }
	else
	  printf("%2x(%2x) ",bd[i],CB(i));
    };
  printf("\n");
}        

#endif



#if ttblsz



int
ProbeTTable (short int side,
	     short int depth,
	     short int ply,
	     short int *alpha,
	     short int *beta,
	     short int *score)

/*
 * Look for the current board position in the transposition table.
 */

{
  register struct hashentry *ptbl;
  register /*unsigned*/ short i;  /*to match new type of rehash --tpm*/

  ptbl = &ttable[side][hashkey & (ttblsize - 1)];

  /* rehash max rehash times */
  for (i = 0; (ptbl->depth) && (ptbl->hashbd != hashbd) && (i < rehash); i++) ptbl++;
  if ((ptbl->depth) && (ptbl->hashbd == hashbd))
/*     ^^^^^^^^^^^^   we can use some informations of the entry even
                      the depth is not large enough */
    {  
#if defined HASHTEST
      for (i = 0; i < PTBLBDSIZE; i++)
	{
	  if (ptbl->bd[i] != CB (i))
	    {
	      HashCol++;
#if defined DEBUG && !defined BAREBONES
	      ShowMessage (CP[199]);	/*ttable collision detected*/
#endif
	      return(false);
	    }
	}
#endif /* HASHTEST */ 


      PV = SwagHt = ptbl->mv;
      if ((short) ptbl->depth >= depth)
	{
#if !defined BAREBONES
          HashCnt++;
#endif
	  if (ptbl->flags & truescore)
	    {
	      *score = ptbl->score;
	      /* adjust *score so moves to mate is from root */
	      if (*score > SCORE_LIMIT)
		*score -= ply;
	      else if (*score < -SCORE_LIMIT)
		*score += ply;
	      *beta = -((SCORE_LIMIT+1000)*2);
	    }
#ifdef notdef			/* Never happens! see search */
	  else if (ptbl->flags & upperbound)
	    {
	      if (ptbl->score < *beta)
		*beta = ptbl->score + 1;
	    }
#endif
	  else if (ptbl->flags & lowerbound)
	    {
	      if (ptbl->score > *alpha)
		*alpha = ptbl->score - 1;
	    }
	  return (true);
	}
    }
  return (false);
}


int
PutInTTable (short int side,
	     short int score,
	     short int depth,
	     short int ply,
	     short int alpha,
	     short int beta,
	     short unsigned int mv)

/*
 * Store the current board position in the transposition table.
 */

{
  register struct hashentry *ptbl;
  register /*unsigned*/ short i;  /*to match new type of rehash --tpm*/

  ptbl = &ttable[side][hashkey & (ttblsize - 1)];

  /* rehash max rehash times */
  for (i = 0; ptbl->depth && ptbl->hashbd != hashbd && i < rehash; i++)
    ptbl++;
  if (i == rehash) {
#if !defined BAREBONES
	THashCol++;
#endif
	ptbl -= recycle;}
  if (depth >= (short)ptbl->depth || ptbl->hashbd != hashbd)
    {
#if !defined BAREBONES
      TTadd++;
      HashAdd++; 
#endif
      ptbl->hashbd = hashbd;
      ptbl->depth = (unsigned char) depth;
      /* adjust score so moves to mate is from this ply */
      if (score > SCORE_LIMIT)
	score += ply;
      else if (score < -SCORE_LIMIT)
	score -= ply;
      ptbl->score = score;
      ptbl->mv = mv;
#ifdef DEBUG4
      if (debuglevel & 32)
	{
	  algbr (mv >> 8, mv & 0xff, 0);
	  printf ("-add-> d=%d s=%d p=%d a=%d b=%d %s\n", depth, score, ply, alpha, beta, mvstr);
	}
#endif
/*#ifdef notdef
      if (score < alpha)
	ptbl->flags = upperbound;
      else
/*#endif /* 0 */
      if (score > beta)
	{
	  ptbl->flags = lowerbound;
	  score = beta + 1;
	}
      else
	ptbl->flags = truescore;
       
#if defined HASHTEST
      for (i = 0; i < PTBLBDSIZE; i++)
	{
	  ptbl->bd[i] = CB (i);
	}
#endif /* HASHTEST */
      return true;
    }
  return false;
}

   
static struct hashentry *ttageb, *ttagew;

void
ZeroTTable (void)
{
   register struct hashentry *w, *b;
   for ( b=ttable[black], w=ttable[white]; b < &ttable[black][ttblsize]; w++, b++)
     { 
        w->depth = 0; 
        b->depth = 0;
     }
   ttageb = ttable[black]; 
   ttagew = ttable[white];
#ifdef CACHE
   memset ((char *) etab[0], 0, sizeof(struct etable)*(size_t)ETABLE);
   memset ((char *) etab[1], 0, sizeof(struct etable)*(size_t)ETABLE);
#endif
#ifdef notdef
   register unsigned int a;
   for (a = 0; a < ttblsize + (unsigned int)rehash; a++)
     {
       ttable[black][a].depth = 0;
       ttable[white][a].depth = 0;
     }
#endif
    TTadd = 0; 
}

#ifdef HASHFILE
int Fbdcmp(unsigned char *a,unsigned char *b)
{
	register int i;
	for(i = 0; i < PTBLBDSIZE; i++)
		if(a[i] != b[i]) return false;
	return true;
}
int
ProbeFTable (short int side,
	     short int depth,
	     short int ply,
	     short int *alpha,
	     short int *beta,
	     short int *score)

/*
 * Look for the current board position in the persistent transposition table.
 */

{
  register short int i;
  register unsigned long hashix;
  struct fileentry new, t;

  hashix = ((side == black) ? (hashkey & 0xFFFFFFFE) : (hashkey | 1)) & filesz;

  for (i = 0; i < PTBLBDSIZE; i++)
    new.bd[i] = CB (i);
  new.flags = 0;
  for (i = 0; i < frehash; i++)
    {
      fseek (hashfile,
	     sizeof (struct fileentry) * ((hashix + 2 * i) & (filesz)),
	     SEEK_SET);
      fread (&t, sizeof (struct fileentry), 1, hashfile);
      if (!t.depth) break;
       if(!Fbdcmp(t.bd, new.bd)) continue;
      if (((short int) t.depth >= depth) 
	  && (new.flags == (unsigned short)(t.flags & (kingcastle | queencastle))))
	{
#if !defined BAREBONES
	  FHashCnt++;
#endif
	  PV = (t.f << 8) | t.t;
	  *score = (t.sh << 8) | t.sl;
	  /* adjust *score so moves to mate is from root */
	  if (*score > SCORE_LIMIT)
	    *score -= ply;
	  else if (*score < -SCORE_LIMIT)
	    *score += ply;
	  if (t.flags & truescore)
	    {
	      *beta = -((SCORE_LIMIT+1000)*2);
	    }
	  else if (t.flags & lowerbound)
	    {
	      if (*score > *alpha)
		*alpha = *score - 1;
	    }
	  else if (t.flags & upperbound)
	    {
	      if (*score < *beta)
		*beta = *score + 1;
	    }
	  return (true);
	}
    }
  return (false);
}

void
PutInFTable (short int side,
	     short int score,
	     short int depth,
	     short int ply,
	     short int alpha,
	     short int beta,
	     short unsigned int f,
	     short unsigned int t)

/*
 * Store the current board position in the persistent transposition table.
 */

{
  register unsigned short i;
  register unsigned long hashix;
  struct fileentry new, tmp;

  hashix = ((side == black) ? (hashkey & 0xFFFFFFFE) : (hashkey | 1)) & filesz;
  for (i = 0; i < PTBLBDSIZE; i++) 
    new.bd[i] = CB (i);
  new.f = (unsigned char) f;
  new.t = (unsigned char) t;
  if (score < alpha)
    new.flags = upperbound;
  else
    new.flags = ((score > beta) ? lowerbound : truescore);
  new.depth = (unsigned char) depth;
  /* adjust *score so moves to mate is from root */
  if (score > SCORE_LIMIT)
    score += ply;
  else if (score < -SCORE_LIMIT)
    score -= ply;


  new.sh = (unsigned char) (score >> 8);
  new.sl = (unsigned char) (score & 0xFF);

  for (i = 0; i < frehash; i++)
    {
      fseek (hashfile,
	     sizeof (struct fileentry) * ((hashix + 2 * i) & (filesz)),
	     SEEK_SET);
      if ( !fread (&tmp, sizeof (struct fileentry), 1, hashfile) )
        {perror("hashfile");exit(1);}
      if (tmp.depth && !Fbdcmp(tmp.bd,new.bd))continue;
      if (tmp.depth == depth) break;
      if (!tmp.depth || (short) tmp.depth < depth)
	{
	  fseek (hashfile,
		 sizeof (struct fileentry) * ((hashix + 2 * i) & (filesz)),
		 SEEK_SET);
#ifdef DEBUG4
          if (debuglevel & 32)
    	    {
	      printf ("-fadd\n");
	    }
#endif
	  fwrite (&new, sizeof (struct fileentry), 1, hashfile);
#if !defined BAREBONES
          FHashAdd++;
#endif
	  break;
	}
    }
}

#endif /* HASHFILE */
#endif /* ttblsz */

void
ZeroRPT (void)
{
#ifdef NOMEMSET
  register int side, i;
  for (side = black; side <= white; side++)
    for (i = 0; i < 256;)
      rpthash[side][i++] = 0;
#else
   memset ((char *) rpthash, 0, sizeof (rpthash));
#endif
}




#if defined CACHE

void
PutInEETable (short int side,int score)

/*
 * Store the current eval position in the transposition table.
 */

{
    register struct etable *ptbl;
    ptbl = &(*etab[side])[hashkey & (ETABLE - 1)];
    if (ptbl->ehashbd == hashbd) return;
#if defined CACHETEST
    { short i;
      for (i = 0; i < PTBLBDSIZE; i++)
	{
	  ptbl->bd[i] = CB (i);
	}
    }
#endif /* CACHETEST */
    ptbl->ehashbd = hashbd;
    ptbl->escore[black] = pscore[black];
    ptbl->escore[white] = pscore[white];
    ptbl->hung[black] = hung[black];
    ptbl->hung[white] = hung[white];
    ptbl->score = score;
    bcopy (svalue, &(ptbl->sscore), sizeof (svalue));
#if !defined BAREBONES
    EADD++;
#endif
    return;
}


int
CheckEETable (short int side)

/* Get an evaluation from the transposition table */
{
    register struct etable *ptbl;
    ptbl = &(*etab[side])[hashkey & (ETABLE - 1)];
    if (hashbd == ptbl->ehashbd) 
      {
        return true;
      }
    return false;
}


int
ProbeEETable (short int side, short int *score)

/* Get an evaluation from the transposition table */
{
    register struct etable *ptbl;
    ptbl = &(*etab[side])[hashkey & (ETABLE - 1)];
    if (hashbd == ptbl->ehashbd)
      {
#if defined CACHETEST
	short i;
        for (i = 0; i < PTBLBDSIZE; i++)
	  {
	    if (ptbl->bd[i] != CB (i))
	      {
#if !defined BAREBONES
	        ShowMessage ("eetable probe collision detected");
#endif
#if DEBUG            
		ShowBD(ptbl->bd);
#endif
	        return false;
	      }
	  }
#endif /* CACHETEST */ 
	  pscore[black] = ptbl->escore[black];
	  pscore[white] = ptbl->escore[white];
	  bcopy (&(ptbl->sscore), svalue, sizeof (svalue));
	  *score = ptbl->score;
          hung[black] = ptbl->hung[black];
          hung[white] = ptbl->hung[white];
#if !defined BAREBONES
	  EGET++;
#endif
	  return true;
      }
    return false;

}


#endif /* CACHE */ 




