
#define BOARD_SIZE		9
#define DROP_LINES		3
#define DROP_COLS		3
#ifdef WESTERN_BITMAPS
#define LARGE_SQUARE_SIZE	32
#define MEDIUM_SQUARE_SIZE	32
#else
#define LARGE_SQUARE_SIZE	64
#define MEDIUM_SQUARE_SIZE	49
#endif
#define SMALL_SQUARE_SIZE       32
#define LINE_GAP		2
#define MAX_MOVES		512
#define MSG_SIZ			256
#define DIALOG_SIZE		256
#define MOVE_
#define MOVE_LEN		16      /* enough for "Black resigns\000"*/
#define TIME_CONTROL		"5"	/* in minutes */
#define TIME_DELAY		"1.0"	/* seconds between moves */
#define MOVES_PER_SESSION	40	/* moves per TIME_CONTROL */
#define BlackOnMove(move)	((int) ((move) % 2) == 0)
#define ATTENTION
#ifdef ISS
/* #define ISS_HOST                "hellspark.wharton.upenn.edu" */
#define ISS_HOST                "130.91.160.217"
#define ISS_PORT	        6969
#define tstwd                   "si\177mon says "
#endif
#define BELLCHAR                '\007'
#define NULLCHAR                '\000'
#ifndef FIRST_CHESS_PROGRAM
#define FIRST_CHESS_PROGRAM	"gnushogix"
#endif
#ifndef SECOND_CHESS_PROGRAM
#define SECOND_CHESS_PROGRAM	"gnushogix"
#endif
#ifndef FIRST_HOST
#define FIRST_HOST		"localhost"
#endif
#ifndef SECOND_HOST
#define SECOND_HOST		"localhost"
#endif
#define MATCH_MODE		"False"
#define INIT_STRING		"beep\neasy\nrandom\n"
#define BLACK_STRING		"black\ngo\n"
#define WHITE_STRING		"white\ngo\n"
#define DEFAULT_SIZE            "Small"
#define BLACK_PIECE_COLOR	"#FFFFD7"
#define WHITE_PIECE_COLOR	"#FFFFD7"
#define LIGHT_SQUARE_COLOR	"#EBDFB0"
#define DARK_SQUARE_COLOR	"#EBDFB0"
#define MAIN_FONT    "-*-helvetica-medium-o-normal--*-*-*-*-*-*-*-*"
#define COORD_FONT   "-*-helvetica-bold-r-normal--*-*-*-*-*-*-*-*"
#define DEFAULT_FONT "*font: -*-helvetica-medium-r-normal--*-120-*-*-*-*-*-*"
#define BLINK_COUNT 3	/* number of machines to-square blinks */
#define BORDER_X_OFFSET         3
#define BORDER_Y_OFFSET         27



typedef enum {
	Large, Medium, Small
} BoardSize;

typedef enum {
	BeginningOfGame, MachinePlaysBlack, MachinePlaysWhite, TwoMachinesPlay,
	ForceMoves, PlayFromGameFile, PauseGame, EndOfGame,
	EditPosition
} GameMode;

typedef enum {
	MatchFalse, MatchInit, MatchPosition, MatchOpening
} MatchMode;

#ifdef ISS
          
typedef enum {
    IssIdle, IssPlayingWhite, IssPlayingBlack, IssObserving
  } IssMode;

#endif

typedef enum {
	BlackPawn, BlackLance, BlackKnight, BlackSilver, BlackGold, BlackBishop, BlackRook, 
        BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver, BlackPBishop, BlackPRook, BlackKing,
	WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, WhiteGold, WhiteBishop, WhiteRook, 
        WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver, WhitePBishop, WhitePRook, WhiteKing,
	EmptySquare,
	ClearBoard, BlackPlay, WhitePlay /*for use on EditPosition menus*/
} ChessSquare;

typedef ChessSquare Board[BOARD_SIZE][BOARD_SIZE];
                   
typedef int Catched[2][8];

typedef enum {
	BlackPromotion = 1, WhitePromotion, BlackDrop, WhiteDrop,
        NormalMove,
	BlackWins, WhiteWins, GameIsDrawn, StartGame, BadMove, Comment,
	AmbiguousMove
} ChessMove;

typedef enum {
	ResetTimers, DecrementTimers, SwitchTimers, 
	ReDisplayTimers, StopTimers, StartTimers
} ClockMode;

/*
 * Various compatibility grunge
 */
#ifdef __STDC__
#define	P(args)		args
#define VOID_PROC	(void(*)(char*))
#else
#define P(args)		()
#define VOID_PROC
#endif
