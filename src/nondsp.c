/*
 * nondsp.c - UNIX & MSDOS AND NON-DISPLAY interface for GNU SHOGI
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
extern int EADD,EGET;
#include <ctype.h>
#include <signal.h>
#if defined THINK_C
#include <time.h>
#elif defined MSDOS
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#else
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
void TerminateSearch (int), Die (int);

#endif /* MSDOS */

#include "gnushogi.h"

#ifdef DEBUG
short int debuglevel = 1024;
#endif /* DEBUG */
unsigned short int MV[MAXDEPTH];
int MSCORE;

#if defined XSHOGI
short int xshogi = 1;

#else
short int xshogi = 0;

#endif /* XSHOGI */
int mycnt1, mycnt2;
char *DRAW;
extern char *InPtr;
extern short int pscore[];

void
Initialize (void)
{
  mycnt1 = mycnt2 = 0;
#if defined XSHOGI && !defined THINK_C && !defined MSDOS
#ifndef SYSV
  setlinebuf (stdout);
#else
  setvbuf (stdout, NULL, _IOLBF, BUFSIZ);
#endif
  printf ("GNU Shogi %sp%s\n", version, patchlevel);
#endif XSHOGI
#ifdef HARDTIMELIMIT
  if (!TCflag && (MaxResponseTime == 0))
    MaxResponseTime = 15L*100L;
#endif
}

void
ExitChess (void)
{
  signal (SIGTERM, SIG_IGN);
#ifndef NOLIST
  ListGame ();
#endif
}

#ifndef MSDOS			/* never called!!! */
void
Die (int sig)
{
  char s[80];

  ShowMessage (CP[31]);		/*Abort?*/
  scanz ("%s", s);
  if (strcmp (s, CP[210]) == 0)	/*yes*/
    ExitChess ();
}

#endif /* MSDOS */

void
TerminateSearch (int sig)
{
#ifdef MSDOS
  sig++;			/* shut up the compiler */
#endif /* MSDOS */
#ifdef INTERRUPT_TEST
  ElapsedTime(INIT_INTERRUPT_MODE);
#endif
  if (!flag.timeout)
    flag.back = true; /* previous: flag.timeout = true; */
  flag.bothsides = false;
#ifdef DEBUG
  printf("Terminate Search\n");
#endif
}

 


void
help (void)
{
  ClrScreen ();
  /*printz ("SHOGI command summary\n");*/
  printz (CP[40]);
  printz ("----------------------------------------------------------------\n");
  /*printz ("7g7f      move from 7g to 7f      quit      Exit Shogi\n");*/
  printz (CP[158]);
  /*printz ("S6h       move silver to 6h       beep      turn %s\n", (flag.beep) ? "off" : "on");*/
  printz (CP[86], (flag.beep) ? CP[92] : CP[93]);
  /*printz ("2d2c+     move from 2d to 2c and promote\n");*/
  printz (CP[128], (flag.material) ? CP[92] : CP[93]);
  /*printz ("P*5e      drop pawn to 5e         easy      turn %s\n", (flag.easy) ? "off" : "on");*/
  printz (CP[173], (flag.easy) ? CP[92] : CP[93]);
  /*printz ("                                  hash      turn %s\n", (flag.hash) ? "off" : "on");*/
  printz (CP[174], (flag.hash) ? CP[92] : CP[93]);
  /*printz ("bd        redraw board            reverse   board display\n");*/
  printz (CP[130]);
  /*printz ("list      game to shogi.lst       book      turn %s used %d of %d\n", (Book) ? "off" : "on", bookcount);*/
  printz (CP[170], (Book) ? CP[92] : CP[93], bookcount,booksize);
  /*printz ("undo      undo last ply           remove    take back a move\n");*/
  printz (CP[200]);
  /*printz ("edit      edit board              force     enter game moves\n");*/
  printz (CP[153]);
  /*printz ("switch    sides with computer     both      computer match\n");*/
  printz (CP[194]);
  /*printz ("black     computer plays black    white     computer plays white\n");*/
  printz (CP[202]);
  /*printz ("depth     set search depth        clock     set time control\n");*/
  printz (CP[149]);
  /*printz ("post      principle variation     hint      suggest a move\n");*/
  printz (CP[177]);
  /*printz ("save      game to file            get       game from file\n");*/
  printz (CP[188]);
    printz ("xsave     pos. to xshogi file     xget      pos. from xshogi file\n");
  /*printz ("random    randomize play          new       start new game\n");*/
  printz (CP[181]);
  printz ("----------------------------------------------------------------\n");
  /*printz ("Computer: %-12s Opponent:            %s\n",*/
  printz (CP[46],
	  ColorStr[computer], ColorStr[opponent]);
  /*printz ("Depth:    %-12d Response time:       %d sec\n",*/
  printz (CP[51],
	  MaxSearchDepth, MaxResponseTime/100);
  /*printz ("Random:   %-12s Easy mode:           %s\n",*/
  printz (CP[99],
	  (dither) ? CP[93] : CP[92], (flag.easy) ? CP[93] : CP[92]);
  /*printz ("Beep:     %-12s Transposition file: %s\n",*/
  printz (CP[36],
	  (flag.beep) ? CP[93] : CP[92], (flag.hash) ? CP[93] : CP[92]);
  /*printz ("Tsume:    %-12s Force:               %s\n")*/
  printz (CP[232], 
	  (flag.tsume) ? CP[93] : CP[92], (flag.force) ? CP[93] : CP[92]);
  /*printz ("Time Control %s %d moves %d seconds %d opr %d depth\n", (TCflag) ? "ON" : "OFF",*/
  printz (CP[110], (TCflag) ? CP[93] : CP[92],
	  TimeControl.moves[black], TimeControl.clock[black] / 100, TCadd/100, MaxSearchDepth);
  signal (SIGINT, TerminateSearch);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, TerminateSearch);
#endif /* MSDOS */
}                 


void
EditBoard (void)

/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, Nf3 will place a knight on square f3.
 */

{
  short a, r, c, sq, i, found;
  char s[80];

  flag.regularstart = true;
  Book = BOOKFAIL;
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
  /*printz (".   exit to main\n");*/
  printz (CP[29]);
  /*printz ("#   clear board\n");*/
  printz (CP[28]);
  /*printz ("c   change sides\n");*/
  printz (CP[136]);
  /*printz ("enter piece & location: \n");*/
  printz (CP[155]);

  a = black;
  do
    {
      scanz ("%s", s);
      found=0;
      if (s[0] == CP[28][0])	/*#*/
	{ short side;
	  for (sq = 0; sq < NO_SQUARES; sq++)
	    {
	      board[sq] = no_piece;
	      color[sq] = neutral;
	    };
	  ClearCaptured ();
	}
      if (s[0] == CP[136][0])	/*c*/
	a = otherside[a];
      if ( s[1] == '*' )
	{ 
	  for ( i = pawn; i <= king; i++)
	    if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
              { 
	      	Captured[a][i]++; 
                found=1;
		break;
 	      } 
	  c = -1;
          r = -1;
	}
      else
	{
          c = '9' - s[1];
          r = 'i' - s[2];
	}
      if ((c >= 0) && (c < NO_COLS) && (r >= 0) && (r < NO_ROWS))
	{
	  sq = locn (r, c);
	  color[sq] = a;
	  board[sq] = no_piece;
	  for (i = no_piece; i <= king; i++)
	    if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
	      { 
		if ( s[3] == '+' )
		  board[sq] = promoted[i];
		else
		  board[sq] = i;
		found=1;
		break;
	      }
	  if (found==0) color[sq] = neutral;	
	}
  } while (s[0] != CP[29][0]);
  for (sq = 0; sq < NO_SQUARES; sq++)
    Mvboard[sq] = ((board[sq] != Stboard[sq]) ? 10 : 0);
  GameCnt = 0;
  Game50 = 1;
  ZeroRPT ();
  Sdepth = 0;
  InitializeStats ();
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
}

void
SetupBoard (void)

/*
 * Set up a board position.
 * Nine lines of nine characters are used to setup the board. 9a-1a is the
 * first line. White pieces are  represented  by  uppercase characters.
 */

{
  short r, c, sq, i;
  char ch;
  char s[80];

  NewGame ();

  gets (s);			/* skip "setup" command */
  for (r = NO_ROWS-1; r >= 0; r--)
    {
      gets (s);
      for (c = 0; c <= (NO_COLS-1); c++)
	{
	  ch = s[c];
	  sq = locn (r, c);
	  color[sq] = neutral;
	  board[sq] = no_piece;
	  for (i = no_piece; i <= king; i++)
	    if (ch == pxx[i])
	      {
		color[sq] = white;
		board[sq] = i;
		break;
	      }
	    else if (ch == qxx[i])
	      {
		color[sq] = black;
		board[sq] = i;
		break;
	      }
	}
    }
  for (sq = 0; sq < NO_SQUARES; sq++)
    Mvboard[sq] = ((board[sq] != Stboard[sq]) ? 10 : 0);
  InitializeStats ();
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
  /*printz ("Setup successful\n");*/
  printz (CP[106]);
}

void
ShowDepth (char ch)
{
#ifdef MSDOS
  ch++;				/* shut up the compiler */
#endif /* MSDOS */
#if !defined BAREBONES
  printz (CP[53], Sdepth, ch);	/*Depth= %d%c*/
  printz ("\n");
#endif
}


void
ShowStage (void)
{
  printz("stage = %d\n",stage);
  printz("balance[black] = %d balance[white] = %d\n",balance[black],balance[white]);
}


void
ShowLine (short unsigned int *bstline)
{
  register int i;

  for (i = 1; bstline[i] > 0; i++)
    {
      if ((i > 1) && (i % 8 == 1))
	printf ("\n                          ");
      algbr ((short) (bstline[i] >> 8), (short) (bstline[i] & 0xFF), false);
      printf ("%5s ", mvstr[0]);
    }
  printf ("\n");
}

void
ShowResults (short int score, short unsigned int *bstline, char ch)
{
  if (flag.post)
    {
      ElapsedTime (2);
      printf ("%2d%c %6d %4ld %8ld  ", Sdepth, ch, score, et / 100, NodeCnt);
      ShowLine (bstline);
    }
}

void
ShowPatternCount (short side, short n)
{
  if (flag.post)
    {
        printz("%s matches %d pattern(s)\n",ColorStr[side],n);
    }
}

void
ShowResponseTime (void)
{
#ifdef DEBUG
  if (flag.post)
    {
        printz("RT=%ld TCC=%d TCL=%ld EX=%ld ET=%ld TO=%d\n",
          ResponseTime,TCcount,TCleft,ExtraTime,et,flag.timeout);
    }
#endif
}

void
ShowGameType (void)
{
  if (flag.post)
    {
        printz("%c vs. %c\n",GameType[black],GameType[white]);
    }
}

void
SearchStartStuff (short int side)
{
  signal (SIGINT, TerminateSearch);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, TerminateSearch);
#endif /* MSDOS */
  if (flag.post)
    {
      printf (CP[123],
	       GameCnt/2+1,
	       ResponseTime, TimeControl.clock[side]);
    }
}
void
OutputMove (void)
{
#ifdef DEBUG11
  if (1)
    {
      FILE *D;
      extern unsigned short int PrVar[];
      char d[80];
      int r, c, l, i;
      D = fopen ("/tmp/DEBUGA", "a+");
      fprintf (D, "inout move is %s\n", mvstr[0]);
      strcpy (d, mvstr[0]);
      for (i = 1; PrVar[i] > 0; i++)
	{
	  algbr ((short) (PrVar[i] >> 8), (short) (PrVar[i] & 0xFF), false);
	  fprintf (D, "%5s ", mvstr[0]);
	}
      fprintf (D, "\n");
      fprintf_current_board (D);
      fclose (D);
      strcpy (mvstr[0], d);
    }
#endif
  if (flag.illegal) {printf("%s\n",CP[225]);return;}
  if (mvstr[0][0] == '\0') goto nomove;
#ifdef XSHOGI
  /* add remaining time in milliseconds to xshogi */
  printz ("%d. ... %s %ld\n", ++mycnt1, mvstr[0], (TimeControl.clock[player]-et)*10);
#else
  printz ("%d. ... %s\n", ++mycnt1, mvstr[0]);
#endif
#ifdef notdef /* optional pass best line to frontend with move */
  if (flag.post)
    {
      register int i;

      printz (" %6d%c ", MSCORE, MV[30]);
      for (i = 1; MV[i] > 0; i++)
	{
	  algbr ((short) (MV[i] >> 8), (short) (MV[i] & 0xFF), false);
	  printz ("%5s ", mvstr[0]);
	}
    }
  printz ("\n");
#endif
nomove:
  if ((root->flags & draw)||(root->score == -(SCORE_LIMIT+999))||
      (root->score == (SCORE_LIMIT+998))) goto summary;
  if (flag.post)
    {
      short h, l, t;

      h = TREE;
      l = 0;
      t = TREE >> 1;
      while (l != t)
	{
	  if (Tree[t].f || Tree[t].t)
	    l = t;
	  else
	    h = t;
	  t = (l + h) >> 1;
	}
      /*printf ("Nodes %ld Tree %d Eval %ld Rate %ld RS high %ld low %ld\n",*/
      printf (CP[89],GenCnt,NodeCnt,t,EvalNodes,(et>100)?(NodeCnt/(et/100)):0,EADD,EGET,reminus,replus);
      /*printf ("Hin/Hout/Coll/Fin/Fout = %ld/%ld/%ld/%ld/%ld\n",*/
      printf (CP[71],
	       HashAdd, HashCnt, THashCol, HashCol,FHashCnt, FHashAdd);
    }
  UpdateDisplay (root->f, root->t, 0, root->flags);
  if ( !xshogi )
    {
      /*printf ("My move is: %s\n", mvstr[0]);*/
      printf (CP[83], mvstr[0]);
      if (flag.beep)
        printz ("%c", 7);
    }
 summary:
  if (root->flags & draw)
    /*	printf ("Drawn game!\n");*/
    printf (CP[57]);
  else if (root->score == -(SCORE_LIMIT+999))
    printf("%s mates!\n",ColorStr[opponent]);
  else if (root->score == (SCORE_LIMIT+998))
    printf("%s mates!\n",ColorStr[computer]);
#if !defined BAREBONES
#ifdef VERYBUGGY
  else if (root->score < -SCORE_LIMIT)
    printf("%s has a forced mate in %d moves!\n",
    	ColorStr[opponent], SCORE_LIMIT+999 + root->score - 1);
  else if (root->score > SCORE_LIMIT)
    printf("%s has a forced mate in %d moves!\n",
    	ColorStr[computer], SCORE_LIMIT+998 - root->score - 1);
#endif /*VERYBUGGY*/
#endif /* BAREBONES */
}

void
ClrScreen (void)
{
#if !defined BAREBONES
  printz ("\n");
#endif
}

void
UpdateDisplay (short int f, short int t, short int redraw, short int isspec)
{

  short r, c, l, m;

  if (redraw && !xshogi)
    {
      printz ("\n");
      r = (short)(TimeControl.clock[black] / 6000);
      c = (short)((TimeControl.clock[black] % 6000) / 100);
      l = (short)(TimeControl.clock[white] / 6000);
      m = (short)((TimeControl.clock[white] % 6000) / 100);
      /*printz ("Black %d:%02d  White %d:%02d\n", r, c, l, m);*/
      printz (CP[116], r, c, l, m);
      printz ("\n");
      for (r = (NO_ROWS-1); r >= 0; r--)
	{
	  for (c = 0; c <= (NO_COLS-1); c++)
	    { char pc;
	      l = ((flag.reverse) ? locn ((NO_ROWS-1) - r, (NO_COLS-1) - c) : locn (r, c));
	      pc = (is_promoted[board[l]] ? '+' : ' ');
	      if (color[l] == neutral)
		printz (" -");
	      else if (color[l] == black)
		printz ("%c%c", pc, qxx[board[l]]);
	      else
		printz ("%c%c", pc, pxx[board[l]]);
	    }
	  printz ("\n");
	}
      printz ("\n");
      {  
	short side;
	for (side = black; side <= white; side++)
	  { short piece, c; 
	    printz((side==black)?"black ":"white ");
            for (piece = pawn; piece <= king; piece++)
	      if (c = Captured[side][piece]) 
		printz("%i%c ",c,pxx[piece]);
            printz("\n");
          };
      }
    }
}

void
ShowMessage (char *s)
{
  printf("%s\n", s);
}

void
ShowSidetoMove (void)
{
}

void
PromptForMove (void)
{
#if !defined BAREBONES
  /*printz ("\nYour move is? ");*/
  printz (CP[124]);
#endif /* BAREBONES */
}


void
ShowCurrentMove (short int pnt, short int f, short int t)
{
#ifdef MSDOS
  f++;
  t++;
  pnt++;			/* shut up the compiler */
#endif /* MSDOS */
}

void
ChangeAlphaWindow (void)
{
  printz ("WAwindow: ");
  scanz ("%hd", &WAwindow);
  printz ("BAwindow: ");
  scanz ("%hd", &BAwindow);
}

void
ChangeBetaWindow (void)
{
  printz ("WBwindow: ");
  scanz ("%hd", &WBwindow);
  printz ("BBwindow: ");
  scanz ("%hd", &BBwindow);
}

void
GiveHint (void)
{
  if (hint)
    {
      algbr ((short) (hint >> 8), (short) (hint & 0xFF), false);
      printf(CP[72], mvstr[0]);	/*hint*/
    }
  else
    printz (CP[223]);
}

void
SelectLevel (char *sx)
{

  char T[NO_SQUARES], *p, *q;

  if ( (p = strstr(sx,CP[169])) != NULL ) 
    p += strlen(CP[169]);
  else if ( (p = strstr(sx,CP[217])) != NULL ) 
    p += strlen(CP[217]);
  strcat(sx,"XX");
  q = T; *q = '\0';
  for(;*p != 'X';*q++ = *p++);
  *q = '\0';
/* line empty ask for input */
  if(!T[0]){ printz (CP[61]); gets(T); strcat(T,"XX"); }
/* skip blackspace */
  for (p = T; *p == ' '; p++) ;
/* could be moves or a fischer clock */
  if(*p == 'f') { /* its a fischer clock game */
	p++;
	TCminutes = (short)strtol(p,&q,10);
	TCadd = (short)strtol(q,NULL,10) *100;
	TCseconds = 0;
	TCmoves = 50;
  } else { /* regular game */
  TCadd = 0;
  TCmoves = (short)strtol (p, &q, 10);
  TCminutes = (short)strtol (q, &q, 10);
  if (*q == ':')
    TCseconds = (short)strtol (q + 1, (char **) NULL, 10);
  else
    TCseconds = 0;
#ifdef OPERATORTIME
  printz (CP[94]);
  scanz ("%hd", &OperatorTime);
#endif
  if (TCmoves == 0) {
    TCflag = false;
    MaxResponseTime = TCminutes*60L*100L + TCseconds*100L;
    TCminutes = TCseconds = 0;
  } else {
    TCflag = true;
    MaxResponseTime = 0;
  }
}
  TimeControl.clock[black] = TimeControl.clock[white] = 0;
  SetTimeControl ();
#if defined XSHOGI
  printz ("Clocks: %ld %ld\n",TimeControl.clock[black]*10,TimeControl.clock[white]*10);
#endif
}

#ifdef DEBUG
void
ChangeDbLev (void)
{
  printz (CP[146]);
  scanz ("%hd", &debuglevel);
}

#endif /* DEBUG */

void
ChangeSearchDepth (void)
{
  printz ("depth= ");
  scanz ("%hd", &MaxSearchDepth);
  TCflag = !(MaxSearchDepth > 0);
}

void
ChangeHashDepth (void)
{
  printz ("hashdepth= ");
  scanz ("%hd", &HashDepth);
  printz ("MoveLimit= ");
  scanz ("%hd", &HashMoveLimit);
}

void
SetContempt (void)
{
  printz ("contempt= ");
  scanz ("%hd", &contempt);
}

void
ChangeXwindow (void)
{
  printz ("xwndw= ");
  scanz ("%hd", &xwndw);
}

void
ShowPostnValue (short int sq)

/*
 * must have called ExaminePosition() first
 */

{
  short score;
  score = ScorePosition (color[sq]);
  if (color[sq] != neutral){
#if defined SAVE_SVALUE
    printz ("???%c ", (color[sq] == white)?'b':'w');}
#else
    printz ("%3d%c ", svalue[sq],(color[sq] == white)?'b':'w');}
#endif
  else
    printz(" *   ");
}

void
DoDebug (void)
{
  short c, p, sq, tp, tc, tsq, score,j,k;
  char s[40];

  ExaminePosition (opponent);
  ShowMessage (CP[65]);
  scanz ("%s", s);
  c = neutral;
  if (s[0] == CP[9][0] || s[0] == CP[9][1])     /* w W*/ c = black;
  if (s[0] == CP[9][2] || s[0] == CP[9][3])     /*b B*/ c = white;
  for (p = king; p > no_piece; p--)
    if ((s[1] == pxx[p]) || (s[1] == qxx[p])) break;
  if(p > no_piece)
  for(j=(NO_ROWS-1);j>=0;j--){
  for(k=0;k<(NO_COLS);k++){
      sq=j*(NO_COLS)+k;
      tp = board[sq];
      tc = color[sq];
      board[sq] = p;
      color[sq] = c;
      tsq = PieceList[c][1];
      PieceList[c][1] = sq;
      ShowPostnValue (sq);
      PieceList[c][1] = tsq;
      board[sq] = tp;
      color[sq] = tc;
    }
      printz("\n");
    }                                
  score = ScorePosition (opponent);
  for(j=(NO_ROWS-1);j>=0;j--){
  for(k=0;k<(NO_COLS);k++){
      sq=j*(NO_COLS)+k;
      if (color[sq] != neutral){
#if defined SAVE_SVALUE
        printz ("%?????%c ", (color[sq] == white)?'b':'w');}
#else
        printz ("%5d%c ", svalue[sq],(color[sq] == white)?'b':'w');}
#endif
      else
        printz("    *  ");
    }
      printz("\n");
    }
    printz("stage = %d\n",stage);
  printz (CP[103], score, 
             mtl[computer], pscore[computer], GameType[computer],
             mtl[opponent], pscore[opponent], GameType[opponent]);
}

void
DoTable (short table[NO_SQUARES])
{
  short  sq,j,k;
  ExaminePosition (opponent);
  for(j=(NO_ROWS-1);j>=0;j--){
  for(k=0;k<NO_COLS;k++){
    sq=j*(NO_ROWS)+k;
    printz ("%3d ", table[sq]);
  }
printz("\n");
}
}

void
ShowPostnValues (void)
{
  short sq, score,j,k;
  ExaminePosition (opponent);
  for(j=(NO_ROWS-1);j>=0;j--){
  for(k=0;k<NO_COLS;k++){
  sq=j*NO_COLS+k;
    ShowPostnValue (sq);
  }
    printz("\n");
  }
  score = ScorePosition (opponent);
 printz (CP[103], score, 
           mtl[computer], pscore[computer], GameType[computer],
           mtl[opponent], pscore[opponent], GameType[opponent]);
 printz("\nhung black %d hung white %d\n",hung[black],hung[white]);
}

