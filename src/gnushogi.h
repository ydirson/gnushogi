/*
 * gnushogi.h - Header file for GNU SHOGI
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
 * the Free Software Foundation.
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

#if defined THINK_C 

/* #define NONDSP */
/* #define DEBUG */
/* #define DEBUG41 */
/* #define DEBUG40 */
/* #define DEBUG_EVAL */
/* OPT */
/* #define XSHOGI */
/* #define BAREBONES */
#define GDX
#define SAVE_PTYPE_DISTDATA
#define SAVE_DISTDATA
#define HARDTIMELIMIT
#define DEBUG9
/* #define NULLMOVE */
#define VERYBUGGY
#define CHECKTRP
#define AG4
#define QUIETBACKGROUND
/* #define HASGETTIMEOFDAY */
#define K32SEGMENTS
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
extern int gettimeofday(struct timeval *tp, void *tzp);
#define HISTORY
/* #define EXACTHISTORY */
#define CACHE
#define NOSCORESPACE
#define SEMIQUIETBOOKGEN
/* #define NOTTABLE */
/* GENOPT */
#define DROPBONUS
#define FIELDBONUS
#define TESUJIBONUS
/* FILES */
#define LANGFILE "gnushogi.lang"
#define BOOK "gnushogi.book"
#define BINBOOK "gnushogi.book.data"
#define HASHFILE "gnushogi.hash"
#define PATTERNFILE "gnushogi.pattern"
/* #define DEBUG */
#define small_short char
#define small_ushort unsigned char

#else

#define small_short char
#define small_ushort short

#endif


#if !defined(__STDC__) && !defined(MSDOS)
#define const
#endif

#ifndef __GNUC__
#define inline
#endif

#include <stdio.h>

#define SEEK_SET 0
#define SEEK_END 2
#ifdef DEBUG
void
ShowDBLine (char *, short int, short int,
	    short int, short int, short int,
	    short unsigned int *);
     extern FILE *debugfd;
     extern short int debuglevel;

#endif /* DEBUG */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifdef THINK_C
#define RWA_ACC "r+b"
#define WA_ACC "w+b"
#elif defined MSDOS
#include <time.h>
#include <malloc.h>
#define RWA_ACC "r+b"
#define WA_ACC "w+b"
#ifdef timeout
#undef timeout
#endif
#define printz printf
#define scanz scanf
#else
#define RWA_ACC "r+"
#define WA_ACC "w+"
#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>
#endif /* MSDOS */
#if defined NONDSP || defined THINK_C
#define printz printf
#define scanz scanf
#else
#include <curses.h>
#define scanz fflush(stdout),scanw
#define printz printw
#endif

#ifdef notdef
#if defined(__STDC__) || defined(MSDOS)
/* <stdio.h> */
     extern int fclose (FILE *);
#ifndef __ultrix /* work around bug in c89 compiler --t.mann */
     extern int fscanf (FILE *, const char *, ...);
     extern int fprintf (FILE *, const char *, ...);
#endif /*__ultrix*/
     extern int fflush (FILE *);

/* <stdlib.h> */
     extern int abs (int);
     extern int atoi (const char *);

/* <time.h> */
     extern long int time (long int *);

/* <string.h> */
     extern void *memset (void *, int, size_t);
#endif
#endif

#define NO_PIECES 15
#define NO_PTYPE_PIECES 15
#define NO_SQUARES 81
#define NO_COLS 9
#define NO_ROWS 9

#if defined HASHFILE || defined CACHE
#define PTBLBDSIZE (NO_SQUARES+NO_PIECES)
#endif

/* Piece values and relative values */
#define valueP   70 /* 1 */
#define valuePp 150 /* 2 */
#define valueL  240 /* 3 */
#define valueN  250 /* 4 */
#define valueLp 260 /* 5 */
#define valueNp 270 /* 6 */
#define valueS  340 /* 7 */
#define valueSp 350 /* 8 */
#define valueG  360 /* 9 */
#define valueB  850 /* 10 */
#define valueBp 870 /* 11 */
#define valueR  920 /* 12 */
#define valueRp 970 /* 13 */
#define valueK  1200 /* 14 */
#define SCORE_LIMIT 12000

/* masks into upper 16 bits of ataks array */
/* observe order of relative piece values */
#define CNT_MASK 0x000000FF
#define ctlP  0x00200000
#define ctlPp 0x00100000
#define ctlL  0x00080000
#define ctlN  0x00040000
#define ctlLp 0x00020000
#define ctlNp 0x00010000
#define ctlS  0x00008000
#define ctlSp 0x00004000
#define ctlG  0x00002000
#define ctlB  0x00001000
#define ctlBp 0x00000800
#define ctlR  0x00000400
#define ctlRp 0x00000200
#define ctlK  0x00000100

/* attack functions */
#define Patak(c, u) (atak[c][u] > ctlP)
#define Anyatak(c, u) (atak[c][u] > 0)

/* hashtable flags */
#define truescore 0x0001
#define lowerbound 0x0002
#define upperbound 0x0004
#define kingcastle 0x0008
#define queencastle 0x0010
#define evalflag 0x0020

/* king positions */
#define BlackKing PieceList[black][0]
#define WhiteKing PieceList[white][0]
#define OwnKing PieceList[c1][0]
#define EnemyKing PieceList[c2][0]

/* constants */

#define OPENING_HINT 0x141d /* P7g-7f (20->29) */

/* truth values */
#define false 0
#define true 1

/* colors */
#define black 0
#define white 1
#define neutral 2

/* piece code defines */
#define no_piece 0
#define pawn 1
#define lance 2
#define knight 3
#define silver 4
#define gold 5
#define bishop 6
#define rook 7
#define ppawn 8
#define plance 9
#define pknight 10
#define psilver 11
#define pbishop 12
#define prook 13
#define king 14

#define ptype_no_piece 0
#define ptype_pawn 0
#define ptype_lance 1
#define ptype_knight 2
#define ptype_silver 3
#define ptype_gold 4
#define ptype_bishop 5
#define ptype_rook 6
#define ptype_pbishop 7
#define ptype_prook 8
#define ptype_king 9
#define ptype_wpawn 10
#define ptype_wlance 11
#define ptype_wknight 12
#define ptype_wsilver 13
#define ptype_wgold 14

/* node flags */
#define pmask        0x000f /*    15 */
#define promote      0x0010 /*    16 */
#define dropmask     0x0020 /*    32 */
#define exact        0x0040 /*    64 */
#define tesuji       0x0080 /*   128 */
#define check        0x0100 /*   256 */
#define capture      0x0200 /*   512 */
#define draw         0x0400 /*  1024 */
#define stupid       0x0800 /*  2048 */
#define questionable 0x1000 /*  4096 */
#define kingattack   0x2000 /*  8192 */
#define book         0x4000 /* 16384 */

/* move symbols */
#define pxx (CP[2])
#define qxx (CP[1])
#define rxx (CP[4])
#define cxx (CP[3])
/***************************************************************************/
/***************** Table limits ********************************************/
/*
 * ttblsz must be a power of 2. Setting ttblsz 0 removes the transposition
 * tables.
 */
#if defined NOTTABLE
#define vttblsz 0
#elif defined MSDOS
#define vttblsz (1 << 11)
#elif defined THINK_C
#define vttblsz (1 << 10)
#define huge
#else
#ifdef DEBUG
#define vttblsz (1 << 11)
#else
#define vttblsz (1 << 16)
#endif
/* #define vttblsz (1 << 17) */
#define huge
#endif /* MSDOS */
#define ttblsz vttblsz

#if defined THINK_C
#define TREE 2200		/* max number of tree entries */
#else
#define TREE 4000		/* max number of tree entries */
#endif

#define MAXDEPTH 20		/* max depth a search can be carried */
#define MINDEPTH 2		/* min search depth =1 (no hint), >1 hint */
#define MAXMOVES 300		/* max number of half moves in a game */
#if defined THINK_C
#define BOOKSIZE 1000
#elif defined MSDOS
#define BOOKSIZE 10000		/* Number of unique position/move combinations allowed */
#else
#define BOOKSIZE 250000		/* Number of unique position/move combinations allowed */
#endif
#define CPSIZE 235		/* size of lang file max */
#ifdef THINK_C
#define ETABLE (2<<10)		/* static eval cache */
#else
#define ETABLE (2<<12)		/* static eval cache */
#endif
/***************** tuning paramaters **********************************************/
#define MINGAMEIN 4
#define MINMOVES 15
#define CHKDEPTH 1		/* always look forward CHKDEPTH half-moves if in check */
#ifdef THINK_C
#define DEPTHBEYOND 9	        /* Max to go beyond Sdepth */
#else
#define DEPTHBEYOND 11	        /* Max to go beyond Sdepth */
#endif
#define HASHDEPTH 4		/* depth above which to use HashFile */
#define HASHMOVELIMIT 40	/* Use HashFile only for this many moves */
#define PTVALUE 0	        /* material value below which pawn threats at 5 & 3 are used */
#define ZDEPTH 3		/* depth beyond which to check ZDELTA for extra time */
#define ZDELTA 10		/* score delta per ply to cause extra time to be given */
#ifdef THINK_C
#define ZNODES (flag.tsume ? 200 : 500)	 /* check the time every ZNODES positions */
#else
#define ZNODES (flag.tsume ? 400 : 1000) /* check the time every ZNODES positions */
#endif
#define MAXTCCOUNTX  10		/* max number of time clicks per search to complete ply */
#define MAXTCCOUNTR 4		/* max number of time clicks per search extensions*/
#define SCORESPLIM 8		/* Score space doesn't apply after this stage */
#define SDEPTHLIM Sdepth+1
#define HISTORYLIM 4096		/* Max value of history killer */
#ifdef EXACTHISTORY
#ifdef THINK_C
#define HISTORY_MASK 0x8000     /* mask to MSB of history index */
#define HISTORY_SIZE 0x10000    /* size of history table */
#else
#define HISTORY_MASK (1 << 15)  /* mask to MSB of history index */
#define HISTORY_SIZE (1 << 16)  /* size of history table */
#endif
#else
/* smaller history table, but dangerous because of collisions */
#define HISTORY_MASK 0x3fff     /* mask to significant bits of history index */
#define HISTORY_SIZE (1 << 14)  /* size of history table */
#endif
                                               
#define sizeof_history (sizeof(unsigned short) * (size_t)HISTORY_SIZE)

#ifdef EXACTHISTORY
/* Map from.to (8bit.8bit) to from.to (0.7bit.8bit) */
#define hmove(mv) (mv & 0x7fff)
#else
/* Map from.to (8bit.8bit) to from.to (00.7bit.7bit) */
/* Swap bits of ToSquare in case of promotions, hoping that
   no catastrophic collision occur. */
#define hmove(mv) (((mv & 0x7f00) >> 1) | \
                   ((mv & 0x0080) ? ((mv & 0x007f) ^ 0x007f) : (mv & 0x007f)))
#endif

/* mask color to 15th bit */
#if defined DEBUG    
     extern char mvstr[4][6];
     extern void movealgbr (short int move, char *s);
     extern void algbr (short int f, short int t, short int flag);
static
unsigned short
hindex(unsigned short c, unsigned short mv)
{ unsigned short h;
  h = hmove(mv);
#ifdef EXACTHISTORY
  if ( (h & 0xff) != (mv & 0xff) ||
       ((h>>8) & 0x7f) != ((mv>>8) & 0x7f) ) { 
     algbr ( mv >> 8, mv & 0xff, 0);
     printf("hindex error h=%x mv=%x %s\n",h,mv,mvstr[0]);
  }                
#else
  if ( ((h & 0x7f) ^ ((mv&0x0080)?0x007f:0)) != (mv & 0x7f) ||
       ((h>>7) & 0x7f) != ((mv>>8) & 0x7f) ) { 
     algbr ( mv >> 8, mv & 0xff, 0);
     printf("hindex error h=%x mv=%x %s\n",h,mv,mvstr[0]);
  }                
#endif
#ifdef EXACTHISTORY 
  if ( c ) h |= HISTORY_MASK;
#else               
  /* for white, swap bits, hoping that no catastrophic collision occur. */
  if ( c ) h = (~h) & HISTORY_MASK;
#endif
  if ( h >= HISTORY_SIZE ) {
    printf("hindex(%x,%x)=%x exceeds HISTORY_SIZE=%d\n",
              c, mv, h, HISTORY_SIZE);
    exit(1);
  };
  return (h);
}
#else
#ifdef EXACTHISTORY
#define hindex(c,mv) ((c ? HISTORY_MASK : 0) | hmove(mv))
#else 
/* for white, swap bits, hoping that no catastrophic collision occur. */
#define hindex(c,mv) (c ? ((~hmove(mv)) & HISTORY_MASK) : hmove(mv))
#endif
#endif /* DEBUG */

#define EWNDW 10		/* Eval window to force position scoring at depth greater than Sdepth + 2 */
#define WAWNDW 90		/* alpha window when computer black*/
#define WBWNDW 90		/* beta window when computer black*/
#define BAWNDW 90		/* alpha window when computer white*/
#define BBWNDW 90		/* beta window when computer white*/
#define BXWNDW 90		/* window to force position scoring at lower */
#define WXWNDW 90		/* window to force position scoring at lower */

#define DITHER 5		/* max amount random can alter a pos value */
#define LBONUS 1		/* points per stage value of L increases */
#define BBONUS 2		/* points per stage value of B increases */
#define RBONUS 2		/* points per stage value of R increases */
                                                           
#define QUESTIONABLE (valueK)   /* malus for questionable moves */
#define STUPID (valueR<<1)      /* malus for stupid moves */

#define KINGPOSLIMIT ( -1)	/* King positional scoring limit */
#define KINGSAFETY  32
#define MAXrehash (7)

#if defined AG0
#define WHITEAG0
#define BLACKAG0

#elif defined AG1
#define WHITEAG1
#define BLACKAG1

#elif defined AG2
#define WHITEAG2
#define BLACKAG2

#elif defined AG3
#define WHITEAG3
#define BLACKAG3

#elif defined AGB
#define WHITEAG2
#define BLACKAG2

#elif defined AG4
#define WHITEAG4
#define BLACKAG4
#endif
/************************* parameters for Opening Book *********************************/
#define BOOKFAIL 5		/* if no book move found for BOOKFAIL turns stop using book */
#ifdef THINK_C
#define BOOKMAXPLY 18		/* Max plys to keep in book database */
#else
#define BOOKMAXPLY 28		/* Max plys to keep in book database */
#endif
#define BOOKPOCKET 64
#define BOOKRAND 1000		/* used to select an opening move from a list */
#define BOOKENDPCT 950		/* 5 % chance a BOOKEND will stop the book */
#define DONTUSE -32768		/* flag move as don't use */
/*************************** Book access defines ****************************************/
#define SIDEMASK 0x1
#define LASTMOVE 0x4000		/* means this is the last move of an opening */
#define BADMOVE 0x8000		/* means this is a bad move in this position */
/****************************************************************************************/
     struct hashval
     {
       unsigned long key, bd;
     };
     struct hashentry
     {
       unsigned long hashbd;
       unsigned short mv;
       unsigned char flags, depth;	/* char saves some space */
       unsigned short age;
       short score;
#ifdef HASHTEST
       unsigned char bd[PTBLBDSIZE];
#endif /* HASHTEST */ 

     };

#if defined HASHFILE || defined CACHE
     struct etable
     { 
	unsigned long ehashbd;
	short int escore[2];
	short int sscore[NO_SQUARES];
	short int score;
	small_short hung[2];
#ifdef CACHETEST
       	unsigned char bd[PTBLBDSIZE];
#endif /* CACHETEST */ 
     } ;
#if defined CACHE
typedef struct etable etable_field[ETABLE];
extern etable_field *etab[2];
#endif

/*
 * persistent transposition table. The size must be a power of 2. If you
 * change the size, be sure to run gnuchess -c before anything else.
 */
#define frehash 6
#ifdef MSDOS
#define Deffilesz (1 << 11) -1
#elif defined THINK_C
#define Deffilesz (vttblsz - 1)
#else
#define Deffilesz (1 << 17) -1
#endif /* MSDOS */
     struct fileentry
     {
       unsigned char bd[PTBLBDSIZE];
       unsigned char f, t, flags, depth, sh, sl;
     };

#endif /* HASHFILE */


     struct leaf
     {
       small_ushort f, t;
       short score, reply, width;
       unsigned short flags;
     };
     struct GameRec
     {
       unsigned short gmove;	/* this move */
       short score;		/* score after this move */
       short depth;		/* search depth this move */
       long time;               /* search time this move */
       short piece;		/* piece captured */
       short color;		/* color */
       short flags;		/* move flags capture, promote, castle */
       short Game50;		/* flag for repetition */
       long nodes;		/* nodes searched for this move */
       unsigned long hashkey, hashbd;	/* board key before this move */
#ifdef DEBUG40
       int d1;
       int d2;
       int d3;
       int d4;
       int d5;
       int d6;
       int d7;
#endif
     };
     struct TimeControlRec
     {
       short moves[2];
       long clock[2];
     };

     struct flags
     {
       short mate;		/* the game is over */
       short post;		/* show principle variation */
       short quit;		/* quit/exit */
       short regularstart;	/* did the game start from standard
				 * initial board ? */
       short reverse;		/* reverse board display */
       short bothsides;		/* computer plays both sides */
       short hash;		/* enable/disable transposition table */
       short force;		/* enter moves */
       short easy;		/* disable thinking on opponents time */
       short beep;		/* enable/disable beep */
       short timeout;		/* time to make a move */
       short musttimeout;	/* time to make a move */
       short back;		/* time to make a move */
       short rcptr;		/* enable/disable recapture heuristics */
       short rv;		/* reverse video */
       short stars;		/* add stars to uxdsp screen */
       short coords;		/* add coords to visual screen */
       short shade;
       short material;		/* draw on lack of material */
       short illegal;		/* illegal position */
       short onemove;		/* timing is onemove */
       short gamein;		/* timing is gamein */
       short tsume;		/* first consider checks */
     };

#ifdef DEBUG
     extern FILE *debugfile;

#endif /* DEBUG */

#ifdef HISTORY
     extern unsigned short *history;
#endif
     extern char *ColorStr[2];
     extern unsigned short int MV[MAXDEPTH];
     extern int MSCORE;
     extern int mycnt1, mycnt2;
     extern short int ahead;
     extern short int xshogi;
     extern struct leaf Tree[], *root,rootnode;
     extern char savefile[], listfile[];
     extern short TrPnt[];
     extern small_short board[], color[]; 
     extern small_short PieceList[2][NO_SQUARES], PawnCnt[2][NO_COLS];
     extern small_short Captured[2][NO_PIECES];

#ifdef NOMEMSET
#define ClearCaptured() \
  { short piece, color;\
    for (color = black; color <= white; color++)\
      for (piece = 0; piece < NO_PIECES; piece++)\
        Captured[color][piece] = 0;\
  }
#else
#define ClearCaptured() \
  memset ((char *)Captured, 0,(unsigned long)sizeof(Captured))
#endif /* NOMEMSET */

     extern small_short Mvboard[];

     extern short svalue[NO_SQUARES];
     extern short pscore[2]; /* eval.c */
     extern int EADD; /* eval.c */
     extern int EGET; /* eval.c */
     extern struct flags flag;
     extern short opponent, computer, INCscore;
     extern short WAwindow, BAwindow, WBwindow, BBwindow;
     extern short dither, player;
     extern short xwndw, contempt;
     extern long ResponseTime, ExtraTime, MaxResponseTime, et, et0, time0, ft;
     extern long reminus, replus;
     extern long GenCnt, NodeCnt, ETnodes, EvalNodes, HashAdd, HashCnt, HashCol, THashCol,
      FHashCnt, FHashAdd;
     extern short HashDepth, HashMoveLimit;
     extern struct GameRec GameList[];
     extern short GameCnt, Game50;
     extern short Sdepth, MaxSearchDepth;
     extern int Book;
     extern struct TimeControlRec TimeControl;
     extern int TCadd;
     extern short TCflag, TCmoves, TCminutes, TCseconds, OperatorTime;
     extern int timecomp[MINGAMEIN], timeopp[MINGAMEIN];
     extern int compptr,oppptr;
     extern short XCmore, XCmoves[], XCminutes[], XCseconds[], XC;
     extern const short otherside[];
     extern const small_short Stboard[];
     extern const small_short Stcolor[];
     extern unsigned short hint;
     extern short int TOflag;
     extern short stage, stage2; 
     extern small_short ChkFlag[], CptrFlag[];
     extern short Pscore[], Tscore[];
     extern /*unsigned*/ short rehash;  /* -1 is used as a flag --tpm */
     extern unsigned int ttblsize;
     extern short mtl[], pmtl[], hung[], emtl[];
     extern small_short Pindex[];
     extern small_short PieceCnt[];
     extern short FROMsquare, TOsquare;
     extern small_short HasPiece[2][NO_PIECES];
     extern const short kingP[];
     extern const small_short sweep[];
     extern unsigned short killr0[], killr1[];
     extern unsigned short killr2[], killr3[];
     extern unsigned short PV, SwagHt, Swag0, Swag1, Swag2, Swag3, Swag4, sidebit;
     extern short mtl[2], pmtl[2], hung[2];
     extern const short value[];
     extern const small_short relative_value[];
#define CatchedValue(side,piece)\
  (10*(((piece==pawn)?3:2)-Captured[side][piece])+value[piece]) 
     extern const long control[];
     extern small_short diagonal(short delta);
     extern const small_short promoted[NO_PIECES],unpromoted[NO_PIECES];
     extern const small_short is_promoted[NO_PIECES];

typedef unsigned char next_array[NO_SQUARES][NO_SQUARES];
typedef small_short   distdata_array[NO_SQUARES][NO_SQUARES];

     extern next_array *nextpos[NO_PTYPE_PIECES];
     extern next_array *nextdir[NO_PTYPE_PIECES];

     extern const small_short ptype[2][NO_PIECES];

     extern long filesz,hashmask,hashbase;
     extern FILE *hashfile;
     extern unsigned int starttime;

#ifdef SAVE_DISTDATA
#define distance(a,b) \
	((abs(column (a) - column (b)) > abs (row (a) - row (b)))\
	? abs(column (a) - column (b)) : abs (row (a) - row (b)))
#else
     extern small_short distdata[NO_SQUARES][NO_SQUARES]; 
#define distance(a,b) (int)distdata[(int)a][(int)b]
#endif
     extern short ptype_distance (short ptyp, short f, short t);

#define UNKNOWN 'U'
#define STATIC_ROOK 'S'
#define RANGING_ROOK 'R'

     extern char GameType[2];
   
#define CANNOT_REACH -1

#ifdef SAVE_PTYPE_DISTDATA
#define piece_distance(side,piece,f,t) \
		(short)ptype_distance(ptype[side][piece],f,t)
#else
     extern distdata_array *ptype_distdata[NO_PTYPE_PIECES];
#define piece_distance(side,piece,f,t) \
		(short)(*ptype_distdata[ptype[side][piece]])[f][t]
#endif                      

     extern union U admin;
     extern short unsigned bookmaxply;
     extern int unsigned bookcount;
     extern int unsigned booksize;
     extern unsigned long hashkey, hashbd;
     extern struct hashval hashcode[2][NO_PIECES][NO_SQUARES+(2*NO_PIECES)];
     extern char *CP[];
#ifdef QUIETBACKGROUND
     extern short background;
#endif /* QUIETBACKGROUND */

#if ttblsz
     extern struct hashentry huge *ttable[2];
#endif

/*
 * hashbd contains a 32 bit "signature" of the board position. hashkey
 * contains a 16 bit code used to address the hash table. When a move is
 * made, XOR'ing the hashcode of moved piece on the from and to squares with
 * the hashbd and hashkey values keeps things current.
 */
#define UpdateHashbd(side, piece, f, t) \
{\
  if ((f) >= 0)\
    {\
      hashbd ^= hashcode[side][piece][f].bd;\
      hashkey ^= hashcode[side][piece][f].key;\
    }\
  if ((t) >= 0)\
    {\
      hashbd ^= hashcode[side][piece][t].bd;\
      hashkey ^= hashcode[side][piece][t].key;\
    }\
}




     extern short rpthash[2][256];
     extern char *DRAW;


#define row(a) ((a) / 9)
#define column(a) ((a) % 9)

#define locn(a,b) ((a*9)+b)

/* init external functions */
     extern void InitConst (char *lang);
     extern void Initialize_dist (void);
     extern void NewGame (void);
     extern int parse (FILE * fd, short unsigned int *mv, short int side, char *opening);
     extern void GetOpenings (void);
     extern int OpeningBook (unsigned short int *hint, short int side);
     extern void SelectMove (short int side, short int iop);
     extern int
      search (short int side,
	       short int ply,
	       short int depth,
	       short int alpha,
	       short int beta,
	       short unsigned int *bstline,
	       short int *rpt);
#ifdef CACHE
	void
	  PutInEETable (short int side,int score);
	int
	  CheckEETable (short int side);
	int
	  ProbeEETable (short int side, short int *score);
#endif
#if ttblsz
     extern int
      ProbeTTable (short int side,
		    short int depth,
		    short int ply,
		    short int *alpha,
		    short int *beta,
		    short int *score);
     extern int
      PutInTTable (short int side,
		    short int score,
		    short int depth,
		    short int ply,
		    short int alpha,
		    short int beta,
		    short unsigned int mv);
     extern void ZeroTTable (void);
     extern void ZeroRPT (void);
     extern void Initialize_ttable (void);
     extern unsigned int urand (void);
#ifdef HASHFILE
     extern void gsrand (unsigned int);
     extern int
      ProbeFTable (short int side,
		    short int depth,
		    short int ply,
		    short int *alpha,
		    short int *beta,
		    short int *score);
     extern void
      PutInFTable (short int side,
		    short int score,
		    short int depth,
		    short int ply,
		    short int alpha,
		    short int beta,
		    short unsigned int f,
		    short unsigned int t);

#endif /* HASHFILE */
#endif /* ttblsz */
     extern void Initialize_moves (void);
     extern void MoveList (short int side, short int ply);
     extern void CaptureList (short int side, short int ply);

     extern void ataks (short int side, long int *a);

extern void
      MakeMove (short int side,
		 struct leaf * node,
		 short int *tempb,
		 short int *tempc,
		 short int *tempsf,
		 short int *tempst,
		 short int *INCscore);
     extern void
      UnmakeMove (short int side,
		   struct leaf * node,
		   short int *tempb,
		   short int *tempc,
		   short int *tempsf,
		   short int *tempst);
     extern void InitializeStats (void);
     extern int
      evaluate (short int side,
		 short int ply,
		 short int alpha,
		 short int beta,
		 short int INCscore,
		 short int *InChk);
     extern short int ScorePosition (short int side);
     extern void ExaminePosition (void);
     extern void UpdateWeights (void);
     extern void Initialize (void);
     extern void InputCommand (void);
     extern void ExitChess (void);
     extern void ClrScreen (void);
     extern void SetTimeControl (void);
     extern void SelectLevel (char *sx);
     extern void
      UpdateDisplay (short int f,
		      short int t,
		      short int flag,
		      short int iscastle);
     extern void ElapsedTime (short int iop);
     extern void ShowSidetoMove (void);
     extern void SearchStartStuff (short int side);
     extern void ShowDepth (char ch);
     extern void TerminateSearch (int);
     extern void
      ShowResults (short int score,
		    short unsigned int *bstline,
		    char ch);
     extern void PromptForMove (void);
     extern void SetupBoard (void);
     extern void algbr (short int f, short int t, short int flag);
     extern void OutputMove (void);
     extern void ShowCurrentMove (short int pnt, short int f, short int t);
     extern void ListGame (void);
     extern void ShowMessage (char *s);
     extern void ClrScreen (void);
     extern void gotoXY (short int x, short int y);
     extern void ClrEoln (void);
     extern void DrawPiece (short int sq);
     extern void UpdateClocks (void);
     extern void DoDebug (void);
     extern void DoTable (short table[NO_SQUARES]);
     extern void ShowPostnValues (void);
     extern void ChangeXwindow (void);
     extern void SetContempt (void);
     extern void ChangeHashDepth (void);
     extern void ChangeBetaWindow (void);
     extern void ChangeAlphaWindow (void);
     extern void GiveHint (void);
     extern void ShowPrompt (void);
     extern void EditBoard (void);
     extern void help (void);
     extern void ChangeSearchDepth (void);
     extern void skip (void);
     extern void skipb (void);
     extern void EnPassant (short int xside, short int f, short int t, short int iop);
     extern void ShowNodeCnt (long int NodeCnt);
     extern void ShowLine (short unsigned int *bstline);
     extern int pick (short int p1, short int p2);
     extern int VerifyMove (char *s, short int inp, unsigned short *mv);
     extern void AgeTT();
     extern unsigned short TTage;
#ifdef GDX
     extern struct gdxadmin B;
#endif
