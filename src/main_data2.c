#if defined THINK_C
#include "gnushogi.h"
#endif

small_short ChkFlag[MAXDEPTH], CptrFlag[MAXDEPTH];
short Pscore[MAXDEPTH], Tscore[MAXDEPTH];
short FROMsquare, TOsquare;
short mtl[2], pmtl[2], hung[2];
small_short Pindex[NO_SQUARES];
small_short PieceCnt[2];

struct GameRec GameList[MAXMOVES + MAXDEPTH];

char *ColorStr[2];
char *CP[CPSIZE];

