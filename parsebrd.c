#if !defined NO_MAIN_PARSEBRD

#include <stdio.h>
#include <fcntl.h>
#ifdef THINK_C
#include <unix.h>
#include <console.h>
#endif


#define NULLCHAR '\0'

typedef enum { False, True } Boolean;


#include "xshogi.h"

#define pawn 0
#define lance 1
#define knight 2
#define silver 3
#define gold 4
#define bishop 5
#define rook 6
#define king 7
#define no_piece 8
#define ppawn 9
#define plance 10
#define pknight 11
#define psilver 12
#define pbishop 13
#define prook 14

#define NO_PIECES 15
#define NO_SQUARES 81
#define NO_COLS 9
#define NO_ROWS 9

Boolean ParseBoard (char *string);


Boolean debugMode = True;

int  iss_column_bug = 1;  /* column number in reversed order */

char IssPieceChar[15] =
 { 'p', 'l', 'n', 's', 'G', 'b', 'r', 'K', ' ', 'P', 'L', 'N', 'S', 'B', 'R' };

ChessSquare IssChessSquare[2][15] =
 { BlackPawn, BlackLance, BlackKnight, BlackSilver, BlackGold, BlackBishop, BlackRook, BlackKing,
      EmptySquare, BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver, BlackPBishop, BlackPRook,
   WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, WhiteGold, WhiteBishop, WhiteRook, WhiteKing,
      EmptySquare, WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver, WhitePBishop, WhitePRook,
 };
 
char IssSideChar[3] =
 { '#', '=', ' ' };
 
#endif


#ifdef ISS_DEBUG

char *PieceString[2][15] =
 { " p", " l", " n", " s", " g", " b", " r", " k", "  ", 
         "+p", "+l", "+n", "+s", "+b", "+r",
   " P", " L", " N", " S", " G", " B", " R", " K", "  ", 
         "+P", "+L", "+N", "+S", "+B", "+R" };

char *ColorStr[3] = {"Black", "White", "Neutral"};

#endif
         

char *InPtr;      

Boolean PrsError = False;


Boolean ParseError (n)
  short n;
{
  if ( *InPtr == NULLCHAR )
    fprintf(stderr,"parse error %d: unexpected eos\n",n);
  else if ( *InPtr == '\n' )
    fprintf(stderr,"parse error %d: unexpected eoln\n",n);
  else
    fprintf(stderr,"parse error %d while reading '%c'\n",n,*InPtr);
  PrsError = True;
}

#define eoln() (*InPtr == NULLCHAR || *InPtr == '\n')

void skip ()
{
  while (*InPtr != ' ' && !eoln()) {
    InPtr++;
  }
  while (*InPtr == ' ')
    InPtr++;
}

Boolean copyskip (s)
  char *s;
{
  while (*InPtr != ' ' && !eoln()) {
    if ( *InPtr == '\n' ) {
      ParseError(3);     
      return False;
    }
    if ( *InPtr == NULLCHAR ) {
      ParseError(4);
      return False;
    }
    *s = *InPtr; s++;
    InPtr++;
  }
  *s = NULLCHAR;
  while (*InPtr == ' ')
    InPtr++;  
  return True;
}

void skipb ()
{
  while (*InPtr == ' ') {
    InPtr++;
  }
}

Boolean skipc (c)
  char c;
{
  if (*InPtr == c)
    InPtr++;
  else {
    fprintf(stderr,"expecting '%c'\n",c);
    ParseError(5);
    return False;
  }              
  return True;
}


void skipln ()
{
  while (*InPtr != '\n' && *InPtr != NULLCHAR)
    InPtr++;
  if ( *InPtr != NULLCHAR )
    InPtr++;
}



int ParseNumber ()
{ 
    short n;
    n = 0;
    while( isdigit(*InPtr) ) {
      n = n*10 + (*InPtr - '0');
      InPtr++;
    }
    return n;
}

void ParseCaptures (inhand)
  short *inhand;
{
  short piece, n;
  for ( piece=0; piece<=king; piece++ )
    inhand[piece] = 0;
  skipb();
  while ( !eoln() ) {
    n = ParseNumber();
    if ( n == 0 ) n = 1;
    for (piece = 0; piece < NO_PIECES; piece++) {
      if ( IssPieceChar[piece] == *InPtr ) {
        if ( piece > king )
          ParseError(6);
        else
          inhand[piece] = n;
        break;
      }
    }
    InPtr++;
    skipb();
  }
  
}


short ParseToSquare (tosquare, move_str)
  int *tosquare;
  char *move_str;
/*
 * Parse move and determine target square. Move is converted to
 * gnushogi standard notations.
 */
{
  int n, drop=0;
  char *m = move_str;
  skipb();
  if ( *InPtr == '-' || *InPtr == '|' || eoln())
    return False;       
  *move_str++ = *InPtr++; /* skip piece character */
  if ( *InPtr == '*' ) {
    *move_str++ = *InPtr++;
    drop=1;
    if ( *InPtr == '-' )
      InPtr++;
  } else {
    if ( iss_column_bug )
      *move_str++ = '9' - *InPtr++ + 1 + '0'; /* working around ISS column bug! */
    else
      *move_str++ = *InPtr++;    
    *move_str++ = *InPtr++;
    if ( *InPtr == '-' ) {
      InPtr++;
    }
  }
  if ( iss_column_bug ) {
     n = *InPtr - '1';
    *move_str++ = '9' - *InPtr++ + 1 + '0'; /* working around ISS column bug! */
  } else {
     n = '9' - *InPtr;
    *move_str++ = *InPtr++;
  }   
  n = n + NO_COLS*('i' - *InPtr);
  *tosquare = n;
  *move_str++ = *InPtr++;
  if ( *InPtr == '+' )
    *move_str++ = *InPtr;
  skip();
  *move_str = NULLCHAR;
  if ( !drop ) {
    /* remove piece character */
    while ( *m != NULLCHAR ) {
      *m = m[1]; m++;
    }
  }
  return True;
    
}


short ParseTime (timeleft)
  long *timeleft;
{
  short minus=0, hours=0, minutes=0, seconds=0;
  if ( *InPtr == '-' ) {
    minus = 1;
    InPtr++;
  };
  seconds = ParseNumber();
  if ( *InPtr == ':' ) {
    InPtr++;
    minutes = seconds;
    seconds = ParseNumber();
    if ( *InPtr == ':' ) {
      hours = minutes;
      minutes = seconds;
      seconds = ParseNumber();
    }
  }
  if ( minus )
    *timeleft = -(3600*hours + 60*minutes + seconds);
  else
    *timeleft = (3600*hours + 60*minutes + seconds);
  skipb();
  if ( *InPtr == '<' )
    return True;
  else
    return False; 
     
}





/* Parse a game score from the character string "game", and
   record it as the history of the current game.  The game
   score is assumed to start from the standard position. 
   The display is not updated in any way.
   
   MANY UPDATES FOR ISS (not shure about relationships between ICS and ISS
*/

Boolean ParseGameHistory(game)
     char *game;
{
    ChessMove move_type;
    int from_x, from_y, to_x, to_y, boardIndex;
    char promo_char;
    char *p;

#if defined NO_MAIN_PARSEBRD
    if (localPlayer.appData.debugMode)
      fprintf(stderr, "Parsing game history: %s (NYI)\n", game);
#else
    if (debugMode)
      fprintf(stderr, "Parsing game history: %s (NYI)\n", game);
#endif

    InPtr = game;
    
#ifdef needs_update
      
    /* Parse out names of players */
    while (*game == ' ') game++;
    p = iss_white;
    while (*game != ' ') *p++ = *game++;
    *p = NULLCHAR;
    while (*game == ' ') game++;
    p = iss_black;
    while (*game != ' ' && *game != '\n') *p++ = *game++;
    *p = NULLCHAR;

    /* Parse moves */
    CopyBoard(boards[0], initialPosition);
    startedFromSetupPosition = False;
    boardIndex = 0;
    for (;;) {
	move_type = yylexstr(boardIndex, game, &game);
	switch (move_type) {
	  case NormalMove:
	  case BadMove:
	  case AmbiguousMove:
	    /* bug? */
	    fprintf(stderr, "%s: ?bad move in ISS output: %s\n",
		    programName, yytext);
	    /* fall thru */
	  case 0:	/* end of file */
	    if (boardIndex < backwardMostMove) {
		/* Oops, gap.  How did that happen? */
		return;
	    }
	    backwardMostMove = 0;
	    if (boardIndex > forwardMostMove) {
		forwardMostMove = boardIndex;
	    }
	    return;
	  case ElapsedTime:
	    if (boardIndex > 0) {
		strcat(parseList[boardIndex-1], " ");
		strcat(parseList[boardIndex-1], yytext);
	    }
	    continue;
	  case Comment:
	  case StartGame:
	  default:
	    /* ignore */
	    continue;
	  case WhiteWins:
	  case BlackWins:
	  case GameIsDrawn:
	    strncpy(endMessage, yytext, MOVE_LEN * 4);
	    endMessage[MOVE_LEN * 4 - 1] = NULLCHAR;
	    continue;
	}
	(void) CoordsToAlgebraic(from_x, from_y, to_x, to_y, promo_char,
				 boardIndex, parseList[boardIndex]);
	CopyBoard(boards[boardIndex + 1], boards[boardIndex]);
	boardIndex++;
	ApplyMove(&move_type, from_x, from_y, to_x, to_y,
		  boards[boardIndex]);
    }

#endif         
  return False;
}



Boolean ParseBoard(string)
     char *string;
{              
                          
    int gamenum;
    int move_num;
    char PlayerName[2][20];
    char PlayerRank[2][10];
    
    short board[NO_SQUARES];
    short color[NO_SQUARES];
    short inhand[2][NO_PIECES];
    long  timeleft[2];
    short sidetomove = 3;
    int   tosquare = -1;
    char  move_str[6][20];
    int   move_nr = 0;
    
    int side, piece, col, row, sq;

#if defined NO_MAIN_PARSEBRD

    IssMode new_iss_mode = iss_mode;
    char str[80];
    int k,j;

    DisplayClocks(StopTimers);
                   
    if (localPlayer.appData.debugMode)
      fprintf(stderr, "Parsing board: %s\n", string);
      
#endif              

    InPtr = string;
    
    skipb(); /* skip leading blanks */

    if ( isdigit(*InPtr) ) { 
      gamenum = atoi(InPtr); skip();
    } else {
      gamenum = 0;
    }
    for ( side = 1; side >= 0; side--) {
      skip(); /* skip shogi indicator "(S):" or "vs" */
      if ( !copyskip(PlayerName[side]) ) return False;
      if ( !skipc('[') ) return False; skipb();
      if ( !copyskip(PlayerRank[side]) ) return False;
      if ( !skipc(']') ) return False; skipb();
    }
    skipln(); /* goto next line */
    skipb();
    iss_column_bug = (*InPtr == '1'); /* column numbers in reverse order: ISS bug! */
    skipln(); /* skip column numbers */

    for ( row=NO_ROWS-1; row>=0; row-- ) {
      skipln(); /* skip border */
      skipb(); /* skip leading blanks */
      if ( !skipc('a'+(NO_ROWS-row-1)) ) return False; /* skip row number */
      InPtr++; /* skip ' ' or '>' */
      for ( col=0; col<NO_COLS; col++ ) {
        sq = row*NO_COLS + col;
        board[sq] = no_piece; color[sq] = 3;
        if ( !skipc('|') ) return False; skipc(' ');
        for ( side=0; side<3; side++ ) {
          if ( *InPtr == IssSideChar[side] ) {
            color[sq] = side;
            break;
          }
        }
        InPtr++;
        for ( piece=0; piece<NO_PIECES; piece++ ) {
          if ( *InPtr == IssPieceChar[piece] ) {
            board[sq] = piece;
            break;
          }
        }
        InPtr++;
        if ( !skipc(' ') ) return False;
      }
      if ( !skipc('|') ) return False;
      skipb();
      skip(); /* skip row number */
      if ( row == 8 ) {
        /* parse white captures */
        ParseCaptures(inhand[1]);
      } else if ( row == 7 ) {
        /* parse white time */
        if ( ParseTime(&timeleft[1]) )
          sidetomove = 1;
      } else if ( row == 5 || row == 4 || row == 3) {
        int n;
        /* parse target square of last move */
	if ( ParseToSquare(&n,move_str[move_nr]) ) {
	  move_nr++;
          tosquare = n;                               
          if ( *InPtr == '|' ) { 
            skip();
            if ( ParseToSquare(&n,move_str[move_nr]) ) {
	      move_nr += 1;                
              tosquare = n;                               
	    }
          }
	}
      } else if ( row == 1 ) {
        /* parse black time */
        if ( ParseTime(&timeleft[0]) )
          sidetomove = 0;
      } else if ( row == 0 ) {
        /* parse black captures */
        ParseCaptures(inhand[0]);
      }
      skipln();
    } 

#ifdef needs_update
#else                    
    /* don't know the move number but need, whether black or white has to move.
     */
    if ( (move_nr & 1) != (forwardMostMove & 1) )
      move_num = forwardMostMove + 1;
    else
      move_num = forwardMostMove;
#ifdef notdef
    printf("forwardMostMove = %d move_nr = %d move_num = %d\n",
      forwardMostMove, move_nr, move_num);
#endif
#endif
    
#ifdef ISS_DEBUG 

#if defined NO_MAIN_PARSEBRD
    if (localPlayer.appData.debugMode) {
#else
    if (debugMode) {
#endif
      short piece, i;
      fprintf(stdout,"GameNumber: %d MoveNumber: %d\n",gamenum,move_num);
      for ( side=0; side<=1; side++ ) {
        fprintf(stdout, "Player Color: %s Name: %s Rank: %s\n",
          ColorStr[side], PlayerName[side], PlayerRank[side]);
      }
      for ( row=NO_ROWS-1; row>=0; row-- ) {
        fprintf(stdout, "\n");
        for ( col=0; col<NO_COLS; col++ ) {
          sq = row*NO_COLS + col;
          if ( board[sq] == no_piece )
            fprintf(stdout, " -");
          else
            fprintf(stdout, "%s",PieceString[color[sq]][board[sq]]);
        }
      }
      fprintf(stdout,"\n");
      for ( side=0; side<=1; side++ ) {
        fprintf(stdout,"%s captures: ",ColorStr[side]);
        for ( piece=pawn; piece<king; piece++) {
          if ( inhand[side][piece] )
            fprintf(stdout, "%d%c ", inhand[side][piece], IssPieceChar[piece]);
        }
        fprintf(stdout," TimeLeft: %ld\n",timeleft[side]);
      }
      for ( i=0; i<move_nr; i++ )
         fprintf(stdout,"move %d: %s\n",i,move_str[i]);
      fprintf(stdout,"Last move to square: %d\n",tosquare);
      fprintf(stdout,"Side to move: %s\n",ColorStr[sidetomove]);
    }
#endif
    
#if defined NO_MAIN_PARSEBRD

/*
    if (playing_white == '*')
      new_iss_mode = IssPlayingWhite;
    else if (playing_black == '*')
      new_iss_mode = IssPlayingBlack;
    else
      new_iss_mode = IssObserving;
*/
    
    /* Take action if this is the first board of a new game */
#ifdef needs_update
      if (gamenum != iss_gamenum) {
	if (iss_mode == IssObserving) {
	    /* Error: xboard can't handle two games at once */
	    /* Stop observing the old game */
	    fprintf(stderr, "%s: Aren't you currently observing game %d",
		    programName, iss_gamenum);
	    fprintf(stderr, "?  Attempting to stop observing it.\n");
	    sprintf(str, "observe %d\n", iss_gamenum);
	    SendToISS(str);
	    /* continue as in normal case */
	} else if (iss_mode != IssIdle) {
	    /* Error: xboard can't handle two games at once */
	    if (new_iss_mode == IssObserving) {
		/* Stop observing the new game */
		fprintf(stderr, "%s: Aren't you playing a game now?  ",
			programName);
		fprintf(stderr, "Attempting to stop observing game %d.\n",
			gamenum);
		SendToISS("observe\n");
		/* ignore this board */
		return False;
	    } else /* new_iss_mode == IssPlaying(White|Black) */ {
		/* Playing two games???  ISS supposedly can't do this. */
		fprintf(stderr, "%s: BUG: playing two games (%d and %d)\n",
			programName, iss_gamenum, gamenum);
		/* continue as in normal case, hoping old game is gone */
	    }
	}
	/* Normal case (iss_mode == IssIdle), or error recovered */
	Reset(False);     
	if (move_num > 0) {
	    /* Need to get game history */
	    iss_getting_history = True;
	    sprintf(str, "moves %d\n", gamenum);
	    SendToISS(str);
	}          
    }
    /* Initially flip the board to have black on the bottom iff playing
       black, but let the user change it with the Flip View button. */
    if (iss_mode == IssIdle)
      flipView = (new_iss_mode == IssPlayingBlack);
#endif

    /* Update known move number limits */               
    if (iss_mode == IssIdle)
      forwardMostMove = backwardMostMove = currentMove = move_num;
    if (move_num > forwardMostMove + 1 || move_num < forwardMostMove) {
	/* Different game or out of sequence positions (should not happen) */
	backwardMostMove = forwardMostMove = move_num;
    } else if (move_num > forwardMostMove) {
	forwardMostMove = move_num;
    }
    if (gameMode != PauseGame) 
      currentMove = move_num;

    /* Done with values from previous mode; copy in new ones */
    iss_mode = new_iss_mode;
    iss_gamenum = gamenum;
    strcpy(iss_black, PlayerName[0]);
    strcpy(iss_white, PlayerName[1]);

    /* Display opponents and material strengths */
    sprintf(str, "%s (%s) B vs. %s (%s) W",
	    iss_black, PlayerRank[0], iss_white, PlayerRank[1]);
    DisplayTitle(str);
    
    /* Set current board from parsed position */
    for (k = 0; k < NO_ROWS; k++)
      for (j = 0; j < NO_COLS; j++) {
        short sq = k*NO_COLS + j;
	if ( board[sq] == no_piece )
	  boards[move_num][k][j] = EmptySquare;
	else
	  boards[move_num][k][j] = IssChessSquare[color[sq]][board[sq]];
      }
      
    /* Set current captures from parsed position */
    for (k = 0; k <= 1; k++)
      for (j = 0; j < king; j++ )
        catches[move_num][k][j] = inhand[k][j];
    
    /* Put the move on the move list, first converting
       to canonical algebraic form. */
    if (move_num > 0) {
      ChessMove move_type;
      int from_x, from_y, to_x, to_y;
      char promo_piece;

      if ( move_nr > 0 )	
	if (move_num - 1 < backwardMostMove) {
	    /* We don't know what the board looked like before
	       this move.  Punt. */
	    strcpy(parseList[move_num - 1], move_str[move_nr-1]);
	} else {
	    ParseMachineMove(move_str[move_nr-1], &move_type,
			     &from_x, &from_y, &to_x, &to_y);

	    if ( move_type == WhitePromotion || move_type == BlackPromotion )
		promo_piece = '+';
	    else
		promo_piece = NULLCHAR;

	    (void) MakeAlg(from_x, from_y, to_x, to_y, promo_piece,
				     move_num - 1, parseList[move_num - 1]);
#ifdef needs_update
	    strcat(parseList[move_num - 1], " ");
	    strcat(parseList[move_num - 1], elapsed_time);
#endif
	}
    }
    
    /* Update the clocks */
    timeRemaining[1][forwardMostMove] = blackTimeRemaining = timeleft[0] * 1000;
    timeRemaining[0][forwardMostMove] = whiteTimeRemaining = timeleft[1] * 1000;

    /* show move */                   
    if ( move_nr > 0 ) {   
      strcpy(parseList[currentMove-1],move_str[move_nr-1]);
      DisplayMove(currentMove-1);                          
      /* DisplayMessage(move_str[move_nr-1],False); */
#if BLINK_COUNT
      /* blink target square */
      if ( tosquare > 0 ) {
        short to_y, to_x;
        to_y = tosquare / 9;
	to_x = tosquare % 9;
	BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
      }
#endif
    } else {
      DisplayMessage("board has beed updated",False);
    }

#endif 

    return True;

}



#if !defined NO_MAIN_PARSEBRD

int main (argc, argv)
  int argc;
  char **argv;
{
    FILE *fd;

#ifdef THINK_C
    ccommand(&argv);
#endif

    if ((fd = fopen ("board.example", "r")) != NULL) {
    
          static char parse[2000];
          int i = 0;
          char c;
          
    	  while ((c = getc (fd)) != EOF) {
    	    parse[i++] = c;
    	  }
    	  parse[i] = NULLCHAR;
    	  
    	  fclose(fd);
    	  
    	  (void) ParseBoard(parse);

    }

}


#endif


