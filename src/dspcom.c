/*
 * dspcom.c - C source for GNU SHOGI
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
#include "ataks.h"
#ifdef DEBUG40
int whichway;
#endif
#if defined HASGETTIMEOFDAY && !defined THINK_C
#include <sys/time.h>
#endif
extern char *version, *patchlevel;
extern unsigned int TTadd;
char mvstr[4][6];
char *InPtr;
int	InBackground = false;


#include <ctype.h>
#include <signal.h>
#if defined THINK_C
#include <time.h>
#define BOOKTEST
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
#endif

#ifndef EVALFILE
#ifdef THINK_C
#define EVALFILE "EVAL"
#else
#define EVALFILE "/tmp/EVAL"
#endif
#endif

#if defined DEBUG || defined BOOKTEST

void
movealgbr (short int m, char *s)
{
    unsigned int f, t;
    short piece = 0, flag = 0;
    f = m >> 8 & 0x7f;
    t = m & 0xff;
    if ( f > NO_SQUARES )
      { short piece = f - NO_SQUARES;
        if ( f > NO_PIECES ) f -= NO_PIECES;
        flag = (dropmask | piece );
      }
    if ( t & 0x80 )
      {
        flag |= promote;
        t &= 0x7f;
      }
    if ( flag & dropmask )
      {
        *s = pxx[piece]; s++;
        *s = '*'; s++;
        *s = cxx[column (t)]; s++;
        *s = rxx[row (t)]; s++;
      }
    else
      {
        *s = cxx[column (f)]; s++;
        *s = rxx[row (f)]; s++;
        *s = cxx[column (t)]; s++;
        *s = rxx[row (t)]; s++;
        if ( flag & promote )
          {
            *s = '+'; s++;
          }
      }
    if (m & 0x8000)
      {
        *s = '?'; s++;
      }
    *s = '\0';
}

#endif


void
algbr (short int f, short int t, short int flag)


/*
 * Generate move strings in different formats.
 */

{
  int m3p;

  if ( f > NO_SQUARES ) 
    { short piece;
      piece = f - NO_SQUARES;
      if ( f > (NO_SQUARES+NO_PIECES) )
        piece -= NO_PIECES;
      flag = (dropmask | piece); 
    }
  if ( (t & 0x80) != 0 )
    {
      flag |= promote;
      t &= 0x7f;
    }
  if ( f == t && (f != 0 || t != 0) ) 
    { 
#if !defined BAREBONES
      printz("error in algbr: FROM=TO=%d, flag=0x%4x\n",t,flag);
#endif
      mvstr[0][0] = mvstr[1][0] = mvstr[2][0] = mvstr[3][0] = '\0';
    }
  else
  if ( (flag & dropmask) != 0 )
    { short piece = flag & pmask;
      mvstr[0][0] = pxx[piece];
      mvstr[0][1] = '*';
      mvstr[0][2] = cxx[column (t)];
      mvstr[0][3] = rxx[row (t)];
      mvstr[0][4] = '\0';
      strcpy (mvstr[1], mvstr[0]);
      strcpy (mvstr[2], mvstr[0]);
      strcpy (mvstr[3], mvstr[0]);
    }
  else
  if (f != 0 || t != 0)
    {
      /* algebraic notation */
      mvstr[0][0] = cxx[column (f)];
      mvstr[0][1] = rxx[row (f)];
      mvstr[0][2] = cxx[column (t)];
      mvstr[0][3] = rxx[row (t)];
      mvstr[0][4] = mvstr[3][0] = '\0';
      mvstr[1][0] = pxx[board[f]];

      mvstr[2][0] = mvstr[1][0];
      mvstr[2][1] = mvstr[0][1]; 

      mvstr[2][2] = mvstr[1][1] = mvstr[0][2];	/* to column */
      mvstr[2][3] = mvstr[1][2] = mvstr[0][3];	/* to row */
      mvstr[2][4] = mvstr[1][3] = '\0';
      strcpy (mvstr[3], mvstr[2]);
      mvstr[3][1] = mvstr[0][0];
      if (flag & promote)
        {
  		strcat(mvstr[0], "+");
  		strcat(mvstr[1], "+");
  		strcat(mvstr[2], "+");
  		strcat(mvstr[3], "+");
        }
    }
  else
    mvstr[0][0] = mvstr[1][0] = mvstr[2][0] = mvstr[3][0] = '\0';
}
           
 
int
VerifyMove (char *s, short int iop, short unsigned int *mv)

/*
 * Compare the string 's' to the list of legal moves available for the
 * opponent. If a match is found, make the move on the board.
 */

{
  static short pnt, tempb, tempc, tempsf, tempst, cnt;
  static struct leaf xnode;
  struct leaf *node;

  *mv = 0;
  if (iop == 2)
    {
      UnmakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
      return (false);
    }
  cnt = 0;
  MoveList (opponent, 2);
  pnt = TrPnt[2];
  while (pnt < TrPnt[3])
    {
      node = &Tree[pnt++];
      algbr (node->f, node->t, (short) node->flags);
      if (strcmp (s, mvstr[0]) == 0 || strcmp (s, mvstr[1]) == 0 ||
	  strcmp (s, mvstr[2]) == 0 || strcmp (s, mvstr[3]) == 0)
	{
	  cnt++;
	  xnode = *node;
	}
    }
  if (cnt == 1)
    {
      MakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst, &INCscore);
      if (SqAtakd (PieceList[opponent][0], computer))
	{
	  UnmakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
#ifdef NONDSP
/* Illegal move in check */
	  printz (CP[77]);
	  printz ("\n");
#else
/* Illegal move in check */
	  ShowMessage (CP[77]);
#endif
	  return (false);
	}
      else
	{
	  if (iop == 1)
	    return (true);
	  UpdateDisplay (xnode.f, xnode.t, 0, (short) xnode.flags);
	  GameList[GameCnt].depth = GameList[GameCnt].score = 0;
	  GameList[GameCnt].nodes = 0;
	  ElapsedTime (1);
	  GameList[GameCnt].time = (short) (et+50)/100;
	  if (TCflag)
	    {
	      TimeControl.clock[opponent] -= et;
	      timeopp[oppptr] = et;
	      --TimeControl.moves[opponent];
	    }
	  *mv = (xnode.f << 8) | xnode.t;
	  algbr (xnode.f, xnode.t, false);
	  return (true);
	}
    }
#ifdef NONDSP
/* Illegal move */
  printz (CP[75], s);
#ifdef DEBUG8
  if (1)
    {
      FILE *D;
      int r, c, side, l;
      extern unsigned short int PrVar[];
      extern void debug_position();
      D = fopen ("/tmp/DEBUG", "a+");
      pnt = TrPnt[2];
      fprintf (D, "resp = %d\n", ResponseTime);
      fprintf (D, "iop = %d\n", iop);
      fprintf (D, "matches = %d\n", cnt);
      algbr (hint >> 8, hint & 0xff, (short) 0);
      fprintf (D, "hint %s\n", mvstr[0]);
      fprintf (D, "inout move is %s\n", s);
      for (r = 1; PrVar[r]; r++)
	{
	  algbr (PrVar[r] >> 8, PrVar[r] & 0xff, (short) 0);
	  fprintf (D, " %s", mvstr[0]);
	}
      fprintf (D, "\n");
      fprintf (D, "legal move are \n");
      while (pnt < TrPnt[3])
	{
	  node = &Tree[pnt++];
	  algbr (node->f, node->t, (short) node->flags);
	  fprintf (D, "%s %s %s %s\n", mvstr[0], mvstr[1], mvstr[2], mvstr[3]);
	}
      debug_position (D);
      fclose (D);
      abort ();
    }
#endif
#else
/* Illegal move */
  ShowMessage (CP[76]);
#endif
#if !defined BAREBONES
  if (cnt > 1)
    ShowMessage (CP[32]);
#endif 
  return (false);
}

int
parser (char *f, int side)
{
  int c1, r1, c2, r2;

  c1 = '9' - f[0];
  r1 = 'a' - f[1];
  c2 = '9' - f[2];
  r2 = 'i' - f[3];
  return (locn (r1, c1) << 8) | locn (r2, c2);
  /*NOTREACHED*/
}

void
GetGame (void)
{
  FILE *fd;
  char fname[256], *p;
  int c, i, j;
  short sq;
  short side, isp;
/* enter file name */
  ShowMessage (CP[63]);
  scanz ("%s", fname);
/* shogi.000 */
  if (fname[0] == '\0')
    strcpy (fname, CP[137]);
  if ((fd = fopen (fname, "r")) != NULL)
    {
      NewGame ();
      fgets (fname, 256, fd);
      computer = opponent = black;
      InPtr = fname;
      skip ();
      if (*InPtr == 'c')
	computer = white;
      else
	opponent = white;
      skip ();
      skip ();
      skip ();
      Game50 = atoi (InPtr);
      fgets (fname, 256, fd);
      InPtr = &fname[11];
      skipb ();
      TCflag = atoi (InPtr);
      skip ();
      InPtr += 14;
      skipb ();
      OperatorTime = atoi (InPtr);
      fgets (fname, 256, fd);
      InPtr = &fname[11];
      skipb ();
      TimeControl.clock[black] = atol (InPtr);
      skip ();
      skip ();
      TimeControl.moves[black] = atoi (InPtr);
      fgets (fname, 256, fd);
      InPtr = &fname[11];
      skipb ();
      TimeControl.clock[white] = atol (InPtr);
      skip ();
      skip ();
      TimeControl.moves[white] = atoi (InPtr);
      for (i = NO_ROWS-1; i > -1; i--)
	{
	  fgets (fname, 256, fd);
	  p = &fname[2];
	  InPtr = &fname[21];
	  for (j = 0; j < NO_COLS; j++)
	    {
	      sq = i * NO_COLS + j;
	      isp = ( *p == '+' );
	      p++;
	      if (*p == '.')
		{
		  board[sq] = no_piece;
		  color[sq] = neutral;
		}
	      else
		{
		  for (c = 0; c < NO_PIECES; c++)
		    {
		      if (*p == pxx[c])
			{
			  if ( isp )
			    board[sq] = promoted[c];
			  else
			    board[sq] = unpromoted[c];
			  color[sq] = white;
			}
		    }
		  for (c = 0; c < NO_PIECES; c++)
		    {
		      if (*p == qxx[c])
			{
			  if ( isp )
			    board[sq] = promoted[c];
			  else
			    board[sq] = unpromoted[c];
			  color[sq] = black;
			}
		    }
		}
	      p++;
	      Mvboard[sq] = atoi (InPtr);
	      skip ();
	    }
	}
      fgets (fname, 256, fd);  /* 9 8 7 ... */
      fgets (fname, 256, fd);  /* p l n ... */
      ClearCaptured ();
      for ( side = 0; side <= 1; side++ ) {
        fgets (fname, 256, fd);
	InPtr = fname;
	skipb ();
        Captured[side][pawn] = atoi (InPtr);
	skip ();
        Captured[side][lance] = atoi (InPtr);
	skip ();
        Captured[side][knight] = atoi (InPtr);
	skip ();
        Captured[side][silver] = atoi (InPtr);
	skip ();
        Captured[side][gold] = atoi (InPtr);
	skip ();
        Captured[side][bishop] = atoi (InPtr);
	skip ();
        Captured[side][rook] = atoi (InPtr);
	skip ();
        Captured[side][king] = atoi (InPtr);
      }
      GameCnt = 0;
      flag.regularstart = true;
      Book = BOOKFAIL;
      fgets (fname, 256, fd);   /*  move score ... */
      while (fgets (fname, 256, fd))
	{
	  struct GameRec *g;
	  int side = computer;

	  side = side ^ 1;
	  ++GameCnt;
	  InPtr = fname;
	  skipb ();
	  g = &GameList[GameCnt];
	  g->gmove = parser (InPtr, side);
	  skip ();
	  g->score = atoi (InPtr);
	  skip ();
	  g->depth = atoi (InPtr);
	  skip ();
	  g->nodes = atol (InPtr);
	  skip ();
	  g->time = atol (InPtr);
	  skip ();
	  g->flags = c = atoi (InPtr);
	  skip ();
	  g->hashkey = strtol (InPtr, (char **) NULL, 16);
	  skip ();
	  g->hashbd = strtol (InPtr, (char **) NULL, 16);
	  g->piece = no_piece;
	  g->color = neutral;
	  if (c & capture)
	    {
		  skip ();
		  for (c = 0; c < NO_COLS; c++)
		    if (pxx[c] == *InPtr)
		      break;
		  g->piece = c;
  	    }
	  skip ();
	  g->color = ((*InPtr == CP[119][0]) ? white : black);
	  skip ();
	  if ( *InPtr == '+' ) 
	    g->piece = promoted[g->piece];
	}
      if (TimeControl.clock[black] > 0)
	TCflag = true;
      fclose (fd);
    }
  ZeroRPT ();
  InitializeStats ();
  UpdateDisplay (0, 0, 1, 0);
  Sdepth = 0;
  hint = 0;
}

void
GetXGame (void)
{
  FILE *fd;
  char fname[256], *p;
  int c, i, j;
  short sq;
  short side, isp;
/* Enter file name */
  ShowMessage (CP[63]);
  scanz ("%s", fname);
  if (fname[0] == '\0')
/* XSHOGI.position.read*/
    strcpy (fname, CP[205]);
  if ((fd = fopen (fname, "r")) != NULL)
    {
      NewGame ();
      flag.regularstart = false;
      Book = false;
      /* xshogi position file ... */
      fgets (fname, 256, fd);
#ifdef notdef
      fname[6] = '\0';
      if (strcmp (fname, CP[206]))
	return;
#endif
      /* -- empty line -- */
      fgets (fname, 256, fd);
      /* -- empty line -- */
      fgets (fname, 256, fd);
      for (i = NO_ROWS-1; i > -1; i--)
	{
	  fgets (fname, 256, fd);
	  p = fname;
	  for (j = 0; j < NO_COLS; j++)
	    {
	      sq = i * NO_COLS + j;
	      isp = ( *p == '+' );
	      p++;
	      if (*p == '.')
		{
		  board[sq] = no_piece;
		  color[sq] = neutral;
		}
	      else
		{
		  for (c = 0; c < NO_PIECES; c++)
		    {
		      if (*p == qxx[c])
			{
			  if ( isp )
			    board[sq] = promoted[c];
			  else
			    board[sq] = unpromoted[c];
			  color[sq] = white;
			}
		    }
		  for (c = 0; c < NO_PIECES; c++)
		    {
		      if (*p == pxx[c])
			{
			  if ( isp )
			    board[sq] = promoted[c];
			  else
			    board[sq] = unpromoted[c];
			  color[sq] = black;
			}
		    }
		}
	      p++;
	    }
	}
	ClearCaptured ();
        for ( side = 0; side <= 1; side++ ) {
	  fgets (fname, 256, fd);
	  InPtr = fname;
	  Captured[side][pawn]   = atoi (InPtr);
	  skip ();
	  Captured[side][lance]  = atoi (InPtr);
	  skip ();
	  Captured[side][knight] = atoi (InPtr);
	  skip ();
	  Captured[side][silver] = atoi (InPtr);
	  skip ();
	  Captured[side][gold]   = atoi (InPtr);
	  skip ();
	  Captured[side][bishop] = atoi (InPtr);
	  skip ();
	  Captured[side][rook]   = atoi (InPtr);
	  skip ();
	  Captured[side][king]   = atoi (InPtr);
        };
        if (fgets(fname, 256, fd) != NULL && strncmp(fname, "white", 5) == 0)
        {
          computer = black;
          opponent = white;
          xwndw = BXWNDW;
	}
      fclose (fd);
    }
  ZeroRPT ();
  InitializeStats ();
  UpdateDisplay (0, 0, 1, 0);
  Sdepth = 0;
  hint = 0;
}

void
SaveGame (void)
{
  FILE *fd;
  char fname[256];
  short sq, i, c, f, t;
  char p;
  short side, piece;

  if (savefile[0])
    strcpy (fname, savefile);
  else
    {
/* Enter file name*/
      ShowMessage (CP[63]);
      scanz ("%s", fname);
    }

  if (fname[0] == '\0')
/* shogi.000 */
    strcpy (fname, CP[137]);
  if ((fd = fopen (fname, "w")) != NULL)
    {
      char *b, *w;

      b = w = CP[74];
      if (computer == white)
	w = CP[141];
      if (computer == black)
	b = CP[141];
      fprintf (fd, CP[37], w, b, Game50);
      fprintf (fd, CP[111], TCflag, OperatorTime);
      fprintf (fd, CP[117],
	       TimeControl.clock[black], TimeControl.moves[black],
	       TimeControl.clock[white], TimeControl.moves[white]);
      for (i = NO_ROWS-1; i > -1; i--)
	{
	  fprintf (fd, "%c ", 'i' - i);
	  for (c = 0; c < NO_COLS; c++)
	    { 
	      sq = i * NO_COLS + c;
	      piece = board[sq];
	      p = is_promoted[piece] ? '+' : ' ';
   	      fprintf (fd, "%c", p);
	      switch (color[sq])
		{
		case white:
		  p = pxx[piece];
		  break;
		case black:
		  p = qxx[piece];
		  break;
		default:
		  p = '.';
		}
	      fprintf (fd, "%c", p);
	    }
	  for (f = i * NO_COLS; f < i * NO_COLS + NO_ROWS; f++)
	    fprintf (fd, " %d", Mvboard[f]);
	  fprintf (fd, "\n");
	}
      fprintf (fd, "   9 8 7 6 5 4 3 2 1\n");
      fprintf (fd, "  p  l  n  s  g  b  r  k\n");
      for ( side = 0; side <= 1; side++ ) {
        fprintf (fd, " %2d", Captured[side][pawn]); 
        fprintf (fd, " %2d", Captured[side][lance]); 
        fprintf (fd, " %2d", Captured[side][knight]); 
        fprintf (fd, " %2d", Captured[side][silver]); 
        fprintf (fd, " %2d", Captured[side][gold]); 
        fprintf (fd, " %2d", Captured[side][bishop]); 
        fprintf (fd, " %2d", Captured[side][rook]); 
        fprintf (fd, " %2d", Captured[side][king]); 
        fprintf (fd, "\n");
      }
      fprintf (fd, CP[126]);
      for (i = 1; i <= GameCnt; i++)
	{
	  struct GameRec *g = &GameList[i];

	  f = g->gmove >> 8;
	  t = (g->gmove & 0xFF);
	  algbr (f, t, g->flags);
	  fprintf (fd, "%5s %6d %5d %7ld %6ld %5d  %#08lx %#08lx  %c %s %c\n",
		   mvstr[0], g->score, g->depth,
		   g->nodes, g->time, g->flags, g->hashkey, g->hashbd,
	   pxx[g->piece], ((g->color == 2) ? "     " : ColorStr[g->color]),
           (is_promoted[g->piece] ? '+' : ' '));
	}
      fclose (fd);
/* Game saved */
      ShowMessage (CP[70]);
    }
  else
    /*ShowMessage ("Could not open file");*/
    ShowMessage (CP[48]);
}

void
ListGame (void)
{
  FILE *fd;
  short i, f, t;
#ifndef MSDOS
  time_t when;
  char fname[256], dbuf[256];
#else
  char fname[256];
#endif

  if (listfile[0])
    strcpy (fname, listfile);
  else
    {
#ifdef MSDOS
      sprintf (fname, "chess.lst");
#else
      time (&when);
      strncpy (dbuf, ctime (&when), 20);
      dbuf[7] = '\0';
      dbuf[10] = '\0';
      dbuf[13] = '\0';
      dbuf[16] = '\0';
      dbuf[19] = '\0';
/* use format "CLp16.Jan01-020304B" when patchlevel is 16,
   date is Jan 1
   time is 02:03:04
   program played white */
      sprintf (fname, "CLp%s.%s%s-%s%s%s%c", patchlevel, dbuf + 4, dbuf + 8, dbuf + 11, dbuf + 14, dbuf + 17, ColorStr[computer][0]);
      /* replace space padding with 0 */
      for (i = 0; fname[i] != '\0'; i++)
	if (fname[i] == ' ')
	  fname[i] = '0';
#endif /* MSDOS */
    }
  fd = fopen (fname, "w");
  if (!fd)
    {
      printf (CP[219], fname);
      exit (1);
    }
  /*fprintf (fd, "gnuchess game %d\n", u);*/
  fprintf (fd, CP[161], patchlevel);
  fprintf (fd, CP[10]);
  fprintf (fd, CP[11]);
  for (i = 1; i <= GameCnt; i++)
    {
      f = GameList[i].gmove >> 8;
      t = (GameList[i].gmove & 0xFF);
      algbr (f, t, GameList[i].flags);
      if(GameList[i].flags & book)
          fprintf (fd, "%5s  %5d    Book%7ld %5d", mvstr[0],
	       GameList[i].score, 
	       GameList[i].nodes, GameList[i].time);
      else
          fprintf (fd, "%5s  %5d     %2d %7ld %5d", mvstr[0],
	       GameList[i].score, GameList[i].depth,
	       GameList[i].nodes, GameList[i].time);
      if ((i % 2) == 0)
	{
	  fprintf (fd, "\n");
#ifdef DEBUG40
	  if (computer == white)
	    fprintf (fd, " %d %d %d %d %d %d %d\n",
		     GameList[i].d1,
		     GameList[i].d2,
		     GameList[i].d3,
		     GameList[i].d4,
		     GameList[i].d5,
		     GameList[i].d6,
		     GameList[i].d7);
	  else
	    fprintf (fd, " %d %d %d %d %d %d %d\n",
		     GameList[i - 1].d1,
		     GameList[i - 1].d2,
		     GameList[i - 1].d3,
		     GameList[i - 1].d4,
		     GameList[i - 1].d5,
		     GameList[i - 1].d6,
		     GameList[i - 1].d7);
#endif
	}
      else
	fprintf (fd, "         ");
    }
  fprintf (fd, "\n\n");
  if (GameList[GameCnt].flags & draw)
    {
      fprintf (fd, CP[54], DRAW);
    }
  else if (GameList[GameCnt].score == -(SCORE_LIMIT+999))
    {
      fprintf (fd, "%s\n", ColorStr[player ]);
    }
  else if (GameList[GameCnt].score == (SCORE_LIMIT+998))
    {
      fprintf (fd, "%s\n", ColorStr[player ^ 1]);
    }
  fclose (fd);
}

void
Undo (void)

/*
 * Undo the most recent half-move.
 */

{
  short f, t;
  f = GameList[GameCnt].gmove >> 8;
  t = GameList[GameCnt].gmove & 0x7F;
   if ( f > NO_SQUARES )
     { /* the move was a drop */
        Captured[color[t]][board[t]]++;
	board[t] = no_piece;
	color[t] = neutral;
	Mvboard[t]--;
     }
   else
     {
	if ( GameList[GameCnt].flags & promote )
	  board[f] = unpromoted[board[t]];
	else
	  board[f] = board[t];
	color[f] = color[t];
	board[t] = GameList[GameCnt].piece;
	color[t] = GameList[GameCnt].color;
	if ( board[t] != no_piece )
	  Captured[color[f]][unpromoted[board[t]]]--;
	if (color[t] != neutral)
	  Mvboard[t]--;
        Mvboard[f]--;
     }
   InitializeStats ();

  if (TCflag && (TCmoves>1))
    ++TimeControl.moves[color[f]];
  hashkey = GameList[GameCnt].hashkey;
  hashbd = GameList[GameCnt].hashbd;
  GameCnt--;
  computer = computer ^ 1;
  opponent = opponent ^ 1;
  flag.mate = false;
  Sdepth = 0;
  player = player ^ 1;
  ShowSidetoMove ();
  UpdateDisplay (0, 0, 1, 0);
  if (flag.regularstart)
  Book = false;
}

void
  ShowMoves ()
{
#ifdef NONDSP
  unsigned j;
  for (j = TrPnt[2]; j < TrPnt[3]; j++)
    {
      struct leaf *node = &Tree[j];
      algbr (node->f, node->t, node->flags);
      printf ("%s %s %s %s %d 0x%x\n", mvstr[0], mvstr[1], mvstr[2], mvstr[3],node->score,node->flags);
    }
#endif
}

void
 TestSpeed (void (*f) (short int side, short int ply), unsigned j)
{
#ifdef test
  unsigned jj;
#endif
  unsigned i;
  long cnt, rate, t1, t2;
#ifdef HASGETTIMEOFDAY
struct timeval tv;
#endif

#ifdef HASGETTIMEOFDAY
  gettimeofday(&tv,NULL);
  t1 = (tv.tv_sec*100+(tv.tv_usec/10000));
#else
  t1 = time (0);
#endif
  for (i = 0; i < j; i++)
    {
      f (opponent, 2);
#ifdef test
	for(jj=TrPnt[2];i<TrPnt[3];jj++)if(!pick(jj,TrPnt[3]-1))break;
#endif
    }
#ifdef HASGETTIMEOFDAY
  gettimeofday(&tv,NULL);
  t2 = (tv.tv_sec*100+(tv.tv_usec/10000));
#else
  t2 = time (0);
#endif
  cnt = j * (TrPnt[3] - TrPnt[2]);
  if (t2 - t1)
    et = (t2 - t1);
  else
    et = 1;
  rate = (((et) ? ((cnt*100) / et) : 0));
  /*printz ("Nodes= %ld Nodes/sec= %ld\n", cnt, rate);*/
#ifdef NONDSP
  printz (CP[91], cnt, rate);
#ifdef DEBUG9
  for (j = TrPnt[2]; j < TrPnt[3]; j++)
    {
      struct leaf *node = &Tree[j];
      algbr (node->f, node->t, node->flags);
      printf ("%s %s %s %s %d %x\n", mvstr[0], mvstr[1], mvstr[2], mvstr[3],node
->score,node->flags);
    }
#endif
#else
  ShowNodeCnt (cnt);
#endif
}

void
 TestPSpeed (short int (*f) (short int side), unsigned j)
{
  short i;
  long cnt, rate, t1, t2;
#ifdef HASGETTIMEOFDAY
struct timeval tv;
#endif

#ifdef HASGETTIMEOFDAY
  gettimeofday(&tv,NULL);
  t1 = (tv.tv_sec*100+(tv.tv_usec/10000));
#else
  t1 = time (0);
#endif
  for (i = 0; i < j; i++)
    {
      (void) f (opponent);
    }
#ifdef HASGETTIMEOFDAY
  gettimeofday(&tv,NULL);
  t2 = (tv.tv_sec*100+(tv.tv_usec/10000));
#else
  t2 = time (0);
#endif
  cnt = j;
  if (t2 - t1)
    et = (t2 - t1);
  else
    et = 1;
  rate = (et) ? ((cnt*100) / et) : 0;
  /*printz ("Nodes= %ld Nodes/sec= %ld\n", cnt, rate);*/
#ifdef NONDSP
  printz (CP[91], cnt, rate);
#else
  ShowNodeCnt (cnt);
#endif
}


void
SetOppTime (char *s)
{
  char *time;
  register tmp = 0;
  int m, t,sec;
  sec = 0;
  time = &s[strlen (CP[197])];
  t = (int)strtol (time, &time, 10);
  if(*time == ':'){time++; sec=(int)strtol(time, &time,10);}
  m = (int)strtol (time, &time, 10);
  if (t) 
    TimeControl.clock[opponent] = t;
  if (m)
    TimeControl.moves[opponent] = m;
#if defined XSHOGI 
  printz (CP[222], m, t);
#endif
}



void
SetMachineTime (char *s)
{
  char *time;
  long tmp = 0;
  int m, t,sec;
  sec = 0;
  time = &s[strlen (CP[197])];
  t = (int)strtol (time, &time, 10);
  if(*time == ':'){time++; sec=(int)strtol(time, &time,10);}
  m = (int)strtol (time, &time, 10);
  if (t)
    TimeControl.clock[computer] = t;
  if (m)
    TimeControl.moves[computer] = m;
#if defined XSHOGI 
  printz (CP[222], m, t);
#endif
}


#ifdef DEBUG_EVAL
short debug_eval = false;
FILE  *debug_eval_file = NULL;
#endif


void
InputCommand (void)

/*
 * Process the users command. If easy mode is OFF (the computer is thinking
 * on opponents time) and the program is out of book, then make the 'hint'
 * move on the board and call SelectMove() to find a response. The user
 * terminates the search by entering ^C (quit siqnal) before entering a
 * command. If the opponent does not make the hint move, then set Sdepth to
 * zero.
 */

{
  int eof = 0;
  short have_shown_prompt = false;
  short ok;
  unsigned short mv;
  char s[80], sx[80];

  ok = flag.quit = false;
  player = opponent;
#if ttblsz
  if(TTadd > ttblsize)ZeroTTable();
#endif
  if (hint > 0 && !flag.easy && !flag.force )
      {
	ft = time0;
	fflush (stdout);
	algbr ((short) hint >> 8, (short) hint & 0xff, false);
	strcpy (s, mvstr[0]);
#ifdef DEBUG12
#include "debug12.h"
#endif
#ifdef DEBUG40
whichway = 0;
#endif
	if (flag.post) GiveHint ();
	if (VerifyMove (s, 1, &mv))
	  {
	    Sdepth = 0;
#ifdef QUIETBACKGROUND
#ifdef NONDSP
	    PromptForMove ();
#else
	    ShowSidetoMove ();
	    ShowPrompt ();
#endif
	    have_shown_prompt = true;
#endif /* QUIETBACKGROUND */
	    SelectMove (computer, 2);
	    VerifyMove (s, 2, &mv);
	    Sdepth = 0;
	  }
#ifdef DEBUG40
	else whichway = 1;
#endif
	time0 = ft;
      }
  while (!(ok || flag.quit))
    {
      player = opponent;
#ifdef QUIETBACKGROUND
      if (!have_shown_prompt)
	{
#endif /* QUIETBACKGROUND */
#ifdef NONDSP
	  PromptForMove ();
#else
	  ShowSidetoMove ();
	  ShowPrompt ();
#endif
#ifdef QUIETBACKGROUND
	}
      have_shown_prompt = false;
#endif /* QUIETBACKGROUND */
#ifdef NONDSP
      s[0] = sx[0] = '\0';
      while (!sx[0])
	(void) gets (sx);
#else
      fflush (stdout);
#if defined MSDOS || defined THINK_C
      s[0] = '\0';
      eof = ( gets (sx) == NULL );
#else
      eof = ( getstr (sx) == ERR );
#endif
#endif
      sscanf (sx, "%s", s);
      if (eof)
	ExitChess ();
      if (s[0] == '\0')
	continue;
      if (strcmp (s, CP[131]) == 0)	/*bd*/
	{
#if defined XSHOGI
	  xshogi = 0;
#endif
	  ClrScreen ();
	  UpdateDisplay (0, 0, 1, 0);
#if defined XSHOGI
	  xshogi = 1;
#endif
	}
      else if (strcmp (s, CP[129]) == 0) /* noop */ ;	/*alg*/
      else if ((strcmp (s, CP[180]) == 0) || (strcmp (s, CP[216]) == 0))	/* quit exit*/
	flag.quit = true;
      else if (strcmp (s, CP[178]) == 0)	/*post*/
	{
	  flag.post = !flag.post;
	}
      else if ((strcmp (s, CP[191]) == 0) || (strcmp (s, CP[154]) == 0))	/*set edit*/
	EditBoard ();
#ifdef NONDSP
      else if (strcmp (s, CP[190]) == 0)	/*setup*/
	SetupBoard ();
#endif
      else if (strcmp (s, CP[156]) == 0)	/*first*/
	{
	  ok = true;
	}
      else if (strcmp (s, CP[162]) == 0)	/*go*/
	{
	  ok = true;
	  flag.force = false;
	  if (computer == black)
	    {
	      computer = white;
	      opponent = black;
	    }
	  else
	    {
	      computer = black;
	      opponent = white;
	    }
	}
      else if (strcmp (s, CP[166]) == 0)	/*help*/
	help ();
      else if (strcmp (s, CP[221]) == 0)	/*material*/
	flag.material = !flag.material;
      else if (strcmp (s, CP[157]) == 0)	/*force*/
	{flag.force = !flag.force; flag.bothsides = false;}
      else if (strcmp (s, CP[134]) == 0)	/*book*/
	Book = Book ? 0 : BOOKFAIL;
      else if (strcmp (s, CP[172]) == 0)	/*new*/
	{
	  NewGame ();
	  UpdateDisplay (0, 0, 1, 0);
	}
      else if (strcmp (s, CP[171]) == 0)	/*list*/
	ListGame ();
      else if (strcmp (s, CP[169]) == 0 || strcmp (s, CP[217]) == 0)	/*level clock*/
	SelectLevel (sx);
      else if (strcmp (s, CP[165]) == 0)	/*hash*/
	flag.hash = !flag.hash;
      else if (strcmp (s, CP[227]) == 0)	/*gamein*/
	flag.gamein = !flag.gamein;
      else if (strcmp (s, CP[226]) == 0)	/*beep*/
	flag.beep = !flag.beep;
      else if (strcmp (s, CP[197]) == 0)	/*time*/
	{ SetMachineTime (sx); }
      else if (strcmp (s, CP[228]) == 0)	/*time*/
	{ SetOppTime (sx); }
      else if (strcmp (s, CP[33]) == 0)	/*Awindow*/
	ChangeAlphaWindow ();
      else if (strcmp (s, CP[39]) == 0)	/*Bwindow*/
	ChangeBetaWindow ();
      else if (strcmp (s, CP[183]) == 0)	/*rcptr*/
	flag.rcptr = !flag.rcptr;
      else if (strcmp (s, CP[168]) == 0)	/*hint*/
	GiveHint ();
      else if (strcmp (s, CP[135]) == 0)	/*both*/
	{
	  flag.bothsides = !flag.bothsides;
          flag.force = false;
	  Sdepth = 0;
	  ElapsedTime (1);
	  SelectMove (opponent, 1);
	  ok = true;
	}
      else if (strcmp (s, CP[185]) == 0)	/*reverse*/
	{
	  flag.reverse = !flag.reverse;
	  ClrScreen ();
	  UpdateDisplay (0, 0, 1, 0);
	}
      else if (strcmp (s, CP[195]) == 0)	/*switch*/
	{
	  computer = computer ^ 1;
	  opponent = opponent ^ 1;
	  xwndw = (computer == black) ? WXWNDW : BXWNDW;
	  flag.force = false;
	  Sdepth = 0;
	  ok = true;
	}
      else if (strcmp (s, CP[203]) == 0)	/*black*/
	{
	  computer = white;
	  opponent = black;
	  xwndw = WXWNDW;
	  flag.force = false;
	  Sdepth = 0;

	  /*
           * ok = true; don't automatically start with black command
           */
	}
      else if (strcmp (s, CP[133]) == 0)	/*white*/
	{
	  computer = black;
	  opponent = white;
	  xwndw = BXWNDW;
	  flag.force = false;
	  Sdepth = 0;

	  /*
           * ok = true; don't automatically start with white command
           */
	}
      else if (strcmp (s, CP[201]) == 0 && GameCnt > 0)	/*undo*/
	{
	  Undo ();
	}
      else if (strcmp (s, CP[184]) == 0 && GameCnt > 1)	/*remove*/
	{
	  Undo ();
	  Undo ();
	}
      else if (strcmp (s, CP[160]) == 0)	/*get*/
	GetGame ();
      else if (strcmp (s, CP[207]) == 0)	/*xget*/
	GetXGame ();
      else if (strcmp (s, CP[189]) == 0)	/*save*/
	SaveGame ();
      else if (strcmp (s, CP[151]) == 0)	/*depth*/
	ChangeSearchDepth ();
#ifdef DEBUG
      else if (strcmp (s, CP[147]) == 0)	/*debuglevel*/
	ChangeDbLev ();
#endif /* DEBUG */
      else if (strcmp (s, CP[164]) == 0)	/*hashdepth*/
	ChangeHashDepth ();
      else if (strcmp (s, CP[182]) == 0)	/*random*/
	dither = DITHER;
      else if (strcmp (s, CP[229]) == 0)	/*hard*/
	flag.easy = false;
      else if (strcmp (s, CP[152]) == 0)	/*easy*/
	flag.easy = !flag.easy;
      else if (strcmp (s, CP[230]) == 0)	/*tsume*/
	flag.tsume = !flag.tsume;
      else if (strcmp (s, CP[143]) == 0)	/*contempt*/
	SetContempt ();
      else if (strcmp (s, CP[209]) == 0)	/*xwndw*/
	ChangeXwindow ();
      else if (strcmp (s, CP[186]) == 0)	/*rv*/
	{
	  flag.rv = !flag.rv;
	  UpdateDisplay (0, 0, 1, 0);
	}
      else if (strcmp (s, CP[145]) == 0)	/*coords*/
	{
	  flag.coords = !flag.coords;
	  UpdateDisplay (0, 0, 1, 0);
	}
      else if (strcmp (s, CP[193]) == 0)	/*stars*/
	{
	  flag.stars = !flag.stars;
	  UpdateDisplay (0, 0, 1, 0);
	}
      else if (strcmp (s, CP[5]) == 0)	        /*moves*/
	{
	  ShowMessage (CP[108]);/*test movelist*/
	  TestSpeed (MoveList, 1);
	  ShowMessage (CP[107]);/*test capturelist*/
	  TestSpeed (CaptureList, 1);
	  ShowMessage (CP[85]);/*test score position*/
          ExaminePosition();
	  TestPSpeed (ScorePosition, 1);
	}
      else if (strcmp (s, CP[196]) == 0)	/*test*/
	{
	  ShowMessage (CP[108]);/*test movelist*/
	  TestSpeed (MoveList, 20000);
	  ShowMessage (CP[107]);/*test capturelist*/
	  TestSpeed (CaptureList, 30000);
	  ShowMessage (CP[85]);/*test score position*/
          ExaminePosition();
	  TestPSpeed (ScorePosition, 15000);
	}                                             
#ifdef DEBUG_EVAL
      else if (strcmp (s, "eval") == 0)		/*eval*/
	{             
	  debug_eval = true;
	  if ( debug_eval_file = fopen(EVALFILE,"w") ) {
	    ExaminePosition();
	    fprintf (debug_eval_file, "\nscoring for %s to move...\n\n",
		ColorStr[player]);
	    ScorePosition (player);
	    fclose (debug_eval_file);
	  }
	  debug_eval = false;
	}        
      else if (strcmp (s, "pattern") == 0)		/*pattern*/
	{             
	  debug_eval = true;
	  if ( debug_eval_file = fopen(EVALFILE,"w") ) {
	    short side;
	    for (side=black; side<=white; side++) {
	      extern short ScorePatternDistance();
	      short s = ScorePatternDistance (side); 
	      fprintf (debug_eval_file, "\npattern distance score for %s is %d\n\n",
		 ColorStr[side], s);
	    }
	    fclose (debug_eval_file);
	  }
	  debug_eval = false;
	}        
#endif
      else
      if (strcmp (s, CP[179]) == 0)	/*p*/
	ShowPostnValues ();
      else if (strcmp (s, CP[148]) == 0)	/*debug*/
	DoDebug ();
      else
	{
	  if ( flag.mate )
	    ok = true;
	  else
	    ok = VerifyMove (s, 0, &mv);
	  Sdepth = 0;
	}
    }

  ElapsedTime (1);
  if (flag.force)
    {
      computer = opponent;
      opponent = computer ^ 1;
    }
#if defined XSHOGI 
  printz ("%d. %s\n", ++mycnt2, s);
#ifdef notdef /* optional pass best line to frontend with move */
  if (flag.post)
    {
      register int i;

      printz (" %6d ", MSCORE);
      for (i = 1; MV[i] > 0; i++)
	{
	  algbr ((short) (MV[i] >> 8), (short) (MV[i] & 0xFF), false);
	  printz ("%5s ", mvstr[0]);
	}
    }
  printz ("\n");
#endif
#endif /* XSHOGI */
  signal (SIGINT, TerminateSearch);
#if !defined MSDOS && !defined THINK_C
  signal (SIGQUIT, TerminateSearch);
#endif /* MSDOS */
}


#ifdef NOFIONREAD
#ifdef FIONREAD
#undef FIONREAD
#endif
#endif


#ifdef HASGETTIMEOFDAY
void
ElapsedTime (short int iop)


/*
 * Determine the time that has passed since the search was started. If the
 * elapsed time exceeds the target (ResponseTime+ExtraTime) then set timeout
 * to true which will terminate the search. iop = 0 calculate et bump ETnodes
 * iop = 1 calculate et set timeout if time exceeded, calculate et
 */

{
struct timeval tv;
#ifndef MSDOS
  int nchar;
  extern int errno;
  int i;
#ifdef FIONREAD
  if (i = ioctl ((int) 0, FIONREAD, &nchar))
    {
      perror ("FIONREAD");
      fprintf (stderr,
        "You probably have a non-ANSI <ioctl.h>; see README. %d %d %x\n",
	i, errno, FIONREAD);
      exit (1);
    }

  if (nchar)
    {
      if (!flag.timeout)
	flag.back = true;
      flag.bothsides = false;
    }
#endif /*FIONREAD*/
#else
  if (kbhit ())
    {
      if (!flag.timeout)
	flag.back = true;
      flag.bothsides = false;
    }
#endif /* MSDOS */
  gettimeofday(&tv,NULL);
  et = (tv.tv_sec*100+(tv.tv_usec/10000)) - time0;
  ETnodes = NodeCnt + ZNODES;
  if (et < 0)
    et = 0;
  if (iop == 1)
    {
      if (et > ResponseTime + ExtraTime && Sdepth > MINDEPTH)
	flag.timeout = true;
      ETnodes = NodeCnt + ZNODES;
      gettimeofday(&tv,NULL);
      time0 = tv.tv_sec*100+tv.tv_usec/10000;
    }
#if !defined NONDSP
#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    UpdateClocks ();
#endif
}
#else
void
ElapsedTime (short int iop)


/*
 * Determine the time that has passed since the search was started. If the
 * elapsed time exceeds the target (ResponseTime+ExtraTime) then set timeout
 * to true which will terminate the search. iop = 0 calculate et bump ETnodes
 * iop = 1 calculate et set timeout if time exceeded, calculate et
 */

{
#ifndef MSDOS
  int nchar;
  extern int errno;
  int i;
#ifdef FIONREAD
  if (i = ioctl ((int) 0, FIONREAD, &nchar))
    {
      perror ("FIONREAD");
      fprintf (stderr,
        "You probably have a non-ANSI <ioctl.h>; see README. %d %d %x\n",
	i, errno, FIONREAD);
      exit (1);
    }

  if (nchar)
    {
      if (!flag.timeout)
	flag.back = true;
      flag.bothsides = false;
    }
#endif /*FIONREAD*/
#else
  if (kbhit ())
    {
      if (!flag.timeout)
	flag.back = true;
      flag.bothsides = false;
    }
#endif /* MSDOS */
#if defined THINK_C
  et = (time ((time_t *) 0) - time0) * 100;
#else
  et = (time ((long *) 0) - time0) * 100;
#endif
  ETnodes = NodeCnt + ZNODES;
  if (et < 0)
    et = 0;
  if (iop == 1)
    {
      if (et > ResponseTime + ExtraTime && Sdepth > MINDEPTH)
	flag.timeout = true;
      ETnodes = NodeCnt + ZNODES;
      time0 = time ((time_t *) 0);
    }
#if !defined NONDSP
#ifdef QUIETBACKGROUND
  if (!background)
#endif /* QUIETBACKGROUND */
    UpdateClocks ();
#endif
}
#endif
void
SetTimeControl (void)
{
  if (TCflag)
    {
      TimeControl.moves[black] = TimeControl.moves[white] = TCmoves;
      TimeControl.clock[black] += 6000L * TCminutes + TCseconds * 100;
      TimeControl.clock[white] += 6000L * TCminutes + TCseconds * 100;
    }
  else
    {
      TimeControl.moves[black] = TimeControl.moves[white] = 0;
      TimeControl.clock[black] = TimeControl.clock[white] = 0;
    }
  flag.onemove = (TCmoves == 1);
  et = 0;
  ElapsedTime (1);
}
