/*
 * gnushogi.h - Header file for GNU SHOGI
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

/* OPT */
/* #define XSHOGI */
/* #define BAREBONES */
#define SMALL_MEMORY
#define SLOW_CPU
/* #define SAVE_SSCORE */
#if !defined EXTRA_2MB
#define SAVE_PTYPE_DISTDATA
/* #define SAVE_DISTDATA */
#define SAVE_NEXTPOS
#endif
#define HARDTIMELIMIT
#define DEEPSEARCHCUT
#define NULLMOVE
#define VERYBUGGY
#define QUIETBACKGROUND
/* #define HASGETTIMEOFDAY */
/* #define clocktime() (100l * clock() / CLOCKS_PER_SEC) */
#define clocktime() (100l * time(0))
#if defined NOTABLES
#define NOTTABLE
#else
#if !defined DEBUG_TTABLE
#define HISTORY
/* #define EXACTHISTORY */
#define CACHE
#endif
/* #define NOTTABLE */
#endif
#define SEMIQUIETBOOKGEN
/* GENOPT */
#define CHECKBONUS
/* #define DROPBONUS */
/* #define FIELDBONUS */
/* #define TESUJIBONUS */
#define EASY_OPENINGS
/* FILES */
#define LANGFILE "gnushogi.lng"
#define BOOK "gnushogi.tbk"
#define BINBOOK "gnushogi.bbk"
/* #define HASHFILE "gnushogi.hsh" */
#define PATTERNFILE "gnushogi.pat"
/* #define DEBUG */    

#define NOBYTEOPS
#include <string.h>

#define small_short char
#define small_ushort unsigned char

#elif defined(MSDOS) && !defined(__GO32__)

/* OPT */
/* #define XSHOGI */
/* #define BAREBONES */
#define SMALL_MEMORY
#define SLOW_CPU
/* #define SAVE_SSCORE */
#define SAVE_PTYPE_DISTDATA
/* #define SAVE_DISTDATA */
#define SAVE_NEXTPOS
#define HARDTIMELIMIT
#define DEEPSEARCHCUT
#define NULLMOVE
#define VERYBUGGY
/* #define QUIETBACKGROUND */
/* #define HASGETTIMEOFDAY */
#define clocktime() (100l * time(0))
#define HISTORY
/* #define EXACTHISTORY */
#define CACHE
#define SEMIQUIETBOOKGEN
/* #define NOTTABLE */
/* GENOPT */
#define CHECKBONUS
/* #define DROPBONUS */
/* #define FIELDBONUS */
/* #define TESUJIBONUS */
#define EASY_OPENINGS
/* FILES */
#define LANGFILE "gnushogi.lng"
#define BOOK "gnushogi.tbk"
#define BINBOOK "gnushogi.bbk"
/* #define HASHFILE "gnushogi.hsh" */
#define PATTERNFILE "gnushogi.pat"
/* #define DEBUG */

#define NOBYTEOPS
/* #define NOMEMOPS */
/* #include <mem.h> */

#define small_short char
#define small_ushort unsigned char

#else

#ifdef __STDC__
#define small_short signed char
#else
/*
 * type small_short must cover -128..127. In case of trouble,
 * try to uncommend "signed". If this doesn't help, use short. 
 */
#ifdef CHAR_IS_SIGNED
#define small_short char
#else
#define small_short short
#endif
#endif

#define small_ushort unsigned char

#endif


typedef small_short BYTE;      
typedef small_ushort UBYTE;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int INT;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;


#if defined(MSDOS) && !defined(__GO32__)
#define HEAP_ALLOC(n) _fmalloc(n)        
#define HEAP_FREE(p) _ffree(p)
#else        
#define far
#define HEAP_ALLOC(n) malloc(n)        
#define HEAP_FREE(p) free(p)
#endif  




#if defined NOBYTEOPS && defined NOMEMOPS
#define array_copy(src,dst,len) \
  { long i; char far *psrc=(char far *)src, *pdst=(char far *)dst;\
    for (i=len; i; pdst[--i] = psrc[i]);\
  }
#define array_zero(dst,len) \
  { long i; char far *pdst=(char far *)dst;\
    for (i=len; i; pdst[--i] = 0);\
  }
#elif defined NOBYTEOPS
#ifdef MSDOS
#define array_copy(src,dst,len) _fmemcpy(dst,src,len)
#define array_zero(dst,len) _fmemset(dst,0,len)
#else
#define array_copy(src,dst,len) memcpy(dst,src,len)
#define array_zero(dst,len) memset(dst,0,len)
#endif
#else
#define array_copy(src,dst,len) bcopy(src,dst,len)
#define array_zero(dst,len) bzero(dst,len)
#endif


#if !defined(__STDC__) && !defined(MSDOS)
#define const
#endif

#ifndef __GNUC__
#define inline
#endif

#include <stdio.h>

#ifdef BINBOOK
extern char *binbookfile;
#endif
extern char *bookfile;
extern short int ahead;
extern char far *xwin;
extern char far *Lang;



#define SEEK_SET 0
#define SEEK_END 2
#if defined DEBUG || defined DEBUG_EVAL
     extern void ShowDBLine (char *, short int, short int,
	    		short int, short int, short int,
	    		short unsigned int *);
     extern FILE *debugfd;
     extern short int debuglevel;
       
     extern void debug_position (FILE *D);
     extern void debug_ataks (FILE *D, long *atk);
#endif /* DEBUG */

#include <ctype.h>
#if !defined NO_STRING_INCLUDE
#include <string.h>     
#endif
#include <stdlib.h>

#ifdef THINK_C
#define RWA_ACC "r+b"
#define WA_ACC "w+b"
#elif defined(MSDOS) && !defined(__GO32__)
#include <time.h>
#include <malloc.h>
#define malloc(size) farmalloc(size)
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
#if defined NONDSP || defined THINK_C || defined MSDOS
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
#define MAX_CAPTURED 19
#define NO_PTYPE_PIECES 15
#define NO_SQUARES 81
#define NO_SQUARES_1 80
#define NO_COLS 9
#define NO_ROWS 9

#if defined DEBUG || defined HASHFILE || defined CACHE
#define PTBLBDSIZE (NO_SQUARES+NO_PIECES)
#endif

#include "eval.h"

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
#define Anyatak(c, u) (atak[c][u] != 0)

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


/* board properties */
#define InBlackCamp(sq) ((sq) < 27)
#define InWhiteCamp(sq) ((sq) > 53)
#define InPromotionZone(side,sq) (((side)==black)?InWhiteCamp(sq):InBlackCamp(sq))

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

/* move quality flags */
#define goodmove     tesuji
#define badmove      stupid
#ifdef EASY_OPENINGS
#define difficult    questionable
#endif

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
#elif defined SMALL_MEMORY
#if defined SAVE_SSCORE
#define vttblsz (1 << 12)
#else
#if defined EXTRA_2MB
#define vttblsz (1 << 12)
#else
#define vttblsz (1 << 10)
#endif
#endif
#else
#ifdef DEBUG
#define vttblsz (8001)
#else /* !DEBUG */
#define vttblsz (100001)
#endif
#endif

#if defined SMALL_MEMORY
#define MINTTABLE (0)
#else
#ifdef DEBUG
#define MINTTABLE (2000)	/* min ttable size -1 */
#else
#define MINTTABLE (8000)	/* min ttable size -1 */
#endif
#endif

#define ttblsz vttblsz

#if defined SMALL_MEMORY
#if !defined SAVE_SSCORE
#define TREE 1500               /* max number of tree entries */
#else
#define TREE 2500               /* max number of tree entries */
#endif
#else
#define TREE 4000               /* max number of tree entries */
#endif

#define MAXDEPTH 40             /* max depth a search can be carried */
#define MINDEPTH 2              /* min search depth =1 (no hint), >1 hint */
#define MAXMOVES 300            /* max number of half moves in a game */
#define CPSIZE 235              /* size of lang file max */
#if defined THINK_C || defined(MSDOS) && !defined(__GO32__) || defined SMALL_MEMORY
#if defined SAVE_SSCORE
#define ETABLE (1<<10)		/* static eval cache */
#else
#if defined EXTRA_2MB
#define ETABLE (1<<10)		/* static eval cache */
#else
#define ETABLE (1<<8)		/* static eval cache */
#endif
#endif
#else
#ifdef DEBUG
#define ETABLE (1001)
#else
#define ETABLE (10001)          /* static eval cache */
#endif
#endif
/***************** tuning paramaters **********************************************/
#if defined VERY_SLOW_CPU
#define MINRESPONSETIME 300	
#elif defined SLOW_CPU
#define MINRESPONSETIME 200	
#else
#define MINRESPONSETIME 100		/* 1 s */
#endif
#define MINGAMEIN 4
#define MINMOVES 15
#define CHKDEPTH 1              /* always look forward CHKDEPTH half-moves if in check */
#if defined SLOW_CPU || defined VERY_SLOW_CPU
#define DEPTHBEYOND 7           /* Max to go beyond Sdepth */
#else
#define DEPTHBEYOND 11          /* Max to go beyond Sdepth */
#endif
#define HASHDEPTH 4             /* depth above which to use HashFile */
#define HASHMOVELIMIT 40        /* Use HashFile only for this many moves */
#define PTVALUE 0               /* material value below which pawn threats at 5 & 3 are used */
#define ZDEPTH 3                /* depth beyond which to check ZDELTA for extra time */
#define ZDELTA 10               /* score delta per ply to cause extra time to be given */
#define BESTDELTA 90
/* about 1/2 second worth of nodes for your machine */
#if defined VERY_SLOW_CPU
#define ZNODES (flag.tsume ? 20 : 50)    /* check the time every ZNODES positions */
#elif defined SLOW_CPU
#define ZNODES (flag.tsume ? 40 : 100)   /* check the time every ZNODES positions */
#else
#define ZNODES (flag.tsume ? 400 : 1000) /* check the time every ZNODES positions */
#endif
#define MAXTCCOUNTX  10         /* max number of time clicks per search to complete ply */
#define MAXTCCOUNTR 4           /* max number of time clicks per search extensions*/
#define SCORESPLIM 8            /* Score space doesn't apply after this stage */
#define SDEPTHLIM (Sdepth+1)
#define HISTORYLIM 4096         /* Max value of history killer */
#ifdef EXACTHISTORY
#if defined SMALL_MEMORY
#define HISTORY_MASK 0x8000     /* mask to MSB of history index */
#define HISTORY_SIZE 0x10000    /* size of history table */
#else
#define HISTORY_MASK (1 << 15)  /* mask to MSB of history index */
#define HISTORY_SIZE (1 << 16)  /* size of history table */
#endif
#else
/* smaller history table, but dangerous because of collisions */
#define HISTORY_MASK 0x3fff     /* mask to significant bits of history index */
#if defined SMALL_MEMORY
#define HISTORY_SIZE 0x4000	/* size of history table */
#else
#define HISTORY_SIZE (1 << 14)  /* size of history table */
#endif
#endif
					       
#define sizeof_history (sizeof(unsigned short) * (size_t)HISTORY_SIZE)

#ifdef EXACTHISTORY
/* Map from.to (8bit.8bit) to from.to (0.7bit.8bit) */
#define khmove(mv) (mv & 0x7fff)
#define hmove(mv) ((mv & 0x7fff) ^ 0x5555)
#else
/* Map from.to (8bit.8bit) to from.to (00.7bit.7bit) */
/* Swap bits of ToSquare in case of promotions, hoping that
   no catastrophic collision occur. */
#define khmove(mv) (((mv & 0x7f00) >> 1) | \
		   ((mv & 0x0080) ? ((mv & 0x007f) ^ 0x007f) : (mv & 0x007f)))
#define hmove(mv) (khmove(mv) ^ 0x2aaa)
#endif

/* mask color to 15th bit */
#ifdef EXACTHISTORY
#define hindex(c,mv) ((c ? HISTORY_MASK : 0) | hmove(mv))
#else 
/* for white, swap bits, hoping that no catastrophic collision occur. */
#define hindex(c,mv) (c ? ((~hmove(mv)) & HISTORY_MASK) : hmove(mv))
#endif

#define EWNDW 10                /* Eval window to force position scoring at depth greater than Sdepth + 2 */
#define WAWNDW 90               /* alpha window when computer black*/
#define WBWNDW 90               /* beta window when computer black*/
#define BAWNDW 90               /* alpha window when computer white*/
#define BBWNDW 90               /* beta window when computer white*/
#define BXWNDW 90               /* window to force position scoring at lower */
#define WXWNDW 90               /* window to force position scoring at lower */

#define DITHER 5                /* max amount random can alter a pos value */
#define LBONUS 1                /* points per stage value of L increases */
#define BBONUS 2                /* points per stage value of B increases */
#define RBONUS 2                /* points per stage value of R increases */
							   
#define QUESTIONABLE (valueK)   /* malus for questionable moves */

#if defined STUPID
#undef STUPID
#endif

#define STUPID (valueR<<1)      /* malus for stupid moves */

#define KINGPOSLIMIT ( -1)      /* King positional scoring limit */
#define KINGSAFETY  32
#define MAXrehash (7)

/************************* parameters for Opening Book *********************************/
#define BOOKSIZE 8000          /* Number of unique position/move combinations allowed */
#define BOOKMAXPLY 40           /* Max plys to keep in book database */
#define BOOKFAIL (BOOKMAXPLY/2) /* if no book move found for BOOKFAIL turns stop using book */
#define BOOKPOCKET 64
#define BOOKRAND 1000           /* used to select an opening move from a list */
#define BOOKENDPCT 950          /* 5 % chance a BOOKEND will stop the book */
#define DONTUSE -32760          /* flag move as don't use */
#define ILLEGAL_TRAPPED -32761  /* flag move as illegal: no move from this square */
#define ILLEGAL_DOUBLED -32762  /* flag move as illegal: two pawns on one column */
#define ILLEGAL_MATE -32763     /* flag move as illegal: pawn drop with mate */
/****************************************************************************************/
     struct hashval
     {
       unsigned long key, bd;
     };
     struct hashentry
     {
       unsigned long hashbd;
       unsigned short mv;
       unsigned char depth; /* unsigned char saves some space */
       unsigned char flags;
#ifdef notdef
       unsigned short age;
#endif
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
#if !defined SAVE_SSCORE
	short int sscore[NO_SQUARES];
#endif
	short int score;
	small_short hung[2];
#ifdef CACHETEST
	unsigned char bd[PTBLBDSIZE];
#endif /* CACHETEST */ 
     } ;

#if defined CACHE
extern short use_etable;
typedef struct etable etable_field[ETABLE];
extern etable_field far *etab[2];
#endif

/*
 * persistent transposition table. By default, the size is (1 << vfilesz). If you
 * change the size, be sure to run gnuchess -c [vfilesz] before anything else.
 */
#define frehash 6
#if defined SMALL_MEMORY
#define vfilesz 10
#else
#define vfilesz 14
#endif
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
       short INCscore;
       unsigned short flags;
     };
     struct GameRec
     {
       unsigned short gmove;    /* this move */
       short score;             /* score after this move */
       short depth;             /* search depth this move */
       long time;               /* search time this move */
       short fpiece;            /* moved or dropped piece */
       short piece;             /* piece captured */
       short color;             /* color */
       short flags;             /* move flags capture, promote, castle */
       short Game50;            /* flag for repetition */
       long nodes;              /* nodes searched for this move */
       unsigned long hashkey, hashbd;   /* board key before this move */
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
       short mate;              /* the game is over */
       short post;              /* show principle variation */
       short quit;              /* quit/exit */
       short regularstart;      /* did the game start from standard
				 * initial board ? */
       short reverse;           /* reverse board display */
       short bothsides;         /* computer plays both sides */
       short hash;              /* enable/disable transposition table */
       short force;             /* enter moves */
       short easy;              /* disable thinking on opponents time */
       short beep;              /* enable/disable beep */
       short timeout;           /* time to make a move */
       short musttimeout;       /* time to make a move */
       short back;              /* time to make a move */
       short rcptr;             /* enable/disable recapture heuristics */
       short rv;                /* reverse video */
       short stars;             /* add stars to uxdsp screen */
       short coords;            /* add coords to visual screen */
       short shade;
       short material;          /* draw on lack of material */
       short illegal;           /* illegal position */
       short onemove;           /* timing is onemove */
       short gamein;            /* timing is gamein */
       short tsume;             /* first consider checks */
     };

     extern FILE *debugfile;

#ifndef EVALFILE
#ifdef THINK_C
#define EVALFILE "EVAL"
#else
#define EVALFILE "/tmp/EVAL"
#endif
#endif

extern FILE *debug_eval_file;
extern short debug_eval;
extern short debug_moves;


#ifdef HISTORY
     extern short use_history;
     extern unsigned short far *history;
#endif
     extern long znodes;

     extern char ColorStr[2][10];
#ifdef DEBUG_EVAL
     extern char *PieceStr[NO_PIECES];
#endif
     extern char mvstr[4][6]; 
     extern unsigned short int MV[MAXDEPTH];
     extern int MSCORE;
     extern int mycnt1, mycnt2;
     extern short int ahead;
     extern short int xshogi;
     extern struct leaf rootnode;
     extern struct leaf far *Tree;
     extern struct leaf far *root;
     extern char savefile[], listfile[];
     extern short TrPnt[];
     extern small_short board[], color[]; 
     extern const small_short sweep[NO_PIECES]; 
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

#if !defined SAVE_SVALUE
     extern short svalue[NO_SQUARES];
#endif
     extern short pscore[2]; /* eval.c */
     extern int EADD; /* eval.c */
     extern int EGET; /* eval.c */
     extern struct flags flag;
     extern short opponent, computer, INCscore;
     extern short WAwindow, BAwindow, WBwindow, BBwindow;
     extern short dither, player;
     extern short xwndw, contempt;
     extern long ResponseTime, ExtraTime, TCleft, MaxResponseTime, et, et0, time0, ft;
     extern int TCcount;
#ifdef INTERRUPT_TEST
     extern long itime0, it;
#endif
     extern long reminus, replus;
     extern long GenCnt, NodeCnt, ETnodes, EvalNodes, HashAdd, HashCnt, HashCol, THashCol,
      FHashCnt, FHashAdd;
     extern short HashDepth, HashMoveLimit;
     extern struct GameRec far *GameList;
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

#define in_opening_stage (!flag.tsume && (stage < 33))
#define in_middlegame_stage (!flag.tsume && (stage >= 33) && (stage <= 66))
#define in_endgame_stage (flag.tsume || (stage > 66))

     extern short int ahead, hash;
     extern short balance[2];
     extern small_short ChkFlag[], CptrFlag[], TesujiFlag[];
     extern short Pscore[], Tscore[];
     extern /*unsigned*/ short rehash;  /* -1 is used as a flag --tpm */
     extern char version[], patchlevel[];
     extern unsigned int ttbllimit;
     extern unsigned int TTadd;
     extern unsigned int ttblsize;
     extern short mtl[], hung[];
     extern small_short Pindex[];
     extern small_short PieceCnt[];
     extern short FROMsquare, TOsquare;
     extern small_short HasPiece[2][NO_PIECES];
     extern const short kingP[];
     extern unsigned short killr0[], killr1[];
     extern unsigned short killr2[], killr3[];
     extern unsigned short int PrVar[MAXDEPTH];
     extern unsigned short PV, SwagHt, Swag0, Swag1, Swag2, Swag3, Swag4, sidebit;
     extern short mtl[2], pmtl[2], hung[2];
     extern const small_short relative_value[];
     extern const long control[];
     extern small_short diagonal(short delta);
     extern const small_short promoted[NO_PIECES],unpromoted[NO_PIECES];
     extern const small_short is_promoted[NO_PIECES];

typedef unsigned char next_array[NO_SQUARES][NO_SQUARES];
typedef small_short distdata_array[NO_SQUARES][NO_SQUARES];

     extern const small_short inunmap[NO_SQUARES];
     extern const small_short nunmap[(NO_COLS+2)*(NO_ROWS+4)];
#if defined SAVE_NEXTPOS
     extern const small_short direc[NO_PTYPE_PIECES][8];
     extern short first_direction(short ptyp, short *d, short sq);
     extern short next_direction(short ptyp, short *d, short sq);
     extern short next_position(short ptyp, short *d, short sq, short u);
#else
     extern short use_nextpos;
     extern next_array far *nextpos[NO_PTYPE_PIECES];
     extern next_array far *nextdir[NO_PTYPE_PIECES];
#endif

     extern value_array far *value;
     extern fscore_array far *fscore;

#ifndef SAVE_DISTDATA
     extern short use_distdata;
     extern distdata_array far *distdata;
#endif

#ifndef SAVE_PTYPE_DISTDATA
     extern short use_ptype_distdata;
     extern distdata_array far *ptype_distdata[NO_PTYPE_PIECES];
#endif

     extern const small_short ptype[2][NO_PIECES];

     extern long filesz,hashmask,hashbase;
     extern FILE *hashfile;
     extern unsigned int starttime;

     /* eval.c */
typedef small_short Mpiece_array[2][NO_SQUARES];
     extern Mpiece_array *Mpiece[NO_PIECES];
     extern short ADVNCM[NO_PIECES];

#define computed_distance(a,b) \
	((abs(column (a) - column (b)) > abs (row (a) - row (b)))\
	? abs(column (a) - column (b)) : abs (row (a) - row (b)))

     extern short distance (short a, short b);
     extern short ptype_distance (short ptyp, short f, short t);
     extern short piece_distance(short side,short piece,short f,short t);

#if defined UNKNOWN
#undef UNKNOWN
#endif

#define UNKNOWN 'U'
#define STATIC_ROOK 'S'
#define RANGING_ROOK 'R'

     extern char GameType[2];
     void ShowGameType(void);
   
     extern short unsigned bookmaxply;
     extern int unsigned bookcount;
     extern int unsigned booksize;
     extern unsigned long hashkey, hashbd;

typedef struct hashval hashcode_array[2][NO_PIECES][NO_SQUARES];
typedef struct hashval drop_hashcode_array[2][NO_PIECES][NO_SQUARES];

     extern hashcode_array far *hashcode;
     extern drop_hashcode_array far *drop_hashcode;
     extern char far *CP[];
#ifdef QUIETBACKGROUND
     extern short background;
#endif /* QUIETBACKGROUND */

#if ttblsz
     extern short use_ttable;
     extern struct hashentry far *ttable[2];
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
      hashbd ^= (*hashcode)[side][piece][f].bd;\
      hashkey ^= (*hashcode)[side][piece][f].key;\
    }\
  if ((t) >= 0)\
    {\
      hashbd ^= (*hashcode)[side][piece][t].bd;\
      hashkey ^= (*hashcode)[side][piece][t].key;\
    }\
}

#define UpdateDropHashbd(side, piece, count) \
{\
  hashbd ^= (*drop_hashcode)[side][piece][count].bd;\
  hashkey ^= (*drop_hashcode)[side][piece][count].key;\
}




     extern short rpthash[2][256];
     extern char *DRAW;


#define row(a) ((a) / 9)
#define column(a) ((a) % 9)

#define locn(a,b) (((a)*9)+b)

/* init external functions */
     extern void InitConst (char *lang); /* init.c */
     extern int Initialize_data (void); /* init.c */
     extern void Free_data (void); /* init.c */
     extern int Lock_data (void); /* init.c */
     extern void Unlock_data (void); /* init.c */
     extern void Initialize_dist (void); /* init.c */
     extern void Initialize_eval (void); /* eval.c */
     extern void NewGame (void);
     extern int parse (FILE * fd, short unsigned int *mv, short int side, char *opening);
     extern void GetOpenings (void);
     extern int OpeningBook (unsigned short int *hint, short int side);
     
typedef enum { REMOVE_PIECE = 1, ADD_PIECE } UpdatePieceList_mode;

     extern void UpdatePieceList (short int side, short int sq, UpdatePieceList_mode iop);

typedef enum { FOREGROUND_MODE = 1, BACKGROUND_MODE } SelectMove_mode;

     extern void SelectMove (short int side, SelectMove_mode iop);
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
#if !defined SAVE_NEXTPOS
     extern void Initialize_moves (void);
#endif

     extern short generate_move_flags;

     extern void MoveList (short int side, short int ply, 
			   short int in_check, short int blockable);
     extern void CaptureList (short int side, short int ply, 
			      short int in_check, short int blockable);

     /* from ataks.c */
     extern int SqAtakd (short int square, short int side, short int *blockable);
     
extern void
      MakeMove (short int side,
		 struct leaf far *node,
		 short int *tempb,
		 short int *tempc,
		 short int *tempsf,
		 short int *tempst,
		 short int *INCscore);
     extern void
      UnmakeMove (short int side,
		   struct leaf far *node,
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
		 short int *InChk,
		 short int *blockable);
     extern short int ScorePosition (short int side);
     extern void ExaminePosition (short side);
     extern short ScorePatternDistance(short side);
     extern void DetermineStage (short side);
     extern void UpdateWeights (short side);
     extern int  InitMain (void);
     extern void ExitMain (void);
     extern void Initialize (void);
     extern void InputCommand (char *command);
     extern void ExitChess (void);
     extern void ClrScreen (void);
     extern void SetTimeControl (void);
     extern void SelectLevel (char *sx);
     extern void
      UpdateDisplay (short int f,
		      short int t,
		      short int flag,
		      short int iscastle);

typedef enum { COMPUTE_AND_INIT_MODE = 1, COMPUTE_MODE
#ifdef INTERRUPT_TEST
		, INIT_INTERRUPT_MODE, COMPUTE_INTERRUPT_MODE
#endif
    } ElapsedTime_mode; 

     extern void ElapsedTime (ElapsedTime_mode iop);
     extern void SetResponseTime (short int side);
     extern void CheckForTimeout (int score, int globalscore, int Jscore, int zwndw);

     extern void ShowSidetoMove (void);
     extern void ShowResponseTime (void);
     extern void ShowPatternCount (short side, short n);
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
     extern short int repetition (void);
     extern void TimeCalc (void);
     extern short DropPossible (short int piece, short int side, short int sq); /* genmoves.c */
     extern short IsCheckmate (short int side, short int in_check, short int blockable); /* genmoves.c */


typedef enum { VERIFY_AND_MAKE_MODE, VERIFY_AND_TRY_MODE, UNMAKE_MODE } VerifyMove_mode;

     extern int VerifyMove (char *s, VerifyMove_mode iop, unsigned short *mv);
     extern void AgeTT();
     extern unsigned short TTage;

