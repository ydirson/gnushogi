/*
 * search.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993 Matthias Mutz
 *
 * GNU SHOGI is based on GNU CHESS
 *
 * Copyright (c) 1988,1989,1990 John Stanback Copyright (c) 1992 Free Software
 * Foundation
 *
 * This file is part of GNU SHOGI.
 *
 * GNU Shogi is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * GNU Shogi is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Shogi; see the file COPYING.  If not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "gnushogi.h" 
#if !defined OLDTIME && defined HASGETTIMEOFDAY
double pow();
#endif
short background = 0;
static short int DepthBeyond;
unsigned short int PrVar[MAXDEPTH];
extern char mvstr[4][6];
#if ttblsz
extern short int recycle;
#endif
#ifdef NULLMOVE
short int null;         /* Null-move already made or not */
short int PVari;        /* Is this the PV */
#endif
#ifdef DEBUG40
extern int whichway;
#endif
#ifdef DEBUG
unsigned short DBLINE[MAXDEPTH];
struct leaf *dbptr;

#endif 


short int zwndw;

#include "ataks.h"

#include "debug41.h"
/* ............    MOVE GENERATION & SEARCH ROUTINES    .............. */

#ifdef REPITITION

inline
short int
repetition ()

/*  Check for draw by threefold repetition.  */

{
  register short i, c,cnt;
  register unsigned short m;
  short b[NO_SQUARES];

  cnt = c = 0;
  /* try to avoid work */
  if (GameCnt > Game50 + 2)
    {
#if defined(NOMEMSET) || defined(MSDOS)
      for (i = 0; i < NO_SQUARES; b[i++] = 0);
#else
      memset ((char *) b, 0, (unsigned long)sizeof (b));
#endif /* NOMEMSET */
      for (i = GameCnt; i >= Game50; i--)
	{
	  m = GameList[i].gmove;
	  /* does piece exist on diff board? */
	  if (b[m & 0xff])
	    {
	      /* does diffs cancel out, piece back? */
	      if ((b[m >> 8] += b[m & 0xff]) == 0)
		--c;
	      b[m & 0xff] = 0;
	    }
	  else
	    {
	      /* create diff */
	      ++c;
	      /* does diff cancel out another diff? */
	      if (!(b[m >> 8] -= (b[m & 0xff] = board[m & 0xff] +
				  (color[m & 0xff] << 8))))
		--c;;
	    }
	  /* if diff count is 0 we have a repetition */
	  if (c == 0)
	    if ((i ^ GameCnt) & 1)
	      cnt++;
	}
    }
    return cnt;
}

#endif



int plyscore, globalscore;
int
pick (short int p1, short int p2)

/*
 * Find the best move in the tree between indexes p1 and p2. Swap the best
 * move into the p1 element.
 *
 */
{
  register struct leaf *p, *q, *r, *k;
  register s0;
  struct leaf temp;

  k = p = &Tree[p1];
  q = &Tree[p2];
  s0 = p->score;
  for (r = p + 1; r <= q; r++)
    if ((r->score) > s0)
      {
	s0 = (r->score);
	p = r;
      }
  if (p != k)
    {
      temp = *p;
      *p = *k;
      *k = temp;
      return true;
    }
  return false;
}

#ifdef DEBUG
unsigned short trace[MAXDEPTH];
char traceline[256];
unsigned short tracelog[MAXDEPTH];
int tracen = 0;
int traceflag = 0;
int traceply = 0;
#endif
int bookflag = false;

/* #define PLYDEBUG */

#ifdef PLYDEBUG
static int MaxPly = 0;
static int MaxDepth = 0;
#endif

static int TCcount, TCleft = 0;
void
SelectMove (short int side, short int iop)


/*
 * Select a move by calling function search() at progressively deeper ply
 * until time is up or a mate or draw is reached. An alpha-beta window of
 * -Awindow to +Bwindow points is set around the score returned from the
 * previous iteration. If Sdepth != 0 then the program has correctly
 * predicted the opponents move and the search will start at a depth of
 * Sdepth+1 rather than a depth of 1.
 */

{
  static short int i, tempb, tempc, tempsf, tempst, xside, rpt;
  static short int alpha, beta, score;
  static struct GameRec *g;
  int Jscore = 0;
#ifdef PLYDEBUG
  MaxPly = 0;
  MaxDepth = 0;
#endif
#ifdef DEBUG

if(debuglevel & (512|1024)){
	char b[32];
	short c1,c2,r1,r2;
	tracen=0;
	traceflag = false;
	traceply = 0;
	tracelog[0]=0;
	while(true){
	  printf("debug?");
	  gets(b);
	  if(b[0] == 'p')traceply = atoi(&b[1]);
	  else
	  if(b[0] == '\0')break;
	  else{
		c1 = '9' - b[0];
      		r1 = 'i' - b[1];
      		c2 = '9' - b[2];
      		r2 = 'i' - b[3];
      		trace[++tracen] = (locn (r1, c1) << 8) | locn (r2, c2);
	  }
	  if(tracen == 0 && traceply >0)traceflag = true;
	}
	
}
#endif

#ifdef BOOKTEST
  printf("hashbd = %ld (hashkey>>16)|side = %d\n",
           hashbd,(hashkey>>16)|side);
#endif

  flag.timeout = false;
  flag.back = false;
  flag.musttimeout = false;
  xside = side ^ 1;    
#if ttblsz
  recycle = (GameCnt % rehash) - rehash;
#endif
  /* if background mode set to infinite */
  if (iop == 2)
    {
      ResponseTime = 9999999;
      background = true;
    }
  else
    {
      player = side;
      if (TCflag)
	{
          TCcount = 0;
	  background = false;
	  if (TimeControl.moves[side] < 1)
	    TimeControl.moves[side] = 1;
	  /* special case time per move specified */
	  if (flag.onemove)
	    {
	      ResponseTime = TimeControl.clock[side] - 100;
	      TCleft = 0;
	    }
	  else
	    {
	      /* calculate avg time per move remaining */
	      TimeControl.clock[side] += TCadd;
	      ResponseTime = 
                  (TimeControl.clock[side]) / 
                  (((TimeControl.moves[side]) * 2) + 1);
	      TCleft = (int)ResponseTime / 3; 
	      ResponseTime += TCadd/2;
	      if (TimeControl.moves[side] < 5)
		  TCcount = MAXTCCOUNTX - 10;
	    }
	  if (ResponseTime < 100)
	    {
	      ResponseTime = 100;
	      TCcount = MAXTCCOUNTX - 10;
	    }
	  else if (ResponseTime < 200)
	    {
	      TCcount = MAXTCCOUNTX - 10;
	    }
	}
      else
	{
	  ResponseTime = MaxResponseTime;
	  TCleft = 0;
          ElapsedTime(1);
	}
      if ( TCleft )
	{
          TCcount = ((int)((TimeControl.clock[side] - ResponseTime)) / 2) / TCleft;

	  if (TCcount > MAXTCCOUNTX)
	    TCcount = 0;
	  else
	    TCcount = MAXTCCOUNTX - TCcount;
	}
      else
        {
          TCcount = MAXTCCOUNTX;
        }
    }

  ExtraTime = 0;
  ExaminePosition ();
  score = ScorePosition (side);

#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    ShowSidetoMove ();

#ifdef notdef
  if (TCflag && TCcount < MAXTCCOUNT)
    if (score < SCORETIME)
      {
	ExtraTime += TCleft;
	TCcount++;
      }
#endif

#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    SearchStartStuff (side);

#ifdef HISTORY
#if defined(NOMEMSET) || defined(MSDOS)
  for (i = 0; i < HISTORY_SIZE; i++)
    history[i] = 0;
#else
  memset ((unsigned char *) history, 0, (unsigned long)sizeof_history);
#endif /* NOMEMSET */
#endif

  FROMsquare = TOsquare = -1;
  PV = 0;
  if (iop == 1)
    hint = 0;
  for (i = 0; i < MAXDEPTH; i++)
    PrVar[i] = killr0[i] = killr1[i] = killr2[i] = killr3[i] = 0;

  /* set initial window for search */

  alpha = score - ((computer == white) ? BAwindow : WAwindow);
  beta = score + ((computer == white) ? BBwindow : WBwindow);
  rpt = 0;
  TrPnt[1] = 0;
  root = &Tree[0];
  MoveList (side, 1);
  for (i = TrPnt[1]; i < TrPnt[2]; i++)
    if (!pick (i, TrPnt[2] - 1))
      break;

  /* can I get a book move? */

  if (flag.regularstart && Book)
    {
      flag.timeout = bookflag = OpeningBook (&hint, side);

      if (TCflag)
	ResponseTime += ResponseTime;
    }

  /* zero stats for hash table */

  reminus = replus = 0;
  GenCnt = NodeCnt = ETnodes = EvalNodes = HashCnt = FHashAdd = HashAdd = FHashCnt = THashCol = HashCol = 0;

  globalscore = plyscore = score;
  zwndw = 20;

#include "debug4.h"

  /********************* main loop ********************************/

  Sdepth = (MaxSearchDepth < (MINDEPTH-1)) ? MaxSearchDepth : (MINDEPTH-1);

  while (!flag.timeout)
    {
      /* go down a level at a time */
      Sdepth++;
#ifdef NULLMOVE
      null = 0;
      PVari = 1;
#endif
      DepthBeyond = Sdepth + ((Sdepth == 1) ? 7 : 11);

#if !defined BAREBONES
#ifdef QUIETBACKGROUND
      if (!background)
#endif /* QUIETBACKGROUND */
	ShowDepth (' ');
#endif
      /* search at this level returns score of PV */
      score = search (side, 1, Sdepth, alpha, beta, PrVar, &rpt);
      /* save PV as killer */
      for (i = 1; i <= Sdepth; i++)
	killr0[i] = PrVar[i];

      /* low search failure re-search with (-inf,score) limits  */
      if (score < alpha)
	{
#if !defined BAREBONES
	  reminus++;
#ifdef QUIETBACKGROUND
	  if (!background)
#endif /* QUIETBACKGROUND */
	    ShowDepth ('-');
#endif
	  if (TCflag && TCcount < MAXTCCOUNTR)
	    {
	      TCcount = MAXTCCOUNTR - 1;
	      ExtraTime += (8 * TCleft);
	    }
	  score = search (side, 1, Sdepth, -(SCORE_LIMIT+999), score, PrVar, &rpt);
	}
      /* high search failure re-search with (score, +inf) limits */
      if (score > beta && !(root->flags & exact))
	{
#if !defined BAREBONES
	  replus++;
#ifdef QUIETBACKGROUND
	  if (!background)
#endif /* QUIETBACKGROUND */
	    ShowDepth ('+');
#endif
	  score = search (side, 1, Sdepth, score, (SCORE_LIMIT+999), PrVar, &rpt);
	}
      /**************** out of search ********************************************/
      if (flag.musttimeout || Sdepth >= MaxSearchDepth)
	flag.timeout = true;

      else if (TCflag && (Sdepth > (MINDEPTH - 1)) && (TCcount < MAXTCCOUNTR))
	{
	  if (killr0[1] != PrVar[1] /* || Killr0[2] != PrVar[2] */ )
	    {
	      TCcount++;
	      ExtraTime += TCleft;
	    }
	  if ((abs (score - globalscore) / Sdepth) > ZDELTA)
	    {
	      TCcount++;
	      ExtraTime += TCleft;
	    }
	}
      if (score > (Jscore - zwndw) && score > (Tree[1].score + 250)) ExtraTime = 0;
      ElapsedTime(2);
      if (root->flags & exact) flag.timeout = true;
      /*else if (Tree[1].score < -SCORE_LIMIT) flag.timeout = true;*/
#if defined OLDTIME || !defined HASGETTIMEOFDAY
      else if (!(Sdepth < MINDEPTH) && TCflag && ((4 * et) > (2*ResponseTime + ExtraTime)))
			 flag.timeout = true;
#else
      else if (!(Sdepth < MINDEPTH) && TCflag && 
		((int)(1.93913099l * (pow((double)et,1.12446928l))) > (ResponseTime + ExtraTime)))
			 flag.timeout = true;
#endif
/************************ time control ***********************************/

      /* save PV as killer */
      for (i = 1; i <= Sdepth + 1; i++) killr0[i] = PrVar[i];
      if (!flag.timeout) Tscore[0] = score;
      /* if (!flag.timeout) */
      for (i = TrPnt[1]+1; i < TrPnt[2]; i++) if (!pick (i, TrPnt[2] - 1)) break;
      /* if done or nothing good to look at quit */
      if ((root->flags & exact) || (score < -SCORE_LIMIT)) flag.timeout = true;
      /* find the next best move put below root */
#include "debug13.h"
      if (!flag.timeout)
	{
	  /* */
#if !defined NODYNALPHA
	  Jscore = (plyscore + score) >> 1;
#endif
	  zwndw = 20 + abs (Jscore / 12);
	  plyscore = score;
	  /* recompute search window */
	  beta = score + ((computer == white) ? BBwindow : WBwindow);
#if !defined NODYNALPHA
	  alpha = ((Jscore < score) ? Jscore : score) - ((computer == white) ? BAwindow : WAwindow) - zwndw;
#else
	  alpha = score - ((computer == white) ? BAwindow : WAwindow);
#endif
	}
#if !defined BAREBONES
#ifdef QUIETBACKGROUND
      if (!background)
#endif /* QUIETBACKGROUND */
	ShowResults (score, PrVar, '.');
#ifdef DEBUG41
      debug41 (score, PrVar, '.');
#endif
#endif
#include "debug16.h"
    }

  /******************************* end of main loop ***********************************/

  /* background mode */
  if (iop == 2)
    return;
#include "debug4.h"
  if (rpt >= 2)
    {
      root->flags |= draw;
      DRAW = CP[101];		/* Repetition */
    }
  else
    /* if no moves and not in check then draw */
  if ((score == -(SCORE_LIMIT+999)) && !(SqAtakd (PieceList[side][0], xside)))
    {
      root->flags |= draw;
      DRAW = CP[87];		/* No moves */
    }
  else if (GameCnt == MAXMOVES)
    {
      root->flags |= draw;
      DRAW = CP[80];		/* Max Moves */
    }
  /* not in book so set hint to guessed move for other side */
  if ( !bookflag )
    hint = ((PrVar[1]) ? PrVar[2] : 0);

  /* if not mate or draw make move and output it */
  if (((score > -(SCORE_LIMIT+999)) && (rpt <= 2)) || (root->flags & draw))
    {
      MakeMove (side, &Tree[0], &tempb, &tempc, &tempsf, &tempst, &INCscore);
      algbr (root->f, root->t, (short) root->flags);
    }
  else
    {
      algbr (0, 0, 0);		/* Zero move string when mate. */
      root->score = score;	/* When mate, ignore distinctions!
				 * --SMC */
    }
  g = &GameList[GameCnt];
  if ((g->flags & capture) && (g->piece == king))
    {
      flag.mate = flag.illegal = true;
    }
  /* If Time Control get the elapsed time */
  if ( TCflag )
    ElapsedTime (1);
  OutputMove ();
  /* if mate set flag */
#if BESTDEBUG
  printf("score=%d\n",score);
#endif
  if ((score == -(SCORE_LIMIT+999) || score == (SCORE_LIMIT+998)))
    flag.mate = true;
  /* if mate clear hint */
  if (flag.mate)
    hint = 0;
  Game50 = GameCnt;
  /*ZeroRPT; */ 
  /* add move to game list */
  g->score = score;
  g->nodes = NodeCnt;
  g->time = (et +50)/100;
/*
  g->time = TCcount;
*/
  g->depth = Sdepth;
#include "debug40.h"
  /* update time control info */
  if (TCflag)
    {
#if defined HARDTIMELIMIT
      TimeControl.clock[side] -= (et + OperatorTime + 45);
      timecomp[compptr] = (et + OperatorTime + 45);
#else
      TimeControl.clock[side] -= (et + OperatorTime);
      timecomp[compptr] = (et + OperatorTime);
#endif
      /* finished our required moves - setup the next set */
      --TimeControl.moves[side];
    }
  /* check for end conditions */
  if ((root->flags & draw) /* && flag.bothsides */ )
#if !defined CLIENT
     flag.mate = true;
#else 
	;
#endif
  else if (GameCnt == MAXMOVES)
    {
      flag.mate = true;
    }
  /* out of move store, you loose */
  else
    /* switch to other side */
    player = xside;
  Sdepth = 0;
}

int
search (short int side,
	register short int ply,
	register short int depth,
	short int alpha,
	short int beta,
	short unsigned int *bstline,
	short int *rpt)

/*
 * Perform an alpha-beta search to determine the score for the current board
 * position. If depth <= 0 only capturing moves and
 * responses to check are generated and searched, otherwise all moves are
 * processed. The search depth is modified for check evasions, certain
 * re-captures and threats. Extensions may continue for up to 11 ply beyond
 * the nominal search depth.
 */


{
  register short j, pnt;
  short tempb, tempc, tempsf, tempst;
  short xside, pbst, score, rcnt, slk, InChk;
  unsigned short mv, nxtline[MAXDEPTH];
  struct leaf *node, tmp;
  short best = -(SCORE_LIMIT+3000);
  short bestwidth = 0;
  short mustcut;
#ifdef NULLMOVE
  short int PVsave;
  short int PVarisave;
#endif
#ifdef DEBUG
  int xxxtmp;
  int tracetmp;
#endif
  NodeCnt++;
#ifdef PLYDEBUG
  if (ply > MaxPly) {
    MaxPly = ply;
    printf("ply %d\n",ply);
  }
  if (depth > MaxDepth) {
    MaxDepth = depth;
    printf("depth %d\n",depth);
  }
#endif
  /* look every ZNODE nodes for a timeout */
#ifdef NULLMOVE
  if (!null) {
#endif
  if (NodeCnt > ETnodes )
    { 
      ElapsedTime (2);
#if TIMEDEBUG
      printf("looking for a timeout... et=%d Resp.Time=%d ExtraTime=%d Sdepth=%d\n",
               et, ResponseTime, ExtraTime, Sdepth);
#endif
      if (flag.back)
	{
	  flag.back = false;
	  flag.timeout = true;
	  flag.musttimeout = false;
	}
      else if (TCflag || MaxResponseTime)
	{ 
	  if ((et >= (ResponseTime + ExtraTime)) && Sdepth > MINDEPTH )
	    {
	      /* try to extend to finish ply */
	      if (flag.back || (TCflag && TCcount < MAXTCCOUNTX))
		{ flag.back = false;
		  flag.musttimeout = true;
		  TCcount += 1;
		  ExtraTime += TCleft;
		}
	      else
		{ flag.back = false;
		  flag.timeout = true;
		  flag.musttimeout = false;
		}
	    }
	}
	else       if (flag.back)
        {
          flag.back = false;
          flag.timeout = true;
          flag.musttimeout = false;
        } 
    }
  else if (!TCflag && flag.musttimeout && Sdepth > MINDEPTH)
    {
      flag.timeout = true;
      flag.musttimeout = false;
    }
#ifdef NULLMOVE
  }
#endif
  xside = side ^ 1;
  score = evaluate (side, ply, alpha, beta, INCscore, &InChk);


#ifndef REPITITION
  *rpt = 0;
#else
  /*
   * check for possible repitition if so call repitition - rpt is
   * repeat count
   */
  if ((ply <= Sdepth + 3) && rpthash[side][hashkey & 0xFF] > 0)
    {
      *rpt = repetition ();

      /*
       * repeat position >2 don't need to return score it's taken
       * care of above
       */
      if (*rpt == 1) score /= 2;
    }
  else
    *rpt = 0;
#endif

  /* score > SCORE_LIMIT its a draw or mate */
  if (score > SCORE_LIMIT)
    {
      bstline[ply] = 0;
      return (score);
    }
  /* Do we need to add depth because of special conditions */
  /* if in check or in capture sequence search deeper */
  /*************************************** depth extensions ***********************************/
  if (depth > 0)
    {
      /* Allow opponent a chance to check again */
      if (InChk)
	depth = (depth < 2) ? 2 : depth;
      else if ( flag.rcptr && (score > alpha) &&
                (score < beta) && (ply > 2) && CptrFlag[ply - 1] && CptrFlag[ply - 2] )
	++depth;
    }
  else 
    {
      if (score >= alpha &&
	  (InChk || (hung[side] > 1 && ply == Sdepth + 1)))
	depth = 1;
      else if (score <= beta &&
	       ((ply < Sdepth + 4) && (ply > 4) &&
		ChkFlag[ply - 2] && ChkFlag[ply - 4] &&
		ChkFlag[ply - 2] != ChkFlag[ply - 4]))
	depth = 1;
    }
  /*******************************************************************************************/
  /* try the local transition table if it's there */
#if ttblsz
  if ( /* depth > 0 && */ flag.hash && ply > 1 )
    {
      if (ProbeTTable (side, depth, ply, &alpha, &beta, &score) == true)
	{
	  bstline[ply] = PV;
	  bstline[ply + 1] = 0;
#include "debug64.h"
	  if (beta == -((SCORE_LIMIT+1000)*2))
	    return (score);
	  if (alpha > beta)
	    return (alpha);
	}
#ifdef HASHFILE
      /* ok try the transition file if its there */
      else if (hashfile && (depth > HashDepth) && (GameCnt < HashMoveLimit)
	 && (ProbeFTable (side, depth, ply, &alpha, &beta, &score) == true))
	{
#ifdef notdef
	  int hgood = false;
	  int f = PV >> 8;
	  int t = PV & 0xff;
	  register int i;

	  /*
	   * if you find something put it in the local table
	   * for future reference
	   */
	  hgood = false;
	  for (i = TrPnt[ply]; i < TrPnt[ply + 1]; i++)
	    {
	      if (Tree[i].f == f && Tree[i].t == t)
		{
		  hgood = true;
		  break;
		}
	    }
	  if (hgood)
	    {
#endif
	      PutInTTable (side, score, depth, ply, alpha, beta, PV);
	      bstline[ply] = PV;
	      bstline[ply + 1] = 0;
	      if (beta == -((SCORE_LIMIT+1000)*2))
		return (score);
	      if (alpha > beta)
		return (alpha);
#ifdef notdef
	    }
#endif
#include "debug10.h"
	}
#endif /* HASHFILE */
    }
#endif /* ttblsz */

  if ( TrPnt[ply] > TREE-300 )
    {
      mustcut = true;
#ifdef NONDSP
      printf("mustcut ply %d TrPnt[ply] %d\n",ply,TrPnt[ply]);
#endif
    }
  else
    {
      mustcut = false;
    }

  /*
   * if more then DepthBeyond ply past goal depth or at goal depth and
   * score > beta quit - means we are out of the window
   */
  if (mustcut || ply > DepthBeyond || (depth < 1 && score > beta))
    {
      return (score);
    }

  /*
   * if below first ply and not at goal depth generate all moves else
   * only capture moves
   */
  if (ply > 1)
    if (depth > 0  || ply<(SDEPTHLIM) || 
	(background && ply<Sdepth + 2))
      {
	MoveList (side, ply);
      }
    else
      {
        CaptureList (side, ply);
      }

  /* no moves return what we have */

  /*
   * normally a search will continue til past goal and no more capture
   * moves exist
   */
  /* unless it hits DepthBeyond */
  if (TrPnt[ply] == TrPnt[ply + 1])
    {
      return (score);
    }

  /* if not at goal set best = -inf else current score */
  best = (depth > 0) ? -(SCORE_LIMIT+3000) : score;
#ifdef NULLMOVE
 
  PVarisave = PVari;
  if (!null &&                         /* no previous null-move */
      !PVari &&                        /* no null-move during the PV */
      (ply > 1) &                      /* not at ply 1 */
      (depth > 1) &&                   /* not during the quienscesearch */
      !InChk)                          /* no check */
    /* enough material such that zugzwang is unlike but who knows which value
       is suitable? */
    {
      
      /* ok, we make a null move, i.e.  this means we have nothing to do
 	 but we have to keep the some arrays up to date otherwise gnuchess
 	 gets confused.  Maybe somebody knows exactly which informations are
	 important and which not.

	 Another idea is that we try the null-move first and generate the
	 moves later.  This may save time but we have to take care that
	 PV and other variables contain the right value so that the move
	 ordering works right.
      */
      register struct GameRec *g;
      
      nxtline[ply + 1] = 0;
      CptrFlag[ply] = 0;
      Tscore[ply] = score;
      /*PVsave = PV;
      PV = 0;*/
      null = 1;
      g = &GameList[++GameCnt];
      g->hashkey = hashkey;
      g->hashbd = hashbd;
      FROMsquare = TOsquare = -1;
      g->Game50 = Game50;
      g->gmove = -1;
      g->flags = 0;
      g->piece = 0;
      g->color = neutral;
      
      best = -search(xside, ply+1, depth - 2, -beta-1, -beta, nxtline,&rcnt);
      null = 0;
      /*PV = PVsave;*/
      GameCnt--;
      if (best > beta)
 	return (best);
      else
 	best = -(SCORE_LIMIT+3000);
    }
#endif
  /* if best so far is better than alpha set alpha to best */
  if (best>alpha) 
    alpha = best;
  /********************** main loop ************************************************************************/
  /* look at each move until no more or beta cutoff */
  for (pnt = pbst = TrPnt[ply]; pnt < TrPnt[ply + 1] && best <= beta; pnt++)
    {
      /* find the most interesting looking of the remaining moves */
      if (ply > 1)
	pick (pnt, TrPnt[ply + 1] - 1);
#ifdef NULLMOVE
      PVari = PVarisave && (pnt == TrPnt[ply]);  /* Is this the PV? */
#endif

      node = &Tree[pnt];
      /* is this a forbidden move */
      if (ply == 1 && node->score == DONTUSE)
	continue;
#ifdef DEBUG
	if(debuglevel & (512 | 1024)){
		if(!tracen)traceflag = ((ply >traceply)?false:true);
	 	else
		if(ply <= tracen && (ply ==1 || traceflag))
			{ 
			if(trace[ply] == (Tree[pnt].t |(Tree[pnt].f<<8))) traceflag = true; else traceflag = false; }
		tracelog[ply] = (Tree[pnt].t |(Tree[pnt].f<<8));
		tracelog[ply+1] = 0;
}
#endif
      nxtline[ply + 1] = 0;

      /* if at top level */
      if (ply == 1)
	{			/* at the top update search status */
	  if (flag.post)
#ifdef QUIETBACKGROUND
	    if (!background)
#endif /* QUIETBACKGROUND */
	      ShowCurrentMove (pnt, node->f, node->t);
	}
      if ( !(node->flags & exact) )
	{
	  /* make the move and go deeper */
	  MakeMove (side, node, &tempb, &tempc, &tempsf, &tempst, &INCscore);
	  CptrFlag[ply] = (node->flags & capture);
	  Tscore[ply] = node->score;
	  PV = node->reply;
#ifdef DEBUG
	  xxxtmp = node->score;
	  tracetmp = traceflag;
#endif
	  node->score = -search (xside, ply + 1,
				 (depth > 0)?depth-1:0,
				 -beta, -alpha,
				 nxtline, &rcnt);
	  node->width = (ply % 2 == 1) ? (TrPnt[ply + 2] - TrPnt[ply + 1]) : 0;
	  if (abs (node->score) > SCORE_LIMIT) node->flags |= exact;
	  else if (rcnt == 1) node->score /= 2;
#include "debug256.h"
	  if ((rcnt >= 2 || GameCnt - Game50 > 99 || (node->score == (SCORE_LIMIT+999) - ply && !ChkFlag[ply])))
	    {
	      node->flags |= (draw | exact);
	      DRAW = CP[58];	/* Draw */
	      node->score = ((side == computer) ? contempt : -contempt);
	    }
	  node->reply = nxtline[ply + 1];
	  /* reset to try next move */
	  UnmakeMove (side, node, &tempb, &tempc, &tempsf, &tempst);
	}
      /* if best move so far */
      if (!flag.timeout && ((node->score > best) || ((node->score == best) && (node->width > bestwidth))))
	{
	  /*
	   * all things being equal pick the denser part of the
	   * tree
	   */
	  bestwidth = node->width;

	  /*
	   * if not at goal depth and better than alpha and not
	   * an exact score increment by depth
	   */
	  if (depth > 0 && node->score > alpha && !(node->flags & exact))
	    node->score += depth;
	  best = node->score;
	  pbst = pnt;
	  if (best > alpha) { alpha = best; }
	  /* update best line */
	  for (j = ply + 1; nxtline[j] > 0; j++) bstline[j] = nxtline[j];
	  bstline[j] = 0;
	  bstline[ply] = (node->f << 8) | node->t;
	  /* if at the top */
	  if (ply == 1)
	    {
	      /*
	       * if its better than the root score make it
	       * the root
	       */
	      if ((best > root->score) || ((best == root->score) && (bestwidth > root->width)))
		{
		  tmp = Tree[pnt];
		  for (j = pnt - 1; j >= 0; j--) Tree[j + 1] = Tree[j];
		  Tree[0] = tmp;
		  pbst = 0;
		}
#ifndef BAREBONES
#ifdef QUIETBACKGROUND
	      if (!background)
#endif /* QUIETBACKGROUND */
		if (Sdepth > 2)
		  if (best > beta)
		    {
		      ShowResults (best, bstline, '+');
#ifdef DEBUG41
		      debug41 (best, bstline, '+');
#endif
		    }
		  else if (best < alpha)
		    {
		      ShowResults (best, bstline, '-');
#ifdef DEBUG41
		      debug41 (best, bstline, '-');
#endif
		    }
		  else
		    ShowResults (best, bstline, '&');
#ifdef DEBUG41
	        debug41 (best, bstline, '&');
#endif
#else /* !BAREBONES */
	      if ( !background && Sdepth >2 && best < alpha) {
	         ExtraTime=8*TCleft;
	      }
#endif
	    }
	}
      if (flag.timeout)
	{
	  return (Tscore[ply - 1]);
	}
    }

  /******************************************************************************************/
  node = &Tree[pbst];
  mv = (node->f << 8) | node->t;
#ifdef NULLMOVE
  PVari = PVarisave;
#endif
#ifdef DEBUG
#include "debug512.h"
#endif

  /*
   * we have a move so put it in local table - if it's already there
   * done else if not there or needs to be updated also put it in
   * hashfile
   */
#if ttblsz
  if (flag.hash && ply <= Sdepth && *rpt == 0 && best == alpha)
    {
#ifdef notdef
algbr(node->f,node->t,0);
printf("IN-> %lx %lx %d %d %s\n",hashkey,hashbd,depth,side,mvstr[0]);
#endif
      if (PutInTTable (side, best, depth, ply, alpha, beta, mv)
#ifdef HASHFILE
	  && hashfile && (depth > HashDepth) && (GameCnt < HashMoveLimit))
	{
#ifdef notdef
printf("FT %d %d %d %x\n",side,best,depth,mv);
#endif
	  PutInFTable (side, best, depth, ply, alpha, beta, node->f, node->t);
	}
#else
	);
#endif /* HASHFILE */
    }
#endif /* ttblsz */
  if (depth > 0)
    {
#ifdef HISTORY
      unsigned short h;
      h = mv;
      if (history[hindex(side,h)] < HISTORYLIM)
	history[hindex(side,h)] += (unsigned short) 1<<depth;
#endif
      if (node->t != (short)(GameList[GameCnt].gmove & 0xFF))
	if (best <= beta)
	  killr3[ply] = mv;
	else if (mv != killr1[ply])
	  {
	    killr2[ply] = killr1[ply];
	    killr1[ply] = mv;
	  }
      killr0[ply] = ((best > SCORE_LIMIT) ? mv : 0);
    }
  return (best);
}


void
UpdatePieceList (short int side, short int sq, short int iop)

/*
 * Update the PieceList and Pindex arrays when a piece is captured or when a
 * capture is unmade.
 */

{
  register short i;

  if (iop == 1)
    {
      PieceCnt[side]--;
      for (i = Pindex[sq]; i <= PieceCnt[side]; i++)
	{
	  PieceList[side][i] = PieceList[side][i + 1];
	  Pindex[PieceList[side][i]] = i;
	}
    }
  else
  if ( board[sq] == king )
    { /* king must have index 0 */
      for (i = PieceCnt[side]; i >= 0; i--)
	{
	  PieceList[side][i + 1] = PieceList[side][i];
	  Pindex[PieceList[side][i + 1]] = i + 1;
	}
      PieceCnt[side]++;
      PieceList[side][0] = sq;
      Pindex[sq] = 0;
    }
  else
    {
      PieceCnt[side]++;
      PieceList[side][PieceCnt[side]] = sq;
      Pindex[sq] = PieceCnt[side];
    }
}


void
drop (short int side, short int piece, short int f, short int t, short int iop)

/* Make or Unmake drop move. */

{
  if ( iop == 1 )
    {   short cv;
	board[t] = piece;
	color[t] = side;
	svalue[t] = 0;
	mtl[side] -= (cv = CatchedValue(side,piece));
	mtl[side] += value[piece];
        --Captured[side][piece];
	UpdatePieceList (side, t, 2);
	if ( piece == pawn )
	  {
	    ++PawnCnt[side][column(t)];
 	    pmtl[side] -= cv;
	    pmtl[side] += value[pawn];
	  };
        Mvboard[t]++;
	HasPiece[side][piece]++;
        UpdateHashbd (side, piece, f, t);
    }
  else
    {   short cv;
	board[t] = no_piece;
	color[t] = neutral;
	++Captured[side][piece];
	mtl[side] += (cv = CatchedValue(side,piece));
	mtl[side] -= value[piece];
	UpdatePieceList (side, t, 1);
	if ( piece == pawn )
	  {
	    --PawnCnt[side][column(t)];
 	    pmtl[side] += cv;
	    pmtl[side] -= value[pawn];
	  };
        Mvboard[t]--;
	HasPiece[side][piece]--;
        UpdateHashbd (side, piece, f, t);
    }

}


#ifdef HASHKEYTEST
static
int
CheckHashKey ()
{ unsigned long chashkey, chashbd;
  short side, offset, sq, Stposition;
  chashbd = chashkey = 0;
  Stposition = true;
  for (sq = 0; sq < NO_SQUARES; sq++)
    { 
      if ((color[sq] != Stcolor[sq]) || (board[sq] != Stboard[sq]))
        Stposition = false;
      if (color[sq] != neutral)
        {
	  chashbd ^= hashcode[color[sq]][board[sq]][sq].bd;
	  chashkey ^= hashcode[color[sq]][board[sq]][sq].key;
        }
    }
  for ( side = 0, offset = NO_SQUARES; side <= 1; side++, offset += NO_PIECES ) {
    short piece;
    for ( piece = pawn; piece <= king; piece++ ) {
       short n = Captured[side][piece];
       if ( n > 0 ) {
  	 unsigned long bd, key;
	 unsigned short fpiece = piece + offset;
         short i;
         Stposition = false;
	 bd = hashcode[side][piece][fpiece].bd;
	 key = hashcode[side][piece][fpiece].key;
         for ( i = 0; i < n; i++ ) {
           chashbd ^= bd;
           chashkey ^= key;
         };   
       };
    };
  };       
  if ( Stposition )
    chashbd = chashkey = 0;
  if ( chashbd != hashbd ) 
    printf("chashbd %ld != hashbd %ld\n",chashbd,hashbd);
  if ( chashkey != hashkey ) 
    printf("chashkey %ld != hashkey %ld\n",chashkey,hashkey);
  if ( chashbd != hashbd || chashkey != hashkey )  {
    return(1);                                
  }
  return(0);
};             
#endif

void
MakeMove (short int side,
	  struct leaf *node,
	  short int *tempb,	/* piece at to square */
	  short int *tempc,	/* color of to square */
	  short int *tempsf,	/* static value of piece on from */
	  short int *tempst,	/* static value of piece on to */
	  short int *INCscore)	/* score increment */

/*
 * Update Arrays board[], color[], and Pindex[] to reflect the new board
 * position obtained after making the move pointed to by node. Also update
 * miscellaneous stuff that changes when a move is made.
 */

{
  register short f, t, xside, ct, cf;
  register struct GameRec *g;
  register short int fromb,fromc;

  xside = side ^ 1;
  g = &GameList[++GameCnt];
  g->hashkey = hashkey;
  g->hashbd = hashbd;
  FROMsquare = f = node->f;
  TOsquare = t = (node->t & 0x7f);

  *INCscore = 0;
  g->Game50 = Game50;
  g->gmove = (f << 8) | node->t;
  g->flags = node->flags;
#ifdef HASHKEYTEST
    if ( CheckHashKey () ) {
      algbr(f,t,node->flags);
      printf("error before MakeMove: %s\n", mvstr[0]);
      exit(1);
    }
#endif

  if (f > NO_SQUARES )
    { short piece;
      piece = f - NO_SQUARES;
      if ( side == white ) piece -= NO_PIECES;
      g->flags = node->flags = dropmask | piece; 
      g->piece = *tempb = no_piece;
      g->color = *tempc = neutral;
      *tempsf = 0;
      *tempst = svalue[t];
      (void) drop (side, piece, f, t, 1);
      Game50 = GameCnt;
    }
  else
    {
      Game50 = GameCnt;
      *tempsf = svalue[f];
      *tempst = svalue[t];
      g->piece = *tempb = board[t];
      g->color = *tempc = color[t];
      fromb = board[f];
      fromc = color[f];
      if (*tempc != neutral)
	{ /* Capture a piece */
	  UpdatePieceList (*tempc, t, 1);
	  /* if capture decrement pawn count */
	  if (*tempb == pawn)
	    {
	      --PawnCnt[*tempc][column (t)];
	    }
	  mtl[xside] -= value[*tempb];
	  HasPiece[xside][*tempb]--;
	  { short n, fpiece, upiece = unpromoted[*tempb];
	    ++Captured[side][upiece];
	    mtl[side] += CatchedValue(side,upiece);
 	    fpiece = upiece + NO_SQUARES;
	    if ( side == white )
	      fpiece += NO_PIECES;
	    UpdateHashbd (side, upiece, fpiece, -1);
	  }
	  if (*tempb == pawn)
	    {
	      pmtl[xside] -= value[pawn];
	      pmtl[side] += CatchedValue(side,pawn);
	    }
	  UpdateHashbd (xside, *tempb, -1, t);
	  Mvboard[t]++;
	}
      color[t] = fromc;
      svalue[t] = svalue[f];
      svalue[f] = 0;
      Pindex[t] = Pindex[f];
      PieceList[side][Pindex[t]] = t;
      color[f] = neutral;
      board[f] = no_piece;
      if (node->flags & promote)
	{ short tob;
	  board[t] = tob = promoted[fromb];
	  UpdateHashbd (side, fromb, f, -1);
	  UpdateHashbd (side, tob, -1, t);
	  mtl[side] += value[tob] - value[fromb];
	  if ( fromb == pawn )
	    { --PawnCnt[side][column(f)];
	      pmtl[side] -= value[pawn];
            };
	  HasPiece[side][fromb]--;
	  HasPiece[side][tob]++;
	}
      else
	{
	  board[t] = fromb;
	  UpdateHashbd (side, fromb, f, t);
	}
      Mvboard[f]++;
    } 
#ifdef HASHKEYTEST
    algbr(f,t,node->flags);
    if ( CheckHashKey () ) {
      printf("error in MakeMove: %s\n", mvstr[0]);
      exit(1);
    }
#endif
}

void
UnmakeMove (short int side,
	    struct leaf *node,
	    short int *tempb,
	    short int *tempc,
	    short int *tempsf,
	    short int *tempst)

/*
 * Take back a move.
 */

{
  register short f, t, xside;

  xside = side ^ 1;
  f = node->f;
  t = node->t & 0x7f;
  Game50 = GameList[GameCnt].Game50;
  if (f > NO_SQUARES )
    { short piece;
      piece = f - NO_SQUARES;
      if ( piece >= NO_PIECES ) piece -= NO_PIECES;
      node->flags = (dropmask | piece); 
    }
  if (node->flags & dropmask)
    {
      (void) drop (side, (node->flags & pmask), f, t, 2);
      svalue[t] = *tempst;
    } 
  else
    { short tob, fromb;
      color[f] = color[t];
      board[f] = tob = fromb = board[t];
      svalue[f] = *tempsf;
      Pindex[f] = Pindex[t];
      PieceList[side][Pindex[f]] = f;
      color[t] = *tempc;
      board[t] = *tempb;
      svalue[t] = *tempst;
      /* Undo move */
      if (node->flags & promote)
	{ 
	  board[f] = fromb = unpromoted[tob];
	  mtl[side] += value[fromb] - value[tob];
	  if ( fromb == pawn )
	    {
	      ++PawnCnt[side][column (f)];
	      pmtl[side] += value[pawn];
	    }
	  HasPiece[side][fromb]++;
	  HasPiece[side][tob]--;
	  UpdateHashbd (side, fromb, f, -1);
          UpdateHashbd (side, tob, -1, t);
	}
      else
	{
	  if ( fromb == pawn )
	    {
	      --PawnCnt[side][column (t)];
	      ++PawnCnt[side][column (f)];
	    };
	  UpdateHashbd (side, fromb, f, t);
	}
      /* Undo capture */
      if (*tempc != neutral)
	{ short fpiece, upiece = unpromoted[*tempb];
	  UpdatePieceList (*tempc, t, 2);
	  if (*tempb == pawn)
	    {
	      ++PawnCnt[*tempc][column (t)];
	    }
	  mtl[xside] += value[*tempb];
	  HasPiece[xside][*tempb]++;
	  mtl[side] -= CatchedValue(side,upiece);
          fpiece = upiece + NO_SQUARES;
          if (side == white) 
              fpiece += NO_PIECES;
	  UpdateHashbd (side, upiece, fpiece, -1);
	  if (*tempb == pawn)
	    {
	      pmtl[xside] += value[pawn];
	      pmtl[side] -= CatchedValue(side,pawn);
	    }
	  --Captured[side][upiece];
	  UpdateHashbd (xside, *tempb, -1, t);
	  Mvboard[t]--;
	}
      Mvboard[f]--;
      /* rpthash[side][hashkey & 0xFF]--; */
    }
    GameCnt--;
#ifdef HASHKEYTEST
    algbr(f,t,node->flags);
    if ( CheckHashKey () ) {
      printf("error in UnmakeMove: %s\n", mvstr[0]);
      exit(1);
    }
#endif
}


void
InitializeStats (void)

/*
 * Scan thru the board seeing what's on each square. If a piece is found,
 * update the variables PieceCnt, PawnCnt, Pindex and PieceList. Also
 * determine the material for each side and set the hashkey and hashbd
 * variables to represent the current board position. Array
 * PieceList[side][indx] contains the location of all the pieces of either
 * side. Array Pindex[sq] contains the indx into PieceList for a given
 * square.
 */

{
  register short i, sq;
#if HASHKEYTEST
  short Stposition;
#endif
  for (i = 0; i < NO_COLS; i++)
    {
      PawnCnt[black][i] = PawnCnt[white][i] = 0;
    }
  mtl[black] = mtl[white] = pmtl[black] = pmtl[white] = 0;
  PieceCnt[black] = PieceCnt[white] = 0;
  hashbd = hashkey = 0;
#if HASHKEYTEST
  Stposition = true;
#endif
  for (sq = 0; sq < NO_SQUARES; sq++)
    {                       
#if HASHKEYTEST
      if (color[sq] != Stcolor[sq] || board[sq] != Stboard[sq])
         Stposition = false;
#endif
      if (color[sq] != neutral)
        {
	  mtl[color[sq]] += value[board[sq]];
	  if (board[sq] == pawn)
	    {
	      pmtl[color[sq]] += value[pawn];
	      ++PawnCnt[color[sq]][column (sq)];
	    }
	  Pindex[sq] = ((board[sq] == king) ? 0 : ++PieceCnt[color[sq]]);

	  PieceList[color[sq]][Pindex[sq]] = sq;
	  hashbd ^= hashcode[color[sq]][board[sq]][sq].bd;
	  hashkey ^= hashcode[color[sq]][board[sq]][sq].key;
        }
    }
  { short color, offset;
    for ( color = 0, offset = NO_SQUARES; color <= 1; color++, offset += NO_PIECES ) {
      short piece;
      for ( piece = pawn; piece <= king; piece++ ) {
         short n = Captured[color][piece];
	 if ( n > 0 ) {
	   unsigned long bd, key;
	   unsigned short fpiece = piece + offset;
#if HASHKEYTEST
           Stposition = false;
#endif
	   bd = hashcode[color][piece][fpiece].bd;
	   key = hashcode[color][piece][fpiece].key;
    	   Captured[color][piece] = 0;
           for ( i = 0; i < n; i++ ) {
	     ++Captured[color][piece];
	     mtl[color] += CatchedValue(color,piece);
             hashbd ^= bd;
             hashkey ^= key;
           };
         };
      };
    };
  }
#ifdef HASHKEYTEST
    if ( Stposition )
      hashbd = hashkey = 0;
    if ( CheckHashKey () ) {
      printf("error in InitializeStats\n");
      exit(1);
    }
#endif
}
