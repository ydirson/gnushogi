/*
 * uxdsp.c - ALPHA interface for GNU SHOGI
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

#include <ctype.h>
#include <signal.h>

#if defined MSDOS
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ESC 0x1B
#define refresh() fflush(stdout)

static void param (short n);

#elif defined THINK_C

#include <stdio.h>
#include <console.h>

#define clear() cgotoxy(1,1,stdout),ccleos(stdout)
#define refresh() fflush(stdout)

#else

#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#include <curses.h>

#endif /* MSDOS */

#include "gnushogi.h"

int mycnt1, mycnt2;

#define TAB (58)

#define VIR_C(s)  ((flag.reverse) ? 8-column(s) : column(s))
#define VIR_R(s)  ((flag.reverse) ? 8-row(s) : row(s))

unsigned short int MV[MAXDEPTH];
int MSCORE;
char *DRAW;

void TerminateSearch (int), Die (int);

void
Initialize (void)
{
  signal (SIGINT, Die);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, Die);
  initscr ();
  crmode ();
#else
  mycnt1 = mycnt2 = 0;
#endif /* MSDOS */
}

void
ExitChess (void)
{ 
#ifndef NOLIST
  ListGame ();
#endif
  gotoXY (1, 24);
#if !defined MSDOS && !defined THINK_C
  refresh();
  nocrmode ();
  endwin ();
#endif /* MSDOS */
  exit (0);
}

void
Die (int Sig)
{
  char s[80];

  signal (SIGINT, SIG_IGN);
#if defined MSDOS || defined THINK_C
  Sig++;			/* shut up the compiler */
#else
  signal (SIGQUIT, SIG_IGN);
#endif /* MSDOS */
  ShowMessage (CP[31]);		/*Abort?*/
  scanz ("%s", s);
  if (strcmp (s, CP[210]) == 0)	/*yes*/
    ExitChess ();
  signal (SIGINT, Die);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, Die);
#endif /* MSDOS */
}

void
TerminateSearch (int Sig)
{
  signal (SIGINT, SIG_IGN);
#if defined MSDOS || defined THINK_C
  Sig++;			/* shut up the compiler */
#else
  signal (SIGQUIT, SIG_IGN);
#endif /* MSDOS */
  if (!flag.timeout)
    flag.musttimeout = true;
  ShowMessage("Terminate Search");
  flag.bothsides = false;
  signal (SIGINT, Die);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, Die);
#endif /* MSDOS */
}
void
ShowLine (short unsigned int *bstline)
{
}

void
help (void)
{
  ClrScreen ();
  /*printz ("GNU Shogi ??p? command summary\n");*/
  printz (CP[40], version, patchlevel);
  printz ("----------------------------------------------------------------\n");
  /*printz ("7g7f      move from 7g to 7f      quit      Exit Chess\n");*/
  printz (CP[158]);
  /*printz ("S6h       move silver to 6h       beep      turn %s\n", (flag.beep) ? "off" : "on");*/
  printz (CP[86], (flag.beep) ? CP[92] : CP[93]);
  /*printz ("2d2c+     move to 2c and promote\n");*/
  printz (CP[128], (flag.material) ? CP[92] : CP[93]);
  /*printz ("P*5e      drop a pawn to 5e       easy      turn %s\n", (flag.easy) ? "off" : "on");*/
  printz (CP[173], (flag.easy) ? CP[92] : CP[93]);
  /*printz ("                                  hash      turn %s\n", (flag.hash) ? "off" : "on");*/
  printz (CP[174], (flag.hash) ? CP[92] : CP[93]);
  /*printz ("bd        redraw board            reverse   board display\n");*/
  printz (CP[130]);
  /*printz ("list      game to shogi.lst       book      turn %s used %d of %d\n", (Book) ? "off" : "on", book
count, booksize);*/
  printz (CP[170], (Book) ? CP[92] : CP[93], bookcount, BOOKSIZE);
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
  /*printz ("hint      suggest a move         post      turn %s principle variation\n", (flag.post) ? "off" :
"on");*/
  printz (CP[177], (flag.post) ? CP[92] : CP[93]);
  /*printz ("save      game to file            get       game from file\n");*/
  printz (CP[188]);
  /*printz ("random    randomize play          new       start new game\n");*/
  printz (CP[181]);
  gotoXY (10, 20);
  printz (CP[47], ColorStr[computer]);
  gotoXY (10, 21);
  printz (CP[97], ColorStr[opponent]);
  gotoXY (10, 22);
  printz (CP[79], MaxResponseTime/100);
  gotoXY (10, 23);
  printz (CP[59], (flag.easy) ? CP[93] : CP[92]);
  gotoXY (25, 23);
  printz (CP[231], (flag.tsume) ? CP[93] : CP[92]);
  gotoXY (40, 20);
  printz (CP[52], MaxSearchDepth);
  gotoXY (40, 21);
  printz (CP[100], (dither) ? CP[93] : CP[92]);
  gotoXY (40, 22);
  printz (CP[112], (flag.hash) ? CP[93] : CP[92]);
  gotoXY (40, 23);
  printz (CP[73]);
  gotoXY (10, 24);
  printz (CP[110], (TCflag) ? CP[93] : CP[92],
	  TimeControl.moves[black], TimeControl.clock[black] / 100, OperatorTime, MaxSearchDepth);
  refresh ();
#ifdef BOGUS
  fflush (stdin); /*what is this supposed to do??*/
#endif /*BOGUS*/
  getchar ();
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
}
    

static const short x0[2] = {54, 2};
static const short y0[2] = {20, 4};


static
void 
UpdateCatched ()

{
  short side;
  for ( side = black; side <= white; side++ )
    { 
	short x, y, piece, cside, k;
	cside = flag.reverse ? (side ^ 1) : side;
        x = x0[cside];
        y = y0[cside];
	k = 0;
        for ( piece = pawn; piece <= king; piece++ )
    	  { short n;
	    if ( n = Captured[side][piece] )
	      { gotoXY(x,y); 
	        printz("%i%c",n,pxx[piece]);
	        if ( cside == black ) y--; else y++;
	      }
	    else
		k++;
	  };
	while ( k )
	  { k--;
	    gotoXY(x,y);
	    printz("  ");
	    if ( cside == black ) y--; else y++;
	  }
   }
  refresh();
}


void
EditBoard (void)

/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, N3f will place a knight on square 3f.
 * P* will put a pawn to the captured pieces.
 */

{
  short a, r, c, sq, i;
  char s[80];

  flag.regularstart = true;
  Book = BOOKFAIL;
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
  gotoXY (TAB, 3);
  printz (CP[29]);
  gotoXY (TAB, 4);
  printz (CP[28]);
  gotoXY (TAB, 5);
  printz (CP[136]);
  gotoXY (TAB, 7);
  printz (CP[64]);
  a = black;
  do
    {
      gotoXY (TAB, 6);
      printz (CP[60], ColorStr[a]);	/*Editing %s*/
      gotoXY (TAB + 24, 7);
      ClrEoln ();
      scanz ("%s", s);
      if (s[0] == CP[28][0])	/*#*/
	{
	  for (sq = 0; sq < NO_SQUARES; sq++)
	    {
	      board[sq] = no_piece;
	      color[sq] = neutral;
	      DrawPiece (sq);
	    }
	  ClearCaptured ();
          UpdateCatched ();
	}
      if (s[0] == CP[136][0])	/*c*/
	a = otherside[a];
      if (s[1] == '*')
	{
	  for (i = NO_PIECES; i > no_piece; i--)
	    if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
	      break;
	  Captured[a][unpromoted[i]]++;
          UpdateCatched ();
	  c = -1;
	}
      else
	{
      	  c = '9' - s[1];
      	  r = 'i' - s[2];
	}
      if ((c >= 0) && (c < NO_COLS) && (r >= 0) && (r < NO_ROWS))
	{
	  sq = locn (r, c);
	  for (i = NO_PIECES; i > no_piece; i--)
	    if ((s[0] == pxx[i]) || (s[0] == qxx[i]))
	      break;
	  if ( s[3] == '+' )
	    i = promoted[i];
          else
            i = unpromoted[i];
	  board[sq] = i;
	  color[sq] = ((board[sq] == no_piece) ? neutral : a);
	  DrawPiece (sq);
	}
  } while (s[0] != CP[29][0]);	/*.*/

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
ShowPlayers (void)
{
  gotoXY (5, ((flag.reverse) ? 23 : 2));
  printz ("%s", (computer == white) ? CP[218] : CP[74]);
  gotoXY (5, ((flag.reverse) ? 2 : 23));
  printz ("%s", (computer == black) ? CP[218] : CP[74]);
}

void
ShowDepth (char ch)
{
  gotoXY (TAB, 4);
  printz ("Depth= %2d%c", Sdepth, ch);	/*Depth= %d%c*/
  ClrEoln ();
}

void
ShowStage (void)
{
  gotoXY (TAB, 19);
  printz("Stage= %2d%c B= %2d W= %2d",
  	stage,flag.tsume?'T':' ',balance[black],balance[white]);
  ClrEoln ();
}

void
ShowScore (short score)
{
  gotoXY (TAB, 5);
  printz (CP[104], score);
  ClrEoln ();
}

void
ShowMessage (char *s)
{
  gotoXY (TAB, 6);
  printz ("%s", s);
  ClrEoln ();
}

void
ClearMessage (void)
{
  gotoXY (TAB, 6);
  ClrEoln ();
}

void
ShowCurrentMove (short int pnt, short int f, short int t)
{
  algbr (f, t, false);
  gotoXY (TAB, 7);
  printz ("(%2d) %5s ", pnt, mvstr[0]);
}

void
ShowHeader (void)
{
  gotoXY (TAB, 2);
  printz (CP[69], version, patchlevel);
}

void
ShowSidetoMove (void)
{
  gotoXY (TAB, 14);
  printz ("%2d:   %s", 1 + GameCnt / 2, ColorStr[player]);
  ClrEoln ();
}

void
ShowPrompt (void)
{
  gotoXY (TAB, 17);
  printz (CP[121]);		/*Your move is?*/
  ClrEoln ();
}

void
ShowNodeCnt (long int NodeCnt)
{
  gotoXY (TAB, 22);
  /* printz (CP[90], NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0); */
  printz ("n=%ld n/s=%ld", NodeCnt, (et > 100) ? NodeCnt / (et / 100) : 0);
  ClrEoln ();
}

void
ShowResults (short int score, short unsigned int *bstline, char ch)
{
  unsigned char d, ply;

  if (flag.post)
    {
      ShowDepth (ch);
      ShowScore (score);
      d = 7;
      for (ply = 1; bstline[ply] > 0; ply++)
	{
	  if (ply % 2 == 1)
	    {
	      gotoXY (TAB, ++d);
	      ClrEoln ();
	    }
	  algbr ((short) bstline[ply] >> 8, (short) bstline[ply] & 0xFF, false);
	  printz ("%5s ", mvstr[0]);
	}
      ClrEoln ();
      while (d < 13)
	{
	  gotoXY (TAB, ++d);
	  ClrEoln ();
	}
    }
}

void
ShowPatternCount (short side, short n)
{
  if (flag.post)
    {
	gotoXY(TAB+10+3*side,20);
	if ( n >= 0 )
          printz("%3d",n);
	else
          printz("   ");
    }
}

void
ShowGameType (void)
{
  if (flag.post)
    {
    	gotoXY(TAB,20);
        printz("%c vs. %c",GameType[black],GameType[white]);
    }
}

void
ShowResponseTime (void)
{
  if (flag.post)
    {   short TCC=TCcount;
    	gotoXY(TAB,21);
        /* printz("RT=%ld TCC=%d TCL=%ld EX=%ld ET=%ld TO=%d",
	  ResponseTime,TCC,TCleft,ExtraTime,et,flag.timeout); */
        printz("%ld,%d,%ld,%ld,%ld,%d",
	  ResponseTime,TCC,TCleft,ExtraTime,et,flag.timeout);
	ClrEoln ();
    }
}

void
SearchStartStuff (short int side)
{
  short i;

  signal (SIGINT, TerminateSearch);
#if defined MSDOS || defined THINK_C
  side++;			/* shut up the compiler */
#else
  signal (SIGQUIT, TerminateSearch);
#endif /* MSDOS */
  for (i = 4; i < 14; i++)
    {
      gotoXY (TAB, i);
      ClrEoln ();
    }
}

void
OutputMove (void)
{

  UpdateDisplay (root->f, root->t, 0, (short) root->flags);
  gotoXY (TAB, 16);
  if(flag.illegal){printz(CP[225]);return;}
  printz (CP[84], mvstr[0]);	/*My move is %s*/
  if (flag.beep)
    putchar (7);
  ClrEoln ();

  gotoXY (TAB, 18);
  if (root->flags & draw)
    printz (CP[58]);
  else if (root->score == -(SCORE_LIMIT+999))
    printz (CP[95]);
  else if (root->score == SCORE_LIMIT+998)
    printz (CP[44]);
#ifdef VERYBUGGY
  else if (root->score < -SCORE_LIMIT)
    printz (CP[96], SCORE_LIMIT+999 + root->score - 1);
  else if (root->score > SCORE_LIMIT)
    printz (CP[45], SCORE_LIMIT+998 - root->score - 1);
#endif /*VERYBUGGY*/
  ClrEoln ();
  if (flag.post)
    {
      register short h, l, t;

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

      ShowNodeCnt (NodeCnt);
      gotoXY (TAB, 23);
      printz (CP[81], t); /*Max Tree=*/
      ClrEoln ();
    }
  ShowSidetoMove ();
}

void
UpdateClocks (void)
{
  short m, s;
  long dt;

  if (TCflag)
    {
      m = (short) ((dt = (TimeControl.clock[player] - et)) / 6000);
      s = (short) ((dt - 6000 * (long) m) / 100);
    }
  else
    {
      m = (short) ((dt = et) / 6000);
      s = (short) (et - 6000 * (long) m) / 100;
    }
  if (m < 0)
    m = 0;
  if (s < 0)
    s = 0;
  if (player == black)
    gotoXY (20, (flag.reverse) ? 2 : 23);
  else
    gotoXY (20, (flag.reverse) ? 23 : 2);
  /* printz ("%d:%02d %ld  ", m, s, dt); */
  printz ("%d:%02d  ", m, s); 
  if (flag.post)
    ShowNodeCnt (NodeCnt);
  refresh ();
}

void
gotoXY (short int x, short int y)
{
#if defined MSDOS
  putchar (ESC);
  putchar ('[');
  param (y);
  putchar (';');
  param (x);
  putchar ('H');
#elif defined THINK_C
  cgotoxy (x, y, stdout);
#else
  move (y - 1, x - 1);
#endif /* MSDOS */
}

void
ClrScreen (void)
{
#ifdef MSDOS
  putchar (ESC);
  putchar ('[');
  putchar ('2');
  putchar ('J');
#else
  clear ();
#endif /* MSDOS */
  refresh ();
}

void
ClrEoln (void)
{
#ifdef MSDOS
  putchar (ESC);
  putchar ('[');
  putchar ('K');
#elif defined THINK_C
  ccleol (stdout);
#else
  clrtoeol ();
#endif /* MSDOS */
  refresh ();
}

#ifdef MSDOS
void
param (short n)
{
  if (n >= 10)
    {
      register short d, q;

      q = n / 10;
      d = n % 10;
      putchar (q + '0');
      putchar (d + '0');
    }
  else
    putchar (n + '0');
}

#endif /* MSDOS */

void
DrawPiece (short int sq)
{
  register char x, y;

  char piece,l,r,p; 

#if defined(MSDOS) && !defined(SEVENBIT)
  if (color[sq] == white)
    x = '7';			/* print WHITE boldface, */
  else
    x = '1';			/* print BLACK inverted	 */
  piece = board[sq];
  if ( is_promoted[piece] ) {
    p = '+'; y = pxx[unpromoted[piece]];
  } else {
    p = ' '; y = pxx[piece];
  };
  gotoXY (8 + 5 * VIR_C (sq), 4 + 2 * (8 - VIR_R (sq)));
  printz ("\033[%cm%c%c\033[0m", x, p, y);
#else
  if ( color[sq]==neutral ) 
    l = r = ' ';
  else if ( flag.reverse ^ (color[sq]==black) ) {
    l = '/'; r = '\\';
  } else {
    l = '\\', r = '/';
  }; 
  piece = board[sq];
  if ( is_promoted[piece] ) {
    p = '+'; y = pxx[unpromoted[piece]];
  } else {
    p = ' '; y = pxx[piece];
  };
  gotoXY (8 + 5 * VIR_C (sq), 4 + 2 * (8 - VIR_R (sq)));
  printz ("%c%c%c%c", l, p, y, r);
#endif
}

void
ShowPostnValue (short int sq)

/*
 * must have called ExaminePosition() first
 */

{
  short score;

  gotoXY (4 + 5 * VIR_C (sq), 5 + 2 * (7 - VIR_R (sq)));
  score = ScorePosition (color[sq]);
  if (color[sq] != neutral)
#if defined SAVE_SVALUE
    printz ("??? ");
#else
    printz ("%3d ", svalue[sq]);
#endif
  else
    printz ("   ");
}

void
ShowPostnValues (void)
{
  short sq, score;

  ExaminePosition (opponent);
  for (sq = 0; sq < NO_SQUARES; sq++)
    ShowPostnValue (sq);
  score = ScorePosition (opponent);
  gotoXY (TAB, 5);
  printz (CP[103], score, 
            mtl[computer], pscore[computer], GameType[computer],
            mtl[opponent], pscore[opponent], GameType[opponent]);

  ClrEoln ();
}


void
UpdateDisplay (short int f, short int t, short int redraw, short int isspec)
{
  short i, sq, z;

  if (redraw)
    {
      ShowHeader ();
      ShowPlayers ();

      i = 2;
      gotoXY (3, ++i);
#if defined(MSDOS) && !defined(SEVENBIT)
      printz ("\332\304\304\304\304\302\304\304\304\304\302\304\304\304\304" \
	    "\302\304\304\304\304\302\304\304\304\304\302\304\304\304\304" \
	      "\302\304\304\304\304\302\304\304\304\304\277");
#else            
      printz ("    +----+----+----+----+----+----+----+----+----+");
#endif /* MSDOS && !SEVENBIT */
      while (i < 20)
	{
	  gotoXY (1, ++i);
	  if (flag.reverse)
	    z = (i / 2) - 1;
	  else
	    z = 11 - ((i+1) / 2);
#if defined(MSDOS) && !defined(SEVENBIT)
	  printz ("%d \263    \263    \263    \263    \263    \263    " \
		  "\263    \263    \263", z);
#else
	  printz ("    %c |    |    |    |    |    |    |    |    |    |", 'a'+9-z);
#endif /* MSDOS && !SEVENBIT */
	  gotoXY (3, ++i);
	  if (i < 20)
#if defined(MSDOS) && !defined(SEVENBIT)
	    printz ("\303\304\304\304\304\305\304\304\304\304\305\304\304" \
		    "\304\304\305\304\304\304\304\305\304\304\304\304\305" \
		    "\304\304\304\304\305\304\304\304\304\305\304\304\304" \
		    "\304\264");
#else              
	    printz ("    +----+----+----+----+----+----+----+----+----+");
#endif /* MSDOS && !SEVENBIT */
	}
#if defined(MSDOS) && !defined(SEVENBIT)
      printz ("\300\304\304\304\304\301\304\304\304\304\301\304\304\304\304" \
	    "\301\304\304\304\304\301\304\304\304\304\301\304\304\304\304" \
	      "\301\304\304\304\304\301\304\304\304\304\331");
#else 
      printz ("    +----+----+----+----+----+----+----+----+----+");
#endif /* MSDOS && !SEVENBIT */
      gotoXY (3, 22);
      printz("    ");
      if (flag.reverse)
	printz (CP[16]);
      else
	printz (CP[15]);
      for (sq = 0; sq < NO_SQUARES; sq++)
	DrawPiece (sq);
    }
  else /* not redraw */
    {
      if ( f < NO_SQUARES )
        DrawPiece (f);
      DrawPiece (t & 0x7f);
    }
  if ( (isspec & capture) || (isspec & dropmask) || redraw )
    { short side;
      for ( side = black; side <= white; side++ )
          { short x, y, piece, cside, k;
	        cside = flag.reverse ? (side ^ 1) : side;
            x = x0[cside];
            y = y0[cside];
	        k = 0;
            for ( piece = pawn; piece <= king; piece++ )
    	      { short n;
	        if ( n = Captured[side][piece] )
	          { gotoXY(x,y); 
	            printz("%i%c",n,pxx[piece]);
	            if ( cside == black ) y--; else y++;
	          }
		    else
	 	      k++;
	      };
	    while ( k )
	      { k--;
		gotoXY(x,y);
		printz("  ");
	        if ( cside == black ) y--; else y++;
	      }
          }
    }
  refresh ();
}

extern char *InPtr;

void
ChangeAlphaWindow (void)
{
  ShowMessage (CP[114]);
  scanz ("%hd", &WAwindow);
  ShowMessage (CP[34]);
  scanz ("%hd", &BAwindow);
}

void
ChangeBetaWindow (void)
{
  ShowMessage (CP[115]);
  scanz ("%hd", &WBwindow);
  ShowMessage (CP[35]);
  scanz ("%hd", &BBwindow);
}

void
GiveHint (void)
{
  char s[40];
  if (hint)
    {
      algbr ((short) (hint >> 8), (short) (hint & 0xFF), false);
      strcpy (s, CP[198]);	/*try*/
      strcat (s, mvstr[0]);
      ShowMessage (s);
    }
  else
    ShowMessage (CP[223]);
}

void
ChangeHashDepth (void)
{
  ShowMessage (CP[163]);
  scanz ("%hd", &HashDepth);
  ShowMessage (CP[82]);
  scanz ("%hd", &HashMoveLimit);
}

void
ChangeSearchDepth (void)
{
  ShowMessage (CP[150]);
  scanz ("%hd", &MaxSearchDepth);
  TCflag = !(MaxSearchDepth > 0);
}

void
SetContempt (void)
{
  ShowMessage (CP[142]);
  scanz ("%hd", &contempt);
}

void
ChangeXwindow (void)
{
  ShowMessage (CP[208]);
  scanz ("%hd", &xwndw);
}

void
SelectLevel (char *sx)
{
  int item;

  ClrScreen ();
  gotoXY (32, 2);
  printz (CP[41], version, patchlevel);
  gotoXY (20, 4);
  printz (CP[18]);
  gotoXY (20, 5);
  printz (CP[19]);
  gotoXY (20, 6);
  printz (CP[20]);
  gotoXY (20, 7);
  printz (CP[21]);
  gotoXY (20, 8);
  printz (CP[22]);
  gotoXY (20, 9);
  printz (CP[23]);
  gotoXY (20, 10);
  printz (CP[24]);
  gotoXY (20, 11);
  printz (CP[25]);
  gotoXY (20, 12);
  printz (CP[26]);
  gotoXY (20, 13);
  printz (CP[27]);

  OperatorTime = 0;
  TCmoves = 40;
  TCminutes = 5;
  TCseconds = 0;

  gotoXY (20, 17);
  printz (CP[62]);
  refresh ();
  scanz ("%d", &item);
  switch (item)
    {
    case 1:
      TCmoves = 40;
      TCminutes = 5;
      break;
    case 2:
      TCmoves = 40;
      TCminutes = 15;
      break;
    case 3:
      TCmoves = 40;
      TCminutes = 30;
      break;
    case 4:
      TCmoves = 80;
      TCminutes = 15;
      flag.gamein = true;
      break;
    case 5:
      TCmoves = 80;
      TCminutes = 30;
      flag.gamein = true;
      break;
    case 6:
      TCmoves = 80;
      TCminutes = 15;
      TCadd = 3000;
      flag.gamein = true;
      break;
    case 7:
      TCmoves = 80;
      TCminutes = 30;
      TCadd = 3000;
      break;
    case 8:
      TCmoves = 1;
      TCminutes = 1;
      flag.onemove = true;
      break;
    case 9:
      TCmoves = 1;
      TCminutes = 15;
      flag.onemove = true;
      break;
    case 10:
      TCmoves = 1;
      TCminutes = 30;
      flag.onemove = true;
      break;
    }

  TCflag = (TCmoves > 0);

  TimeControl.clock[black] = TimeControl.clock[white] = 0; 

  SetTimeControl ();
  ClrScreen ();
  UpdateDisplay (0, 0, 1, 0);
}

void
DoDebug (void)
{
  short c, p, sq, tp, tc, tsq, score;
  char s[40];

  ExaminePosition (opponent);
  ShowMessage (CP[65]);
  scanz ("%s", s);
  c = neutral;
  if (s[0] == CP[9][0] || s[0] == CP[9][1])	/*b B*/
    c = black;
  if (s[0] == CP[9][2] || s[0] == CP[9][3])	/*w W*/
    c = white;
  for (p = king; p > no_piece; p--)
    if ((s[1] == pxx[p]) || (s[1] == qxx[p]))
      break;
  for (sq = 0; sq < NO_SQUARES; sq++)
    {
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
  score = ScorePosition (opponent);
  gotoXY (TAB, 5);
  printz (CP[103], score, 
             mtl[computer], pscore[computer], GameType[computer],
             mtl[opponent], pscore[opponent], GameType[opponent]);

  ClrEoln ();
}

void
DoTable (short table[NO_SQUARES])
{
  short  sq;
  ExaminePosition (opponent);
  for (sq=0;sq<NO_SQUARES;sq++) {
    gotoXY (4 + 5 * VIR_C (sq), 5 + 2 * (7 - VIR_R (sq)));
    printz ("%3d ", table[sq]);
  }
} 



