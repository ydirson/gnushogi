/*
 * util.c - C source for GNU SHOGI
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

#include "gnushogi.h"
#ifdef DEBUG
#include <assert.h>
#endif
unsigned int TTadd = 0;
short int recycle; 
short int ISZERO = 1;


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
#ifdef DEBUG
    assert(i!=NO_SQUARES || (Captured[black][0]==0 && Captured[white][0]==0));
#endif
    i -= NO_SQUARES;
    return ( (Captured[black][i] << 4) | Captured[white][i] );
  }
}

      

#if defined DEBUG               
                          
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



#ifdef DEBUG_TTABLE

void ShowPTBL (struct hashentry *ptbl)
{
  movealgbr(ptbl->mv,mvstr[0]);
  printf("hk=%lx hb=%lx ptbl=%lx bd=%lx mv=%s d=%d f=%d s=%d\n",
  		hashkey, hashbd, ptbl, 
  		ptbl->hashbd, mvstr[0], ptbl->depth, ptbl->flags, ptbl->score);
}
#endif


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
  register struct hashentry far *ptbl;
  register /*unsigned*/ short i = 0;  /*to match new type of rehash --tpm*/

#ifdef DEBUG_TTABLE
      /* printf("FOR hk=%lx hb=%lx d=%d\n",hashkey,hashbd,depth); */
#endif 

  ptbl = &ttable[side][hashkey % ttblsize];

  while (true)
    {
      if ((ptbl->depth) == 0)
        return false;
      if (ptbl->hashbd == hashbd)
        break;
      if (++i > rehash)
        return false;
      ptbl++;
    }

  /* rehash max rehash times */
  if (((short)(ptbl->depth) >= (short) depth))
    {
#ifdef HASHTEST
      for (i = 0; i < PTBLBDSIZE; i++)
        {
          if (ptbl->bd[i] != CB (i))
            {
#ifndef BAREBONES
              HashCol++;
              ShowMessage (CP[199]);    /*ttable collision detected*/
	      ShowBD(ptbl->bd);
	      printf("hashkey = 0x%x, hashbd = 0x%x\n", hashkey, hashbd);
#endif
              break;
            }
        }
#endif /* HASHTEST */


      PV = SwagHt = ptbl->mv;
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
          *beta = -2*(SCORE_LIMIT+1000);
        }
      else if (ptbl->flags & lowerbound)
        {
          if (ptbl->score > *alpha)
            *alpha = ptbl->score - 1;
        }
#ifdef DEBUG_TTABLE
      /* printf("GET "); ShowPTBL(ptbl); */
#endif
      return (true);
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
  register struct hashentry far *ptbl;
  register /*unsigned*/ short i = 0;  /*to match new type of rehash --tpm*/

  ptbl = &ttable[side][hashkey % ttblsize];

  while (true)
    {
      if ((ptbl->depth) == 0 || ptbl->hashbd == hashbd)
        break;
      if (++i > rehash)
        {
#ifndef BAREBONES
          THashCol++;
#endif
          ptbl += recycle;
          break;
        }
      ptbl++;
    }

#ifndef BAREBONES
  TTadd++;
  HashAdd++;
#endif
  /* adjust score so moves to mate is from this ply */
  if (score > SCORE_LIMIT)
    score += ply;
  else if (score < -SCORE_LIMIT)
    score -= ply;
  ptbl->hashbd = hashbd;
  ptbl->depth = (unsigned char) depth;
  ptbl->score = score;
  ptbl->mv = mv;

#ifdef DEBUG4
  if (debuglevel & 32)
    {
      algbr (mv >> 8, mv & 0xff, 0);
      printf ("-add-> d=%d s=%d p=%d a=%d b=%d %s\n", depth, score, ply, alpha, beta, mvstr);
    }
#endif
  if (score > beta)
    {
      ptbl->flags = lowerbound;
      ptbl->score = beta + 1;
    }
  else
    ptbl->flags = truescore;
       
#if defined HASHTEST
  for (i = 0; i < PTBLBDSIZE; i++)
    {
      ptbl->bd[i] = CB (i);
    }
#endif /* HASHTEST */

#ifdef DEBUG_TTABLE
      /* printf("PUT "); ShowPTBL(ptbl); */
#endif

  return true;
}

                                  
#if ttblsz
static struct hashentry far *ttageb, *ttagew;
#endif

void
ZeroTTable (void)
{
#ifdef notdef
   register struct hashentry far *w, *b;
   for ( b=ttable[black], w=ttable[white]; b < &ttable[black][ttblsize]; w++, b++)
     { 
        w->depth = 0; 
        b->depth = 0;
     }
   ttageb = ttable[black]; 
   ttagew = ttable[white];
   register unsigned int a;
   for (a = 0; a < ttblsize + (unsigned int)rehash; a++)
     {
       (ttable[black])[a].depth = 0;
       (ttable[white])[a].depth = 0;
     }
#endif
   array_zero(ttable[black],(ttblsize+rehash));
   array_zero(ttable[white],(ttblsize+rehash));
#ifdef DEBUG_TTABLE
   printf("TTable zeroed\n");
#endif
#ifdef CACHE
   array_zero(etab[0],sizeof(struct etable)*(size_t)ETABLE);
   array_zero(etab[1],sizeof(struct etable)*(size_t)ETABLE);
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

  hashix = ((side == black) ? (hashkey & 0xFFFFFFFE) : (hashkey | 1)) % filesz;

  for (i = 0; i < PTBLBDSIZE; i++)
    new.bd[i] = CB (i);
  new.flags = 0;
  for (i = 0; i < frehash; i++)
    {
      fseek (hashfile,
	     sizeof (struct fileentry) * ((hashix + 2 * i) % (filesz)),
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

  hashix = ((side == black) ? (hashkey & 0xFFFFFFFE) : (hashkey | 1)) % filesz;
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
	     sizeof (struct fileentry) * ((hashix + 2 * i) % (filesz)),
	     SEEK_SET);
      if ( !fread (&tmp, sizeof (struct fileentry), 1, hashfile) )
        {perror("hashfile");exit(1);}
      if (tmp.depth && !Fbdcmp(tmp.bd,new.bd))continue;
      if (tmp.depth == depth) break;
      if (!tmp.depth || (short) tmp.depth < depth)
	{
	  fseek (hashfile,
		 sizeof (struct fileentry) * ((hashix + 2 * i) % (filesz)),
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
  if ( ISZERO )
    {
      array_zero (rpthash, sizeof (rpthash));
      ISZERO = 0;
    }
}




#if defined CACHE

void
PutInEETable (short int side,int score)

/*
 * Store the current eval position in the transposition table.
 */

{
    register struct etable far *ptbl;
    ptbl = &(*etab[side])[hashkey % (ETABLE)];
    ptbl->ehashbd = hashbd;
    ptbl->escore[black] = pscore[black];
    ptbl->escore[white] = pscore[white];
    ptbl->hung[black] = hung[black];
    ptbl->hung[white] = hung[white];
    ptbl->score = score;
#if !defined SAVE_SSCORE
    array_copy (svalue, &(ptbl->sscore), sizeof (svalue));
#endif
#if !defined BAREBONES
    EADD++;
#endif
    return;
}


int
CheckEETable (short int side)

/* Get an evaluation from the transposition table */
{
    register struct etable far *ptbl;
    ptbl = &(*etab[side])[hashkey % (ETABLE)];
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
    register struct etable far *ptbl;
    ptbl = &(*etab[side])[hashkey % (ETABLE)];
    if (hashbd == ptbl->ehashbd)
      {
	  pscore[black] = ptbl->escore[black];
	  pscore[white] = ptbl->escore[white];
#if defined SAVE_SSCORE
   	  array_zero (svalue, sizeof(svalue));
#else
	  array_copy (&(ptbl->sscore), svalue, sizeof (svalue));
#endif
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




