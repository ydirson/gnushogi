/*
 * search.c - C source for GNU SHOGI
 *
 * Copyright (c) 1993, 1994, 1995 Matthias Mutz
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
#ifdef DEBUG
#include <assert.h>
#endif
#if !defined OLDTIME && defined HASGETTIMEOFDAY
double pow();
#endif
short background = 0;
static short int DepthBeyond;
unsigned short int PrVar[MAXDEPTH];
extern short int recycle, ISZERO;
#ifdef NULLMOVE
short int null;         /* Null-move already made or not */
short int PVari;        /* Is this the PV */
#endif
#ifdef DEBUG40
extern int whichway;
#endif
#ifdef DEBUG
unsigned short DBLINE[MAXDEPTH];
struct leaf far *dbptr;

#endif 



#ifdef DEBUG_EVAL
extern short debug_eval;
extern FILE *debug_eval_file;
#endif


short int zwndw;


#ifdef DEBUG41
void
debug41 (short int score, short unsigned int xxx[], char ch)
{
  register int i;
  FILE *D;
  int r, c, l;
  struct leaf far *xnode;

  D = fopen ("/tmp/DEBUG", "a+");
  if (D == NULL)
    {
      perror ("opening D");
    }

  ElapsedTime (2);
  fprintf (D, "%2d%c %6d %4ld %8ld  ", Sdepth, ch, score, et / 100, NodeCnt);

  for (i = 1; xxx[i]; i++)
    {
      if ((i > 1) && (i % 8 == 1))
	fprintf (D, "\n                          ");
      algbr ((short) (xxx[i] >> 8), (short) (xxx[i] & 0xFF), false);
      fprintf (D, "%5s ", mvstr[0]);
    }
  fprintf (D, "\n");
  fclose (D);
}

#endif




/* ............    MOVE GENERATION & SEARCH ROUTINES    .............. */




short int
repetition ()

/*  Check for draw by fourfold repetition 
 *  (same side, same captures, same board).
 *  WARNING: this is not save yet due to possible hash collisions.  
 */

{     
  short int i, cnt = 0;

#ifndef NOREPETITION
  struct GameRec far *g;

  if (GameCnt > Game50 + 6)
    {             
      for (i = GameCnt-1; i >= Game50; i -= 2)
	{ 
	  g = &GameList[i];
	  if ( g->hashkey == hashkey && g->hashbd == hashbd )
	    {
	      cnt++;
	    }
	}
    }

#endif

  return (cnt);
}


int plyscore, globalscore;
int
pick (short int p1, short int p2)

/*
 * Find the best move in the tree between indexes p1 and p2. Swap the best
 * move into the p1 element.
 *
 */
{
  register struct leaf far *p, *q, *r, *k;
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
int Jscore = 0;

/* #define PLYDEBUG */

#ifdef PLYDEBUG
static int MaxPly = 0;
static int MaxDepth = 0;
#endif

int TCcount;
long TCleft = 0;



#ifdef DEBUG4
static void debug4(short Sdepth, short alpha, short beta, short stage)
{
  if (debuglevel & 4)
    {
      int j;

      printf ("Sdepth %d alpha %d beta %d stage %d\n", Sdepth, alpha, beta, stage);
      for (j = 1; j < 2; j++)
	{
	  int idb;

	  for (idb = TrPnt[j]; idb < TrPnt[j + 1]; idb++)
	    {
	      algbr (Tree[idb].f, Tree[idb].t, Tree[idb].flags);
	      printf ("level 4 %d-->%d %s %d %d\n", j, idb, mvstr[0], Tree[idb].score, Tree[idb].width);
	    }
	}
    }
}
#endif               


void
SelectMove (short int side, SelectMove_mode iop)


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
  static struct GameRec far *g;
  short blocked;       
  short sqking, in_check, blockable;

#ifdef PLYDEBUG
  MaxPly = 0;
  MaxDepth = 0;
#endif

#ifdef DEBUG

if ( debuglevel & (512 | 1024) ) {
	char b[32];
	short c1,c2,r1,r2,piece;
	tracen = 0;
	traceflag = false;
	traceply = 0;
	tracelog[0] = 0;
	while ( true ) {
	  printf( "debug?" );
	  gets(b);
	  if ( b[0] == 'p' ) 
		traceply = atoi(&b[1]);
	  else if ( b[0] == '\0' )
		break;
	  else 
	    {
		if ( b[1] = '*' || b[1] == '\'' ) 
		  {      
		    for (piece = pawn; piece <= king; piece++)
			if ( b[0] == pxx[piece] || b[0] == qxx[piece] )
			  break;
		    c2 = '9' - b[2];
      		    r2 = 'i' - b[3];
		    if ( side == white )
	piece += NO_PIECES;		
      		    trace[++tracen] = ((NO_SQUARES+piece) << 8) | locn (r2, c2);
		  }
		else
		  {
		    c1 = '9' - b[0];
      		    r1 = 'i' - b[1];
      		    c2 = '9' - b[2];
      		    r2 = 'i' - b[3];
      		    trace[++tracen] = (locn (r1, c1) << 8) | locn (r2, c2);
		    if ( b[4] == '+' )
			trace[tracen] |= 0x80;
		  }
	    }
	  if (tracen == 0 && traceply > 0) 
		traceflag = true;
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

  ExaminePosition (side);

  /* if background mode set to infinite */
  if (iop == BACKGROUND_MODE)
    {
      background = true;
      /* if background mode set response time to infinite */
      ResponseTime = 9999999;
    }
  else
    {
      player = side;
      SetResponseTime (side);
    }

#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    ShowResponseTime ();

  ExtraTime = 0;

  score = ScorePosition (side);

#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    ShowSidetoMove ();

#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    SearchStartStuff (side);

#ifdef HISTORY
  array_zero (history, sizeof_history);
#endif

  FROMsquare = TOsquare = -1;
  PV = 0;
  if (iop == FOREGROUND_MODE)
    hint = 0;
    
#ifdef DEBUG
  { 
    char buf1[8], buf2[8], buf3[8], buf4[8];
    movealgbr(GameList[GameCnt].gmove,buf1);
    movealgbr(PrVar[1],buf2);
    movealgbr(PrVar[2],buf3);
    movealgbr(PrVar[3],buf4);
    printf("gmove: %s PrVar[1]: %s PrVar[2]: %s PrVar[3]: %s\n",
		buf1, buf2, buf3, buf4);
  }
#endif

  /*            
   * If the last move was the hint, select the computed answer to the
   * hint as first move to examine.
   */
                   
#if MAXDEPTH > 3
  if ( GameCnt > 0 ) {
    SwagHt = (GameList[GameCnt].gmove == PrVar[2]) ? PrVar[3] : 0;
  } else      
#endif
    SwagHt = 0;

#ifdef DEBUG
  {
    char buf[8];
    movealgbr(SwagHt,buf);
    printf("SwagHt = %s\n", buf); 
  }
#endif

  for (i = 0; i < MAXDEPTH; i++)
    PrVar[i] = killr0[i] = killr1[i] = killr2[i] = killr3[i] = 0;

  /* set initial window for search */

  if ( flag.tsume ) { 
    alpha =  -(SCORE_LIMIT+999);
    beta = SCORE_LIMIT+999;
  } else { 
    alpha = score - ((computer == white) ? BAwindow : WAwindow);
    beta = score + ((computer == white) ? BBwindow : WBwindow);
  }
 
  rpt = 0;
  TrPnt[1] = 0;
  root = &Tree[0];          
                     
  sqking = PieceList[side][0];
  in_check = (board[sqking] == king) ? SqAtakd(sqking, side^1, &blockable) : false;

  MoveList (side, 1, in_check, blockable);
  for (i = TrPnt[1]; i < TrPnt[2]; i++)
    if (!pick (i, TrPnt[2] - 1))
      break;  

#ifdef DEBUG_EVAL
  if ( debug_eval )
    { short j;
      for (j = TrPnt[1]; j < TrPnt[2]; j++)
        {
          struct leaf far *node = &Tree[j];
          algbr (node->f, node->t, node->flags);
          fprintf (debug_eval_file, "%s %s %s %s %d", 
            mvstr[0], mvstr[1], mvstr[2], mvstr[3],node->score);
          if ( node->flags )
	    { char s[80];
	      FlagString(node->flags, s);
	      fprintf(debug_eval_file,"%s",s);
	    }  
          fprintf (debug_eval_file, "\n");
        }
    }
#endif

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
  Jscore = 0;
  zwndw = 20;

#ifdef DEBUG4
  debug4(Sdepth,alpha,beta,stage);
#endif

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
      /* terminate search at DepthBeyond ply past goal depth */
      if ( flag.tsume )
        DepthBeyond = Sdepth;
      else
#if defined SLOW_CPU
        DepthBeyond = Sdepth + ((Sdepth == 1) ? 3 : 5);
#else
        DepthBeyond = Sdepth + ((Sdepth == 1) ? 7 : 11);
#endif

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
#ifdef HARDTIMELIMIT
	      ExtraTime += (MAXTCCOUNTR - TCcount) * TCleft;
#else
	      ExtraTime += (8 * TCleft);
#endif
	      TCcount = MAXTCCOUNTR - 1;
	    }

	  score = search (side, 1, Sdepth, -(SCORE_LIMIT+999), (SCORE_LIMIT+999), PrVar, &rpt);
	} 

       /* high search failure re-search with (score, +inf) limits */
       else if (score > beta && !(root->flags & exact))
	{
#if !defined BAREBONES
	  replus++;
#ifdef QUIETBACKGROUND
	  if (!background)
#endif /* QUIETBACKGROUND */
	    ShowDepth ('+');
#endif
	  score = search (side, 1, Sdepth, -(SCORE_LIMIT+999), (SCORE_LIMIT+999), PrVar, &rpt);
	}

      /**************** out of search ********************************************/
      CheckForTimeout (score, globalscore, Jscore, zwndw);

      /************************ time control ***********************************/

      /* save PV as killer */
      for (i = 1; i <= Sdepth + 1; i++)
	killr0[i] = PrVar[i];
      if (!flag.timeout) 
	Tscore[0] = score;
      /* if (!flag.timeout) */
/*
      for (i = TrPnt[1]+1; i < TrPnt[2]; i++) 
	if (!pick (i, TrPnt[2] - 1))
	  break;
*/
      /* if done or nothing good to look at quit */
      if ((root->flags & exact) || (score < -SCORE_LIMIT))
	flag.timeout = true;
      /* find the next best move put below root */
#ifdef DEBUG13
      if (flag.timeout && !background)
	{
	  FILE *D;
	  int r, c, l;
	  struct leaf far *xnode;

	  D = fopen ("/tmp/DEBUG", "a+");
	  fprintf (D, " %d ply %d sco %d TC %d gl %d cnt %d\n",
		   Sdepth, plyscore, score, TCcount,
		   globalpnt, TrPnt[2] - TrPnt[1]);
	  for (i = 1; tmp[i]; i++)
	    {
	      algbr (tmp[i] >> 8, tmp[i] & 0xff, 0);
	      fprintf (D, "%s ", mvstr[0]);
	    }
	  fprintf (D, "\n");
	  for (i = 1; PrVar[i]; i++)
	    {
	      algbr (PrVar[i] >> 8, PrVar[i] & 0xff, 0);
	      fprintf (D, "%s ", mvstr[0]);
	    }
	  fprintf (D, "\n");
	  algbr (root->f, root->t, root->flags);
	  fprintf (D, "%s ", mvstr[0]);
	  fprintf (D, "\n");
	  fclose (D);
	}
#endif
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
#ifdef DEBUG4
      if (debuglevel & 16)
	{
	  int j;

	  printf ("Sdepth %d alpha %d beta %d stage %d\n", Sdepth, alpha, beta, stage);
	  for (j = 1; j < 2; j++)
	    {
	      int idb;

	      for (idb = TrPnt[j]; idb < TrPnt[j + 1]; idb++)
		{
		  algbr (Tree[idb].f, Tree[idb].t, Tree[idb].flags);
		  printf ("level 16 %d-->%d %s %d %d %x\n", Sdepth, idb, mvstr[0], Tree[idb].score, Tree[idb].width, Tree[idb].flags);
		}
	    }
	}
#endif
    }

  /******************************* end of main loop ***********************************/
      
  /* background mode */
  if (iop == BACKGROUND_MODE)
    {
      return;
    }

#ifdef DEBUG4
  debug4(Sdepth,alpha,beta,stage);
#endif

  if (rpt >= 3)
    {
      root->flags |= draw;
      DRAW = CP[101];		/* Repetition */
    }
  else
    /* if no moves and not in check then mate for shogi (draw for chess) */
#ifdef notdef 
  if ((score == -(SCORE_LIMIT+999)) && !(SqAtakd (PieceList[side][0], xside, &blocked)))
    {
      root->flags |= mate;
      DRAW = CP[87];		/* No moves */
    }
  else        
#endif
  if (GameCnt == MAXMOVES)
    {
      root->flags |= draw;
      DRAW = CP[80];		/* Max Moves */
    }
  /* not in book so set hint to guessed move for other side */
  if ( !bookflag )
    hint = ((PrVar[1]) ? PrVar[2] : 0);

  /* if not mate or draw make move and output it */
  if (((score > -(SCORE_LIMIT+999)) && (rpt <= 3)) || (root->flags & draw))
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
    ElapsedTime (COMPUTE_AND_INIT_MODE);
  /* update time control info */
  OutputMove ();
  /* if mate set flag */
#if BESTDEBUG
  printf("score=%d\n",score);
#endif
  if ((score == -(SCORE_LIMIT+999) || score == (SCORE_LIMIT+998)))
    flag.mate = true;
  /* add move to game list */
  g->score = score;
  g->nodes = NodeCnt;
  g->time = (et +50)/100;
/*
  g->time = TCcount;
*/
  g->depth = Sdepth;

#ifdef DEBUG40
  g->d1 = TCcount;
  g->d2 = ResponseTime;
  g->d3 = ExtraTime;
  g->d4 = TCleft;
  g->d5 = et;
  g->d6 = hint;
  g->d7 = whichway;
#endif

  /* update time control info */
  if (TCflag)
    {
#if defined XSHOGI
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
    {
      flag.mate = true;
    }
  else if (GameCnt == MAXMOVES)
    {
      flag.mate = true;
    }
  /* out of move store, you loose */
  else
    /* switch to other side */
    player = xside;
  /* if mate clear hint */
  if (flag.mate)
    hint = 0;
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
  short xside, pbst, score, rcnt, slk, in_check, blockable;
  unsigned short mv, nxtline[MAXDEPTH];
  struct leaf far *node, tmp;
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
      ElapsedTime (COMPUTE_MODE);
      if (flag.back)
	{
	  flag.back = false;
	  flag.timeout = true;
	  flag.musttimeout = false;
	}
      else if (TCflag || MaxResponseTime)
	{                                                                           
	  if ((et >= (ResponseTime + ExtraTime)) && (Sdepth > MINDEPTH) )
	    {
	      /* try to extend to finish ply */
	      if (flag.back || (TCflag && TCcount < MAXTCCOUNTX))
		{ flag.back = false;
		  flag.musttimeout = true;
		  TCcount++;
		  ExtraTime += TCleft;
		}
	      else
		{ flag.back = false;
		  flag.timeout = true;
		  flag.musttimeout = false;
		}
	    }
	}
	else if (flag.back)
        {
          flag.back = false;
          flag.timeout = true;
          flag.musttimeout = false;
        } 
#ifdef QUIETBACKGROUND
      if (!background)
#endif
        ShowResponseTime ();
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
  score = evaluate (side, ply, alpha, beta, INCscore, &in_check, &blockable);

  /*
   * check for possible repitition if so call repitition - rpt is
   * repeat count
   */
  if ((ply <= Sdepth + 3) && rpthash[side][hashkey & 0xFF] > 0)
    {
      *rpt = repetition ();

      /*
       * repeat position >3 don't need to return score it's taken
       * care of above
       */
      if (*rpt == 1) { score /= 3; score *= 2; }
      else if (*rpt == 2) score /= 2;
    }
  else
    *rpt = 0;

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
      if (in_check) {
	if (depth < 2) depth = 2;
      } else if ( flag.rcptr && 
#ifdef HARDTIMELIMIT
		!flag.timeout &&
#endif
		(score > alpha) && (score < beta) && (ply > 2) && 
		CptrFlag[ply - 1] && CptrFlag[ply - 2])
	++depth;
    }
  else 
    {
      if (score >= alpha &&
#ifdef HARDTIMELIMIT
	  (in_check || (!flag.timeout && hung[side] > 1 && ply == Sdepth + 1)))
#else
	  (in_check || (hung[side] > 1 && ply == Sdepth + 1)))
#endif
	depth = 1;
      else if (score <= beta &&
	       ((ply < Sdepth + 4) && (ply > 4) &&
		ChkFlag[ply - 2] && ChkFlag[ply - 4] &&
		ChkFlag[ply - 2] != ChkFlag[ply - 4]))
	depth = 1;
#ifdef notdef
      else if ( score >= alpha &&
		TesujiFlag[ply - 1] )
	depth = 1;
#endif
    }
  /*******************************************************************************************/
  /* try the local transition table if it's there */

#if ttblsz
  if ( /* depth > 0 && */ flag.hash && ply > 1 )
    {
      if (use_ttable && ProbeTTable (side, depth, ply, &alpha, &beta, &score) == true)
	{
	  bstline[ply] = PV;
	  bstline[ply + 1] = 0;
#ifdef DEBUG4
	  if (debuglevel & 64)
	    {
	      algbr (PV >> 8, PV & 0xff, 0);
	      printf ("-get-> d=%d s=%d p=%d a=%d b=%d %s\n", depth, score, ply, alpha, beta, mvstr[0]);
	    }
#endif
	  if (beta == -((SCORE_LIMIT+1000)*2)) {
	    return (score);
	  }
	  if (alpha > beta) {
	    return (alpha);  
	  }    
	}
#ifdef HASHFILE
      /* ok try the transition file if its there */
      else if (hashfile && (depth > HashDepth) && (GameCnt < HashMoveLimit)
	 && (ProbeFTable (side, depth, ply, &alpha, &beta, &score) == true))
	{
	      PutInTTable (side, score, depth, ply, alpha, beta, PV);
	      bstline[ply] = PV;
	      bstline[ply + 1] = 0;
	      if (beta == -((SCORE_LIMIT+1000)*2)) {
		return (score);   
 	      } 
	      if (alpha > beta) {
		return (alpha);  
	      }    
#ifdef DEBUG10
	  else
	    {
	      FILE *D;
	      int r, c, l;
	      struct leaf far *xnode;
              short side;

	      D = fopen ("/tmp/DEBUG", "w");
	      pnt = TrPnt[2];
	      fprintf (D, "hashfile failure\n");
	      algbr (PV >> 8, PV & 0xff, 0);
	      fprintf (D, "inout move is %s\n", mvstr);
	      fprintf (D, "legal move are \n");
	      for (r = TrPnt[ply]; r < TrPnt[ply + 1]; r++)
		{
		  xnode = &Tree[r];
		  algbr (xnode->f, xnode->t, (short) xnode->flags);
		  fprintf (D, "%s %s %s %s\n", mvstr[0], mvstr[1], mvstr[2], mvstr[3]);
		}
	      debug_position (D);
	      fclose (D);
	    }
#endif /* DEBUG10 */
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
    if (depth > 0  || ply < (SDEPTHLIM) || 
	(background && ply < Sdepth + 2))
      {
	MoveList (side, ply, in_check, blockable);
      }
    else
      {
        CaptureList (side, ply, in_check, blockable);
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
      (ply > 2) &                      /* not at ply 1 */
      (ply <= Sdepth) &&
      (depth > 3) &&           
      !in_check)                       /* no check */
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
      register struct GameRec far *g;
      
      nxtline[ply + 1] = 0;
      CptrFlag[ply] = 0;
      TesujiFlag[ply] = 0;
      Tscore[ply] = score;
      PVsave = PV;
      PV = 0;
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
      
      best = -search (xside, ply+1, depth - 2, - beta-1, -beta, nxtline, &rcnt);
      null = 0;
      PV = PVsave;
      GameCnt--;
      if (best < alpha) 
	best = -(SCORE_LIMIT+3000);
      else if (best > beta) {
 	return (best);
      } else
 	best = -(SCORE_LIMIT+3000);
    }       
#endif
  /* if best so far is better than alpha set alpha to best */
  if (best > alpha) 
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
      if (/* ply == 1 && */ node->score <= DONTUSE)
	continue;
#ifdef DEBUG
	if(debuglevel & (512 | 1024)){
		if ( !tracen ) traceflag = ((ply >traceply)?false:true);
	 	else
		if(ply <= tracen && (ply ==1 || traceflag))
			{ 
			if(trace[ply] == (Tree[pnt].t |(Tree[pnt].f<<8))) traceflag = true; else traceflag = false; }
		tracelog[ply] = (Tree[pnt].t | (Tree[pnt].f<<8));
		tracelog[ply+1] = 0;
}
#endif
      nxtline[ply + 1] = 0;

      /* if at top level */
#if !defined NOPOST
      if (ply == 1)
	{			/* at the top update search status */
	  if (flag.post)
#ifdef QUIETBACKGROUND
	    if (!background)
#endif /* QUIETBACKGROUND */
	      ShowCurrentMove (pnt, node->f, node->t);
	}
#endif
      if ( !(node->flags & exact) )
	{
	  /* make the move and go deeper */
#ifdef DEBUG_EVAL
	  if ( debug_eval )
	    {                                   
		algbr(node->f, node->t, 0);
		fprintf(debug_eval_file,"%s (ply %d depth %d)\n",
		  	  mvstr[0], ply, depth);
	    }
#endif	  
	  MakeMove (side, node, &tempb, &tempc, &tempsf, &tempst, &INCscore);
	  CptrFlag[ply] = (node->flags & capture);
	  TesujiFlag[ply] = (node->flags & tesuji) && (node->flags & dropmask);
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
/*
          if(!flag.timeout)node->score = score;
*/
	  node->width = (ply % 2 == 1) ? (TrPnt[ply + 2] - TrPnt[ply + 1]) : 0;
	  if (node->score > SCORE_LIMIT || node->score < -SCORE_LIMIT) 
	    node->flags |= exact;
	  else if (rcnt == 1) 
	    node->score /= 2;
#ifdef DEBUG
	  traceflag = tracetmp;
          if (debuglevel & 256 || ((debuglevel & 1024) && traceflag && 
			 	   (!traceply || ply<= traceply))) {
             int i;
             algbr(node->f, node->t, node->flags);
             for (i = 1; i < ply; i++)
                printf("    ");
             printf("%s S%d d%d p%d n%d s%d a%d b%d best%d x%d\n", 
		mvstr[0], Sdepth, depth, ply, node->score, score, alpha, beta, best,xxxtmp);
          }
#endif
	  if ((rcnt >= 3 || (node->score == (SCORE_LIMIT+999) - ply && !ChkFlag[ply])))
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
	    {
	      node->score += depth;
	    }
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
	      if ( !background && Sdepth > 2) {
		if ( best < alpha) { 
		  TCcount = 0; ExtraTime += 9*TCleft;
		}
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
if (debuglevel & 2560)
{
	int             j;

	if (debuglevel & 512 && (tracen > 0 && traceflag))
	{
	traceline[0]='\0';
	for (j=1;tracelog[j];j++){
		algbr(tracelog[j]>>8,tracelog[j]&0xff,0);
		strcat(traceline," ");
		strcat(traceline,mvstr[0]);
	}

	printf("Ply %d alpha %d beta %d score %d %s\n", ply, alpha, beta, score,traceline);
	if(debuglevel & 2048){
		for (j = ply; j < ply + 1; j++)
		{
			int             idb;

			for (idb = TrPnt[j]; idb < TrPnt[j + 1]; idb++)
			{
				algbr(Tree[idb].f, Tree[idb].t, Tree[idb].flags);
				printf("level 512 %d-->%d %s %d %d %x\n", ply, idb, mvstr[0], Tree[idb].score, Tree[idb].width, Tree[idb].flags);
			}
		}
}
}
	}

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
      if (use_ttable && PutInTTable (side, best, depth, ply, alpha, beta, mv)
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
#if defined HISTORY
      unsigned short h,x;
      h = mv;      
      if (history[x=hindex(side,h)] < HISTORYLIM)
	history[x] += (unsigned short) 1<<depth;
#endif               //
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
UpdatePieceList (short int side, short int sq, UpdatePieceList_mode iop)

/*
 * Update the PieceList and Pindex arrays when a piece is captured or when a
 * capture is unmade.
 */

{
  register short i;

  if (iop == REMOVE_PIECE)
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
    {   short cv, n;
	board[t] = piece;
	color[t] = side;
#if !defined SAVE_SVALUE
	svalue[t] = 0;
#endif
	n = Captured[side][piece]--;
#ifdef DEBUG
	assert(n>0);
#endif
	UpdateDropHashbd (side, piece, n);
        UpdateHashbd (side, piece, -1, t);
	UpdatePieceList (side, t, ADD_PIECE);
	if ( piece == pawn )
	  {
	    ++PawnCnt[side][column(t)];
	  };
        Mvboard[t]++;
	HasPiece[side][piece]++;
    }
  else
    {   short cv, n;
	board[t] = no_piece;
	color[t] = neutral;
	n = ++Captured[side][piece];
#ifdef DEBUG
	assert(n>0);
#endif
	UpdateDropHashbd (side, piece, n);
        UpdateHashbd (side, piece, -1, t);
	UpdatePieceList (side, t, REMOVE_PIECE);
	if ( piece == pawn )
	  {
	    --PawnCnt[side][column(t)];
	  };
        Mvboard[t]--;
	HasPiece[side][piece]--;
    }

}


#ifdef HASHKEYTEST
int
CheckHashKey ()
{ unsigned long chashkey, chashbd;
  short side, sq;
  chashbd = chashkey = 0;
  for (sq = 0; sq < NO_SQUARES; sq++)
    {
      if (color[sq] != neutral)
        {
	  chashbd ^= (*hashcode)[color[sq]][board[sq]][sq].bd;
	  chashkey ^= (*hashcode)[color[sq]][board[sq]][sq].key;
        } 
      /* hashcodes for initial board are 0 ! */
      if ( Stcolor[sq] != neutral )
	{
          chashbd ^= (*hashcode)[Stcolor[sq]][Stboard[sq]][sq].bd;
          chashkey ^= (*hashcode)[Stcolor[sq]][Stboard[sq]][sq].key;
	}
    }
  for ( side = 0; side <= 1; side++ ) {
    short piece;
    for ( piece = 0; piece < NO_PIECES; piece++ ) {
       short n = Captured[side][piece];
#ifdef DEBUG
	 assert(n==0 || piece>0);
#endif
       if ( n > 0 ) {
	 short i;   
	 for ( i = 1; i <= n; i++ )
	  {
	    chashbd ^= (*drop_hashcode)[side][piece][i].bd;
	    chashkey ^= (*drop_hashcode)[side][piece][i].key;
	  }
       };
    };
  };       
  if ( chashbd != hashbd ) 
    printf("chashbd %lu != hashbd %lu\n",chashbd,hashbd);
  if ( chashkey != hashkey ) 
    printf("chashkey %lu != hashkey %lu\n",chashkey,hashkey);
  if ( chashbd != hashbd || chashkey != hashkey )  {
    return(1);                                
  }
  return(0);
};             
#endif

void
MakeMove (short int side,
	  struct leaf far *node,
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
  register struct GameRec far *g;
  register short int fromb,fromc;

  xside = side ^ 1;
  g = &GameList[++GameCnt];
  g->hashkey = hashkey;
  g->hashbd = hashbd;
  FROMsquare = f = node->f;
  TOsquare = t = (node->t & 0x7f);
  *INCscore = (short int)node->INCscore;
  g->Game50 = Game50;
  g->gmove = (f << 8) | node->t;
  g->flags = node->flags;

#ifdef HASHKEYTEST
    if ( CheckHashKey () ) {
      short i;
      algbr(f,t,node->flags);
      printf("error before MakeMove: %s\n", mvstr[0]);
      UpdateDisplay (0, 0, 1, 0);
      for ( i=1; i<=GameCnt; i++ ) {
        movealgbr(GameList[i].gmove,mvstr[0]);
        printf("%d: %s\n", i, mvstr[0]);
      }
      exit(1);
    }
#endif

  rpthash[side][hashkey & 0xFF]++, ISZERO++;

#ifdef DEBUG
  assert(f != NO_SQUARES);
#endif

  if (f > NO_SQUARES )
    { 
#ifdef DEBUG
      short piece;
      piece = f - NO_SQUARES;
      if ( side == white ) piece -= NO_PIECES;
      assert(node->flags & dropmask);
      assert((node->flags & pmask) == piece);
#endif
      g->fpiece = (node->flags & pmask); 
      g->piece = *tempb = no_piece;
      g->color = *tempc = neutral;
#if !defined SAVE_SVALUE
      *tempsf = 0;
      *tempst = svalue[t];
#endif
      (void) drop (side, g->fpiece, f, t, 1);
    }
  else
    { short piece;

#if !defined SAVE_SVALUE
      *tempsf = svalue[f];
      *tempst = svalue[t];
#endif
      g->fpiece = board[f];
      g->piece = *tempb = board[t];
      g->color = *tempc = color[t];
      fromb = board[f];
      fromc = color[f];
      if (*tempc != neutral)
	{ /* Capture a piece */
	  UpdatePieceList (*tempc, t, REMOVE_PIECE);
	  /* if capture decrement pawn count */
	  if (*tempb == pawn)
	    {
	      --PawnCnt[*tempc][column (t)];
	    }           
	  mtl[xside] -= (*value)[stage][*tempb];
	  HasPiece[xside][*tempb]--;
	  { short n, upiece = unpromoted[*tempb];
	    /* add "upiece" captured by "side" */ 
	    n = ++Captured[side][upiece];
#ifdef DEBUG
	    assert(n>0);
#endif
	    UpdateDropHashbd (side, upiece, n);
	    mtl[side] += (*value)[stage][upiece];
	  }
	  /* remove "*tempb" of "xside" from board[t] */
	  UpdateHashbd (xside, *tempb, -1, t);
#if !defined SAVE_SVALUE
	  *INCscore += *tempst; /* add value of catched piece to own score */
#endif
	  Mvboard[t]++;
	}
      color[t] = fromc;
#if !defined SAVE_SVALUE
      svalue[t] = svalue[f];
      svalue[f] = 0;
#endif
      Pindex[t] = Pindex[f];
      PieceList[side][Pindex[t]] = t;
      color[f] = neutral;
      board[f] = no_piece;
      if (node->flags & promote)
	{ short tob;
	  board[t] = tob = promoted[fromb];
	  /* remove unpromoted piece from board[f] */
	  UpdateHashbd (side, fromb, f, -1);         
	  /* add promoted piece to board[t] */
	  UpdateHashbd (side, tob, -1, t);
	  mtl[side] += value[stage][tob] - value[stage][fromb];
	  if ( fromb == pawn )
	    { --PawnCnt[side][column(f)];
            };
	  HasPiece[side][fromb]--;
	  HasPiece[side][tob]++;
#if !defined SAVE_SVALUE
	  *INCscore -= *tempsf;
#endif
	}
      else
	{
	  board[t] = fromb;
	  /* remove piece from board[f] and add it to board[t] */
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
#ifdef DEBUG
    assert(Captured[black][0]==0 && Captured[white][0]==0);
#endif
}

void
UnmakeMove (short int side,
	    struct leaf far *node,
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

#ifdef DEBUG
  assert(f != NO_SQUARES);

  if (f > NO_SQUARES )
    { short piece;
      piece = f - NO_SQUARES;
      if ( piece >= NO_PIECES ) piece -= NO_PIECES;
      assert(node->flags & dropmask);
      assert((node->flags & pmask) == piece);
    }       
#endif

  if (node->flags & dropmask)
    {
      (void) drop (side, (node->flags & pmask), f, t, 2);
#if !defined SAVE_SVALUE
      svalue[t] = *tempst;
#endif
    } 
  else
    { short tob, fromb;
      color[f] = color[t];
      board[f] = tob = fromb = board[t];
#if !defined SAVE_SVALUE
      svalue[f] = *tempsf;
#endif
      Pindex[f] = Pindex[t];
      PieceList[side][Pindex[f]] = f;
      color[t] = *tempc;
      board[t] = *tempb;
#if !defined SAVE_SVALUE
      svalue[t] = *tempst;
#endif
      /* Undo move */
      if (node->flags & promote)
	{ 
	  board[f] = fromb = unpromoted[tob];
	  mtl[side] += value[stage][fromb] - value[stage][tob];
	  if ( fromb == pawn )
	    {
	      ++PawnCnt[side][column (f)];
	    }
	  HasPiece[side][fromb]++;
	  HasPiece[side][tob]--;
	  /* add unpromoted piece to board[f] */
	  UpdateHashbd (side, fromb, f, -1);    
	  /* remove promoted piece from board[t] */
          UpdateHashbd (side, tob, -1, t);
	}
      else
	{
	  if ( fromb == pawn )
	    {
	      --PawnCnt[side][column (t)];
	      ++PawnCnt[side][column (f)];
	    };
	  /* remove piece from board[t] and add it to board[f] */
	  UpdateHashbd (side, fromb, f, t);
	}
      /* Undo capture */
      if (*tempc != neutral)
	{ short n, upiece = unpromoted[*tempb];
	  UpdatePieceList (*tempc, t, ADD_PIECE);
	  if (*tempb == pawn)
	    {
	      ++PawnCnt[*tempc][column (t)];
	    }           
	  mtl[xside] += (*value)[stage][*tempb];
	  HasPiece[xside][*tempb]++;
	  mtl[side] -= (*value)[stage][upiece];
	  /* remove "upiece" captured by "side" */
	  n = Captured[side][upiece]--;
#ifdef DEBUG
	  assert(n>0);
#endif
	  UpdateDropHashbd (side, upiece, n);
	  /* replace captured piece on board[t] */
	  UpdateHashbd (xside, *tempb, -1, t);
	  Mvboard[t]--;
	}
      Mvboard[f]--;
    }
    GameCnt--;
    rpthash[side][hashkey & 0xFF]--, ISZERO--;
#ifdef HASHKEYTEST
    algbr(f,t,node->flags);
    if ( CheckHashKey () ) {
      printf("error in UnmakeMove: %s\n", mvstr[0]);
      exit(1);
    }
#endif
#ifdef DEBUG
    assert(Captured[black][0]==0 && Captured[white][0]==0);
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
  
  for (i = 0; i < NO_COLS; i++)
    {
      PawnCnt[black][i] = PawnCnt[white][i] = 0;
    }
  mtl[black] = mtl[white] = 0;
  PieceCnt[black] = PieceCnt[white] = 0;
  hashbd = hashkey = 0;
  for (sq = 0; sq < NO_SQUARES; sq++)
    {             
      if (color[sq] != neutral)
        {
	  mtl[color[sq]] += (*value)[stage][board[sq]];
	  if (board[sq] == pawn)
	    {
	      ++PawnCnt[color[sq]][column(sq)];
	    }
	  Pindex[sq] = ((board[sq] == king) ? 0 : ++PieceCnt[color[sq]]);

	  PieceList[color[sq]][Pindex[sq]] = sq;
	  UpdateHashbd(color[sq],board[sq],sq,-1);
        }
      /* hashcodes for initial board are 0 ! */
      if ( Stcolor[sq] != neutral )
        UpdateHashbd(Stcolor[sq],Stboard[sq],sq,-1);
    }
  { short side;
    for ( side = 0; side <= 1; side++ ) {
      short piece;
      for ( piece = 0; piece < NO_PIECES; piece++ ) {
         short n = Captured[side][piece];
	 if ( n > 0 ) {
    	   Captured[side][piece] = 0;
           for ( i = 1; i <= n; i++ ) {
	     ++Captured[side][piece];
	     UpdateDropHashbd(side,piece,i);
	     mtl[side] += (*value)[stage][piece];
           };
         };
      };
    };
  }
#ifdef HASHKEYTEST
    if ( CheckHashKey () ) {
      printf("error in InitializeStats\n");
      exit(1);
    }
#endif

}
