/*
 * gnuan.c - Analysis interface for gnushogi.
 *
 * Copyright (c) 1993, 1994 Matthias Mutz
 *
 * gnuan was originally the analysis interface for gnuchess
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

/*
 * This is a front end for a shogi analysis program.  It takes a file of
 * moves in algebraic notation and creates a file containing each move that
 * was made, the move it would have made, the score it would give itself
 * after making the move it recommended, and the depth it searched for the
 * move.
 *
 * This is a modification of nondsp.c.  I have cut out code that was not needed
 * for this application such as the help and edit functions.  Most of the
 * modifications were done in InputCommand.
 */

/*
 * This file will generate two different analysis programs.  One is meant to
 * be run as a background process inwhich the in and out files are
 * predefined. The other is meant to be run in the foreground where it will
 * prompt you for the name of the file with the game in it, the maximum depth
 * the search should be conducted to and the time limit per move.  It would
 * be nice to be able to give these parameters on the command line, but that
 * would mean changing main which is in the gnuchess.c file.
 *
 * For each move it will analyse the move until either it has run out of time or
 * it has reached the maximum depth.
 *
 * To build the version for background processing define BACKGROUND_ANALYSIS 1
 * either at the top of this file, or in compilation.  The files and depth
 * used are defined below.  They are MAX_DEPTH, MAX_TIME, OUT_FILE, IN_FILE
 * and PROG_FILE.  The PROG_FILE is a file that will be updated as each move
 * is analysed so you can check its progress.  This is only updated when
 * running the BACKGROUND_ANALYSIS version.  In the version where the
 * filename and depth are entered at runtime, the output goes to stdout.
 */

#define BACKGROUND_ANALYSIS 0

#define MAX_DEPTH  MAXDEPTH
#define MAX_TIME   20
#define OUT_FILE   "gnuan.out"
#define IN_FILE    "gnuan.in"
#define PROG_FILE  "gnuan.prog"


#include <ctype.h>
#include <signal.h>
#ifdef MSDOS
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
#undef rxx
#undef cxx
#undef scanz
#undef printz

#define rxx "ihgfedcba"
#define cxx "987654321"

#define scanz scanf
#define printz printf

char mvstr[4][6];
extern char ColorStr[2][10];
int mycnt1, mycnt2;

static FILE *fpin;
static FILE *fpout;
int samedepth = false;
char *DRAW;
unsigned short int MV[MAXDEPTH];
enum
{
  XSHOGI, GNUSHOGI, OTHER
} InFileType;
char InBuf[256];

#if BACKGROUND_ANALYSIS
static FILE *fpprog;

#endif
static char black_actual_move[20];
static char white_actual_move[20];
static char black_suggest_move[20];
static char white_suggest_move[20];
static int sdw,sdb;
static int black_score;
static int white_score;
int tmpscore;
static int black_moving;
static int current_depth;
static int current_score;
static int enable_update_display = 0;


void
Initialize (void)
{
  mycnt1 = mycnt2 = 0;
}

void
ExitChess (void)
{
  ListGame ();
  exit (0);
}

#ifndef MSDOS			/* never called!!! */
void
Die (int sig)
{
  ExitChess ();
}

#endif /* MSDOS */

void
TerminateSearch (int sig)
{
#ifdef MSDOS
  sig++;			/* shut up the compiler */
#endif /* MSDOS */
  if (!flag.timeout)
    flag.musttimeout = true;
  flag.bothsides = false;
}

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
VerifyMove (char *s, VerifyMove_mode iop, unsigned short int *mv)

/*
 * Compare the string 's' to the list of legal moves available for the
 * opponent. If a match is found, make the move on the board.
 */

{
  static short pnt, tempb, tempc, tempsf, tempst, cnt;
  static struct leaf xnode;
  struct leaf *node;
  char *p, *q;
  short drop;
 
  for (p=s, drop=false; *p != '\0' && !drop; p++)
    if ( *p == '*' || *p == '\'' )
	drop = true;

  if ( !drop ) {
    p = q = s; 
    if ( *p == '+' ) p++;
    if ( isalpha(*p) ) p++;
    while ( *p != '\0' ) *q++ = *p++;
    *q = '\0'; 
  }
    
  *mv = 0;
  if (iop == UNMAKE_MODE)
    {
      UnmakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
      return (false);
    }
  cnt = 0;
  MoveList (opponent, 2, -1, true);
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
    { short blockable;
      MakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst, &INCscore);
      if (SqAtakd (PieceList[opponent][0], computer, &blockable))
	{
	  UnmakeMove (opponent, &xnode, &tempb, &tempc, &tempsf, &tempst);
	  printz ("Illegal move\n");
	  return (false);
	}
      else
	{
	  if (iop == VERIFY_AND_TRY_MODE)
	    return (true);

	  /*
           * UpdateDisplay (xnode.f, xnode.t, 0, (short) xnode.flags);
           */
	  GameList[GameCnt].depth = GameList[GameCnt].score = 0;
	  GameList[GameCnt].nodes = 0;
	  ElapsedTime (COMPUTE_AND_INIT_MODE);
	  GameList[GameCnt].time = (short) et;
	  TimeControl.clock[opponent] -= et;
	  --TimeControl.moves[opponent];
	  *mv = (xnode.f << 8) | xnode.t;
	  algbr (xnode.f, xnode.t, false);
	  return (true);
	}
    }
#ifdef CHESSTOOL
  printz ("Illegal move\n");
#else
  if (cnt > 1)
    ShowMessage ("Ambiguous Move!");
#endif
if(cnt == 0){
 pnt = TrPnt[2];
  while (pnt < TrPnt[3])
    {
      node = &Tree[pnt++];
      algbr (node->f, node->t, (short) node->flags);
      printf("%s -> %s\n",s,mvstr[0]);
    }
  }
  return (false);
}

void
help (void)
{
}

void
EditBoard (void)

/*
 * Set up a board position. Pieces are entered by typing the piece followed
 * by the location. For example, Nf3 will place a knight on square f3.
 */

{
}

void
SetupBoard (void)

/*
 * Compatibility with Unix chess and the nchesstool. Set up a board position.
 * Eight lines of eight characters are used to setup the board. a8-h8 is the
 * first line. White pieces are  represented  by  uppercase characters.
 */

{
}

void
ShowDepth (char ch)
{
#ifdef MSDOS
  ch++;				/* shut up the compiler */
#endif /* MSDOS */
}

ShowStage (void)
{
}

void
ShowPatternCount (short side, short n)
{
}

void
ShowResponseTime (void)
{
}

void
ShowResults (short int score, unsigned short int *bstline, char ch)
{
  int i;

  current_score = score;
  current_depth = Sdepth;
  for (i = 1; bstline[i] > 0; i++)
    {
      MV[i] = bstline[i];
    } MV[i] = 0;
}


void 
ShowGameType (void)
{
}

void
SearchStartStuff (short int side)
{
  signal (SIGINT, TerminateSearch);
#ifndef MSDOS
  signal (SIGQUIT, TerminateSearch);
#endif /* MSDOS */
  if (flag.post)
    {
      fprintf (stderr, "\nMove# %d    Target= %ld    Clock: %ld\n",
	       TCmoves - TimeControl.moves[side] + 1,
	       ResponseTime, TimeControl.clock[side]);
    }
}

void
OutputMove (void)
{
  if (black_moving)
    {
      strcpy (black_suggest_move, mvstr[0]);
      black_score = current_score;
    }
  else
    {
      strcpy (white_suggest_move, mvstr[0]);
      white_score = current_score;
    }
}

void
ElapsedTime (ElapsedTime_mode iop)

/*
 * Determine the time that has passed since the search was started. If the
 * elapsed time exceeds the target (ResponseTime+ExtraTime) then set timeout
 * to true which will terminate the search.
 */
{
  if (ahead)
    {
#ifndef MSDOS
#ifdef FIONREAD
      long nchar;

      ioctl (0, FIONREAD, &nchar);
      if (nchar)
	{
	  flag.timeout = true;
	  flag.bothsides = false;
	}
#endif
#else
      if (kbhit ())
	{
	  flag.timeout = true;
	  flag.bothsides = false;
	}
#endif /* MSDOS */
    }
  et = (time ((long *) 0) - time0) * 100;
  if (et < 0)
    et = 0;
  ETnodes += ZNODES;
  if (iop == COMPUTE_AND_INIT_MODE)
    {
      if (et > ResponseTime + ExtraTime && Sdepth > 1)
	flag.timeout = true;
      time0 = time ((long *) 0);
      ETnodes = NodeCnt + ZNODES;
    }
}

void
SetTimeControl (void)
{
  if (TCflag)
    {
      TimeControl.moves[black] = TimeControl.moves[white] = TCmoves;
      TimeControl.clock[black] = TimeControl.clock[white] = 6000L * TCminutes;
    }
  else
    {
      TimeControl.moves[black] = TimeControl.moves[white] = 0;
      TimeControl.clock[black] = TimeControl.clock[white] = 0;
      MaxResponseTime = 6000L * TCminutes;
    }
  et = 0;
  ElapsedTime (COMPUTE_AND_INIT_MODE);
}

void
ClrScreen (void)
{
#ifndef CHESSTOOL
  printz ("\n");
#endif
}

void
UpdateDisplay (short int f, short int t, short int redraw, short int isspec)
{
#ifndef CHESSTOOL
  short r, c, l;

  if (!enable_update_display)
    return;
  if (redraw)
    {
      fprintf (fpout, "\n");
      for (r = (NO_ROWS-1); r >= 0; r--)
	{
	  for (c = 0; c <= (NO_COLS-1); c++)
	    { char pc;
	      l = locn (r, c);
	      pc = (is_promoted[board[l]] ? '+' : ' ');
	      if (color[l] == neutral)
		fprintf (fpout, " -");
	      else if (color[l] == black)
		fprintf (fpout, "%c%c", pc, qxx[board[l]]);
	      else
		fprintf (fpout, "%c%c", pc, pxx[board[l]]);
	    }
	  fprintf (fpout, "\n");
	}
      fprintf (fpout, "\n");
      {  
	short side;
	for (side = black; side <= white; side++)
	  { short piece, c; 
	    fprintf(fpout, (side==black)?"black ":"white ");
            for (piece = pawn; piece <= king; piece++)
	      if (c = Captured[side][piece]) 
		fprintf(fpout, "%i%c ",c,pxx[piece]);
            fprintf(fpout, "\n");
          };
      }
    }
#endif /* CHESSTOOL */
#ifdef MSDOS
  f++;
  t++;
  isspec++;			/* shut up the compiler */
#endif /* MSDOS */
}

void
GetGame (void)
{
}

void
SaveGame (void)
{
}

void
ListGame (void)
{
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
    Book = BOOKFAIL;
}

void
ShowMessage (char *s)
{
  fprintf (stderr, "%s\n", s);
}

void
ShowSidetoMove (void)
{
}

void
PromptForMove (void)
{
#ifndef CHESSTOOL
  printz ("\nYour move is? ");
#endif /* CHESSTOOL */
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
  printz ("window: ");
}

void
ChangeBetaWindow (void)
{
  printz ("window: ");
}

void
GiveHint (void)
{
  algbr ((short) (hint >> 8), (short) (hint & 0xFF), false);
  fprintf (stderr, "Hint: %s\n", mvstr[0]);
}

void
SelectLevel (char *sx)
{
}

void
ChangeSearchDepth (void)
{
  printz ("depth= ");
  scanz ("%hd", &MaxSearchDepth);
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
TestSpeed (void (*f) (short int side, short int ply))
{
  short i;
  long cnt, rate, t1, t2;

  t1 = time (0);
  for (i = 0; i < 10000; i++)
    {
      f (opponent, 2);
    }
  t2 = time (0);
  cnt = 10000L * (TrPnt[3] - TrPnt[2]);
  rate = cnt / (t2 - t1);
  printz ("Nodes= %ld Nodes/sec= %ld\n", cnt, rate);
}


#define copym() while (*p != ' ' && *p != '\n') *q++ = *p++; *q = '\0';
#define skipb() while(*p==' ')p++;
#define skip() skipb(); while(*p!=' ' && *p!='\n')p++; skipb(); 

int
GetNextMove (char *buffer)
{
  char cbuf[128];
  char sdepth[128];
  int sdindex;
  char *p,*q;
  sdw = sdb =0;
  white_actual_move[0] = '\0';
  black_actual_move[0] = '\0';
  if(fgets(cbuf,sizeof(cbuf),fpin) != NULL){
  p = cbuf;
  for( ; *p; p++ ) {
    if ( isalpha(*p) || isdigit(*p) ) {
      q = black_actual_move;
      copym();
      if( strcmp(black_actual_move,"Book")==0) continue; else break;
    }
  }
  if (*p == '\0' && black_actual_move[0] == '\0')return 0;
  if(InFileType == GNUSHOGI){
    skip(); /* score */
    if ( samedepth ) {
      sdindex = 0;
      while (*p != ' ' && *p != '\n') sdepth[sdindex++] = *p++;
      sdepth[sdindex] = '\0';
      if(strcmp(sdepth,"Book")==0) {
	sdw = -99;
      } else
	sdw = atoi(sdepth);
    } else {
	skip(); /* depth */
    }
    skip(); /* nodes */
    skip(); /* time */
  }

  for( ; *p; p++){
    if(isalpha(*p) || isdigit(*p)){
	q = white_actual_move;
	copym();
        if(strcmp(white_actual_move,"Book")==0) continue; else break;
        break;
    }
  }
  if (*p == '\0' && white_actual_move[0] == '\0')return 1;
  if(InFileType == GNUSHOGI){
      skip(); /* score */
      if ( samedepth ) {
	sdindex = 0;
        while (*p != ' ' && *p != '\n') sdepth[sdindex++] = *p++;
        sdepth[sdindex] = '\0';
        if ( strcmp(sdepth,"Book")==0 )
	  sdb = -99;
        else 
	  sdb = atoi(sdepth);
    }
  }

  return 1;
  } else /* EOF */
  return -1;

}

void
InputCommand (char *command)

/*
 * Open the file of moves to analyse.  Go through the file move by move and
 * do the following for each move.  See what gnuchess would have done in that
 * position.  Record the move it would have made along with the score it
 * would have given and the search depth.  Take back its move. Force the move
 * that was actually made.  Keep track of statistics such as how many moves
 * agreed.
 */

{
  int i;
  short ok;
  unsigned short mv;
  char s[80];
#if !BACKGROUND_ANALYSIS
  int max_minutes;
  char inbuf[256];
#else
  char outfilename[255];
  char progfilename[255];
#endif
  char infilename[255];
  int search_depth;
  int move_number = 1;
  long start_time, end_time, elapsed_time;
  int total_white_moves, total_black_moves;
  int same_white_moves, same_black_moves;
  float black_percent, white_percent;

  /* Initialize necessary variables. */

  flag.quit = false;
  flag.beep = false;
  player = opponent;
  ft = 0;
  Book = false;
  total_white_moves = 0;
  total_black_moves = 0;
  same_white_moves = 0;
  same_black_moves = 0;

#if BACKGROUND_ANALYSIS

  /*
   * Set the in files to standard ones for analysis if background
   * processing selected.
   */

  strcpy (infilename, IN_FILE);
  strcpy (outfilename, OUT_FILE);
  strcpy (progfilename, PROG_FILE);
  fpout = fopen (outfilename, "w");
  if (fpout == NULL)
    {
      fprintf (fpout, "This file does not exist : %s\n", outfilename);
      flag.quit = 1;
      return;
    }
  MaxSearchDepth = search_depth = MAX_DEPTH;
  TCminutes = MAX_TIME;

#else

  /* Request information on where the file is and the depth an time. */

  fpout = stderr;
  fprintf (fpout, "Input the file with the algebraic moves of the game.\n");
  fflush(fpout);
  gets (infilename);
  fprintf (fpout, "\n");
  do
    {
      fprintf (fpout, "Input the search depth you want to use.  (1 to 29)(- for depth from file)\n");
      gets (inbuf);
      search_depth = atoi (inbuf);
      if(search_depth < 0) {samedepth = true; search_depth = -search_depth;}
  } while (search_depth < 1 || search_depth > 29);
  MaxSearchDepth = search_depth;

  fprintf (fpout, "\n");
  do
    {
      fprintf (fpout, "Input the maximum number of minutes per move you want to use.\n");
      gets (inbuf);
      max_minutes = atoi (inbuf);
  } while (max_minutes < 1);
  printf ("\nYou will search to %d half moves\n", search_depth);
  printf ("\nWith no search taking more than %d minutes\n",max_minutes);
  TCminutes = max_minutes;

  fprintf (fpout, "\n\n");

#endif

  fpin = fopen (infilename, "r");
  if (fpin == NULL)
    {
      fprintf (fpout, "This file does not exist : %s\n", infilename);
      flag.quit = 1;
      return;
    }
  /* lets find out about this file */
  fgets (InBuf, 256, fpin);
  InFileType = OTHER;
  if (!strncmp (InBuf, "xshogi", 6))
    {
      InFileType = XSHOGI;
      fgets (InBuf, 256, fpin);
      fgets (InBuf, 256, fpin);
    }
  else if (!strncmp (InBuf, "gnushogi", 6))
    {
      InFileType = GNUSHOGI;
      fgets (InBuf, 256, fpin);
    }
  else
    rewind (fpin);
  TCmoves = 1;
  TCflag = (TCmoves > 1);
  OperatorTime = 0;
  SetTimeControl ();


  fprintf (fpout, "Move Black White      Score Depth     Best Line\n");
  fprintf (fpout, "------------------------------------------------------------------\n");

#if BACKGROUND_ANALYSIS

  /*
   * Update progress in the progress file if this is running in the
   * background.
   */

  fpprog = fopen (progfilename, "w");
  fprintf (fpprog, "Done with move #%d.\n", move_number - 1);
  fclose (fpprog);

#endif

  time (&start_time);
  while (1)
    {
      opponent = white;
      computer = black;
      player = computer;
      black_moving = 1;
      if (!GetNextMove (s))
	{
	  flag.quit = 1;
	  break;
	}
	if(sdw == -99) MaxSearchDepth = search_depth; else
      MaxSearchDepth = (sdw)?sdw:search_depth;
      if (!strcmp (black_actual_move, "White") || !strcmp (black_actual_move, "Black") || !strcmp (black_actual_move, "draw"))
	break;
      flag.force = 0;
      SelectMove (computer, FOREGROUND_MODE);
      Undo ();
      flag.force = 1;
      opponent = black;
      computer = white;
      player = opponent;
      ok = VerifyMove (black_actual_move, VERIFY_AND_MAKE_MODE, &mv);
#ifdef notdef
      ExaminePosition ();
      tmpscore = ScorePosition (black);
#endif
      if (!ok)
	{
	  fprintf (fpout, "\nBad move.  %s  Board position is: \n", s);
	  UpdateDisplay (0, 0, 1, 0);
	  flag.quit = 1;
	  break;
	}
      else
	{
	  char  x[80];
	  strcpy (x, black_actual_move);
	  if (strcmp (black_actual_move, black_suggest_move))
	    strcat (x, "#");
#ifdef notdef
	  fprintf (fpout, "%3d   %-9s      %5d%5d%5d   ", move_number, x,tmpscore, black_score, current_depth);
#endif
	if(sdw == -99)
	  fprintf (fpout, "%3d   %-9s      %5d   Book ", move_number, x, black_score, current_depth);
	else
	  fprintf (fpout, "%3d   %-9s      %5d%5d   ", move_number, x, black_score, current_depth);
	  for (i = 1; MV[i] > 0; i++)
	    {
	      algbr ((short) (MV[i] >> 8), (short) (MV[i] & 0xFF), false);
	      fprintf (fpout,"%5s ", mvstr[0]);
	    }
	  fprintf (fpout,"\n");
	  fflush (fpout);
	  move_number++;
	  total_black_moves++;
	  if (!strcmp (black_actual_move, black_suggest_move))
	    same_black_moves++;
	  Sdepth = 0;
	  ft = 0;
	}
      player = computer;
      black_moving = 0;
	if(sdb == -99) MaxSearchDepth = search_depth; else
      MaxSearchDepth = (sdb)?sdb:search_depth;
      if (!strcmp (white_actual_move, "White") || !strcmp (white_actual_move, "Black") || !strcmp (white_actual_move, "draw"))
	break;
      flag.force = 0;
      SelectMove (computer, FOREGROUND_MODE);
      Undo ();
      flag.force = 1;
      opponent = white;
      computer = black;
      player = opponent;
      ok = VerifyMove (white_actual_move, VERIFY_AND_MAKE_MODE, &mv);
#ifdef notdef
  ExaminePosition ();
  tmpscore = ScorePosition (white);
#endif
      if (!ok)
	{
	  fprintf (fpout, "\nBad move.  %s  Board position is: \n", s);
	  UpdateDisplay (0, 0, 1, 0);
	  flag.quit = 1;
	  break;
	}
      else
	{
	  char x[8];
	  strcpy (x, white_actual_move);
	  if (strcmp (white_actual_move, white_suggest_move))
	    strcat (x, "#");
#ifdef notdef
	  fprintf (fpout, "            %-9s%5d%5d%5d   ", x,tmpscore, white_score, current_depth);
#endif
	if(sdb == -99)
	  fprintf (fpout, "            %-9s%5d  Book  ", x, white_score, current_depth);
	else
	  fprintf (fpout, "            %-9s%5d%5d   ", x, white_score, current_depth);
	  for (i = 1; MV[i] > 0; i++)
	    {
	      algbr ((short) (MV[i] >> 8), (short) (MV[i] & 0xFF), false);
	      fprintf (fpout,"%5s ", mvstr[0]);
	    }
	  fprintf (fpout,"\n");
	  fflush (fpout);

#if BACKGROUND_ANALYSIS

	  /*
           * Update progress in the progress file if this is running in the
           * background.
           */

	  fpprog = fopen (progfilename, "w");
	  fprintf (fpprog, "Done with move #%d.\n", move_number - 1);
	  fclose (fpprog);
#else

	  /*
           * fprintf(stderr, "Done with move #%d.\n", move_number-1);
           */

#endif

	  total_white_moves++;
	  if (!strcmp (white_actual_move, white_suggest_move))
	    same_white_moves++;
	  Sdepth = 0;
	  ft = 0;
	}
    }

  black_percent = ((float) same_black_moves) * 100 / (float) total_black_moves;
  white_percent = ((float) same_white_moves) * 100 / (float) total_white_moves;
  fprintf (fpout, "\n           Black's percentage was %5.2f%%.     White's percentage was %5.2f%%.\n", black_percent, white_percent);
  time (&end_time);
  elapsed_time = end_time - start_time;
  fprintf (fpout, "\n           Elapsed time was %d seconds.\n", elapsed_time);
  fclose (fpin);
  fclose (fpout);
  exit (0);
}
