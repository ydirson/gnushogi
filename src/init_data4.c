#if defined THINK_C
#include "gnushogi.h"
#endif


next_array prook_nextdir, pbishop_nextdir, wgold_nextdir;



const small_short Stboard[NO_SQUARES] =
{lance,knight,silver,gold,king,gold,silver,knight,lance,
 0, bishop, 0, 0, 0, 0, 0, rook, 0,
 pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, pawn, 
 0, rook, 0, 0, 0, 0, 0, bishop, 0,
 lance,knight,silver,gold,king,gold,silver,knight,lance};

const small_short Stcolor[NO_SQUARES] =
{black, black, black, black, black, black, black, black, black, 
 neutral, black, neutral, neutral, neutral, neutral, neutral, black, neutral,
 black, black, black, black, black, black, black, black, black, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, neutral, 
 white, white, white, white, white, white, white, white, white,
 neutral, white, neutral, neutral, neutral, neutral, neutral, white, neutral,
 white, white, white, white, white, white, white, white, white};


small_short board[NO_SQUARES], color[NO_SQUARES];
