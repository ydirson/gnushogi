
#include "gnushogi.h"

#include "pattern.h"


#define NO_MAIN

#include "version.h"

#include "main.c"


/*

small_short PieceCnt[2];
small_short PieceList[2][NO_SQUARES];
small_short PieceIndex[NO_SQUARES];

small_short board[NO_SQUARES];
small_short color[NO_SQUARES];

*/

void
test_distance ()
{
  short side, piece, f, t, d;

  for ( side = 0; side <= 1; side++ ) {
    printf("SIDE = %d\n",side);
    for ( piece = pawn; piece <= king; piece++ ) {
      printf("PIECE = %d\n",piece);
      for ( f = 0; f < NO_SQUARES; f++ ) {
	printf("FROM %d TO ",f);
	for ( t = 0; t < NO_SQUARES; t++ ) {
	  d = piece_distance(side,piece,f,t);
	  if ( d != CANNOT_REACH )
	    printf("%d:%d ",t,d);
	}
	printf("\n");
      }
    }
  }

}



void
main (int argc, char **argv)

{

  short d, sq, side; 
  PatternFields pattern;
  char s[80], *Lang = NULL; 
  
#ifdef THINK_C
#include <console.h>
  ccommand(&argv);
#endif

  InitConst (Lang);
  Initialize_dist ();

  for (sq = 0; sq < NO_SQUARES; sq++ ) {
    board[sq] = no_piece;
    color[sq] = neutral;               
  }                                  

  ClearCaptured ();

  for (side = 0; side <= 1; side++)
    PieceCnt[side] = -1;

#ifdef TEST_DISTANCE

  strcpy(s,"g6i k5i g4i p9g p8g r* s3h p7g b8h B* S5f");

  if ( string_to_board_color (s) )
    {
      printf("ERROR IN string_to_board_color");
      exit(1);
    }
  else
    {
      UpdateDisplay (0, 0, 1, 0);
    }

  strcpy(s,"k8h g7h g6g p9f p8g p7f S5h");

  if ( string_to_patternfields (s, &pattern) )
    {
      printf("ERROR IN string_to_patternfields");
      exit(1);
    }
  else
    {
      DisplayPattern (&pattern);
    }

  d = pattern_distance (black, &pattern);

  printf("distance = %d\n", d);

#endif

  GetOpeningPatterns ();
  ShowOpeningPatterns ();
     
}


