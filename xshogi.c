/*
 * XShogi
 * based on XBoard -- an Xt/Athena user interface for GNU Chess
 *
 * Original authors:  Dan Sears and Chris Sears.
 * Enhancements (Version 2.0 and following):  Tim Mann.
 * Modifications to XShogi (Version 1.0): Matthias Mutz 
 * Enhancements to XShogi (Version 1.1): Matthias Mutz 
 * Thanks to John Chanak for the//
 * Modified implementation of ISS mode for XShogi: Matthias Mutz.
 *                                                                 
 * XShogi borrows its piece bitmaps from CRANES Shogi.  
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.
 * Enhancements Copyright 1992 Free Software Foundation, Inc.
 * Enhancements for XShogi Copyright 1993, 1994, 1995 Matthias Mutz
 *
 * The following terms apply to Digital Equipment Corporation's copyright
 * interest in XBoard:
 * ------------------------------------------------------------------------
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ------------------------------------------------------------------------
 *
 * This file is part of XSHOGI.
 *
 * XSHOGI is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY.  No author or distributor accepts responsibility to anyone for
 * the consequences of using it or for whether it serves any particular
 * purpose or works at all, unless he says so in writing.  Refer to the XSHOGI
 * General Public License for full details.
 *
 * Everyone is granted permission to copy, modify and redistribute XSHOGI, but
 * only under the conditions described in the XSHOGI General Public License. A
 * copy of this license is supposed to have been given to you along with
 * XSHOGI so you can know your rights and responsibilities.  It should be in a
 * file named COPYING.  Among other things, the copyright notice and this
 * notice must be preserved on all copies.
 * ------------------------------------------------------------------------
 *
 * See the file CHANGES for a detailed revision history.
 */


#define XBOARD_VERSION "2.0/2.1"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/ioctl.h>
#ifdef ISS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <time.h>
#ifdef HAS_GETTIMEOFDAY
#ifndef ESIX
#include <sys/time.h>
#endif
#endif
#ifdef	__STDC__
#ifndef ESIX
#ifndef apollo
#include <stdlib.h>
#endif 
#endif
#endif
#if	SYSTEM_FIVE || SYSV
#include <sys/types.h>
#include <sys/stat.h>
#ifdef	AIXV3
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif AIXV3
#if	SVR4
#include <stropts.h>
#ifdef sun
#include <sys/systeminfo.h>
#endif
#endif
#endif
#if defined(__STDC__) || SYSTEM_FIVE || SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#include <pwd.h>
 
#ifdef IRIS
#include <sys/sysmacros.h>
#endif

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/cursorfont.h>

#include "version.h"
#include "xshogi.h"


#include "eastern_bitmaps/icon.xbm"

#include "eastern_bitmaps/bigsolid.xbm"
#include "eastern_bitmaps/smallsolid.xbm"
#include "eastern_bitmaps/bigsolidR.xbm"
#include "eastern_bitmaps/smallsolidR.xbm"

#if defined WESTERN_BITMAPS || defined BOTH_BITMAPS

#include "western_bitmaps/noneW.xbm"

#include "western_bitmaps/kingW.xbm"
#include "western_bitmaps/rookW.xbm"
#include "western_bitmaps/rookPW.xbm"
#include "western_bitmaps/bishopW.xbm"
#include "western_bitmaps/bishopPW.xbm"
#include "western_bitmaps/goldW.xbm"
#include "western_bitmaps/silverW.xbm"
#include "western_bitmaps/silverPW.xbm"
#include "western_bitmaps/knightW.xbm"
#include "western_bitmaps/knightPW.xbm"
#include "western_bitmaps/lanceW.xbm"
#include "western_bitmaps/lancePW.xbm"
#include "western_bitmaps/pawnW.xbm"
#include "western_bitmaps/pawnPW.xbm"

#include "western_bitmaps/kingRW.xbm"
#include "western_bitmaps/rookRW.xbm"
#include "western_bitmaps/rookPRW.xbm"
#include "western_bitmaps/bishopRW.xbm"
#include "western_bitmaps/bishopPRW.xbm"
#include "western_bitmaps/goldRW.xbm"
#include "western_bitmaps/silverRW.xbm"
#include "western_bitmaps/silverPRW.xbm"
#include "western_bitmaps/knightRW.xbm"
#include "western_bitmaps/knightPRW.xbm"
#include "western_bitmaps/lanceRW.xbm"
#include "western_bitmaps/lancePRW.xbm"
#include "western_bitmaps/pawnRW.xbm"
#include "western_bitmaps/pawnPRW.xbm"

#endif

#if !defined WESTERN_BITMAPS || defined BOTH_BITMAPS

#ifdef TOP_PART_BITMAPS

#include "eastern_bitmaps/none.xbm"

#include "eastern_bitmaps/king.xbm"
#include "eastern_bitmaps/rook.xbm"
#include "eastern_bitmaps/rookP.top.xbm"
#include "eastern_bitmaps/bishop.xbm"
#include "eastern_bitmaps/bishopP.top.xbm"
#include "eastern_bitmaps/gold.xbm"
#include "eastern_bitmaps/silver.xbm"
#include "eastern_bitmaps/silverP.xbm"
#include "eastern_bitmaps/knight.xbm"
#include "eastern_bitmaps/knightP.xbm"
#include "eastern_bitmaps/lance.xbm"
#include "eastern_bitmaps/lanceP.xbm"
#include "eastern_bitmaps/pawn.xbm"
#include "eastern_bitmaps/pawnP.xbm"

#include "eastern_bitmaps/kingR.xbm"
#include "eastern_bitmaps/rookR.xbm"
#include "eastern_bitmaps/rookPR.top.xbm"
#include "eastern_bitmaps/bishopR.xbm"
#include "eastern_bitmaps/bishopPR.top.xbm"
#include "eastern_bitmaps/goldR.xbm"
#include "eastern_bitmaps/silverR.xbm"
#include "eastern_bitmaps/silverPR.xbm"
#include "eastern_bitmaps/knightR.xbm"
#include "eastern_bitmaps/knightPR.xbm"
#include "eastern_bitmaps/lanceR.xbm"
#include "eastern_bitmaps/lancePR.xbm"
#include "eastern_bitmaps/pawnR.xbm"
#include "eastern_bitmaps/pawnPR.xbm"

#include "eastern_bitmaps/none_m.xbm"

#include "eastern_bitmaps/bigsolid_m.xbm"
#include "eastern_bitmaps/smallsolid_m.xbm"
#include "eastern_bitmaps/bigsolidR_m.xbm"
#include "eastern_bitmaps/smallsolidR_m.xbm"

#include "eastern_bitmaps/king_m.xbm"
#include "eastern_bitmaps/rook_m.xbm"
#include "eastern_bitmaps/rookP_m.top.xbm"
#include "eastern_bitmaps/bishop_m.xbm"
#include "eastern_bitmaps/bishopP_m.top.xbm"
#include "eastern_bitmaps/gold_m.xbm"
#include "eastern_bitmaps/silver_m.xbm"
#include "eastern_bitmaps/silverP_m.xbm"
#include "eastern_bitmaps/knight_m.xbm"
#include "eastern_bitmaps/knightP_m.xbm"
#include "eastern_bitmaps/lance_m.xbm"
#include "eastern_bitmaps/lanceP_m.xbm"
#include "eastern_bitmaps/pawn_m.xbm"
#include "eastern_bitmaps/pawnP_m.xbm"

#include "eastern_bitmaps/kingR_m.xbm"
#include "eastern_bitmaps/rookR_m.xbm"
#include "eastern_bitmaps/rookPR_m.top.xbm"
#include "eastern_bitmaps/bishopR_m.xbm"
#include "eastern_bitmaps/bishopPR_m.top.xbm"
#include "eastern_bitmaps/goldR_m.xbm"
#include "eastern_bitmaps/silverR_m.xbm"
#include "eastern_bitmaps/silverPR_m.xbm"
#include "eastern_bitmaps/knightR_m.xbm"
#include "eastern_bitmaps/knightPR_m.xbm"
#include "eastern_bitmaps/lanceR_m.xbm"
#include "eastern_bitmaps/lancePR_m.xbm"
#include "eastern_bitmaps/pawnR_m.xbm"
#include "eastern_bitmaps/pawnPR_m.xbm"

#include "eastern_bitmaps/none_l.xbm"

#include "eastern_bitmaps/bigsolid_l.xbm"
#include "eastern_bitmaps/smallsolid_l.xbm"
#include "eastern_bitmaps/bigsolidR_l.xbm"
#include "eastern_bitmaps/smallsolidR_l.xbm"

#include "eastern_bitmaps/king_l.xbm"
#include "eastern_bitmaps/rook_l.xbm"
#include "eastern_bitmaps/rookP_l.top.xbm"
#include "eastern_bitmaps/bishop_l.xbm"
#include "eastern_bitmaps/bishopP_l.top.xbm"
#include "eastern_bitmaps/gold_l.xbm"
#include "eastern_bitmaps/silver_l.xbm"
#include "eastern_bitmaps/silverP_l.xbm"
#include "eastern_bitmaps/knight_l.xbm"
#include "eastern_bitmaps/knightP_l.xbm"
#include "eastern_bitmaps/lance_l.xbm"
#include "eastern_bitmaps/lanceP_l.xbm"
#include "eastern_bitmaps/pawn_l.xbm"
#include "eastern_bitmaps/pawnP_l.xbm"

#include "eastern_bitmaps/kingR_l.xbm"
#include "eastern_bitmaps/rookR_l.xbm"
#include "eastern_bitmaps/rookPR_l.top.xbm"
#include "eastern_bitmaps/bishopR_l.xbm"
#include "eastern_bitmaps/bishopPR_l.top.xbm"
#include "eastern_bitmaps/goldR_l.xbm"
#include "eastern_bitmaps/silverR_l.xbm"
#include "eastern_bitmaps/silverPR_l.xbm"
#include "eastern_bitmaps/knightR_l.xbm"
#include "eastern_bitmaps/knightPR_l.xbm"
#include "eastern_bitmaps/lanceR_l.xbm"
#include "eastern_bitmaps/lancePR_l.xbm"
#include "eastern_bitmaps/pawnR_l.xbm"
#include "eastern_bitmaps/pawnPR_l.xbm"

#else

#include "eastern_bitmaps/none.xbm"

#include "eastern_bitmaps/king.xbm"
#include "eastern_bitmaps/rook.xbm"
#include "eastern_bitmaps/rookP.xbm"
#include "eastern_bitmaps/bishopP.xbm"
#include "eastern_bitmaps/bishop.xbm"
#include "eastern_bitmaps/gold.xbm"
#include "eastern_bitmaps/silver.xbm"
#include "eastern_bitmaps/silverP.xbm"
#include "eastern_bitmaps/knight.xbm"
#include "eastern_bitmaps/knightP.xbm"
#include "eastern_bitmaps/lance.xbm"
#include "eastern_bitmaps/lanceP.xbm"
#include "eastern_bitmaps/pawn.xbm"
#include "eastern_bitmaps/pawnP.xbm"

#include "eastern_bitmaps/kingR.xbm"
#include "eastern_bitmaps/rookR.xbm"
#include "eastern_bitmaps/rookPR.xbm"
#include "eastern_bitmaps/bishopPR.xbm"
#include "eastern_bitmaps/bishopR.xbm"
#include "eastern_bitmaps/goldR.xbm"
#include "eastern_bitmaps/silverR.xbm"
#include "eastern_bitmaps/silverPR.xbm"
#include "eastern_bitmaps/knightR.xbm"
#include "eastern_bitmaps/knightPR.xbm"
#include "eastern_bitmaps/lanceR.xbm"
#include "eastern_bitmaps/lancePR.xbm"
#include "eastern_bitmaps/pawnR.xbm"
#include "eastern_bitmaps/pawnPR.xbm"

#include "eastern_bitmaps/none_m.xbm"

#include "eastern_bitmaps/bigsolid_m.xbm"
#include "eastern_bitmaps/smallsolid_m.xbm"
#include "eastern_bitmaps/bigsolidR_m.xbm"
#include "eastern_bitmaps/smallsolidR_m.xbm"

#include "eastern_bitmaps/king_m.xbm"
#include "eastern_bitmaps/rook_m.xbm"
#include "eastern_bitmaps/rookP_m.xbm"
#include "eastern_bitmaps/bishop_m.xbm"
#include "eastern_bitmaps/bishopP_m.xbm"
#include "eastern_bitmaps/gold_m.xbm"
#include "eastern_bitmaps/silver_m.xbm"
#include "eastern_bitmaps/silverP_m.xbm"
#include "eastern_bitmaps/knight_m.xbm"
#include "eastern_bitmaps/knightP_m.xbm"
#include "eastern_bitmaps/lance_m.xbm"
#include "eastern_bitmaps/lanceP_m.xbm"
#include "eastern_bitmaps/pawn_m.xbm"
#include "eastern_bitmaps/pawnP_m.xbm"

#include "eastern_bitmaps/kingR_m.xbm"
#include "eastern_bitmaps/rookR_m.xbm"
#include "eastern_bitmaps/rookPR_m.xbm"
#include "eastern_bitmaps/bishopR_m.xbm"
#include "eastern_bitmaps/bishopPR_m.xbm"
#include "eastern_bitmaps/goldR_m.xbm"
#include "eastern_bitmaps/silverR_m.xbm"
#include "eastern_bitmaps/silverPR_m.xbm"
#include "eastern_bitmaps/knightR_m.xbm"
#include "eastern_bitmaps/knightPR_m.xbm"
#include "eastern_bitmaps/lanceR_m.xbm"
#include "eastern_bitmaps/lancePR_m.xbm"
#include "eastern_bitmaps/pawnR_m.xbm"
#include "eastern_bitmaps/pawnPR_m.xbm"

#include "eastern_bitmaps/none_l.xbm"

#include "eastern_bitmaps/bigsolid_l.xbm"
#include "eastern_bitmaps/smallsolid_l.xbm"
#include "eastern_bitmaps/bigsolidR_l.xbm"
#include "eastern_bitmaps/smallsolidR_l.xbm"

#include "eastern_bitmaps/king_l.xbm"
#include "eastern_bitmaps/rook_l.xbm"
#include "eastern_bitmaps/rookP_l.xbm"
#include "eastern_bitmaps/bishop_l.xbm"
#include "eastern_bitmaps/bishopP_l.xbm"
#include "eastern_bitmaps/gold_l.xbm"
#include "eastern_bitmaps/silver_l.xbm"
#include "eastern_bitmaps/silverP_l.xbm"
#include "eastern_bitmaps/knight_l.xbm"
#include "eastern_bitmaps/knightP_l.xbm"
#include "eastern_bitmaps/lance_l.xbm"
#include "eastern_bitmaps/lanceP_l.xbm"
#include "eastern_bitmaps/pawn_l.xbm"
#include "eastern_bitmaps/pawnP_l.xbm"

#include "eastern_bitmaps/kingR_l.xbm"
#include "eastern_bitmaps/rookR_l.xbm"
#include "eastern_bitmaps/rookPR_l.xbm"
#include "eastern_bitmaps/bishopR_l.xbm"
#include "eastern_bitmaps/bishopPR_l.xbm"
#include "eastern_bitmaps/goldR_l.xbm"
#include "eastern_bitmaps/silverR_l.xbm"
#include "eastern_bitmaps/silverPR_l.xbm"
#include "eastern_bitmaps/knightR_l.xbm"
#include "eastern_bitmaps/knightPR_l.xbm"
#include "eastern_bitmaps/lanceR_l.xbm"
#include "eastern_bitmaps/lancePR_l.xbm"
#include "eastern_bitmaps/pawnR_l.xbm"
#include "eastern_bitmaps/pawnPR_l.xbm"

#endif

#endif

#ifdef ISS
int establish P((char *host, int port));
void read_from_player P((caddr_t client_data, int *file_num, XtInputId *id));
void read_from_iss P((caddr_t client_data, int *file_num, XtInputId *id));
#endif

void main P((int argc, char **argv));
void CreateGCs P((void));
void CreatePieces P((void));
void CreatePieceMenus P((void));
char *FindFont P((char *pattern, int targetPxlSize));
void PieceMenuPopup P((Widget w, XEvent *event, String *params,
			Cardinal *num_params));
static void PieceMenuSelect P((Widget w, ChessSquare piece, caddr_t junk));
static void SetBlackToPlay P((void));
static void SetWhiteToPlay P((void));
void ReadBitmap P((String name, Pixmap *pm, Pixmap *qm,
	char small_bits[], char medium_bits[], char large_bits[]));
void CreateGrid P((void));
int EventToSquare P((int x));
int EventToXSquare P((int x));
ChessSquare CharToPiece P((int c, int p));
void DrawSquare P((int row, int column, ChessSquare piece));
void BlinkSquare P((int row, int column, ChessSquare piece));
void 
EventProc P((Widget widget, caddr_t unused, XEvent *event));
void DrawPosition P((Widget w, XEvent *event,
		     String *prms, Cardinal *nprms));
void InitPosition P((int redraw));
void CopyBoard P((Board to, Board from));
void CopyCatches P((Catched to, Catched from));
void ClearCatches P((Catched to));
void UpdateCatched P((int c, int f, int d, int a, int cm));
void SendCurrentBoard P((FILE *fp));
void SendBoard P((FILE *fp, Board board, Catched catches));
void HandleUserMove P((Widget w, XEvent *event));
void FinishUserMove P((ChessMove move_type, int to_x, int to_y));
void HandleMachineMove P((char *message, FILE *fp));
void ReadGameFile P((void));
int ReadGameFileProc P((void));
void ApplyMove P((ChessMove *move_type, int from_x, int from_y,
		  int to_x, int to_y, int currentMove));
void MakeMove P((ChessMove *move_type, int from_x, int from_y,
	int to_x, int to_y));
void InitChessProgram P((char *host_name, char *program_name, int *pid,
	FILE **to, FILE **from, XtIntervalId *xid,
	int *sendTime));
void ShutdownChessPrograms P((char *message));
void CommentPopUp P((char *label));
void FileNamePopUp P((char *label, Boolean (*proc)(char *name)));
void FileNameCallback P((Widget w, XtPointer client_data,
			 XtPointer call_data));
void FileNameAction P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void PromotionPopUp P((ChessSquare piece, int to_x, int to_y, int frp));
void PromotionCallback P((Widget w, XtPointer client_data,
			  XtPointer call_data));
void FileModePopUp P((char *name));
void FileModeCallback P((Widget w, XtPointer client_data,
			  XtPointer call_data));
void SelectCommand P((Widget w, XtPointer client_data, XtPointer call_data));
void ModeHighlight P((void));
void QuitProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void QuitRemotePlayerProc P((void));
#ifdef ISS
void DrawProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void DeclineDrawProc P((Widget w, XEvent *event,
			String *prms, Cardinal *nprms));
void ResignProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void CallFlagProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
#endif
Boolean LoadGame P((char *name));
void LoadGameProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
Boolean LoadPosition P((char *name));
void LoadPositionProc P((Widget w, XEvent *event,
			 String *prms, Cardinal *nprms));
void MachineBlackProc P((Widget w, XEvent *event, String *prms,Cardinal *nprms));
void MachineWhiteProc P((Widget w, XEvent *event, String *prms,Cardinal *nprms));
void ForwardProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void MoveNowProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void ResetFileProc P((void));
void ResetChallenge P((void));
void ResetProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void Reset P((int /*Boolean*/ redraw)); 
Boolean Challenge P((char *name));
void ChallengeProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
Boolean SelectLevel P((char *command));
void SelectLevelProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void BackwardProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void FlipViewProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void ForceProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void SaveGameProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
Boolean SaveGame P((char *name));
void SavePositionProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
Boolean SavePosition P((char *name));
void SwitchProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void HintProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void EditPositionProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void EditPositionDone P((void));
#ifdef ISS
void NothingProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
Boolean ParseGameHistory P((char *game));
void GameEnds P((char *why));
#endif
void TwoMachinesProc P((Widget w, XEvent *event, String *prms,
			Cardinal *nprms));
void PauseProc P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void Iconify P((Widget w, XEvent *event, String *prms, Cardinal *nprms));
void PrintOpponents P((FILE *fp));
void PrintPosition P((FILE *fp, int move));
void SendToProgram P((char *message, FILE *fp));
void ReceiveFromProgram P((FILE *fp, int *source, XtInputId *id));
void SendSearchDepth P((FILE *fp));
void SendTimeRemaining P((FILE *fp));
void DisplayMessage P((char *message, int toRemotePlayer));
void DisplayName P((char *name));
void DisplayMove P((int moveNumber));
void DisplayTitle P((char *title));
void Attention P((int pid));
void DisplayClocks P((int clock_mode));
void DisplayTimerLabel P((Widget w, char *color, long timer));
char *TimeString P((long tm));
void Usage P((void));
char *StrStr P((char *string, char *match));
int StrCaseCmp P((char *s1, char *s2));
int ToLower P((int c));
int ToUpper P((int c));
#if	SYSTEM_FIVE || SYSV
char *PseudoTTY P((int *ptyv));
#else
void CatchPipeSignal P((int dummy));
#endif
#define off_board(x) (x < 2 || x > BOARD_SIZE+1)
extern void parseGameFile P((void));
Boolean ParseBoard P((char *string));
void ParseMachineMove P((char *machine_move, ChessMove *move_type, int *from_x, int *from_y, int *to_x, int *to_y));
/*
ChessMove MakeAlg P((int fromX, int fromY, int toX, int toY, char promoPiece, int currentBoardIndex, char *out)); 
*/

/*
 * XShogi depends on Xt R4 or higher
 */
int xtVersion = XtSpecificationRelease;

XtIntervalId firstProgramXID = 0, secondProgramXID = 0,
#ifdef ISS
	telnetPID = 0,
#endif
	readGameXID = 0, timerXID = 0, blinkSquareXID = 0;

XtAppContext appContext;

Boolean (*fileProc) P((char *name));

FILE *fromFirstProgFP, *toFirstProgFP, *fromSecondProgFP,
	*toSecondProgFP, *gameFileFP, *lastMsgFP;

int currentMove = 0, forwardMostMove = 0, backwardMostMove = 0, 
	firstProgramPID = 0,
	secondProgramPID = 0, fromX = -1, 
	fromY = -1, firstMove = True, flipView = False,
	xshogiDebug = True, commentUp = False, filenameUp = False,
        whitePlaysFirst = False, startedFromSetupPosition = False,
	searchTime = 0, pmFromX = -1, pmFromY = -1,
        blackFlag = False, whiteFlag = False, maybeThinking = False,
	filemodeUp = False;

int at_least_gnushogi_1_2p03 = False;

int firstSendTime = 2, secondSendTime = 2;  /* 0=don't, 1=do, 2=test first*/

#ifdef ISS

int iss_input = -1, iss_output = -1, iss_user_moved = 0, iss_gamenum = -1,
  iss_getting_history = 0;

IssMode iss_mode = IssIdle;                              

int  iss_column_bug = 0;  /* column number in reversed order */
int  iss_capture_bug = 0; /* garbage in text area of captured pieces */

/* conversions from/to piece numbers or chess squares to/from ISS characters */                          

char IssPieceChar[15] =
 { 'p', 'l', 'n', 's', 'G', 'b', 'r', 'K', ' ', 'P', 'L', 'N', 'S', 'B', 'R' };

char IssChessSquareChar[29] =
 { 'p', 'l', 'n', 's', 'G', 'b', 'r', 'P', 'L', 'N', 'S', 'B', 'R', 'K',
   'p', 'l', 'n', 's', 'G', 'b', 'r', 'P', 'L', 'N', 'S', 'B', 'R', 'K',
   ' '
 };

ChessSquare IssChessSquare[2][15] =
 { BlackPawn, BlackLance, BlackKnight, BlackSilver, BlackGold, BlackBishop, BlackRook, BlackKing,
      EmptySquare, BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver, BlackPBishop, BlackPRook,
   WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, WhiteGold, WhiteBishop, WhiteRook, WhiteKing,
      EmptySquare, WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver, WhitePBishop, WhitePRook,
 };
 
char IssSideChar[3] =
 { '#', '=', ' ' };
 
#endif


MatchMode matchMode = MatchFalse;
GameMode gameMode = BeginningOfGame, lastGameMode = BeginningOfGame,
  pausePreviousMode = BeginningOfGame;

char moveList[MAX_MOVES][MOVE_LEN], parseList[MAX_MOVES][MOVE_LEN * 2],
	ptyname[24], *chessDir, *programName;

#ifdef ISS
char iss_black[32], iss_white[32];
#endif
char endMessage[MOVE_LEN * 4];     

long blackTimeRemaining, whiteTimeRemaining, timeControl;
long timeRemaining[2][MAX_MOVES];

extern char currentMoveString[];

int updateRemotePlayer = False;
                      
Catched catches[MAX_MOVES];
     
          
#define DIMENSION 100


Widget blackPieceMenu, whitePieceMenu, commentShell;


XSetWindowAttributes attr;
               
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

#define NO_PIECES 15
#define NO_SQUARES 81
#define NO_COLS 9
#define NO_ROWS 9


char catchedIndexToChar[8] = {
  'P', 'L', 'N', 'S', 'G', 'B', 'R', 'K'
};

ChessSquare catchedIndexToPiece[2][8] = {
   BlackPawn, BlackLance, BlackKnight, BlackSilver, BlackGold,
   BlackBishop, BlackRook, BlackKing,
   WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, WhiteGold,
   WhiteBishop, WhiteRook, WhiteKing
};


int pieceToCatchedIndex[] = {
   pawn, lance, knight, silver, gold, bishop, rook,
   pawn, lance, knight, silver, bishop, rook, king,
   pawn, lance, knight, silver, gold, bishop, rook,
   pawn, lance, knight, silver, bishop, rook, king,
   no_piece
};



Board boards[MAX_MOVES], initialPosition = {
	{ BlackLance, BlackKnight, BlackSilver, BlackGold, BlackKing,
          	BlackGold, BlackSilver, BlackKnight, BlackLance },
        { EmptySquare, BlackBishop, EmptySquare, EmptySquare, EmptySquare,
          	EmptySquare, EmptySquare, BlackRook, EmptySquare }, 
        { BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, 
          	BlackPawn, BlackPawn, BlackPawn, BlackPawn },
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare, 
		EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,  
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare, 
		EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,  
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare, EmptySquare, 
		EmptySquare, EmptySquare, EmptySquare, EmptySquare } ,  
        { WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, 
		WhitePawn, WhitePawn, WhitePawn, WhitePawn },
        { EmptySquare, WhiteRook, EmptySquare, EmptySquare, EmptySquare,
          	EmptySquare, EmptySquare, WhiteBishop, EmptySquare }, 
	{ WhiteLance, WhiteKnight, WhiteSilver, WhiteGold, WhiteKing,
          	WhiteGold, WhiteSilver, WhiteKnight, WhiteLance }
};

String gnuButtonStrings[] = {
	"Quit", "Load Game", "Machine White", "Forward",
	"Reset", "Load Position", "Machine Black", "Backward",
	"Flip View", "Save Game", "Force Moves", "Pause",
	"Hint", "Save Position", "Two Machines", "Edit Position",
	"Challenge", "Select Level", "Move NOW",
};

/* must be in same order as buttonStrings! */
XtActionProc gnuButtonProcs[] = {
    QuitProc,         LoadGameProc,     MachineWhiteProc, ForwardProc,
    ResetProc,        LoadPositionProc, MachineBlackProc, BackwardProc,
    FlipViewProc,     SaveGameProc,     ForceProc,        PauseProc,
    HintProc,         SavePositionProc, TwoMachinesProc,  EditPositionProc,
    ChallengeProc,    SelectLevelProc,  MoveNowProc,  
    NULL
  };

#ifdef ISS                             

#ifdef FULL_ISS

String issButtonStrings[] = {
    "Quit",          "Call Flag",    "Load Game",     "Forward",       
    "Reset",         "Draw",         "Save Game",     "Backward",
    "Flip View",     "Decline Draw", "Load Position", "Pause",
    "Edit Position", "Resign",       "Save Position", ""
  };
/* must be in same order as issButtonStrings! */
XtActionProc issButtonProcs[] = {
    QuitProc,         CallFlagProc,    LoadGameProc,     ForwardProc,
    ResetProc,        DrawProc,        SaveGameProc,     BackwardProc,
    FlipViewProc,     DeclineDrawProc, LoadPositionProc, PauseProc,
    EditPositionProc, ResignProc,      SavePositionProc, NothingProc,
    NULL
  };           

#else

String issButtonStrings[] = {
    "Quit",              
    "Reset",
    "Save Position",      
    "Save Game",      
    "Flip View",
    "Draw Position",  
    "Resign", 
    ""
  };
/* must be in same order as issButtonStrings! */
XtActionProc issButtonProcs[] = {
    QuitProc, 
    ResetProc,     
    SavePositionProc,
    SaveGameProc,
    FlipViewProc,
    DrawPosition,
    ResignProc,
    NothingProc,
    NULL
  };           

#endif

#endif

String *buttonStrings;
XtActionProc *buttonProcs;
int buttonCount;



#define PIECE_MENU_SIZE 18
String pieceMenuStrings[PIECE_MENU_SIZE] = {
	"----", "Pawn", "Lance", "Knight", "Silver",
                "Gold", "Bishop", "Rook",
		"PPawn", "PLance", "PKnight", "PSilver",
		"PBishop", "PRook", "King",
	"----", "Empty square", "Clear board"
};
/* must be in same order as PieceMenuStrings! */
ChessSquare pieceMenuTranslation[2][PIECE_MENU_SIZE] = {
	{ (ChessSquare) 0, BlackPawn, BlackLance, BlackKnight, BlackSilver, 
	 	BlackGold, BlackBishop, BlackRook,
		BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver,
		BlackPBishop, BlackPRook, BlackKing,
	 	(ChessSquare) 0, EmptySquare, ClearBoard },
	{ (ChessSquare) 0, WhitePawn, WhiteLance, WhiteKnight, WhiteSilver, 
	 	WhiteGold, WhiteBishop, WhiteRook,
		WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver,
		WhitePBishop, WhitePRook, WhiteKing,
	 	(ChessSquare) 0, EmptySquare, ClearBoard },
};


typedef struct {
	Pixel blackPieceColor;
	Pixel whitePieceColor;
	Pixel lightSquareColor;
	Pixel darkSquareColor;
	Pixel charPieceColor;
	Pixel zeroColor;
	Pixel oneColor;
#ifdef BOTH_BITMAPS
	Boolean westernPieceSet;
#endif
	int movesPerSession;
	String initString;
	String blackString;
	String whiteString;
	String firstChessProgram;
	String secondChessProgram;
	Boolean noChessProgram;
	String firstHost;
	String secondHost;
	String reverseBigSolidBitmap;
	String reverseSmallSolidBitmap;
	String normalBigSolidBitmap;
	String normalSmallSolidBitmap;
	String reversePawnBitmap;
	String reverseLanceBitmap;
	String reverseKnightBitmap;
	String reverseSilverBitmap;
	String reverseGoldBitmap;
	String reverseRookBitmap;
	String reverseBishopBitmap;
	String reversePPawnBitmap;
	String reversePLanceBitmap;
	String reversePKnightBitmap;
	String reversePSilverBitmap;
	String reversePBishopBitmap;
	String reversePRookBitmap;
	String reverseKingBitmap;
	String normalPawnBitmap;
	String normalLanceBitmap;
	String normalKnightBitmap;
	String normalSilverBitmap;
	String normalGoldBitmap;
	String normalRookBitmap;
	String normalBishopBitmap;
	String normalPPawnBitmap;
	String normalPLanceBitmap;
	String normalPKnightBitmap;
	String normalPSilverBitmap;
	String normalPBishopBitmap;
	String normalPRookBitmap;
	String normalKingBitmap;
	String remoteShell;
	float timeDelay;
	String timeControl;
	String gameIn;
#ifdef ISS
	Boolean issActive;
	String issHost;
	int issPort;
	Boolean useTelnet;
	String telnetProgram;
	String gateway;
#endif
        Boolean autoSaveGames;
	String loadGameFile;
	String loadPositionFile;
	String saveGameFile;
	String savePositionFile;
	String matchMode;
	String challengeDisplay;
	Boolean monoMode;
	Boolean debugMode;
	Boolean clockMode;
	String boardSize;
	Boolean Iconic;
	String searchTime;
	int searchDepth;
        Boolean showCoords;
	String mainFont;
	String coordFont;
    	Boolean ringBellAfterMoves;
    	Boolean autoCallFlag;
    	int borderXoffset;
    	int borderYoffset;
} AppData, *AppDataPtr;

XtResource clientResources[] = {
	{
		"blackPieceColor", "BlackPieceColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, blackPieceColor), XtRString,
		BLACK_PIECE_COLOR
	}, {
		"whitePieceColor", "WhitePieceColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, whitePieceColor), XtRString,
		WHITE_PIECE_COLOR
	}, {
		"charPieceColor", "CharPieceColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, charPieceColor), XtRString,
		CHAR_PIECE_COLOR
	}, {
		"oneColor", "OneColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, oneColor), XtRString,
		ONE_COLOR
	}, {
		"zeroColor", "ZeroColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, zeroColor), XtRString,
		ZERO_COLOR
	}, {
		"lightSquareColor", "LightSquareColor", XtRPixel,
		sizeof(Pixel), XtOffset(AppDataPtr, lightSquareColor),
		XtRString, LIGHT_SQUARE_COLOR
	}, {
		"darkSquareColor", "DarkSquareColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, darkSquareColor), XtRString,
		DARK_SQUARE_COLOR
	}, {                     
#ifdef BOTH_BITMAPS
		"westernPieceSet", "WesternPieceSet", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, westernPieceSet), XtRString,
		(XtPointer) False
	}, {       
#endif
		"movesPerSession", "movesPerSession", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, movesPerSession), XtRImmediate,
		(XtPointer) MOVES_PER_SESSION
	}, {
		"initString", "initString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, initString), XtRString, INIT_STRING
	}, {
		"blackString", "blackString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, blackString), XtRString, BLACK_STRING
	}, {
		"whiteString", "whiteString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, whiteString), XtRString, WHITE_STRING
	}, {
		"firstChessProgram", "firstChessProgram", XtRString,
		sizeof(String), XtOffset(AppDataPtr, firstChessProgram),
		XtRString, FIRST_CHESS_PROGRAM
	}, {
		"secondChessProgram", "secondChessProgram", XtRString,
		sizeof(String), XtOffset(AppDataPtr, secondChessProgram),
		XtRString, SECOND_CHESS_PROGRAM
	}, {
		"noChessProgram", "noChessProgram", XtRBoolean,
		sizeof(Boolean), XtOffset(AppDataPtr, noChessProgram),
		XtRImmediate, (XtPointer) False
	}, {
		"firstHost", "firstHost", XtRString, sizeof(String),
		XtOffset(AppDataPtr, firstHost), XtRString, FIRST_HOST
	}, {
		"secondHost", "secondHost", XtRString, sizeof(String),
		XtOffset(AppDataPtr, secondHost), XtRString, SECOND_HOST
	}, {
		"reversePawnBitmap", "reversePawnBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePawnBitmap),
		XtRString, NULL
	}, {
		"reverseLanceBitmap", "reverseLanceBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseLanceBitmap),
		XtRString, NULL
	}, {
		"reverseKnightBitmap", "reverseKnightBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseKnightBitmap),
		XtRString, NULL
	}, {
		"reverseSilverBitmap", "reverseSilverBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseSilverBitmap),
		XtRString, NULL
	}, {
		"reverseGoldBitmap", "reverseGoldBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseGoldBitmap),
		XtRString, NULL
	}, {
		"reverseRookBitmap", "reverseRookBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseRookBitmap),
		XtRString, NULL
	}, {
		"reverseBishopBitmap", "reverseBishopBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseBishopBitmap),
		XtRString, NULL
	}, {
		"reversePPawnBitmap", "reversePPawnBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePPawnBitmap),
		XtRString, NULL
	}, {
		"reversePLanceBitmap", "reversePLanceBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePLanceBitmap),
		XtRString, NULL
	}, {
		"reversePKnightBitmap", "reversePKnightBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePKnightBitmap),
		XtRString, NULL
	}, {
		"reversePSilverBitmap", "reversePSilverBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePSilverBitmap),
		XtRString, NULL
	}, {
		"reversePRookBitmap", "reversePRookBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePRookBitmap),
		XtRString, NULL
	}, {
		"reversePBishopBitmap", "reversePBishopBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reversePBishopBitmap),
		XtRString, NULL
	}, {
		"reverseKingBitmap", "reverseKingBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, reverseKingBitmap),
		XtRString, NULL
	}, {
		"normalPawnBitmap", "normalPawnBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPawnBitmap),
		XtRString, NULL
	}, {
		"normalLanceBitmap", "normalLanceBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalLanceBitmap),
		XtRString, NULL
	}, {
		"normalKnightBitmap", "normalKnightBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalKnightBitmap),
		XtRString, NULL
	}, {
		"normalSilverBitmap", "normalSilverBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalSilverBitmap),
		XtRString, NULL
	}, {
		"normalGoldBitmap", "normalGoldBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalGoldBitmap),
		XtRString, NULL
	}, {
		"normalBishopBitmap", "normalBishopBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalBishopBitmap),
		XtRString, NULL
	}, {
		"normalRookBitmap", "normalRookBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalRookBitmap),
		XtRString, NULL
	}, {
		"normalPPawnBitmap", "normalPPawnBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPPawnBitmap),
		XtRString, NULL
	}, {
		"normalPLanceBitmap", "normalPLanceBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPLanceBitmap),
		XtRString, NULL
	}, {
		"normalPKnightBitmap", "normalPKnightBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPKnightBitmap),
		XtRString, NULL
	}, {
		"normalPSilverBitmap", "normalPSilverBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPSilverBitmap),
		XtRString, NULL
	}, {
		"normalPBishopBitmap", "normalPBishopBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPBishopBitmap),
		XtRString, NULL
	}, {
		"normalPRookBitmap", "normalPRookBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalPRookBitmap),
		XtRString, NULL
	}, {
		"normalKingBitmap", "normalKingBitmap", XtRString,
		sizeof(String), XtOffset(AppDataPtr, normalKingBitmap),
		XtRString, NULL
	}, {
		"remoteShell", "remoteShell", XtRString, sizeof(String),
		XtOffset(AppDataPtr, remoteShell), XtRString, "rsh"
	}, {
		"timeDelay", "timeDelay", XtRFloat, sizeof(float),
		XtOffset(AppDataPtr, timeDelay), XtRString,
		(XtPointer) TIME_DELAY
	}, {
		"timeControl", "timeControl", XtRString, sizeof(String),
		XtOffset(AppDataPtr, timeControl), XtRString,
		(XtPointer) TIME_CONTROL
	}, {
		"gameIn", "gameIn", 
		XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, gameIn), XtRImmediate,
		(XtPointer) False
#ifdef ISS
        }, { 
		"internetShogiServerMode", "internetShogiServerMode",
		XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, issActive), XtRImmediate,
		(XtPointer) False 
	}, { 
		"internetShogiServerHost", "internetShogiServerHost",
		XtRString, sizeof(String),
		XtOffset(AppDataPtr, issHost),
		XtRString, (XtPointer) ISS_HOST 
	}, { 
		"internetShogiServerPort", "internetShogiServerPort",
		XtRInt, sizeof(int),
		XtOffset(AppDataPtr, issPort), XtRImmediate,
		(XtPointer) ISS_PORT 
	}, {  
		"useTelnet", "useTelnet", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, useTelnet), XtRImmediate,
		(XtPointer) False 
	}, { 
		"telnetProgram", "telnetProgram", XtRString, sizeof(String),
		XtOffset(AppDataPtr, telnetProgram), XtRString, "telnet" 
	}, { 
		"gateway", "gateway", XtRString, sizeof(String),
		XtOffset(AppDataPtr, gateway), XtRString, "" 
#endif
        },  { 
		"autoSaveGames", "autoSaveGames", XtRBoolean,
		sizeof(Boolean), XtOffset(AppDataPtr, autoSaveGames),
		XtRImmediate, (XtPointer) False
	}, {
		"loadGameFile", "loadGameFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, loadGameFile), XtRString, NULL
	}, {
		"loadPositionFile", "loadPositionFile", XtRString,
		sizeof(String), XtOffset(AppDataPtr, loadPositionFile),
		XtRString, NULL
	}, {
		"saveGameFile", "saveGameFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, saveGameFile), XtRString, ""
	}, {
		"savePositionFile", "savePositionFile", XtRString,
		sizeof(String), XtOffset(AppDataPtr, savePositionFile),
		XtRString, ""
	}, {
		"challengeDisplay", "challengeDisplay", XtRString,
		sizeof(String), XtOffset(AppDataPtr, challengeDisplay),
		XtRString, NULL
	}, {
		"matchMode", "matchMode", XtRString, sizeof(String),
		XtOffset(AppDataPtr, matchMode), XtRString, MATCH_MODE
	}, {
		"monoMode", "monoMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, monoMode), XtRImmediate,
		(XtPointer) False
	}, {
		"debugMode", "debugMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, debugMode), XtRImmediate,
		(XtPointer) False
	}, {
		"Iconic", "Iconic", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, Iconic), XtRImmediate,
		(XtPointer) False
	}, {
		"clockMode", "clockMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, clockMode), XtRImmediate,
		(XtPointer) True
        }, { 
		"autoCallFlag", "autoCallFlag", XtRBoolean,
		sizeof(Boolean), XtOffset(AppDataPtr, autoCallFlag),
		XtRImmediate, (XtPointer) False
	}, {
		"boardSize", "boardSize", XtRString, sizeof(String),
		XtOffset(AppDataPtr, boardSize), XtRString, DEFAULT_SIZE
	}, {
		"searchTime", "searchTime", XtRString, sizeof(String),
		XtOffset(AppDataPtr, searchTime), XtRString,
		(XtPointer) NULL
	}, {
		"searchDepth", "searchDepth", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, searchDepth), XtRImmediate, 
		(XtPointer) 0
	}, {
		"showCoords", "showCoords", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, showCoords), XtRImmediate,
		(XtPointer) False
	}, {
		"mainFont", "mainFont", XtRString, sizeof(String),
		XtOffset(AppDataPtr, mainFont), XtRString, MAIN_FONT
	}, {
		"coordFont", "coordFont", XtRString, sizeof(String),
		XtOffset(AppDataPtr, coordFont), XtRString, COORD_FONT
        }, { 
		"ringBellAfterMoves", "ringBellAfterMoves",
		XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, ringBellAfterMoves),
		XtRImmediate, (XtPointer) False
        }, { 
    		"borderXoffset", "borderXoffset", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, borderXoffset), XtRImmediate,
		(XtPointer) BORDER_X_OFFSET
        }, { 
    		"borderYoffset", "borderYOffset", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, borderYoffset), XtRImmediate,
		(XtPointer) BORDER_Y_OFFSET
	}
};


struct DisplayData {                     

    AppData appData;

    Arg shellArgs[6];
    Arg boardArgs[3];
    Arg commandsArgs[7];
    Arg messageArgs[3];
    Arg timerArgs[2];
    Arg titleArgs[2];

    Pixmap reversePawnBitmap, reverseLanceBitmap, reverseKnightBitmap, reverseSilverBitmap,
       reverseGoldBitmap, reverseBishopBitmap, reverseRookBitmap,
       reversePPawnBitmap, reversePLanceBitmap, reversePKnightBitmap,
       reversePSilverBitmap, reversePBishopBitmap, reversePRookBitmap, 
       reverseKingBitmap, 
       reverseBigSolidBitmap, reverseSmallSolidBitmap,
       normalBigSolidBitmap, normalSmallSolidBitmap,
       normalPawnBitmap, normalLanceBitmap, normalKnightBitmap, 
       normalSilverBitmap, normalGoldBitmap, 
       normalBishopBitmap, normalRookBitmap,
       normalPPawnBitmap, normalPLanceBitmap, normalPKnightBitmap,
       normalPSilverBitmap, normalPBishopBitmap, normalPRookBitmap,
       normalKingBitmap, iconPixmap;  

    Display *xDisplay;
    int xScreen;
    Window xBoardWindow;

    GC lightSquareGC, darkSquareGC, lineGC, wdPieceGC, wlPieceGC, woPieceGC, boPieceGC,
	bdPieceGC, blPieceGC, wbPieceGC, bwPieceGC, coordGC, dropPiece;

    GC charPieceGC;              

    Font mainFontID, coordFontID;
    XFontStruct *mainFontStruct, *coordFontStruct;

    Widget shellWidget, formWidget, boardWidget, commandsWidget, messageWidget,
	blackTimerWidget, whiteTimerWidget, titleWidget, widgetList[6], 
        promotionShell,
	filemodeShell, challengeWidget;

    XSegment gridSegments[(BOARD_SIZE + 1) * 2];

    Pixel timerForegroundPixel, timerBackgroundPixel;

    BoardSize boardSize;
    int squareSize;
    int black_pixel_is_zero;
    int flipView;
    int promotionUp;

    Boolean monoMode, showCoords, Iconic;
};
       


struct DisplayData localPlayer, remotePlayer;


Pixmap *pieceToReverse[2][28] = {
	&localPlayer.reversePawnBitmap, &localPlayer.reverseLanceBitmap, &localPlayer.reverseKnightBitmap,
	&localPlayer.reverseSilverBitmap, &localPlayer.reverseGoldBitmap, &localPlayer.reverseBishopBitmap,
        &localPlayer.reverseRookBitmap, &localPlayer.reversePPawnBitmap, &localPlayer.reversePLanceBitmap,
        &localPlayer.reversePKnightBitmap, &localPlayer.reversePSilverBitmap, &localPlayer.reversePBishopBitmap,
        &localPlayer.reversePRookBitmap, &localPlayer.reverseKingBitmap,
	&localPlayer.reversePawnBitmap, &localPlayer.reverseLanceBitmap, &localPlayer.reverseKnightBitmap,
	&localPlayer.reverseSilverBitmap, &localPlayer.reverseGoldBitmap, &localPlayer.reverseBishopBitmap,
        &localPlayer.reverseRookBitmap, &localPlayer.reversePPawnBitmap, &localPlayer.reversePLanceBitmap,
        &localPlayer.reversePKnightBitmap, &localPlayer.reversePSilverBitmap, &localPlayer.reversePBishopBitmap,
        &localPlayer.reversePRookBitmap, &localPlayer.reverseKingBitmap,
	&remotePlayer.reversePawnBitmap, &remotePlayer.reverseLanceBitmap, &remotePlayer.reverseKnightBitmap,
	&remotePlayer.reverseSilverBitmap, &remotePlayer.reverseGoldBitmap, &remotePlayer.reverseBishopBitmap,
        &remotePlayer.reverseRookBitmap, &remotePlayer.reversePPawnBitmap, &remotePlayer.reversePLanceBitmap,
        &remotePlayer.reversePKnightBitmap, &remotePlayer.reversePSilverBitmap, &remotePlayer.reversePBishopBitmap,
        &remotePlayer.reversePRookBitmap, &remotePlayer.reverseKingBitmap,
	&remotePlayer.reversePawnBitmap, &remotePlayer.reverseLanceBitmap, &remotePlayer.reverseKnightBitmap,
	&remotePlayer.reverseSilverBitmap, &remotePlayer.reverseGoldBitmap, &remotePlayer.reverseBishopBitmap,
        &remotePlayer.reverseRookBitmap, &remotePlayer.reversePPawnBitmap, &remotePlayer.reversePLanceBitmap,
        &remotePlayer.reversePKnightBitmap, &remotePlayer.reversePSilverBitmap, &remotePlayer.reversePBishopBitmap,
        &remotePlayer.reversePRookBitmap, &remotePlayer.reverseKingBitmap,
};

Pixmap *pieceToNormal[2][28] = {
	&localPlayer.normalPawnBitmap, &localPlayer.normalLanceBitmap, &localPlayer.normalKnightBitmap,
	&localPlayer.normalSilverBitmap, &localPlayer.normalGoldBitmap, &localPlayer.normalBishopBitmap,
        &localPlayer.normalRookBitmap, &localPlayer.normalPPawnBitmap, &localPlayer.normalPLanceBitmap,
        &localPlayer.normalPKnightBitmap, &localPlayer.normalPSilverBitmap, &localPlayer.normalPBishopBitmap,
        &localPlayer.normalPRookBitmap, &localPlayer.normalKingBitmap,
	&localPlayer.normalPawnBitmap, &localPlayer.normalLanceBitmap, &localPlayer.normalKnightBitmap,
	&localPlayer.normalSilverBitmap, &localPlayer.normalGoldBitmap, &localPlayer.normalBishopBitmap,
        &localPlayer.normalRookBitmap, &localPlayer.normalPPawnBitmap, &localPlayer.normalPLanceBitmap,
        &localPlayer.normalPKnightBitmap, &localPlayer.normalPSilverBitmap, &localPlayer.normalPBishopBitmap,
        &localPlayer.normalPRookBitmap, &localPlayer.normalKingBitmap,
	&remotePlayer.normalPawnBitmap, &remotePlayer.normalLanceBitmap, &remotePlayer.normalKnightBitmap,
	&remotePlayer.normalSilverBitmap, &remotePlayer.normalGoldBitmap, &remotePlayer.normalBishopBitmap,
        &remotePlayer.normalRookBitmap, &remotePlayer.normalPPawnBitmap, &remotePlayer.normalPLanceBitmap,
        &remotePlayer.normalPKnightBitmap, &remotePlayer.normalPSilverBitmap, &remotePlayer.normalPBishopBitmap,
        &remotePlayer.normalPRookBitmap, &remotePlayer.normalKingBitmap,
	&remotePlayer.normalPawnBitmap, &remotePlayer.normalLanceBitmap, &remotePlayer.normalKnightBitmap,
	&remotePlayer.normalSilverBitmap, &remotePlayer.normalGoldBitmap, &remotePlayer.normalBishopBitmap,
        &remotePlayer.normalRookBitmap, &remotePlayer.normalPPawnBitmap, &remotePlayer.normalPLanceBitmap,
        &remotePlayer.normalPKnightBitmap, &remotePlayer.normalPSilverBitmap, &remotePlayer.normalPBishopBitmap,
        &remotePlayer.normalPRookBitmap, &remotePlayer.normalKingBitmap,
};

Pixmap *pieceToReverseSolid[2][28] = {
	&localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap,
	&localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
	&localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap,
	&localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseSmallSolidBitmap,
        &localPlayer.reverseSmallSolidBitmap, &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
        &localPlayer.reverseBigSolidBitmap, &localPlayer.reverseBigSolidBitmap,
	&remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap,
	&remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
	&remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap,
	&remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseSmallSolidBitmap,
        &remotePlayer.reverseSmallSolidBitmap, &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
        &remotePlayer.reverseBigSolidBitmap, &remotePlayer.reverseBigSolidBitmap,
};

Pixmap *pieceToNormalSolid[2][28] = {
	&localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap,
	&localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap, &localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap, &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
	&localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap,
	&localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap, &localPlayer.normalSmallSolidBitmap, &localPlayer.normalSmallSolidBitmap,
        &localPlayer.normalSmallSolidBitmap, &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
        &localPlayer.normalBigSolidBitmap, &localPlayer.normalBigSolidBitmap,
	&remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap,
	&remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
	&remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap,
	&remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalSmallSolidBitmap,
        &remotePlayer.normalSmallSolidBitmap, &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
        &remotePlayer.normalBigSolidBitmap, &remotePlayer.normalBigSolidBitmap,
};


int pieceIsPromoted[] = {
 	False, False, False, False, False, False, False,  
        True,  True,  True,  True,  True,  True,  False,
 	False, False, False, False, False, False, False,  
        True,  True,  True,  True,  True,  True,  False,
        False
};

int piecePromotable[] = {
 	True,  True,  True,  True,  False, True,  True,  
 	False, False, False, False, False, False, False,  
 	True,  True,  True,  True,  False, True,  True,  
 	False, False, False, False, False, False, False,  
        False
};

char pieceToChar[] = {                               
	'P', 'L', 'N', 'S', 'G', 'B', 'R', 'P', 'L', 'N', 'S', 'B', 'R', 'K',
	'p', 'l', 'n', 's', 'g', 'b', 'r', 'p', 'l', 'n', 's', 'b', 'r', 'k', 
        '.'
};  


int pieceisWhite[] = {
 	False, False, False, False, False, False, False,  
 	False, False, False, False, False, False, False,  
 	True,  True,  True,  True,  True, True,  True,  
 	True, True, True, True, True, True, True,  
        False
};



ChessSquare pieceToPromoted[] = {                               
	BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver, BlackGold,
	BlackPBishop, BlackPRook,
	BlackPPawn, BlackPLance, BlackPKnight, BlackPSilver, 
	BlackPBishop, BlackPRook, BlackKing,
	WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver, WhiteGold,
	WhitePBishop, WhitePRook,
	WhitePPawn, WhitePLance, WhitePKnight, WhitePSilver, 
	WhitePBishop, WhitePRook, WhiteKing
};

XrmOptionDescRec shellOptions[] = {
	{ "-blackPieceColor", "blackPieceColor", XrmoptionSepArg, NULL },
	{ "-bpc", "blackPieceColor", XrmoptionSepArg, NULL },
	{ "-whitePieceColor", "whitePieceColor", XrmoptionSepArg, NULL },
	{ "-wpc", "whitePieceColor", XrmoptionSepArg, NULL },
	{ "-charPieceColor", "charPieceColor", XrmoptionSepArg, NULL },
	{ "-cpc", "charPieceColor", XrmoptionSepArg, NULL },
	{ "-zeroColor", "zeroColor", XrmoptionSepArg, NULL },
	{ "-zc", "zeroColor", XrmoptionSepArg, NULL },
	{ "-oneColor", "oneColor", XrmoptionSepArg, NULL },
	{ "-oc", "oneColor", XrmoptionSepArg, NULL },
	{ "-lightSquareColor", "lightSquareColor", XrmoptionSepArg, NULL },
	{ "-lsc", "lightSquareColor", XrmoptionSepArg, NULL },
	{ "-darkSquareColor", "darkSquareColor", XrmoptionSepArg, NULL },
	{ "-dsc", "darkSquareColor", XrmoptionSepArg, NULL },
#if defined BOTH_BITMAPS
	{ "-westernPieceSet", "westernPieceSet", XrmoptionSepArg, NULL },
	{ "-wps", "westernPieceSet", XrmoptionSepArg, NULL },
#endif
	{ "-movesPerSession", "movesPerSession", XrmoptionSepArg, NULL },
	{ "-mps", "movesPerSession", XrmoptionSepArg, NULL },
#ifdef ISS
    	{ "-internetShogiServerMode", "internetShogiServerMode",XrmoptionSepArg, NULL },
	{ "-iss", "internetShogiServerMode", XrmoptionSepArg, NULL },
	{ "-internetShogiServerPort", "internetShogiServerPort",XrmoptionSepArg, NULL },
	{ "-issport", "internetShogiServerPort", XrmoptionSepArg, NULL },
	{ "-internetShogiServerHost", "internetShogiServerHost",XrmoptionSepArg, NULL },
	{ "-isshost", "internetShogiServerHost", XrmoptionSepArg, NULL },
#endif
	{ "-firstChessProgram", "firstChessProgram", XrmoptionSepArg, NULL },
	{ "-fcp", "firstChessProgram", XrmoptionSepArg, NULL },
	{ "-secondChessProgram", "secondChessProgram", XrmoptionSepArg, NULL },
	{ "-scp", "secondChessProgram", XrmoptionSepArg, NULL },
	{ "-noChessProgram", "noChessProgram", XrmoptionSepArg, NULL },
	{ "-ncp", "noChessProgram", XrmoptionSepArg, NULL },
	{ "-firstHost", "firstHost", XrmoptionSepArg, NULL },
	{ "-fh", "firstHost", XrmoptionSepArg, NULL },
	{ "-secondHost", "secondHost", XrmoptionSepArg, NULL },
	{ "-sh", "secondHost", XrmoptionSepArg, NULL },
	{ "-reversePawnBitmap", "reversePawnBitmap", XrmoptionSepArg, NULL },
	{ "-rpb", "reversePawnBitmap", XrmoptionSepArg, NULL },
	{ "-reverseLanceBitmap", "reverseLanceBitmap", XrmoptionSepArg, NULL },
	{ "-rlb", "reverseLanceBitmap", XrmoptionSepArg, NULL },
	{ "-reverseKnightBitmap", "reverseKnightBitmap", XrmoptionSepArg, NULL },
	{ "-rnb", "reverseKnightBitmap", XrmoptionSepArg, NULL },
	{ "-reverseSilverBitmap", "reverseSilverBitmap", XrmoptionSepArg, NULL },
	{ "-rsb", "reverseSilverBitmap", XrmoptionSepArg, NULL },
	{ "-reverseGoldBitmap", "reverseGoldBitmap", XrmoptionSepArg, NULL },
	{ "-rgb", "reverseGoldBitmap", XrmoptionSepArg, NULL },
	{ "-reverseRookBitmap", "reverseRookBitmap", XrmoptionSepArg, NULL },
	{ "-rrb", "reverseRookBitmap", XrmoptionSepArg, NULL },
	{ "-reverseBishopBitmap", "reverseBishopBitmap", XrmoptionSepArg, NULL },
	{ "-rbb", "reverseBishopBitmap", XrmoptionSepArg, NULL },
	{ "-reversePPawnBitmap", "reversePPawnBitmap", XrmoptionSepArg, NULL },
	{ "-rppb", "reversePPawnBitmap", XrmoptionSepArg, NULL },
	{ "-reversePLanceBitmap", "reversePLanceBitmap", XrmoptionSepArg, NULL },
	{ "-rplb", "reversePLanceBitmap", XrmoptionSepArg, NULL },
	{ "-reversePKnightBitmap", "reversePKnightBitmap", XrmoptionSepArg, NULL },
	{ "-rpnb", "reversePKnightBitmap", XrmoptionSepArg, NULL },
	{ "-reversePSilverBitmap", "reversePSilverBitmap", XrmoptionSepArg, NULL },
	{ "-rpsb", "reversePSilverBitmap", XrmoptionSepArg, NULL },
	{ "-reversePRookBitmap", "reversePRookBitmap", XrmoptionSepArg, NULL },
	{ "-rprb", "reversePRookBitmap", XrmoptionSepArg, NULL },
	{ "-reversePBishopBitmap", "reversePBishopBitmap", XrmoptionSepArg, NULL },
	{ "-rpbb", "reversePBishopBitmap", XrmoptionSepArg, NULL },
	{ "-reverseKingBitmap", "reverseKingBitmap", XrmoptionSepArg, NULL },
	{ "-rkb", "reverseKingBitmap", XrmoptionSepArg, NULL },
	{ "-outlinePawnBitmap", "outlinePawnBitmap", XrmoptionSepArg, NULL },
	{ "-opb", "normalPawnBitmap", XrmoptionSepArg, NULL },
	{ "-normalLanceBitmap", "normalLanceBitmap", XrmoptionSepArg, NULL },
	{ "-olb", "normalLanceBitmap", XrmoptionSepArg, NULL },
	{ "-normalKnightBitmap", "normalKnightBitmap", XrmoptionSepArg, NULL },
	{ "-onb", "normalKnightBitmap", XrmoptionSepArg, NULL },
	{ "-normalSilverBitmap", "normalSilverBitmap", XrmoptionSepArg, NULL },
	{ "-osb", "normalSilverBitmap", XrmoptionSepArg, NULL },
	{ "-normalGoldBitmap", "normalGoldBitmap", XrmoptionSepArg, NULL },
	{ "-ogb", "normalGoldBitmap", XrmoptionSepArg, NULL },
	{ "-normalRookBitmap", "normalRookBitmap", XrmoptionSepArg, NULL },
	{ "-orb", "normalRookBitmap", XrmoptionSepArg, NULL },
	{ "-normalBishopBitmap", "normalBishopBitmap", XrmoptionSepArg, NULL },
	{ "-obb", "normalBishopBitmap", XrmoptionSepArg, NULL },
	{ "-normalPPawnBitmap", "normalPPawnBitmap", XrmoptionSepArg, NULL },
	{ "-oppb", "normalPPawnBitmap", XrmoptionSepArg, NULL },
	{ "-normalPLanceBitmap", "normalPLanceBitmap", XrmoptionSepArg, NULL },
	{ "-oplb", "normalPLanceBitmap", XrmoptionSepArg, NULL },
	{ "-normalPKnightBitmap", "normalPKnightBitmap", XrmoptionSepArg, NULL },
	{ "-opnb", "normalPKnightBitmap", XrmoptionSepArg, NULL },
	{ "-normalPSilverBitmap", "normalPSilverBitmap", XrmoptionSepArg, NULL },
	{ "-opsb", "normalPSilverBitmap", XrmoptionSepArg, NULL },
	{ "-normalPRookBitmap", "normalPRookBitmap", XrmoptionSepArg, NULL },
	{ "-oprb", "normalPRookBitmap", XrmoptionSepArg, NULL },
	{ "-normalPBishopBitmap", "normalPBishopBitmap", XrmoptionSepArg, NULL },
	{ "-opbb", "normalPBishopBitmap", XrmoptionSepArg, NULL },
	{ "-normalKingBitmap", "normalKingBitmap", XrmoptionSepArg, NULL },
	{ "-okb", "outlineKingBitmap", XrmoptionSepArg, NULL },
	{ "-remoteShell", "remoteShell", XrmoptionSepArg, NULL },
	{ "-rsh", "remoteShell", XrmoptionSepArg, NULL },
	{ "-timeDelay", "timeDelay", XrmoptionSepArg, NULL },
	{ "-td", "timeDelay", XrmoptionSepArg, NULL },
	{ "-timeControl", "timeControl", XrmoptionSepArg, NULL },
	{ "-tc", "timeControl", XrmoptionSepArg, NULL },
	{ "-gameIn", "gameIn", XrmoptionSepArg, NULL },
	{ "-gi", "gameIn", XrmoptionSepArg, NULL },
	{ "-loadGameFile", "loadGameFile", XrmoptionSepArg, NULL },
	{ "-lgf", "loadGameFile", XrmoptionSepArg, NULL },
	{ "-loadPositionFile", "loadPositionFile", XrmoptionSepArg, NULL },
	{ "-lpf", "loadPositionFile", XrmoptionSepArg, NULL },
	{ "-saveGameFile", "saveGameFile", XrmoptionSepArg, NULL },
	{ "-sgf", "saveGameFile", XrmoptionSepArg, NULL },
	{ "-savePositionFile", "savePositionFile", XrmoptionSepArg, NULL },
	{ "-spf", "savePositionFile", XrmoptionSepArg, NULL },
	{ "-challengeDisplay", "challengeDisplay", XrmoptionSepArg, NULL },
	{ "-cd", "challengeDisplay", XrmoptionSepArg, NULL },
	{ "-matchMode", "matchMode", XrmoptionSepArg, NULL },
	{ "-mm", "matchMode", XrmoptionSepArg, NULL },
	{ "-monoMode", "monoMode", XrmoptionSepArg, NULL },
	{ "-mono", "monoMode", XrmoptionSepArg, NULL },
	{ "-debugMode", "debugMode", XrmoptionSepArg, NULL },
	{ "-debug", "debugMode", XrmoptionSepArg, NULL },
	{ "-clockMode", "clockMode", XrmoptionSepArg, NULL },
	{ "-clock", "clockMode", XrmoptionSepArg, NULL },
	{ "-boardSize", "boardSize", XrmoptionSepArg, NULL },
	{ "-size", "boardSize", XrmoptionSepArg, NULL },
	{ "-searchTime", "searchTime", XrmoptionSepArg, NULL },
	{ "-st", "searchTime", XrmoptionSepArg, NULL },
	{ "-searchDepth", "searchDepth", XrmoptionSepArg, NULL },
	{ "-sd", "searchDepth", XrmoptionSepArg, NULL },
	{ "-showCoords", "showCoords", XrmoptionSepArg, NULL },
	{ "-coords", "showCoords", XrmoptionSepArg, NULL },
	{ "-iconic", "Iconic", XrmoptionNoArg, "True" }
};

XtActionsRec boardActions[] = {
	{ "DrawPosition", (XtActionProc) DrawPosition },
	{ "HandleUserMove", (XtActionProc) HandleUserMove },
	{ "ResetProc", (XtActionProc) ResetProc },
	{ "ResetFileProc", (XtActionProc) ResetFileProc },
	{ "LoadGameProc", (XtActionProc) LoadGameProc },
	{ "QuitProc", (XtActionProc) QuitProc },
	{ "ForwardProc", (XtActionProc) ForwardProc },
	{ "BackwardProc", (XtActionProc) BackwardProc },
	{ "PauseProc", (XtActionProc) PauseProc },
	{ "Iconify", (XtActionProc) Iconify },
	{ "FileNameAction", (XtActionProc) FileNameAction },
	{ "PieceMenuPopup", (XtActionProc) PieceMenuPopup },
	{ "SetBlackToPlay", (XtActionProc) SetBlackToPlay },
	{ "SetWhiteToPlay", (XtActionProc) SetWhiteToPlay }
};

char translationsTable[] =
	"<Expose>: DrawPosition() \n \
	 <Btn1Down>: HandleUserMove() \n \
	 <Btn1Up>: HandleUserMove() \n \
	 <Btn2Down>: XawPositionSimpleMenu(menuW) PieceMenuPopup(menuW) \n \
	 <Btn3Down>: XawPositionSimpleMenu(menuB) PieceMenuPopup(menuB) \n \
	 <Key>r: ResetFileProc() ResetProc() \n \
	 <Key>R: ResetFileProc() ResetProc() \n \
	 <Key>g: LoadGameProc() \n \
	 <Key>G: LoadGameProc() \n \
	 <Key>q: QuitProc() \n \
	 <Key>Q: QuitProc() \n \
	 <Message>WM_PROTOCOLS: QuitProc() \n \
	 <Key>f: ForwardProc() \n \
	 <Key>F: ForwardProc() \n \
	 <Key>b: BackwardProc() \n \
	 <Key>B: BackwardProc() \n \
	 <Key>p: PauseProc() \n \
	 <Key>P: PauseProc() \n \
	 <Key>i: Iconify() \n \
	 <Key>I: Iconify() \n \
	 <Key>c: Iconify() \n \
	 <Key>C: Iconify() \n";

char translationsTableReduced[] =
	"<Expose>: DrawPosition() \n \
	 <Btn1Down>: HandleUserMove() \n \
	 <Btn1Up>: HandleUserMove() \n \
	 <Message>WM_PROTOCOLS: QuitProc() \n";

char blackTranslations[] = "<BtnDown>: SetBlackToPlay()\n";
char whiteTranslations[] = "<BtnDown>: SetWhiteToPlay()\n";

String xshogiResources[] = {
	DEFAULT_FONT,
	"*Dialog*value.translations: #override \\n <Key>Return: FileNameAction()",
	NULL
};


int global_argc;       /* number of command args */
char *global_argv[10]; /* pointers to up to 10 command args */



static struct DisplayData *player;


void
CreatePlayerWindow ()
{

        int ok, i, mainFontPxlSize, coordFontPxlSize;
        int min, sec, matched;
	XSetWindowAttributes window_attributes;
	char buf[MSG_SIZ];
	Arg args[10];
	Dimension timerWidth, boardWidth, commandsWidth, w, h;
	Position x, y;
	int local;
	int fromRemotePlayer = (player == &remotePlayer);
           
	player->monoMode = player->appData.monoMode;
	player->showCoords = player->appData.showCoords;

	/*
	 * Parse timeControl resource
	 */
	if (player->appData.timeControl != NULL) {
		int min, sec, matched;

		matched = sscanf(player->appData.timeControl, "%d:%d", &min, &sec);
		if (matched == 1) {
			timeControl = min * 60 * 1000;
		} else if (matched == 2) {
			timeControl = (min * 60 + sec) * 1000;
		} else {
			fprintf(stderr, "%s: bad timeControl option %s\n",
				programName, player->appData.timeControl);
			Usage();
		}
	}

	/*
	 * Parse searchTime resource
	 */
	if (player->appData.searchTime != NULL) {
		int min, sec, matched;

		matched = sscanf(player->appData.searchTime, "%d:%d", &min, &sec);
		if (matched == 1) {
			searchTime = min * 60;
		} else if (matched == 2) {
			searchTime = min * 60 + sec;
		} else {
			fprintf(stderr, "%s: bad searchTime option %s\n",
				programName, player->appData.searchTime);
			Usage();
		}
	}

        if ((player->appData.searchTime != NULL) || (player->appData.searchDepth > 0)
	    || player->appData.noChessProgram)
	  player->appData.clockMode = False;
#ifdef ISS
	if (player->appData.issActive) player->appData.clockMode = True;
#endif

	player->Iconic = False;       
	player->boardSize = Small;
	player->squareSize = SMALL_SQUARE_SIZE;
	player->flipView = (player == &remotePlayer);
	player->promotionUp = False;

	/*
	 * Determine boardSize
	 */             
	if (StrCaseCmp(player->appData.boardSize, "Large") == 0)
		player->boardSize = Large;
	else if (StrCaseCmp(player->appData.boardSize, "Medium") == 0)
		player->boardSize = Medium;
	else if (StrCaseCmp(player->appData.boardSize, "Small") == 0)
		player->boardSize = Small;
	else {
		fprintf(stderr, "%s: bad boardSize option %s\n",
			programName, player->appData.boardSize);
		Usage();
	}

	if ( local = (player == &localPlayer) ) {
	  player->xDisplay = XtDisplay(player->shellWidget);
	  player->xScreen = DefaultScreen(player->xDisplay);
	}

	if (((DisplayWidth(player->xDisplay, player->xScreen) < 800)
	     ||	(DisplayHeight(player->xDisplay, player->xScreen) < 800))
	    && (player->boardSize == Large)) {
		player->boardSize = Medium;
	}                              

	switch (player->boardSize) {
	      case Small:
		player->squareSize = SMALL_SQUARE_SIZE;
       		mainFontPxlSize = 11;
        	coordFontPxlSize = 10;
		break;
	      case Medium:
		player->squareSize = MEDIUM_SQUARE_SIZE;
        	mainFontPxlSize = 17;
        	coordFontPxlSize = 12;
		break;
	      case Large:
		player->squareSize = LARGE_SQUARE_SIZE;
        	mainFontPxlSize = 17;
        	coordFontPxlSize = 14;
		break;
	}

	/*
	 * Detect if there are not enough colors are available and adapt.
	 */
	if (DefaultDepth(player->xDisplay, player->xScreen) <= 2)
		player->monoMode = True;

        /*
         * Determine what fonts to use.
         */
    	player->appData.mainFont = FindFont(player->appData.mainFont, mainFontPxlSize);
    	player->mainFontID = XLoadFont(player->xDisplay, player->appData.mainFont);
    	player->mainFontStruct = XQueryFont(player->xDisplay, player->mainFontID);
    	player->appData.coordFont = FindFont(player->appData.coordFont, coordFontPxlSize);
    	player->coordFontID = XLoadFont(player->xDisplay, player->appData.coordFont);
    	player->coordFontStruct = XQueryFont(player->xDisplay, player->coordFontID);

	/* 
	 * Set default arguments. 
	 */
	XtSetArg(player->shellArgs[0], XtNwidth, 0);
	XtSetArg(player->shellArgs[1], XtNheight, 0);
	XtSetArg(player->shellArgs[2], XtNminWidth, 0);
	XtSetArg(player->shellArgs[3], XtNminHeight, 0);
	XtSetArg(player->shellArgs[4], XtNmaxWidth, 0);
	XtSetArg(player->shellArgs[5], XtNmaxHeight, 0);

	XtSetArg(player->boardArgs[0], XtNborderWidth, 0),
	XtSetArg(player->boardArgs[1], XtNwidth, LINE_GAP + (BOARD_SIZE+4) * (SMALL_SQUARE_SIZE + LINE_GAP));
	XtSetArg(player->boardArgs[2], XtNheight, LINE_GAP + BOARD_SIZE * (SMALL_SQUARE_SIZE + LINE_GAP)); 

	XtSetArg(player->commandsArgs[0], XtNborderWidth, 0);
	XtSetArg(player->commandsArgs[1], XtNdefaultColumns, 4);
	XtSetArg(player->commandsArgs[2], XtNforceColumns, True);
    	XtSetArg(player->commandsArgs[3], XtNcolumnSpacing, 12);          
	XtSetArg(player->commandsArgs[4], XtNlist, (XtArgVal) buttonStrings);
    	XtSetArg(player->commandsArgs[5], XtNnumberStrings, buttonCount);   
    	XtSetArg(player->commandsArgs[6], XtNfont, player->mainFontStruct); 

	XtSetArg(player->messageArgs[0], XtNborderWidth, 0);
	XtSetArg(player->messageArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);
	XtSetArg(player->messageArgs[2], XtNlabel, (XtArgVal) "starting...");

	XtSetArg(player->timerArgs[0], XtNborderWidth, 0);
	XtSetArg(player->timerArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);

	XtSetArg(player->titleArgs[0], XtNborderWidth, 0);
	XtSetArg(player->titleArgs[1], XtNjustify, (XtArgVal) XtJustifyLeft);
                
	boardWidth = LINE_GAP + (BOARD_SIZE+4) * (player->squareSize + LINE_GAP);
	XtSetArg(player->boardArgs[1], XtNwidth, boardWidth);
	XtSetArg(player->boardArgs[2], XtNheight,
		 LINE_GAP + BOARD_SIZE * (player->squareSize + LINE_GAP));

	/*
	 * widget hierarchy
	 */
	player->formWidget = XtCreateManagedWidget("form",
		formWidgetClass, player->shellWidget, NULL, 0);

	player->widgetList[0] = player->blackTimerWidget =
	  XtCreateWidget(local ? "black time:" : "rblack time:", labelWidgetClass,
			 player->formWidget, player->timerArgs, XtNumber(player->timerArgs));
    	XtSetArg(args[0], XtNfont, player->mainFontStruct);
    	XtSetValues(player->blackTimerWidget, args, 1);

	player->widgetList[1] = player->whiteTimerWidget =
	  XtCreateWidget(local ? "white time:" : "rwhite time:", labelWidgetClass,
			 player->formWidget, player->timerArgs, XtNumber(player->timerArgs));
	XtSetArg(args[0], XtNfont, player->mainFontStruct);
   	XtSetValues(player->whiteTimerWidget, args, 1);

	player->widgetList[2] = player->titleWidget =
	  XtCreateWidget(local ? "" : "r", labelWidgetClass,
			 player->formWidget, player->titleArgs, XtNumber(player->titleArgs));
    	XtSetArg(args[0], XtNfont, player->mainFontStruct);
    	XtSetValues(player->titleWidget, args, 1);

	player->widgetList[3] = player->messageWidget =
	  XtCreateWidget(local ? "message" : "rmessage", labelWidgetClass, player->formWidget,
			 player->messageArgs, XtNumber(player->messageArgs));
   	XtSetArg(args[0], XtNfont, player->mainFontStruct);
    	XtSetValues(player->messageWidget, args, 1);
 
	player->widgetList[4] = player->commandsWidget =
	  XtCreateWidget(local ? "commands" : "rcommand", listWidgetClass, player->formWidget,
			 player->commandsArgs, XtNumber(player->commandsArgs));

	player->widgetList[5] = player->boardWidget =
	  XtCreateWidget(local ? "board" : "rboard", widgetClass, player->formWidget,
			 player->boardArgs, XtNumber(player->boardArgs));

	XtManageChildren(player->widgetList, XtNumber(player->widgetList));

	/*
	 * Calculate the width of the timer labels.
	 */
	XtSetArg(args[0], XtNfont, &player->mainFontStruct);
	XtGetValues(player->blackTimerWidget, args, 1);
	if (player->appData.clockMode) {
		/* sprintf(buf, "Black: %s ", TimeString(timeControl));
		   timerWidth = XTextWidth(player->mainFontStruct, buf, strlen(buf)); */
		timerWidth = XTextWidth(player->mainFontStruct, "Black: 8:88:88 ", 15); 
	} else
		timerWidth = XTextWidth(player->mainFontStruct, "Black  ", 7);
	XtSetArg(args[0], XtNwidth, timerWidth);
	XtSetValues(player->blackTimerWidget, args, 1);
	XtSetValues(player->whiteTimerWidget, args, 1);

	XtSetArg(args[0], XtNbackground, &player->timerForegroundPixel);
	XtSetArg(args[1], XtNforeground, &player->timerBackgroundPixel);
	XtGetValues(player->blackTimerWidget, args, 2);

	/*
	 * Calculate the width of the name and message labels.
	 */
	XtSetArg(args[0], XtNwidth, &commandsWidth);
	XtGetValues(player->commandsWidget, args, 1);
        w = (commandsWidth > boardWidth) ? commandsWidth : boardWidth;
	XtSetArg(args[0], XtNwidth, w - timerWidth*2 - 12);
	XtSetValues(player->titleWidget, args, 1);
	XtSetArg(args[0], XtNwidth, w - 8);
	XtSetValues(player->messageWidget, args, 1);

	/*
	 * formWidget uses these constraints but they are stored
	 * in the children.
	 */
	XtSetArg(args[0], XtNfromHoriz, player->blackTimerWidget);
	XtSetValues(player->whiteTimerWidget, args, 1);
	XtSetArg(args[0], XtNfromHoriz, player->whiteTimerWidget);
	XtSetValues(player->titleWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, player->blackTimerWidget);
	XtSetValues(player->messageWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, player->messageWidget);
	XtSetValues(player->commandsWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, player->commandsWidget);
	XtSetValues(player->boardWidget, args, 1);
        
#ifdef ISS
        if (player->appData.issActive) {
	  XtAppAddInput(appContext, iss_input,
		      (XtPointer) (XtInputExceptMask|XtInputReadMask),
		      (XtInputCallbackProc) read_from_iss,
		      (XtPointer) NULL);
	  XtAppAddInput(appContext, fileno(stdin),
		      (XtPointer) XtInputReadMask,
		      (XtInputCallbackProc) read_from_player,
		      (XtPointer) NULL);
        } 
#endif
    
	XtRealizeWidget(player->shellWidget);

	player->xBoardWindow = XtWindow(player->boardWidget);

	/*
	 * Create an icon.
	 */
	player->iconPixmap = 
	  XCreateBitmapFromData(player->xDisplay, XtWindow(player->shellWidget),
		icon_bits, icon_width, icon_height);
	XtSetArg(args[0], XtNiconPixmap, player->iconPixmap);
	XtSetValues(player->shellWidget, args, 1);

	/*
	 * Create a cursor for the board widget.
	 */
	window_attributes.cursor = XCreateFontCursor(player->xDisplay, XC_hand2);
	XChangeWindowAttributes(player->xDisplay, player->xBoardWindow,
		CWCursor, &window_attributes);

	/*
	 * Inhibit shell resizing.
	 */
	player->shellArgs[0].value = (XtArgVal) &w;
	player->shellArgs[1].value = (XtArgVal) &h;
	XtGetValues(player->shellWidget, player->shellArgs, 2);
	player->shellArgs[4].value = player->shellArgs[2].value = w;
	player->shellArgs[5].value = player->shellArgs[3].value = h;
	XtSetValues(player->shellWidget, &player->shellArgs[2], 4);
                                                
	/*
	 * Determine value of black pixel.
	 */
	player->black_pixel_is_zero = 
	  (XBlackPixel(player->xDisplay,player->xScreen) == 0);

	CreateGCs ();
	CreateGrid ();
	CreatePieces ();
	if ( !fromRemotePlayer )
	  CreatePieceMenus ();

	XtAddCallback(player->commandsWidget, XtNcallback, SelectCommand, (XtPointer)fromRemotePlayer);
	
	if ( !fromRemotePlayer )
	  XtAppAddActions(appContext, boardActions, XtNumber(boardActions));

	if ( fromRemotePlayer ) {
	  XtSetArg(args[0], XtNtranslations,
		XtParseTranslationTable(translationsTableReduced));
	  /* Disable key commands because often keys are pressed
             in the board window if using another talk window. */
	  XtSetValues(player->boardWidget, &args[0], 1);
	  XtSetValues(localPlayer.boardWidget, &args[0], 1);
	} else {
	  XtSetArg(args[0], XtNtranslations,
		XtParseTranslationTable(translationsTable));
	  XtSetValues(player->boardWidget, &args[0], 1);
	  XtSetArg(args[0], XtNtranslations,
		XtParseTranslationTable(blackTranslations));
	  XtSetValues(player->blackTimerWidget, &args[0], 1);
	  XtSetArg(args[0], XtNtranslations,
		XtParseTranslationTable(whiteTranslations));
	  XtSetValues(player->whiteTimerWidget, &args[0], 1);
	}

	XtAddEventHandler(player->boardWidget, ExposureMask | ButtonPressMask
		| ButtonReleaseMask | Button1MotionMask | KeyPressMask,
		False, (XtEventHandler) EventProc, (XtPointer)(player == &remotePlayer));

        sprintf(buf, "xshogi version %s, patchlevel %s based on xboard version %s", 
                  version, patchlevel, XBOARD_VERSION);

	/*
	 * If there is to be a machine match, set it up.
	 */
	if (matchMode != MatchFalse && player != &remotePlayer){
		if (player->appData.noChessProgram) {
			fprintf(stderr,
			  "%s: can't have a match with no chess programs!\n",
			  programName);
			exit(1);
		}
		DisplayMessage(buf,fromRemotePlayer);
		TwoMachinesProc(NULL,NULL,NULL,NULL);
	}
	else {
		Reset(True);
		DisplayMessage(buf,fromRemotePlayer);
	}

}


void
main(argc, argv)
	int argc;
	char *argv[];
{  
    XSetWindowAttributes window_attributes;
    char buf[MSG_SIZ];
    Arg args[3];
#ifdef ISS
    int ok;
#endif

    setbuf(stdout, NULL); setbuf(stderr, NULL);
                                                     
    /* 
     * Copy pointers to command line arguments and number of such pointers.
     * (argc, argv will be destroyed by XtAppInitialize) 
     */
    for ( global_argc = 0; global_argc < argc; global_argc++ )
      global_argv[global_argc] = argv[global_argc];

    programName = strrchr(argv[0], '/');
    if (programName == NULL)
      programName = argv[0];
    else
      programName++;
                
    localPlayer.shellWidget = XtAppInitialize(&appContext, "XShogi", shellOptions,
		XtNumber(shellOptions), &argc, argv, xshogiResources, NULL, 0);
    if (argc > 1)
	Usage();

    if ((chessDir = (char *) getenv("SHOGIDIR")) == NULL) {
	chessDir = ".";
    } else {
	if (chdir(chessDir) != 0) {
		fprintf(stderr, "%s: can't cd to SHOGIDIR\n",
				programName);
		perror(chessDir);
		exit(1);
	}
    }

    XtGetApplicationResources(localPlayer.shellWidget, &localPlayer.appData, clientResources,
		XtNumber(clientResources), NULL, 0);

    xshogiDebug = localPlayer.appData.debugMode;

    /*        
     * Determine matchMode state -- poor man's resource converter
     */
    if (StrCaseCmp(localPlayer.appData.matchMode, "Init") == 0)
	matchMode = MatchInit;
    else if (StrCaseCmp(localPlayer.appData.matchMode, "Position") == 0)
	matchMode = MatchPosition;
    else if (StrCaseCmp(localPlayer.appData.matchMode, "Opening") == 0)
	matchMode = MatchOpening;
    else if (StrCaseCmp(localPlayer.appData.matchMode, "False") == 0)
	matchMode = MatchFalse;
    else {
	fprintf(stderr, "%s: bad matchMode option %s\n",
		programName, localPlayer.appData.matchMode);
	Usage();
    }
    
#ifdef ISS
    /*
     * Parse internet chess server status
     */
    if (localPlayer.appData.issActive) {
	fprintf(stderr,
		"%s: trying to connect to ISS host %s port %d\n",
		programName, localPlayer.appData.issHost, localPlayer.appData.issPort);
	ok = establish(localPlayer.appData.issHost,
		       (unsigned short)localPlayer.appData.issPort);
	if (ok == -1) {
	    fprintf(stderr,
		    "%s: could not connect to host %s, port %d: ",
		    programName, localPlayer.appData.issHost, localPlayer.appData.issPort);
	    perror("");
	    exit(1);
	} else {
	    fprintf(stderr,
		    "%s: could connect to host %s, port %d: ",
		    programName, localPlayer.appData.issHost, localPlayer.appData.issPort);
	}
	localPlayer.appData.noChessProgram = True;
	buttonStrings = issButtonStrings;
	buttonProcs = issButtonProcs;
	buttonCount = XtNumber(issButtonStrings);
    } else
#endif 
    {
	buttonStrings = gnuButtonStrings;
	buttonProcs = gnuButtonProcs;
	buttonCount = XtNumber(gnuButtonStrings);
    } 
    
    player = &localPlayer;

    CreatePlayerWindow();  

    XtAppMainLoop(appContext);
}


#ifdef ISS

/*
 * Establish will establish a contact to a remote host.port.
 * Returns 0 if okay, -1 if not.
 */
int establish(host, port)
     char *host;
     int port;
{
    int s;
    char str[100];
    int to_prog[2], from_prog[2];
    struct sockaddr_in sa;
    struct hostent     *hp;
    unsigned short uport;
#if defined(SYSTEM_FIVE) || defined(SYSV)
    char *pty_name;
#endif
    
    if (localPlayer.appData.useTelnet || (*localPlayer.appData.gateway != NULLCHAR)) {
#if defined(SYSTEM_FIVE) || defined(SYSV)
	if ((pty_name = PseudoTTY(&to_prog[1])) == NULL) {
	    fprintf(stderr, "%s: can't open pseudo-tty: ", programName);
	    perror("");
	    exit(1);
	}
	from_prog[0] = to_prog[1];
	to_prog[0] = from_prog[1] = open(pty_name, O_RDWR, 0);
#ifdef SVR4
	if (ioctl (to_prog[0], I_PUSH, "ptem") == -1 ||
	    ioctl (to_prog[0], I_PUSH, "ldterm") == -1 ||
	    ioctl (to_prog[0], I_PUSH, "ttcompat") == -1) {
	    fprintf(stderr, "%s: can't ioctl pseudo-tty: ", programName);
	    perror("");
	    exit(1);
	}
#endif
#else
	pipe(to_prog);
	pipe(from_prog);
#endif
	if ((telnetPID = fork()) == 0) {
	    dup2(to_prog[0], 0);
	    dup2(from_prog[1], 1);
	    close(to_prog[0]);
	    close(to_prog[1]);
	    close(from_prog[0]);
	    close(from_prog[1]);
	    dup2(1, fileno(stderr)); /* force stderr to the pipe */
	    
	    uport = (unsigned short) port;
	    sprintf(str, "%d", uport);
	    if (*localPlayer.appData.gateway != NULLCHAR) {
		execlp(localPlayer.appData.remoteShell, localPlayer.appData.remoteShell,
		       localPlayer.appData.gateway, localPlayer.appData.telnetProgram,
		       host, str, (char *) NULL);
	    } else {
		execlp(localPlayer.appData.telnetProgram, localPlayer.appData.telnetProgram,
		       host, str, (char *) NULL);
	    }
	    perror(localPlayer.appData.telnetProgram);
	    exit(1);
	}
	close(to_prog[0]);
	close(from_prog[1]);
	iss_input = from_prog[0];
	iss_output = to_prog[1];
    } else {
#ifdef SVR4
        memset((char *) &sa, (int)0, sizeof(struct sockaddr_in));
#else				/*!SVR4*/
        bzero((char *) &sa, sizeof(struct sockaddr_in));
#endif
	if (!(hp = gethostbyname(host))) {
	    int b0, b1, b2, b3;
	    if (sscanf(host, "%d.%d.%d.%d", &b0, &b1, &b2, &b3) == 4) {
		hp = (struct hostent *) calloc(1, sizeof(struct hostent));
		hp->h_addrtype = AF_INET;
		hp->h_length = 4;
		hp->h_addr_list = (char **) calloc(2, sizeof(char *));
		hp->h_addr_list[0] = (char *) malloc(4);
		hp->h_addr_list[0][0] = b0;
		hp->h_addr_list[0][1] = b1;
		hp->h_addr_list[0][2] = b2;
		hp->h_addr_list[0][3] = b3;
	    } else {
		fprintf(stderr, "%s: could not gethostbyname %s\n",
			programName, host);
		return(-1);
	    }
	}
	sa.sin_family = hp->h_addrtype;
	uport = (unsigned short) port;
	sa.sin_port = htons(uport);
	
#ifdef SVR4
        memcpy(hp->h_addr, (char *) &sa.sin_addr, hp->h_length);
#else				/*!SVR4*/
        bcopy(hp->h_addr, (char *) &sa.sin_addr, hp->h_length);
#endif

	if ((s = socket(AF_INET, SOCK_STREAM, 6)) < 0) {
	    fprintf(stderr, "%s: could not get socket\n", programName);
	    return(-1);
	}
	if (connect(s, (struct sockaddr *) &sa, 
	            sizeof(struct sockaddr_in)) < 0) {
	    fprintf(stderr, "%s: could not bind socket\n", programName);
	    return(-1);
	}
	iss_input = iss_output = s;
    }
    return(0);
}

void read_from_player(client_data, file_num, id)
     caddr_t client_data;
     int *file_num;
     XtInputId *id;
{
#define BUF_SIZE 1024
    int s = *file_num;
    static char buf[BUF_SIZE];
    int buf_len, tmp_len;
    int out_len = 0;
    
    buf_len = read(s, buf, BUF_SIZE);
    
    if (buf_len > 0) {
	while (out_len < buf_len) {
	    tmp_len = write(iss_output, &buf[out_len], buf_len - out_len);
	    if (tmp_len == -1) {
		fprintf(stderr, "%s: error writing to ISS: ", programName);
		perror("");
		exit(1);
	    }
	    out_len += tmp_len;
	}
    } else {
	fprintf(stderr, "%s: got end of file from keyboard\n", programName);
	if (telnetPID != 0) {
	    if (kill(telnetPID, SIGTERM) == 0)
	      wait((union wait *) 0);
	}
	exit(0);
    }
}

void SendToISS(s)
     char *s;
{
    int i, j, tmp;
    
    i = strlen(s);
    j = 0;
    
    if (localPlayer.appData.debugMode)
      fprintf(stderr, "Sending to ISS: %s", s);

    while (j < i) {
	tmp = write(iss_output, &s[j], i - j);
	if (tmp == -1) {
	    fprintf(stderr, "%s: error writing to ISS: ", programName);
	    perror("");
	    exit(1);
	}
	j += tmp;
    }
}


static int leftover_start = 0, leftover_len = 0;
static char star_match[8][256];

/* Test whether pattern is present at &buf[*index]; if so, return True,
   advance *index beyond it, and set leftover_start to the new value of
   *index; else return False.  If pattern contains the character '*', it
   matches any sequence of characters not containing '\r', '\n', or the
   character following the '*' (if any), and the matched sequence(s) are
   copied into star_match.  The pattern must not contain '\r' or '\n'.
*/
Boolean looking_at(buf, index, pattern)
     char *buf;
     int *index;
     char *pattern;
{
    char *bufp = &buf[*index], *patternp = pattern;
    int star_count = 0;
    char *matchp = star_match[0];
    
    for (;;) {
	if (*patternp == NULLCHAR) {
	    *index = leftover_start = bufp - buf;
	    *matchp = NULLCHAR;
	    return True;
	}
	if (*bufp == NULLCHAR) return False;
	if (*patternp == '*') {
	    if (*bufp == *(patternp + 1)) {
		*matchp = NULLCHAR;
		matchp = star_match[++star_count];
		patternp += 2;
		bufp++;
		continue;
	    } else if (*bufp == '\n' || *bufp == '\r') {
		patternp++;
		if (*patternp == NULLCHAR)
		  continue;
		else
		  return False;
	    } else {
		*matchp++ = *bufp++;
		continue;
	    }
	}
	if (*patternp != *bufp) return False;
	patternp++;
	bufp++;
    }
}


void read_from_iss(client_data, file_num, id)
     caddr_t client_data;
     int *file_num;
     XtInputId *id;
{
#define BUF_SIZE 1024
#define BOARD 1
#define MOVES 2
    
    static int started = 0;
    static char parse[20000];
    static int  parse_pos;
    static char buf[BUF_SIZE + 1];
    
    char str[500];
    int i, oldi;
    int buf_len;
    int next_out;
    
    /* If last read ended with a partial line that we couldn't parse,
       prepend it to the new read and try again. */
    if (leftover_len > 0) {
	for (i=0; i<leftover_len; i++)
	  buf[i] = buf[leftover_start + i];
    }
    buf_len = read(iss_input, &buf[leftover_len], BUF_SIZE - leftover_len);
    next_out = leftover_len;
    
    if (buf_len > 0) {
	buf_len += leftover_len;
	leftover_start = 0;
	buf[buf_len] = NULLCHAR;
	
	i = 0;
	while (i < buf_len) {

	    /* login to ISS */
	    if (looking_at(buf, &i, "Login: ")) {       
		continue;
	    }
	    if (looking_at(buf, &i, "Password: ")) {
		continue;
	    }
	    
	    /* Skip over what people say */
	    if (looking_at(buf, &i, "shouts: *") ||
		looking_at(buf, &i, "tells you: *") ||
		looking_at(buf, &i, "says: *") ||
		looking_at(buf, &i, "whispers: *") ||
		looking_at(buf, &i, "kibitzes: *")) {
		if (strncmp(star_match[0], tstwd, strlen(tstwd)) == 0) {
		    SendToISS(star_match[0] + strlen(tstwd));
		    SendToISS("\n");
		}
		continue;
	    }
                                
	    if (looking_at(buf, &i, "Game set to*")) {
		continue;
	    }

	    oldi = i;
	    if (!iss_getting_history && looking_at(buf, &i, "Game * (S)")) {
		started = BOARD;
		parse_pos = 0;
		fwrite(&buf[next_out], oldi - next_out, 1, stdout);
		continue;
	    } 

	    if (started == BOARD && looking_at(buf, &i, "#>")) {
		/* Board read is done */
		started = 0;
		next_out = i;
		parse[parse_pos] = NULLCHAR;

		if ( ParseBoard(parse) ) {
		  /* Display the board */
		  if (gameMode != PauseGame) {
		    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		    DisplayClocks(StartTimers); 
		    if (localPlayer.appData.ringBellAfterMoves && !iss_user_moved)
		      putc(BELLCHAR, stderr);
		  }
		} else {
		  DisplayMessage("Error while parsing board",False);
		  fprintf(stderr, "%s\n",parse);
		}
		iss_user_moved = 0;
		continue;
	    }
	    
	    oldi = i;
	    if (iss_getting_history && looking_at(buf, &i, "Game ")) {
		/* Beginning of move list */
		started = MOVES;
		parse_pos = 0;
		fwrite(&buf[next_out], oldi - next_out, 1, stdout);
		continue;
	    }				
	    
	    if(looking_at(buf, &i, "#> ")) {
		switch (started) {
		  case 0:
		    continue;
		  case BOARD:
		    /* Something went wrong; found a prompt while
		       accumulating a board */
		    started = 0;
		    fprintf(stderr, "%s: error gathering board\n",
			    programName);
		    continue;
		  case MOVES:
		    started = 0;
		    iss_getting_history = False;
		    parse[parse_pos] = NULLCHAR;
		    if (iss_mode == IssIdle) {
			/* Moves came from user "oldmoves" command */
			Reset(False);
			if ( ParseGameHistory(parse) ) {
			  currentMove = forwardMostMove;
			  DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
			  DisplayClocks(ReDisplayTimers);
			  sprintf(str, "%s vs. %s", iss_white, iss_black);
			  DisplayTitle(str);              
			} else {                          
			  DisplayMessage("couldn't parse game history",False);
			}
		    } else {
			if ( ParseGameHistory(parse) ) {
			} else {
			  DisplayMessage("couldn't parse game history",False);
			}
		    }
		    DisplayMove(currentMove - 1);
		    SendToISS("\n");  /*kludge: force a prompt*/
		    next_out = i;
		    continue;
		}
	    }
	    
	    if (started && i >= leftover_len) {
		/* there is currently an error on ISS in the text area
		   of captured pieces */
	        if ( buf[i] < ' ' && 
		     buf[i] != (char)10 &&
		     buf[i] != (char)13 &&
		     buf[i] != '\r' ) {
                  if ( iss_capture_bug == 0 ) {
	            fprintf(stderr, "ISS capture bug still there...\n");
	            fprintf(stderr, "illegal character while parsing board, ord=%d\n", (int)buf[i]);
		    iss_capture_bug = 1;
		  }
		  buf[i] = '\r'; 
    	        }
		/* Accumulate characters in board
		   or move list*/
		if (buf[i] != '\r')
		  parse[parse_pos++] = buf[i];
	    }
	    
	    /* Start of observation messages */ 
	    if (looking_at(buf, &i, "Adding game*")) {
		InitPosition(True);
		DisplayMessage("observing new game",False);
		sprintf(str, "refresh %d\n", atoi(star_match[0]));
		SendToISS(str);
		iss_mode = IssObserving;
		continue;
	    }
	    
	    /* Start of match messages */ 
	    if (looking_at(buf, &i, "Match*") ||
		looking_at(buf, &i, "Creating match*")) {
		InitPosition(True);
		DisplayMessage("new match started",False);
		iss_mode = IssPlayingWhite;
		continue;
	    }
	    
	    /* Error messages */
	    if (iss_user_moved) {
		if (looking_at(buf, &i, "No such command") ||
		    looking_at(buf, &i, "Illegal move") ||
		    looking_at(buf, &i, "Not a legal move") ||
		    looking_at(buf, &i, "Your king is in check") ||
		    looking_at(buf, &i, "It isn't your turn")) {
		    /**** Illegal move ****/
		    iss_user_moved = 0;
		    if (forwardMostMove > backwardMostMove) {
			currentMove = --forwardMostMove;
			DisplayMessage("Illegal move",False);
			DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
			DisplayClocks(SwitchTimers);
		    }
		    continue;
		}   
		if (looking_at(buf, &i, "*Unknown command*")) {
		    /**** possibly not in match mode  ****/
		    iss_user_moved = 0;
		    if (forwardMostMove > backwardMostMove) {
			currentMove = --forwardMostMove;
			DisplayMessage("not in match mode",False);
			DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
			DisplayClocks(SwitchTimers);
		    }
		    continue;
		}   
	    }

	    if (looking_at(buf, &i, "You and your opponent still have time")) {
		/* We must have called his flag a little too soon */
		whiteFlag = blackFlag = False;
		continue;
	    }

	    /* End-of-game messages */
	    if (looking_at(buf, &i, "{Game * (* vs. *)* * *}")) {
		/* New style generic game start/end messages */
		/* star_match[0] is the game number */
		/*           [1] is the white player's name */
		/*           [2] is the black player's name */
		/*           [3] is either ":" or empty (don't care) */
		/*           [4] is usually the loser's name or a noise word */
		/*           [5] contains the reason for the game end */
		int gamenum = atoi(star_match[0]);
		char *white = star_match[1];
		char *loser = star_match[4];
		char *why = star_match[5];
		
		if (iss_gamenum != gamenum) continue;

		if (StrStr(why, "checkmate")) {
		    if (strcmp(loser, white) == 0)
		      GameEnds("Black mates");
		    else
		      GameEnds("White mates");
		} else if (StrStr(why, "resign")) {
		    if (strcmp(loser, white) == 0)
		      GameEnds("White resigns");
		    else
		      GameEnds("Black resigns");
		} else if (StrStr(why, "forfeits on time")) {
		    if (strcmp(loser, white) == 0)
		      GameEnds("Black wins on time");
		    else
		      GameEnds("White wins on time");
		} else if (StrStr(why, "stalemate")) {
		    GameEnds("Stalemate");
		} else if (StrStr(why, "drawn by mutual agreement")) {
		    GameEnds("Draw agreed");
		} else if (StrStr(why, "repetition")) {
		    GameEnds("Draw by repetition");
		} else if (StrStr(why, "50")) {
		    GameEnds("Draw (50 move rule)");
		} else if (StrStr(why, "neither player has mating")) {
		    GameEnds("Draw (insufficient material)");
		} else if (StrStr(why, "no material")) {
		    GameEnds("Draw (insufficient material to win on time)");
		} else if (StrStr(why, "time")) {
		    GameEnds("Draw (both players ran out of time)");
		} else if (StrStr(why, "disconnected and forfeits")) {
		    /* in this case the word "abuser" preceded the loser */
		    loser = why;
		    why = strchr(loser, ' ');
		    *why++ = NULLCHAR;
		    if (strcmp(loser, white) == 0)
		      GameEnds("Black wins (forfeit)");
		    else
		      GameEnds("White wins (forfeit)");
		} else if (StrStr(why, "assert")) {
		    /* "loser" is actually the winner in this case */
		    if (strcmp(loser, white) == 0)
		      GameEnds("White asserts a win");
		    else
		      GameEnds("Black asserts a win");
		} else if (StrStr(why, "aborted")) {
		    DisplayClocks(StopTimers);
		    DisplayMessage("Game aborted",False);
		    iss_mode = IssIdle;
		    iss_gamenum = -1;
		    iss_user_moved = False;
		} else if (StrStr(why, "removed")) {
		    DisplayClocks(StopTimers);
		    DisplayMessage("Game aborted",False);
		    iss_mode = IssIdle;
		    iss_gamenum = -1;
		    iss_user_moved = False;
		} else if (StrStr(why, "adjourn")) {
		    DisplayClocks(StopTimers);
		    DisplayMessage("Game adjourned",False);
		    iss_mode = IssIdle;
		    iss_gamenum = -1;
		    iss_user_moved = False;
		}   
		continue;
	    }

	    if (looking_at(buf, &i, "Removing game * from observation list")) {
		if (iss_mode == IssObserving &&
		    atoi(star_match[0]) == iss_gamenum)
		  {
		      DisplayClocks(StopTimers);
		      iss_mode = IssIdle;
		      iss_gamenum = -1;
		      iss_user_moved = False;
		  }
		continue;
	    }

	    /* Advance leftover_start past any newlines we find,
	       so only partial lines can get reparsed */
	    if (looking_at(buf, &i, "\n")) continue;
	    if (looking_at(buf, &i, "\r")) continue;
	    
	    i++;	/* skip unparsed character and loop back */
	}
	
	if (started == 0)
	  fwrite(&buf[next_out], i - next_out, 1, stdout);
	
	leftover_len = buf_len - leftover_start;
	/* if buffer ends with something we couldn't parse,
	   reparse it after appending the next read */
	
    } else if (buf_len == 0) {
	fprintf(stderr, "%s: connection closed by ISS\n", programName);
	if (telnetPID != 0) {
	    if (kill(telnetPID, SIGTERM) == 0)
	      wait((union wait *) 0);
	}
	exit(0);
    } else {
	fprintf(stderr, "%s: error reading from ISS: ", programName);
	perror("");
	exit(1);
    }
}       


#endif



#define abs(n) ((n)<0 ? -(n) : (n))

/*
 * Find a font that matches "pattern" that is as close as
 * possible to the targetPxlSize.  Prefer fonts that are k
 * pixels smaller to fonts that are k pixels larger.  The
 * pattern must be in the X Consortium standard format, 
 * e.g. "-*-helvetica-bold-r-normal--*-*-*-*-*-*-*-*".
 * The return value should be freed with XtFree when no
 * longer needed.
 */
char *FindFont(pattern, targetPxlSize)
     char *pattern;
     int targetPxlSize;
{
    char **fonts, *p, *best;
    int i, j, nfonts, minerr, err, pxlSize;

    fonts = XListFonts(player->xDisplay, pattern, 999999, &nfonts);
    if (nfonts < 1) {
	fprintf(stderr, "%s: No fonts match pattern %s\n",
		programName, pattern);
	exit(1);
    }
    best = "";
    minerr = 999999;
    for (i=0; i<nfonts; i++) {
	j = 0;
	p = fonts[i];
	if (*p != '-') continue;
	while (j < 7) {
	    if (*p == NULLCHAR) break;
	    if (*p++ == '-') j++;
	}
	if (j < 7) continue;
	pxlSize = atoi(p);
	if (pxlSize == targetPxlSize) {
	    best = fonts[i];
	    break;
	}
	err = pxlSize - targetPxlSize;
	if (abs(err) < abs(minerr) ||
	    (minerr > 0 && err < 0 && -err == minerr)) {
	    best = fonts[i];
	    minerr = err;
	}
    }
    p = (char *) XtMalloc(strlen(best) + 1);
    strcpy(p, best);
    XFreeFontNames(fonts);
    return p;
}
                  

void
CreateGCs()
{
	XtGCMask value_mask = GCLineWidth | GCLineStyle | GCForeground
		| GCBackground | GCFunction | GCPlaneMask;
	XGCValues gc_values;

	gc_values.plane_mask = AllPlanes;
	gc_values.line_width = LINE_GAP;
	gc_values.line_style = LineSolid;
	gc_values.function = GXcopy;

	gc_values.foreground = XBlackPixel(player->xDisplay, player->xScreen);
	gc_values.background = XBlackPixel(player->xDisplay, player->xScreen);
	player->lineGC = XtGetGC(player->shellWidget, value_mask, &gc_values);
                                                                              
	gc_values.background = XWhitePixel(player->xDisplay, player->xScreen);
	player->coordGC = XtGetGC(player->shellWidget, value_mask, &gc_values);
  	XSetFont(player->xDisplay, player->coordGC, player->coordFontID);

	if (player->monoMode) {
		gc_values.foreground = XWhitePixel(player->xDisplay, player->xScreen);
		gc_values.background = XBlackPixel(player->xDisplay, player->xScreen);
		player->lightSquareGC = player->darkSquareGC = player->wbPieceGC
			= XtGetGC(player->shellWidget, value_mask, &gc_values);
		gc_values.foreground = XBlackPixel(player->xDisplay, player->xScreen);
		gc_values.background = XWhitePixel(player->xDisplay, player->xScreen);
		player->bwPieceGC
			= XtGetGC(player->shellWidget, value_mask, &gc_values);
	} else {
		/* white piece black background */
		gc_values.foreground = XWhitePixel(player->xDisplay, player->xScreen);
		gc_values.background = XBlackPixel(player->xDisplay, player->xScreen);
		player->wbPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* black piece white background */
		gc_values.foreground = XBlackPixel(player->xDisplay, player->xScreen);
		gc_values.background = XWhitePixel(player->xDisplay, player->xScreen);
		player->bwPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* light empty square */
		gc_values.foreground = player->appData.lightSquareColor;
		gc_values.background = player->appData.darkSquareColor;
		player->lightSquareGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* dark empty square */
		gc_values.foreground = player->appData.darkSquareColor;
		gc_values.background = player->appData.lightSquareColor;
		player->darkSquareGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* black piece on dark square */
		gc_values.background = player->appData.blackPieceColor;
		gc_values.foreground = player->appData.darkSquareColor;
		player->bdPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* black piece on light square */
		gc_values.background = player->appData.blackPieceColor;
		gc_values.foreground = player->appData.lightSquareColor;
		player->blPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* white piece on dark square */
		gc_values.background = player->appData.whitePieceColor;
		gc_values.foreground = player->appData.darkSquareColor;
		player->wdPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* white piece on dark square */
		gc_values.background = player->appData.whitePieceColor;
		gc_values.foreground = player->appData.lightSquareColor;
		player->wlPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* black piece off board */
		gc_values.background = player->appData.blackPieceColor;
		gc_values.foreground = XWhitePixel(player->xDisplay, player->xScreen);
		player->boPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* white piece off board */
		gc_values.background = player->appData.whitePieceColor;
		gc_values.foreground = XWhitePixel(player->xDisplay, player->xScreen);
		player->woPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);

		/* piece symbol */
		gc_values.function = (player->black_pixel_is_zero ? GXand : GXor);

		gc_values.foreground = XBlackPixel(player->xDisplay, player->xScreen);
		gc_values.background = XWhitePixel(player->xDisplay, player->xScreen);
		player->charPieceGC = XtGetGC(player->shellWidget, value_mask, &gc_values);
	}
}                


void
CreatePieces()
{
    XSynchronize(player->xDisplay, True);	/* Work-around for xlib/xt
					   	   buffering bug */
        
#if defined WESTERN_BITMAPS || defined BOTH_BITMAPS

#if defined BOTH_BITMAPS
    if ( player->appData.westernPieceSet ) 
#endif

      {

   	ReadBitmap(player->appData.reverseBigSolidBitmap, &player->reverseBigSolidBitmap,
		   NULL,
		   bigsolidR_bits, bigsolidR_bits, bigsolidR_bits);
	ReadBitmap(player->appData.reverseSmallSolidBitmap, &player->reverseSmallSolidBitmap,
		   NULL,
		   smallsolidR_bits, smallsolidR_bits, smallsolidR_bits);

	ReadBitmap(player->appData.normalBigSolidBitmap, &player->normalBigSolidBitmap,
		   NULL,
		   bigsolid_bits, bigsolid_bits, bigsolid_bits);
	ReadBitmap(player->appData.normalSmallSolidBitmap, &player->normalSmallSolidBitmap,
		   NULL,
		   smallsolid_bits, smallsolid_bits, smallsolid_bits);

	ReadBitmap(player->appData.reversePawnBitmap, &player->reversePawnBitmap,
		   &player->reverseSmallSolidBitmap,
		   pawnRW_bits, pawnRW_bits, pawnRW_bits);
	ReadBitmap(player->appData.reverseLanceBitmap, &player->reverseLanceBitmap,
		   &player->reverseSmallSolidBitmap,
		   lanceRW_bits, lanceRW_bits, lanceRW_bits);
	ReadBitmap(player->appData.reverseKnightBitmap, &player->reverseKnightBitmap,
		   &player->reverseSmallSolidBitmap,
		   knightRW_bits, knightRW_bits, knightRW_bits);
	ReadBitmap(player->appData.reverseSilverBitmap, &player->reverseSilverBitmap,
		   &player->reverseBigSolidBitmap,
		   silverRW_bits, silverRW_bits, silverRW_bits);
	ReadBitmap(player->appData.reverseGoldBitmap, &player->reverseGoldBitmap,
		   &player->reverseBigSolidBitmap,
		   goldRW_bits, goldRW_bits, goldRW_bits);
	ReadBitmap(player->appData.reverseRookBitmap, &player->reverseRookBitmap,
		   &player->reverseBigSolidBitmap,
		   rookRW_bits, rookRW_bits, rookRW_bits);
	ReadBitmap(player->appData.reverseBishopBitmap, &player->reverseBishopBitmap,
		   &player->reverseBigSolidBitmap,
		   bishopRW_bits, bishopRW_bits, bishopRW_bits);
	ReadBitmap(player->appData.reversePPawnBitmap, &player->reversePPawnBitmap,
		   &player->reverseSmallSolidBitmap,
		   pawnPRW_bits, pawnPRW_bits, pawnPRW_bits);
	ReadBitmap(player->appData.reversePLanceBitmap, &player->reversePLanceBitmap,
		   &player->reverseSmallSolidBitmap,
		   lancePRW_bits, lancePRW_bits, lancePRW_bits);
	ReadBitmap(player->appData.reversePKnightBitmap, &player->reversePKnightBitmap,
		   &player->reverseSmallSolidBitmap,
		   knightPRW_bits, knightPRW_bits, knightPRW_bits);
	ReadBitmap(player->appData.reversePSilverBitmap, &player->reversePSilverBitmap,
		   &player->reverseBigSolidBitmap,
		   silverPRW_bits, silverPRW_bits, silverPRW_bits);
	ReadBitmap(player->appData.reversePRookBitmap, &player->reversePRookBitmap,
		   &player->reverseBigSolidBitmap,
		   rookPRW_bits, rookPRW_bits, rookPRW_bits);
	ReadBitmap(player->appData.reversePBishopBitmap, &player->reversePBishopBitmap,
		   &player->reverseBigSolidBitmap,
		   bishopPRW_bits, bishopPRW_bits, bishopPRW_bits);
	ReadBitmap(player->appData.reverseKingBitmap, &player->reverseKingBitmap,
		   &player->reverseBigSolidBitmap,
		   kingRW_bits, kingRW_bits, kingRW_bits);

	ReadBitmap(player->appData.normalPawnBitmap, &player->normalPawnBitmap,
		   &player->normalSmallSolidBitmap,
		   pawnW_bits, pawnW_bits, pawnW_bits);
	ReadBitmap(player->appData.normalLanceBitmap, &player->normalLanceBitmap,
		   &player->normalSmallSolidBitmap,
		   lanceW_bits, lanceW_bits, lanceW_bits);
	ReadBitmap(player->appData.normalKnightBitmap, &player->normalKnightBitmap,
		   &player->normalSmallSolidBitmap,
		   knightW_bits, knightW_bits, knightW_bits);
	ReadBitmap(player->appData.normalSilverBitmap, &player->normalSilverBitmap,
		   &player->normalBigSolidBitmap,
		   silverW_bits, silverW_bits, silverW_bits);
	ReadBitmap(player->appData.normalGoldBitmap, &player->normalGoldBitmap,
		   &player->normalBigSolidBitmap,
		   goldW_bits, goldW_bits, goldW_bits);
	ReadBitmap(player->appData.normalRookBitmap, &player->normalRookBitmap,
		   &player->normalBigSolidBitmap,
		   rookW_bits, rookW_bits, rookW_bits);
	ReadBitmap(player->appData.normalBishopBitmap, &player->normalBishopBitmap,
		   &player->normalBigSolidBitmap,
		   bishopW_bits, bishopW_bits, bishopW_bits);
	ReadBitmap(player->appData.normalPPawnBitmap, &player->normalPPawnBitmap,
		   &player->normalSmallSolidBitmap,
		   pawnPW_bits, pawnPW_bits, pawnPW_bits);
	ReadBitmap(player->appData.normalPLanceBitmap, &player->normalPLanceBitmap,
		   &player->normalSmallSolidBitmap,
		   lancePW_bits, lancePW_bits, lancePW_bits);
	ReadBitmap(player->appData.normalPKnightBitmap, &player->normalPKnightBitmap,
		   &player->normalSmallSolidBitmap,
		   knightPW_bits, knightPW_bits, knightPW_bits);
	ReadBitmap(player->appData.normalPSilverBitmap, &player->normalPSilverBitmap,
		   &player->normalBigSolidBitmap,
		   silverPW_bits, silverPW_bits, silverPW_bits);
	ReadBitmap(player->appData.normalPRookBitmap, &player->normalPRookBitmap,
		   &player->normalBigSolidBitmap,
		   rookPW_bits, rookPW_bits, rookPW_bits);
	ReadBitmap(player->appData.normalPBishopBitmap, &player->normalPBishopBitmap,
		   &player->normalBigSolidBitmap,
		   bishopPW_bits, bishopPW_bits, bishopPW_bits);
	ReadBitmap(player->appData.normalKingBitmap, &player->normalKingBitmap,
		   &player->normalBigSolidBitmap,
		   kingW_bits, kingW_bits, kingW_bits);

      }
     
#endif

#if !defined WESTERN_BITMAPS || defined BOTH_BITMAPS

#if defined BOTH_BITMAPS
    else
#endif

      {
                                     
   	ReadBitmap(player->appData.reverseBigSolidBitmap, &player->reverseBigSolidBitmap,
		   NULL,
		   bigsolidR_bits, bigsolidR_m_bits, bigsolidR_l_bits);
	ReadBitmap(player->appData.reverseSmallSolidBitmap, &player->reverseSmallSolidBitmap,
		   NULL,
		   smallsolidR_bits, smallsolidR_m_bits, smallsolidR_l_bits);

	ReadBitmap(player->appData.normalBigSolidBitmap, &player->normalBigSolidBitmap,
		   NULL,
		   bigsolid_bits, bigsolid_m_bits, bigsolid_l_bits);
	ReadBitmap(player->appData.normalSmallSolidBitmap, &player->normalSmallSolidBitmap,
		   NULL,
		   smallsolid_bits, smallsolid_m_bits, smallsolid_l_bits);

	ReadBitmap(player->appData.reversePawnBitmap, &player->reversePawnBitmap,
		   &player->reverseSmallSolidBitmap,
		   pawnR_bits, pawnR_m_bits, pawnR_l_bits);
	ReadBitmap(player->appData.reverseLanceBitmap, &player->reverseLanceBitmap,
		   &player->reverseSmallSolidBitmap,
		   lanceR_bits, lanceR_m_bits, lanceR_l_bits);
	ReadBitmap(player->appData.reverseKnightBitmap, &player->reverseKnightBitmap,
		   &player->reverseSmallSolidBitmap,
		   knightR_bits, knightR_m_bits, knightR_l_bits);
	ReadBitmap(player->appData.reverseSilverBitmap, &player->reverseSilverBitmap,
		   &player->reverseBigSolidBitmap,
		   silverR_bits, silverR_m_bits, silverR_l_bits);
	ReadBitmap(player->appData.reverseGoldBitmap, &player->reverseGoldBitmap,
		   &player->reverseBigSolidBitmap,
		   goldR_bits, goldR_m_bits, goldR_l_bits);
	ReadBitmap(player->appData.reverseRookBitmap, &player->reverseRookBitmap,
		   &player->reverseBigSolidBitmap,
		   rookR_bits, rookR_m_bits, rookR_l_bits);
	ReadBitmap(player->appData.reverseBishopBitmap, &player->reverseBishopBitmap,
		   &player->reverseBigSolidBitmap,
		   bishopR_bits, bishopR_m_bits, bishopR_l_bits);
	ReadBitmap(player->appData.reversePPawnBitmap, &player->reversePPawnBitmap,
		   &player->reverseSmallSolidBitmap,
		   pawnPR_bits, pawnPR_m_bits, pawnPR_l_bits);
	ReadBitmap(player->appData.reversePLanceBitmap, &player->reversePLanceBitmap,
		   &player->reverseSmallSolidBitmap,
		   lancePR_bits, lancePR_m_bits, lancePR_l_bits);
	ReadBitmap(player->appData.reversePKnightBitmap, &player->reversePKnightBitmap,
		   &player->reverseSmallSolidBitmap,
		   knightPR_bits, knightPR_m_bits, knightPR_l_bits);
	ReadBitmap(player->appData.reversePSilverBitmap, &player->reversePSilverBitmap,
		   &player->reverseBigSolidBitmap,
		   silverPR_bits, silverPR_m_bits, silverPR_l_bits);
	ReadBitmap(player->appData.reversePRookBitmap, &player->reversePRookBitmap,
		   &player->reverseBigSolidBitmap,
		   rookPR_bits, rookPR_m_bits, rookPR_l_bits);
	ReadBitmap(player->appData.reversePBishopBitmap, &player->reversePBishopBitmap,
		   &player->reverseBigSolidBitmap,
		   bishopPR_bits, bishopPR_m_bits, bishopPR_l_bits);
	ReadBitmap(player->appData.reverseKingBitmap, &player->reverseKingBitmap,
		   &player->reverseBigSolidBitmap,
		   kingR_bits, kingR_m_bits, kingR_l_bits);

	ReadBitmap(player->appData.normalPawnBitmap, &player->normalPawnBitmap,
		   &player->normalSmallSolidBitmap,
		   pawn_bits, pawn_m_bits, pawn_l_bits);
	ReadBitmap(player->appData.normalLanceBitmap, &player->normalLanceBitmap,
		   &player->normalSmallSolidBitmap,
		   lance_bits, lance_m_bits, lance_l_bits);
	ReadBitmap(player->appData.normalKnightBitmap, &player->normalKnightBitmap,
		   &player->normalSmallSolidBitmap,
		   knight_bits, knight_m_bits, knight_l_bits);
	ReadBitmap(player->appData.normalSilverBitmap, &player->normalSilverBitmap,
		   &player->normalBigSolidBitmap,
		   silver_bits, silver_m_bits, silver_l_bits);
	ReadBitmap(player->appData.normalGoldBitmap, &player->normalGoldBitmap,
		   &player->normalBigSolidBitmap,
		   gold_bits, gold_m_bits, gold_l_bits);
	ReadBitmap(player->appData.normalRookBitmap, &player->normalRookBitmap,
		   &player->normalBigSolidBitmap,
		   rook_bits, rook_m_bits, rook_l_bits);
	ReadBitmap(player->appData.normalBishopBitmap, &player->normalBishopBitmap,
		   &player->normalBigSolidBitmap,
		   bishop_bits, bishop_m_bits, bishop_l_bits);
	ReadBitmap(player->appData.normalPPawnBitmap, &player->normalPPawnBitmap,
		   &player->normalSmallSolidBitmap,
		   pawnP_bits, pawnP_m_bits, pawnP_l_bits);
	ReadBitmap(player->appData.normalPLanceBitmap, &player->normalPLanceBitmap,
		   &player->normalSmallSolidBitmap,
		   lanceP_bits, lanceP_m_bits, lanceP_l_bits);
	ReadBitmap(player->appData.normalPKnightBitmap, &player->normalPKnightBitmap,
		   &player->normalSmallSolidBitmap,
		   knightP_bits, knightP_m_bits, knightP_l_bits);
	ReadBitmap(player->appData.normalPSilverBitmap, &player->normalPSilverBitmap,
		   &player->normalBigSolidBitmap,
		   silverP_bits, silverP_m_bits, silverP_l_bits);
	ReadBitmap(player->appData.normalPRookBitmap, &player->normalPRookBitmap,
		   &player->normalBigSolidBitmap,
		   rookP_bits, rookP_m_bits, rookP_l_bits);
	ReadBitmap(player->appData.normalPBishopBitmap, &player->normalPBishopBitmap,
		   &player->normalBigSolidBitmap,
		   bishopP_bits, bishopP_m_bits, bishopP_l_bits);
	ReadBitmap(player->appData.normalKingBitmap, &player->normalKingBitmap,
		   &player->normalBigSolidBitmap,
		   king_bits, king_m_bits, king_l_bits);
       
      }
#endif

    XSynchronize(player->xDisplay, False);	/* Work-around for xlib/xt
					   	   buffering bug */
}           
        

                    
int 
ReadBitmapFile(display, d, filename, width_return,
     height_return, bitmap_return,
     x_hot_return, y_hot_return)
         Display *display;
         Drawable d;
         char *filename;
         unsigned int *width_return, *height_return;
         Pixmap *bitmap_return;
         int *x_hot_return, *y_hot_return;
{ 
  int n;                
  if ((n = XReadBitmapFile(display, d, filename,
		width_return, height_return, bitmap_return, x_hot_return, y_hot_return)) 
              != BitmapSuccess)
    return(n);
  else
    { 
      /* transform a 1 plane pixmap to a k plane pixmap */
      return(BitmapSuccess);
    }
}


void
ReadBitmap(name, pm, qm, small_bits, medium_bits, large_bits)
	String name;
	Pixmap *pm, *qm;
	char small_bits[], medium_bits[], large_bits[];
{
	int x_hot, y_hot;
	u_int w, h;    

	if (name == NULL || ReadBitmapFile(player->xDisplay, player->xBoardWindow, name,
		&w, &h, pm, &x_hot, &y_hot) != BitmapSuccess
		|| w != player->squareSize || h != player->squareSize) {
		unsigned long fg, bg;
		unsigned int depth;
		depth = DisplayPlanes(player->xDisplay, player->xScreen);
		if ( player->monoMode ) {
		  fg = XBlackPixel(player->xDisplay, player->xScreen);
		  bg = XWhitePixel(player->xDisplay, player->xScreen);
		} else if ( qm == NULL ) {
		  fg = player->appData.oneColor;
		  bg = player->appData.zeroColor; 
		} else {
		  fg = (player->black_pixel_is_zero ? 0 : ~0);
		  bg = (player->black_pixel_is_zero ? ~0 : 0);
		};
		switch (player->boardSize) {
		      case Large:
			*pm = XCreatePixmapFromBitmapData(player->xDisplay, player->xBoardWindow,
				  large_bits, player->squareSize, player->squareSize,
				  fg, bg, depth);
			break;
		      case Medium:
			*pm = XCreatePixmapFromBitmapData(player->xDisplay, player->xBoardWindow,
				  medium_bits, player->squareSize, player->squareSize,
				  fg, bg, depth);
			break;
		      case Small:
			*pm = XCreatePixmapFromBitmapData(player->xDisplay, player->xBoardWindow,
				  small_bits, player->squareSize, player->squareSize,
				  fg, bg, depth);
			break;
		}
	}     
}                  



void
CreateGrid()
{
	int i, offset;

	offset = 2 * (player->squareSize + LINE_GAP);

	for (i = 0; i < BOARD_SIZE + 1; i++) {
		player->gridSegments[i].x1 = offset;
                player->gridSegments[i + BOARD_SIZE + 1].y1 = 0;
		player->gridSegments[i].y1 = player->gridSegments[i].y2
			= LINE_GAP / 2 + (i * (player->squareSize + LINE_GAP));
		player->gridSegments[i].x2 = LINE_GAP + BOARD_SIZE *
		        (player->squareSize + LINE_GAP) + offset;
		player->gridSegments[i + BOARD_SIZE + 1].x1 =
		player->gridSegments[i + BOARD_SIZE + 1].x2 = LINE_GAP / 2
			+ (i * (player->squareSize + LINE_GAP)) + offset;
		player->gridSegments[i + BOARD_SIZE + 1].y2 =
			BOARD_SIZE * (player->squareSize + LINE_GAP);
	}
}               



void
CreatePieceMenus()
{
	int i;
	Widget entry;
	Arg args[1];
	ChessSquare selection;
	
	XtSetArg(args[0], XtNlabel, "Black");
	blackPieceMenu = XtCreatePopupShell("menuW", simpleMenuWidgetClass,
				       localPlayer.boardWidget, args, 1);
	for (i = 0; i < PIECE_MENU_SIZE; i++) {
		String item = pieceMenuStrings[i];

		if (strcmp(item, "----") == 0) {
			entry = XtCreateManagedWidget(item, smeLineObjectClass,
						      blackPieceMenu, NULL, 0);
		} else {
			entry = XtCreateManagedWidget(item, smeBSBObjectClass,
						      blackPieceMenu, NULL, 0);
			selection = pieceMenuTranslation[0][i];
			XtAddCallback(entry, XtNcallback,
				      (XtCallbackProc) PieceMenuSelect,
				      (caddr_t) selection);
			if (selection == BlackPawn) {
				XtSetArg(args[0], XtNpopupOnEntry, entry);
				XtSetValues(blackPieceMenu, args, 1);
			}
		}
	}

	XtSetArg(args[0], XtNlabel, "White");
	whitePieceMenu = XtCreatePopupShell("menuB", simpleMenuWidgetClass,
				       localPlayer.boardWidget, args, 1);
	for (i = 0; i < PIECE_MENU_SIZE; i++) {
		String item = pieceMenuStrings[i];

		if (strcmp(item, "----") == 0) {
			entry = XtCreateManagedWidget(item, smeLineObjectClass,
						      whitePieceMenu, NULL, 0);
		} else {
			entry = XtCreateManagedWidget(item, smeBSBObjectClass,
						      whitePieceMenu, NULL, 0);
			selection = pieceMenuTranslation[1][i];
			XtAddCallback(entry, XtNcallback,
				      (XtCallbackProc) PieceMenuSelect,
				      (caddr_t) selection);
			if (selection == WhitePawn) {
				XtSetArg(args[0], XtNpopupOnEntry, entry);
				XtSetValues(whitePieceMenu, args, 1);
			}
		}
	}

	XtRegisterGrabAction(PieceMenuPopup, True,
			     (unsigned)(ButtonPressMask|ButtonReleaseMask),
			     GrabModeAsync, GrabModeAsync);
}	

void
PieceMenuPopup(w, event, params, num_params)
	Widget w;
	XEvent *event;
        String *params;
        Cardinal *num_params;
{
	if (event->type != ButtonPress) return;
	if (gameMode != EditPosition) return;
	if (((pmFromX = EventToXSquare(event->xbutton.x)) < 1) ||
	   (pmFromX > BOARD_SIZE+2) ||
	    ((pmFromY = EventToSquare(event->xbutton.y)) < 0)) {
		pmFromX = pmFromY = -1;
		return;
	}
	if (localPlayer.flipView)
	  pmFromX = BOARD_SIZE + 3 - pmFromX;
	else
	  pmFromY = BOARD_SIZE - 1 - pmFromY;

	XtPopupSpringLoaded(XtNameToWidget(localPlayer.boardWidget, params[0]));
}

static void
PieceMenuSelect(w, piece, junk)
     Widget w;
     ChessSquare piece;
     caddr_t junk;
{
	if (pmFromX < 0 || pmFromY < 0) return;
	if (off_board(pmFromX) ) {
	  int i, c;
	  switch (piece) {
	    case ClearBoard:
	      	break;
	    case BlackPlay:
	      	break;
	    case WhitePlay:
	      	break;
            default: 
       		i = pieceToCatchedIndex[piece];
		c = (piece >= WhitePawn );
		catches[0][c][i]++;
		DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		XSync(localPlayer.xDisplay, False);
		return;
    	  }
	}
	pmFromX -= 2;
	switch (piece) {
	      case ClearBoard:
		for (pmFromY = 0; pmFromY < BOARD_SIZE; pmFromY++)
		  for (pmFromX = 0; pmFromX < BOARD_SIZE; pmFromX++) {
			  boards[0][pmFromY][pmFromX] = EmptySquare;
		  }
		ClearCatches(catches[0]);
		DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		break;

	      case BlackPlay:  /*not currently on menu*/
		SetBlackToPlay();
		break;

	      case WhitePlay:  /*not currently on menu*/
		SetWhiteToPlay();
		break;

	      default:
		boards[0][pmFromY][pmFromX] = piece;
		DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		break;
	}
	XSync(localPlayer.xDisplay, False);
}

static void
SetBlackToPlay()
{
	int saveCM;

	if (gameMode != EditPosition) return;
	whitePlaysFirst = False;
	saveCM = currentMove;
	currentMove = 0;  /*kludge*/
	DisplayClocks(ReDisplayTimers);
	currentMove = saveCM;
}

static void
SetWhiteToPlay()
{
	int saveCM;

	if (gameMode != EditPosition) return;
	whitePlaysFirst = True;
	saveCM = currentMove;
	currentMove = 1;  /*kludge*/
	DisplayClocks(ReDisplayTimers);
	currentMove = saveCM;
}

/*
 * If the user selects on a border boundary or off the board, return failure.
 * Otherwise map the event coordinate to the square.
 */
int
EventToSquare(x)
	int x;
{
	if (x < LINE_GAP)
		return -1;
	x -= LINE_GAP;
	if ((x % (player->squareSize + LINE_GAP)) >= player->squareSize)
		return -1;
	x /= (player->squareSize + LINE_GAP);
	if (x >= BOARD_SIZE)
		return -1;
	return x;
}



int
EventToXSquare(x)
	int x;
{
	if (x < LINE_GAP)
		return -1;
	x -= LINE_GAP;
	if ((x % (player->squareSize + LINE_GAP)) >= player->squareSize)
		return -1;
	x /= (player->squareSize + LINE_GAP);
	if (x >= BOARD_SIZE+4)
		return -1;
	return x;
}



ChessSquare
CharToPiece(c, p)
	int c;
	int p;
{               
	if ( p )
		switch (c) {
		default:
		case '.':	return EmptySquare;
		case 'P':	return BlackPPawn;
		case 'L':	return BlackPLance;
		case 'N':	return BlackPKnight;
		case 'S':	return BlackPSilver;
		case 'G':	return BlackGold;
		case 'R':	return BlackPRook;
		case 'B':	return BlackPBishop;
		case 'K':	return BlackKing;
		case 'p':	return WhitePPawn;
		case 'l':	return WhitePLance;
		case 'n':	return WhitePKnight;
		case 's':	return WhitePSilver;
		case 'g':	return WhiteGold;
		case 'r':	return WhitePRook;
		case 'b':	return WhitePBishop;
		case 'k':	return WhiteKing;
		}
	else
		switch (c) {
		default:
		case '.':	return EmptySquare;
		case 'P':	return BlackPawn;
		case 'L':	return BlackLance;
		case 'N':	return BlackKnight;
		case 'S':	return BlackSilver;
		case 'G':	return BlackGold;
		case 'R':	return BlackRook;
		case 'B':	return BlackBishop;
		case 'K':	return BlackKing;
		case 'p':	return WhitePawn;
		case 'l':	return WhiteLance;
		case 'n':	return WhiteKnight;
		case 's':	return WhiteSilver;
		case 'g':	return WhiteGold;
		case 'r':	return WhiteRook;
		case 'b':	return WhiteBishop;
		case 'k':	return WhiteKing;
		}                              

}


/* Convert coordinates to normal algebraic notation.
   promoPiece must be NULLCHAR if not a promotion.
*/
ChessMove
MakeAlg(fromX, fromY, toX, toY, promoPiece, currentBoardIndex, out)
     int fromX, fromY, toX, toY;
     char promoPiece;
     int currentBoardIndex;
     char *out;
{
	ChessSquare piece;
	ChessMove ret;
	char *outp = out;
	int i;

        if ( fromX > 80 ) {
		ChessSquare piece;
		piece = (fromX - 81);
                *outp++ = catchedIndexToChar[piece];
                *outp++ = '*';
		*outp++ = '9' - toX;
		*outp++ = 'i' - toY;
		*outp++ = NULLCHAR;
                return (BlackOnMove(forwardMostMove) ? BlackDrop : WhiteDrop);
	} else {
		*outp++ = '9' - fromX;
		*outp++ = 'i' - fromY;
		*outp++ = '9' - toX;
		*outp++ = 'i' - toY;
		*outp++ = promoPiece;
		*outp++ = NULLCHAR;
                if ( promoPiece == NULLCHAR )
                   return NormalMove;
		else
		   return (BlackOnMove(forwardMostMove) ? BlackPromotion : WhitePromotion);
	}

}


/*
  Parse Shogi board in ISS board style
*/


#ifdef ISS

#define NO_MAIN_PARSEBRD

#include "parsebrd.c"

#endif



void
DrawSquare(row, column, piece)
	int row, column;
	ChessSquare piece; 
{
    int square_color, x, y, direction, font_ascent, font_descent;
    char string[2];
    XCharStruct overall;
    struct DisplayData *player;

    for ( player = &localPlayer; True; player = &remotePlayer) { 

        int offset, remote;
                          
	remote = (player == &remotePlayer);
	offset = 2 * (player->squareSize + LINE_GAP);

	if (player->flipView) {
		x = LINE_GAP + ((BOARD_SIZE-1)-column) * 
		    (player->squareSize + LINE_GAP) + offset;
		y = LINE_GAP + row * (player->squareSize + LINE_GAP);
	} else {
		x = LINE_GAP + column * (player->squareSize + LINE_GAP) + offset;
		y = LINE_GAP + ((BOARD_SIZE-1)-row) * 
		    (player->squareSize + LINE_GAP);
	}

	square_color = ((column + row) % 2) ? LIGHT : DARK;

	if (piece == EmptySquare) {
		if ( column < 0 || column >= BOARD_SIZE ) {
		  /* empty square off board */
		  XFillRectangle(player->xDisplay, player->xBoardWindow,
			         player->wbPieceGC,
			         x, y, player->squareSize, player->squareSize);
		} else {
		  /* empty square on board */
		  XFillRectangle(player->xDisplay, player->xBoardWindow,
			         (square_color == LIGHT) ? player->lightSquareGC : player->darkSquareGC,
			         x, y, player->squareSize, player->squareSize);
		}
	} else if (player->monoMode) {
		/* in mono mode */
		if (square_color == LIGHT)
		  XCopyArea(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormal[remote][(int) piece]
			     : *pieceToReverse[remote][(int) piece],
			     player->xBoardWindow, 
			     (player->monoMode ? player->wbPieceGC : player->wlPieceGC), 
			     0, 0,
			     player->squareSize, player->squareSize, x, y);
		else
		  XCopyArea(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormal[remote][(int) piece]
			     : *pieceToReverse[remote][(int) piece],
			     player->xBoardWindow, 
			     (player->monoMode ? player->bwPieceGC : player->blPieceGC), 
			     0, 0,
			     player->squareSize, player->squareSize, x, y);
	} else {          
		/* in colour mode */
		if ( column < 0 || column >= BOARD_SIZE ) {
		  /* off board */
		  XCopyPlane(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormalSolid[remote][(int) piece]
			     : *pieceToReverseSolid[remote][(int) piece],
			     player->xBoardWindow, 
			     pieceisWhite[(int)piece] ? player->woPieceGC : player->boPieceGC, 
			     0, 0,
			     player->squareSize, player->squareSize, x, y, 1);
		  XCopyArea(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormal[remote][(int) piece]
			     : *pieceToReverse[remote][(int) piece],
			     player->xBoardWindow, 
			     player->charPieceGC, 
			     0, 0,
			     player->squareSize, player->squareSize, x, y);
		} else if (square_color == LIGHT) {
		  /* on board, light square */
		  XCopyPlane(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormalSolid[remote][(int) piece]
			     : *pieceToReverseSolid[remote][(int) piece],
			     player->xBoardWindow, 
			     pieceisWhite[(int)piece] ? player->wlPieceGC : player->blPieceGC, 
			     0, 0,
			     player->squareSize, player->squareSize, x, y, 1);
		  XCopyArea(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormal[remote][(int) piece]
			     : *pieceToReverse[remote][(int) piece],
			     player->xBoardWindow, 
			     player->charPieceGC,
			     0, 0,
			     player->squareSize, player->squareSize, x, y);
		} else {
		  /* on board, dark square */
		  XCopyPlane(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormalSolid[remote][(int) piece]
			     : *pieceToReverseSolid[remote][(int) piece],
			     player->xBoardWindow, 
			     pieceisWhite[(int)piece] ? player->wdPieceGC : player->bdPieceGC, 
			     0, 0,
			     player->squareSize, player->squareSize, x, y, 1);
		  XCopyArea(player->xDisplay, ((int) piece < (int) WhitePawn) ^ player->flipView
			     ? *pieceToNormal[remote][(int) piece]
			     : *pieceToReverse[remote][(int) piece],
			     player->xBoardWindow, 
			     player->charPieceGC, 
			     0, 0,
			     player->squareSize, player->squareSize, x, y);
		}
	}
        string[1] = NULLCHAR;
	if (player->showCoords && column >= 0 && column < 9 && row == (player->flipView ? 8 : 0)) {
	    string[0] = '9' - column;
	    XTextExtents(player->coordFontStruct, string, 1, &direction, 
		&font_ascent, &font_descent, &overall);
	    if (player->monoMode) {
	       XDrawImageString(player->xDisplay, player->xBoardWindow, player->coordGC,
		    x + player->squareSize - overall.width - 2, 
		    y + player->squareSize - font_descent - 1, string, 1);
	    } else {
	       XDrawString(player->xDisplay, player->xBoardWindow, player->coordGC,
		    x + player->squareSize - overall.width - 2, 
		    y + player->squareSize - font_descent - 1, string, 1);
	    }
	}
	if (player->showCoords && row >= 0 && row < 9 && column == (player->flipView ? 8 : 0)) {
	    string[0] = 'i' - row;
	    XTextExtents(player->coordFontStruct, string, 1, &direction, 
		&font_ascent, &font_descent, &overall);
	    if (player->monoMode) {
	       XDrawImageString(player->xDisplay, player->xBoardWindow, player->coordGC,
		    x + 2, y + font_ascent + 1, string, 1);
	    } else {
	       XDrawString(player->xDisplay, player->xBoardWindow, player->coordGC,
		    x + 2, y + font_ascent + 1, string, 1);
	    }	    
	}

	if ( !updateRemotePlayer || (player == &remotePlayer) )
	  break;

    }

}

void
EventProc(widget, client_data, event)
	Widget widget;
	caddr_t client_data;
	XEvent *event;
{
	int fromRemotePlayer = (int)client_data;

	if (event->type == MappingNotify) {
		XRefreshKeyboardMapping((XMappingEvent *) event);
		return;
	}

	if (!XtIsRealized(widget))
		return;

	if ((event->type == ButtonPress) || (event->type == ButtonRelease))
		if (event->xbutton.button != Button1)
			return;

	switch (event->type) {
	case Expose:
		DrawPosition(widget, event, NULL, NULL);
		break;
	default:
		return;
	}
}

/*
 * event handler for redrawing the board
 */
void DrawPosition(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
Arg args[1];
int i, j;
    static Board lastBoard;
    static Catched lastCatches;
    static int lastBoardValid = 0;
    static int lastFlipView = 0, lastRemoteFlipView = 1;

    if (!player->Iconic) {
	XtSetArg(args[0], XtNiconic, False);
	XtSetValues(localPlayer.shellWidget, args, 1);
    }

    /*
     * It would be simpler to clear the window with XClearWindow()
     * but this causes a very distracting flicker.
     */
    if (w == localPlayer.boardWidget && event == NULL && lastBoardValid 
	&& lastFlipView == localPlayer.flipView 
	&& (!updateRemotePlayer || lastRemoteFlipView == remotePlayer.flipView)) {
	for (i = 0; i < BOARD_SIZE; i++)
	  for (j = 0; j < BOARD_SIZE; j++)
	    if (boards[currentMove][i][j] != lastBoard[i][j])
	      DrawSquare(i, j, boards[currentMove][i][j]);
	for ( i = 0; i < 2; i++ )
	  for ( j = 0; j < 8; j++ )
	    if (catches[currentMove][i][j] != lastCatches[i][j]) {
		UpdateCatched(i,0,False,True,currentMove);
		break;
	    }
    } else {        
        XDrawSegments(localPlayer.xDisplay, localPlayer.xBoardWindow, localPlayer.lineGC,
		      localPlayer.gridSegments, (BOARD_SIZE + 1) * 2);
        if ( updateRemotePlayer )
	    XDrawSegments(remotePlayer.xDisplay, remotePlayer.xBoardWindow, remotePlayer.lineGC,
		          remotePlayer.gridSegments, (BOARD_SIZE + 1) * 2);
	for (i = 0; i < BOARD_SIZE; i++)
	  for (j = 0; j < BOARD_SIZE; j++)
	    DrawSquare(i, j, boards[currentMove][i][j]);
        UpdateCatched(0,0,False,True,currentMove);
        UpdateCatched(1,0,False,True,currentMove);
    }

    CopyBoard(lastBoard, boards[currentMove]);
    CopyCatches(lastCatches, catches[currentMove]);
    lastBoardValid = 1;
    lastFlipView = localPlayer.flipView;
    if ( updateRemotePlayer )
      lastRemoteFlipView = remotePlayer.flipView;

    XSync(localPlayer.xDisplay, False);
    if ( updateRemotePlayer )
	XSync(remotePlayer.xDisplay, False);
}

void
InitPosition(redraw)
        int redraw;
{
	currentMove = forwardMostMove = backwardMostMove = 0;
	CopyBoard(boards[0], initialPosition);
        ClearCatches(catches[0]);
	           
	if ( redraw )
	  DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);

}

void
CopyBoard(to, from)
	Board to, from;
{
	int i, j;

	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			to[i][j] = from[i][j];
}
        


void
CopyCatches(to, from)
	Catched to, from;
{
	int i, j;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 8; j++)
			to[i][j] = from[i][j];
}




void
SendCurrentBoard(fp)
	FILE *fp;
{
	SendBoard(fp, boards[currentMove], catches[currentMove]);
}

void
SendBoard(fp, board, catches)
	FILE *fp;
        Board board;
	Catched catches;
{
	char message[MSG_SIZ];
	ChessSquare *bp;
	int i, j;

	SendToProgram("edit\n", fp);
	SendToProgram("#\n", fp);
	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		bp = &board[i][0];
		for (j = 0; j < BOARD_SIZE; j++, bp++) {
			if ((int) *bp < (int) WhitePawn) {
				sprintf(message, "%c%c%c%s\n",
					pieceToChar[(int) *bp],
					'9' - j, 'i' - i,
				        pieceIsPromoted[(int) *bp] ? "+" : "" );
				SendToProgram(message, fp);
			}
		}
	}

	for ( i = 0; i <= 7; i++ ) {
		int n;
		for ( n = catches[0][i]; n > 0; n-- ) {
			sprintf(message, "%c*\n",
				catchedIndexToChar[i]);
				SendToProgram(message, fp);
		};
	}

	SendToProgram("c\n", fp);
	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		bp = &board[i][0];
		for (j = 0; j < BOARD_SIZE; j++, bp++) {
			if (((int) *bp != (int) EmptySquare)
				&& ((int) *bp >= (int) WhitePawn)) {
				sprintf(message, "%c%c%c%s\n",
					pieceToChar[(int) *bp - (int) WhitePawn],
					'9' - j, 'i' - i,
				        pieceIsPromoted[(int) *bp] ? "+" : "" );
				SendToProgram(message, fp);
			}
		}
	}

	for ( i = 0; i <= 7; i++ ) {
		int n;
		for ( n = catches[1][i]; n > 0; n-- ) {
			sprintf(message, "%c*\n",
				catchedIndexToChar[i]);
				SendToProgram(message, fp);
		};
	}

	SendToProgram(".\n", fp);
}






static
int
PromotionPossible (fromY, toY, piece)
	int fromY, toY;
	ChessSquare piece;
{ 
  if ( (int) piece < (int) WhitePawn ) {
    if ( fromY < 6 && toY < 6 ) return(False);
  } else {
    if ( fromY > 2 && toY > 2 ) return(False);
  };    

  return piecePromotable[(int) piece];
 
}


static
void
ShowCount (row, column, n)
   int row, column, n;           
{
   int offset = 2 * (player->squareSize + LINE_GAP);
   int x, y, direction, font_ascent, font_descent;
   char string[2];
   XCharStruct overall;
   struct DisplayData *player;

   DrawSquare (row, column, EmptySquare);
   if ( n <= 1 ) {
     return;
   };                                 

   for ( player = &localPlayer; True; player = &remotePlayer ) {

     if (player->flipView) {
          x = LINE_GAP + ((BOARD_SIZE-1)-column) *
              (player->squareSize + LINE_GAP) + offset;
          y = LINE_GAP + row * (player->squareSize + LINE_GAP);
     } else {
          x = LINE_GAP + column * (player->squareSize + LINE_GAP) + offset;
          y = LINE_GAP + ((BOARD_SIZE-1)-row) *
              (player->squareSize + LINE_GAP);
     }

     x -= player->squareSize / 2;

     string[1] = NULLCHAR;

     if ( n > 9 )
       string[0] = '*';
     else 
       string[0] = '0'+n;

     XTextExtents(player->coordFontStruct, string, 1, &direction,
                &font_ascent, &font_descent, &overall);
     if (player->monoMode) {
        XDrawImageString(player->xDisplay, player->xBoardWindow, player->coordGC,
           x + player->squareSize - overall.width - 2,
           y + player->squareSize - font_descent - 1, string, 1);
     } else {
        XDrawString(player->xDisplay, player->xBoardWindow, player->coordGC,
           x + player->squareSize - overall.width - 2,
           y + player->squareSize - font_descent - 1, string, 1);
     }
    
     if ( !updateRemotePlayer || (player == &remotePlayer) )
	break;
  }

}


void UpdateCatched (Color,Figure,Drop,DropAll,currentMove)
                                            
int Color,Figure,Drop,DropAll,currentMove;   

{

  int n,F,C;
  int x,y;

  /* Determine first row and column. */

  if ( Color ) { 
    x = -1; y = BOARD_SIZE-1;
  } else {
    x = BOARD_SIZE; y = 0;
  }; 


  if ( DropAll )
    n = 0;
  else
    n = catches[currentMove][Color][Figure];

  /* Update the display for captured pieces
     if no piece of the dropped type is there (Drop && n==1)
     or if a piece type is removed (NOT Drop && n==0).
     In the other cases update only the count. */

  if ( DropAll || (Drop && (n==1)) || ((! Drop) && (n==0)) ) {
    /* show all captured pieces */
    n = 0;
    for ( F = pawn; F<=king; F++ ) {
      int c;
      if ( (c=catches[currentMove][Color][F])>0 ) {
        n++;
	DrawSquare(y, x, catchedIndexToPiece[Color][F]); 
        ShowCount(y, Color ? (x-1) : (x+1), c);
        if ( Color ) y--; else y++;
      };
    };
    if ( DropAll ) {
      for ( ; n < 9; n++ ) {
        DrawSquare(y, x, EmptySquare);
        ShowCount(y, Color ? (x-1) : (x+1), 0);
        if ( Color ) y--; else y++;
      }
    } else if ( ! Drop ) {
      /* remove one line ! */
      DrawSquare(y, x, EmptySquare);
      ShowCount(y, Color ? (x-1) : (x+1), 0);
    };
  } else {
    /* show the actual count */
    for ( F = pawn; F <= Figure-1; F++ ) {
      if ( catches[currentMove][Color][F] > 0 ) {
        if ( Color ) y--; else y++;
      };
    };
   ShowCount(y, Color ? (x-1) : (x+1), n); 
  };

};


#if BLINK_COUNT


static int BlinkCount = 0;
static int BlinkRow, BlinkCol;
static ChessSquare BlinkPiece;


void
BlinkSquareProc ()
{
   if ( BlinkCount > 0 ) {
	BlinkCount--;
	DrawSquare (BlinkRow,BlinkCol,
		(BlinkCount & 1) ? EmptySquare : BlinkPiece);
	if ( BlinkCount > 0 )
	  blinkSquareXID = XtAppAddTimeOut(appContext,
		(int) (150),
		(XtTimerCallbackProc) BlinkSquareProc, NULL);
   } else
	BlinkCount = 0;
}


void
BlinkSquare (row, col, piece)
	int row, col;
	ChessSquare piece;
{
   BlinkCount = 2*BLINK_COUNT + 1;
   BlinkRow = row;
   BlinkCol = col;
   BlinkPiece = piece;
   BlinkSquareProc();
}


#endif /* BLINK_COUNT */



static
int
PieceOfCatched (color, x, y, currentMove)
	int color, x, y, currentMove;
{
  int F, n;

  if ( color ) {
	if ( x != 1 ) return (no_piece);
	y = 8 - y;
  } else {
	if ( x != 11 ) return (no_piece);
  };

  for ( F = pawn, n = 0; F <= king; F++ ) {
    if ( catches[currentMove][color][F] > 0 ) {
	if ( n == y ) return (F);
	n++;
    };
  };

  return (no_piece);
}





/*
 * event handler for parsing user moves
 */
void
HandleUserMove(w, event)
	Widget w;
	XEvent *event;
{
	ChessMove move_type;
	ChessSquare from_piece;
	int to_x, to_y, fromRemotePlayer;  

	if ( updateRemotePlayer ) {
	  if (((w != localPlayer.boardWidget) && (w != remotePlayer.boardWidget)) || (matchMode != MatchFalse))
		return;
	  fromRemotePlayer = (w == remotePlayer.boardWidget);
	} else {
	  if ((w != localPlayer.boardWidget) || (matchMode != MatchFalse))
		return;
	  fromRemotePlayer = False;
	}

	player = fromRemotePlayer ? &remotePlayer : &localPlayer;

	if (player->promotionUp) {
		XtPopdown(player->promotionShell);
		XtDestroyWidget(player->promotionShell);
		player->promotionUp = False;
		fromX = fromY = -1;
	}

	switch (gameMode) {
	      case EndOfGame:
	      case PlayFromGameFile:
	      case TwoMachinesPlay:
		return;
	      case MachinePlaysBlack:
		if (BlackOnMove(forwardMostMove)) {
		    DisplayMessage("It is not your turn",fromRemotePlayer);
		    return;   
		}
		break;
	      case MachinePlaysWhite:
		if (!BlackOnMove(forwardMostMove)) {
		    DisplayMessage("It is not your turn",fromRemotePlayer);
		    return;   
		}
		break;
	      case ForceMoves:
#ifdef ISS
		if (player->appData.issActive) {
		    /* it is not yet detected whether ISS is Idle, Observing
	 	     * or in match mode.
         	     */
#ifdef needs_update
		    if (iss_user_moved ||
			iss_mode == IssObserving ||
			iss_mode == IssIdle)
		      return;
#endif
		} else 
#endif
		{
		    forwardMostMove = currentMove;
		}
		break;
	      default:
		break;
	}

        if (currentMove != forwardMostMove) {
	  DisplayMessage("Displayed position is not current",fromRemotePlayer);
	  return;
        }
    
	switch (event->type) {
	      case ButtonPress:
		if ((fromX >= 0) || (fromY >= 0))
		  return;
		if (((fromX = EventToXSquare(event->xbutton.x)) < 1) ||
                    (fromX > BOARD_SIZE+2) ||
		    ((fromY = EventToSquare(event->xbutton.y)) < 0)) {
			fromX = fromY = -1;
			return;
		}
		if (player->flipView)
		    fromX = BOARD_SIZE + 3 - fromX;
		else
		    fromY = BOARD_SIZE - 1 - fromY;
		break;

	      case ButtonRelease:
		if ((fromX < 0) || (fromY < 0)) return;

		if (((to_x = EventToXSquare(event->xbutton.x)) < 1)
		    	|| (to_x > BOARD_SIZE+2)
			|| ((to_y = EventToSquare(event->xbutton.y)) < 0)) {
			if (gameMode == EditPosition && !off_board(fromX)) {
			  fromX -= 2;
			  boards[0][fromY][fromX] = EmptySquare;
			  DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
			  XSync(localPlayer.xDisplay, False);
			  if ( updateRemotePlayer )
			    XSync(remotePlayer.xDisplay, False);
			}
			fromX = fromY = -1;
			return;
		}
		if (player->flipView)
			to_x = BOARD_SIZE + 3 - to_x;
		else
			to_y = BOARD_SIZE - 1 - to_y;
		if ((fromX == to_x) && (fromY == to_y)) {
			fromX = fromY = -1;
			return;
		}

		if (gameMode == EditPosition) {
                  ChessSquare piece;
		  if ( off_board(fromX) ) {
			/* Remove a catched piece */
			int i, c;
			c = ((fromX < 5) ^ player->flipView);
			i = PieceOfCatched(c,fromX,fromY,0);
			if ( i == no_piece ) {
				fromX = fromY = -1;
			 	return;
			} else {
				piece = catchedIndexToPiece[c][i];
                		catches[0][c][i]--;
			}
		  } else {
			/* remove piece from board field */
			fromX -= 2;
			piece = boards[0][fromY][fromX];
			boards[0][fromY][fromX] = EmptySquare;
		  }
		  if ( !off_board(to_x) ) {
		    	/* drop piece to board field */	
                    	ChessSquare catched_piece;
		    	to_x -= 2;
			catched_piece = boards[0][to_y][to_x];
                    	if ( catched_piece != EmptySquare ) {
			  /* put piece to catched pieces */
                      	  int i = pieceToCatchedIndex[catched_piece];
                      	  int c = (catched_piece < WhitePawn );
                      	  catches[0][c][i]++;
                    	};
			/* place moved piece */
			boards[0][to_y][to_x] = piece;
		  }
		  fromX = fromY = -1;
		  DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		  XSync(localPlayer.xDisplay, False);
		  if ( updateRemotePlayer )
		    XSync(remotePlayer.xDisplay, False);
		  return;
		}
		
		if ( off_board(fromX) ) {
			int c = BlackOnMove(forwardMostMove) ? 0 : 1;
			int piece = PieceOfCatched(c,fromX,fromY,currentMove);
			if ( piece == no_piece ) {
				fromX = fromY = -1;
			 	return;
			} else {
				if ( updateRemotePlayer && (BlackOnMove(forwardMostMove) == fromRemotePlayer) ) {
				  DisplayMessage("do not drop opponent pieces",fromRemotePlayer);
				  fromX = fromY = -1;
				  return;
				};
				fromX = fromY = piece + 81;
				to_x -= 2;
				move_type = BlackOnMove(forwardMostMove) ? BlackDrop : WhiteDrop;
		  		MakeMove(&move_type, fromX, fromY, to_x, to_y);
#if BLINK_COUNT
		  		if ( updateRemotePlayer ) 
				    BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif
		  		FinishUserMove(move_type, to_x, to_y);
		  		break;
			};
		} else if ( off_board(to_x) ) {
			fromX = fromY = -1;
			return;
                } else {
		  fromX -= 2; to_x -= 2;
		  from_piece = boards[currentMove][fromY][fromX];
		  if ( (from_piece != EmptySquare) && 
			updateRemotePlayer && ((from_piece < WhitePawn) == fromRemotePlayer) ) {
		 	DisplayMessage("do not move opponent pieces", fromRemotePlayer);
			fromX = fromY = -1;
			return;
		  };
		  if ( PromotionPossible(fromY, to_y, from_piece) ) {
		  	PromotionPopUp(from_piece, to_x, to_y, fromRemotePlayer);
			return;                                
		  }
		  move_type = NormalMove;
		  MakeMove(&move_type, fromX, fromY, to_x, to_y);
#if BLINK_COUNT
		  if ( updateRemotePlayer ) 
		    BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif
		  FinishUserMove(move_type, to_x, to_y);
		  break;
		}
	}
}

void
FinishUserMove(move_type, to_x, to_y)
         ChessMove move_type;
         int to_x, to_y;
{
	char user_move[MSG_SIZ];

#ifdef ISS
	if (localPlayer.appData.issActive) {
	  /* output move for ISS */
	  int pcurrentMove;
	  pcurrentMove = (currentMove > 0) ? currentMove - 1 : 0;
	  switch (move_type) {
	    case BlackPromotion:
	    case WhitePromotion:
		sprintf(user_move, "%c%c%c%c%c%c+\n",
			IssChessSquareChar[boards[pcurrentMove][fromY][fromX]],
			iss_column_bug ? '1' + fromX : '9' - fromX, 
			'i' - fromY, 
			(boards[pcurrentMove][to_y][to_x] == EmptySquare) ? '-' : 'x',
			iss_column_bug ? '1' + to_x : '9' - to_x, 
			'i' - to_y);
	        break;
	    case BlackDrop:
	    case WhiteDrop:
		sprintf(user_move, "%c*%c%c\n",
			IssPieceChar[fromX - 81],
			iss_column_bug ? '1' + to_x : '9' - to_x, 
			'i' - to_y);
		break;
	    case NormalMove:
		sprintf(user_move, "%c%c%c%c%c%c\n",
			IssChessSquareChar[boards[pcurrentMove][fromY][fromX]],
			iss_column_bug ? '1' + fromX : '9' - fromX, 
			'i' - fromY, 
			(boards[pcurrentMove][to_y][to_x] == EmptySquare) ? '-' : 'x',
			iss_column_bug ? '1' + to_x : '9' - to_x, 
			'i' - to_y);
		break;
	    default:
	      fprintf(stderr, "%s: internal error; bad move_type\n",
	  	    (char *)programName);
	      break;
	  }
	  SendToISS(user_move);
	  iss_user_moved = 1;
	} else
#endif
	{
	  /* output move for gnushogi */    
	  switch (move_type) {
	    case BlackPromotion:
	    case WhitePromotion:
		sprintf(user_move, "%c%c%c%c+\n",
			'9' - fromX, 'i' - fromY, '9' - to_x, 'i' - to_y);
		break;
	    case BlackDrop:
	    case WhiteDrop:
		sprintf(user_move, "%c*%c%c\n",
			catchedIndexToChar[fromX - 81],'9' - to_x, 'i' - to_y);
		break;
	    case NormalMove:
		sprintf(user_move, "%c%c%c%c\n",
			'9' - fromX, 'i' - fromY, '9' - to_x, 'i' - to_y);
		break;
	    default:
	      fprintf(stderr, "%s: internal error; bad move_type\n",
	  	    (char *)programName);
	      break;
	  }
	  Attention(firstProgramPID);
#ifdef SENDTIME 
	  if (firstSendTime)
	    SendTimeRemaining(toFirstProgFP);
#endif
	  SendToProgram(user_move, toFirstProgFP);
	  strcpy(moveList[currentMove - 1], user_move);
	}

	fromX = fromY = -1;

	if (gameMode == PauseGame)
	  PauseProc(NULL,NULL,NULL,NULL); /* a user move restarts a paused game*/

	switch (gameMode) {
	      case ForceMoves:
		break;
	      case BeginningOfGame:
		if (localPlayer.appData.noChessProgram)
		  lastGameMode = gameMode = ForceMoves;
		else
		  lastGameMode = gameMode = MachinePlaysWhite;
		ModeHighlight();
		break;
	      case MachinePlaysWhite:
	      case MachinePlaysBlack:
	      default:
		break;
	}
}

/* Simple parser for moves from gnushogi. */
void
ParseMachineMove(machine_move, move_type, from_x, from_y, to_x, to_y)
     char *machine_move;
     ChessMove *move_type;
     int *from_x, *from_y, *to_x, *to_y;

         
{
#define no_digit(c) (c < '0' || c > '9' )

	{                     
                if ( no_digit(machine_move[0]) ) {

		  switch ( machine_move[0] ) {
			case 'P': *from_x = 81; break;
			case 'L': *from_x = 82; break;
			case 'N': *from_x = 83; break;
			case 'S': *from_x = 84; break;
			case 'G': *from_x = 85; break;
			case 'B': *from_x = 86; break;
			case 'R': *from_x = 87; break;
			case 'K': *from_x = 88; break;
                        default: *from_x = -1;
		  };
		  *from_y = *from_x;
		  *to_x   = '9' - machine_move[2];
		  *to_y   = 'i' - machine_move[3];

		} else {

		  *from_x = '9' - machine_move[0] ;
		  *from_y = 'i' - machine_move[1];
		  *to_x = '9' - machine_move[2];
		  *to_y = 'i' - machine_move[3];

		  switch (machine_move[4]) {
		      case '+':
			*move_type = BlackOnMove(forwardMostMove) ?
			  BlackPromotion : WhitePromotion;
			break;
		      default:
			*move_type = NormalMove;
			break;
		  }
		}		  
	}
}                 



void
SkipString (mpr)
	char **mpr;
{
	while ( **mpr == ' ')
	  (*mpr)++;
	while ( **mpr != ' ' && **mpr != NULLCHAR && **mpr != '\n' )
	  (*mpr)++;
	while ( **mpr == ' ')
	  (*mpr)++;
}        


void
HandleMachineMove(message, fp)
	char *message;
        FILE *fp;
{
	char machine_move[MSG_SIZ], buf1[MSG_SIZ], buf2[MSG_SIZ];
	int i, j, from_x, from_y, to_x, to_y;
	ChessMove move_type; 
	long time_remaining;
	char *mpr;

	maybeThinking = False;

	if (strncmp(message, "warning:", 8) == 0) {
		DisplayMessage(message,False);
		if ( updateRemotePlayer )
		  DisplayMessage(message,True);
		return;
	}

	/*
	 * If chess program startup fails, exit with an error message.
	 * Attempts to recover here are futile.
	 */
	if ((StrStr(message, "unknown host") != NULL)
	    || (StrStr(message, "No remote directory") != NULL)
	    || (StrStr(message, "not found") != NULL)
	    || (StrStr(message, "No such file") != NULL)
	    || (StrStr(message, "Permission denied") != NULL)) {
		fprintf(stderr,
			"%s: failed to start shogi program %s on %s: %s\n",
			programName,
			fp == fromFirstProgFP ? localPlayer.appData.firstChessProgram
			    : localPlayer.appData.secondChessProgram,
			fp == fromFirstProgFP ? localPlayer.appData.firstHost
			    : localPlayer.appData.secondHost,
			message);
		ShutdownChessPrograms(message);
		exit(1);
	}

	/*
	 * If the move is illegal, cancel it and redraw the board.
	 */
	if (strncmp(message, "Illegal move", 12) == 0) {

		if (fp == fromFirstProgFP && firstSendTime == 2) {
		    /* First program doesn't have the "time" command */
		    firstSendTime = 0;
		    return;
		} else if (fp == fromSecondProgFP && secondSendTime == 2) {
		    /* Second program doesn't have the "time" command */
		    secondSendTime = 0;
		    return;
		}          

		if (forwardMostMove <= backwardMostMove) return;
		if (gameMode == PauseGame) PauseProc(NULL, NULL, NULL, NULL);
		if (gameMode == PlayFromGameFile) {
		    /* Stop reading this game file */
		    gameMode = ForceMoves;
		    ModeHighlight();
		}
		currentMove = --forwardMostMove;
		if ((gameMode == PlayFromGameFile) || 
		    (gameMode == ForceMoves))
		  DisplayClocks(ReDisplayTimers);
		else
		  DisplayClocks(SwitchTimers);
		sprintf(buf1, "Illegal move: %s", parseList[currentMove]);
		DisplayMessage(buf1,False);
		if ( updateRemotePlayer )
		  DisplayMessage(buf1,True);

#if BLINK_COUNT
		/*
		 * Disable blinking of the target square.
		 */
		if ( BlinkCount > 0 )
		  {       
		    /* If BlinkCount is even, the piece is currently displayed. */
		    if ( !(BlinkCount & 1) )
		      DrawSquare (BlinkRow,BlinkCol,EmptySquare);                       
		    /* BlinkCount = 0 will force the next blink timeout to do nothing. */
		    BlinkCount = 0;
		  }
#endif
		DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);

		XSync(localPlayer.xDisplay, False);
		if ( updateRemotePlayer )
		  XSync(remotePlayer.xDisplay, False);
		return;
	}

	if (StrStr(message, "GNU Shogi") != NULL) {
	  at_least_gnushogi_1_2p03 = True;
	  return;
	}

        if (strncmp(message, "Hint:", 5) == 0) {
		char promoPiece;
		sscanf(message, "Hint: %s", machine_move);
		ParseMachineMove(machine_move, &move_type,
				 &from_x, &from_y, &to_x, &to_y);
		if ( move_type == WhitePromotion || move_type == BlackPromotion )
		  promoPiece = '+';
		else
		  promoPiece = NULLCHAR;
		move_type = MakeAlg(from_x, from_y, to_x, to_y, promoPiece,
			       currentMove, buf1);
		sprintf(buf2, "Hint: %s", buf1);
		DisplayMessage(buf2,False);
		if ( updateRemotePlayer )
		  DisplayMessage(buf2,True);
		return;
	}                              

	if (strncmp(message, "Clocks:", 7) == 0) {
		sscanf(message, "Clocks: %ld %ld", 
			&blackTimeRemaining, &whiteTimeRemaining);
	        DisplayClocks(ReDisplayTimers);
		return;
	} 

	/*
	 * win, lose or draw
	 */
	if (strncmp(message, "Black", 5) == 0) {
		ShutdownChessPrograms("Black wins");
		return;
	} else if (strncmp(message, "White", 5) == 0) {
		ShutdownChessPrograms("White wins");
		return;
	} else if (strncmp(message, "Repetition", 10) == 0) {
		ShutdownChessPrograms("Repetition");
		return;
	} else if (strncmp(message, "opponent mates!", 15) == 0) {
		switch (gameMode == PauseGame ? pausePreviousMode : gameMode) {
		      case MachinePlaysWhite:
			ShutdownChessPrograms("Black wins");
			break;
		      case MachinePlaysBlack:
			ShutdownChessPrograms("White wins");
			break;
		      case TwoMachinesPlay:
			ShutdownChessPrograms(fp == fromFirstProgFP ?
					      "Black wins" : "White wins");
			break;
		      default:
			/* can't happen */
			break;
		}
		return;
	} else if (strncmp(message, "computer mates!", 15) == 0) {
		switch (gameMode == PauseGame ? pausePreviousMode : gameMode) {
		      case MachinePlaysWhite:
			ShutdownChessPrograms("White wins");
			break;
		      case MachinePlaysBlack:
			ShutdownChessPrograms("Black wins");
			break;
		      case TwoMachinesPlay:
			ShutdownChessPrograms(fp == fromFirstProgFP ?
					      "White wins" : "Black wins");
			break;
		      default:
			/* can't happen */
			break;
		}
		return;
	} else if (strncmp(message, "Draw", 4) == 0) {
		ShutdownChessPrograms("Draw");
		return;
	}

	/*
	 * normal machine reply move
	 */
	maybeThinking = True;
	if (StrStr(message, "...") != NULL) {
		sscanf(message, "%s %s %s", buf1, buf2, machine_move);
#ifdef SYNCHTIME
		mpr = message;
		SkipString(&mpr); /* skip move number */
		SkipString(&mpr); /* skip ... */
		SkipString(&mpr); /* skip move */
                if ( gameMode != TwoMachinesPlay && gameMode != ForceMoves &&
		     ( *mpr == '-' || (*mpr >= '0' && *mpr <= '9') ) ) {
		  /* synchronize with shogi program clock */
		  sscanf(mpr, "%ld", &time_remaining);
		  if ( xshogiDebug )
		    printf("from '%s' synchronize %s clock %ld\n",
			message, BlackOnMove(forwardMostMove) ? "Black's" : "White's", time_remaining);
		  if ( BlackOnMove(forwardMostMove) ) 
		    blackTimeRemaining = time_remaining;
  		  else
		    whiteTimeRemaining = time_remaining;
		}
#endif
		if (machine_move[0] == NULLCHAR)
			return;
	} else {
	        mpr = message;
#ifdef SYNCHTIME
		if (StrStr(message, "time") == NULL) {
		  /* remaining time will be determined from move */
		  SkipString(&mpr); /* skip move number */
		  SkipString(&mpr); /* skip move */
		}
                if ( gameMode != TwoMachinesPlay && gameMode != ForceMoves &&
		     ( *mpr == '-' || (*mpr >= '0' && *mpr <= '9') ) ) {
		  /* synchronize with shogi program clock */
		  sscanf(mpr, "%ld", &time_remaining);
		  if ( xshogiDebug )
		    printf("from '%s' synchronize %s clock %ld\n",
			message, !BlackOnMove(forwardMostMove) ? "Black's" : "White's", time_remaining);
		  if ( !BlackOnMove(forwardMostMove) ) 
		    blackTimeRemaining = time_remaining;
  		  else
		    whiteTimeRemaining = time_remaining;
		} else 
#endif
		if ( xshogiDebug )
		  printf("ignore noise: '%s'\n",message);
		return; /* ignore noise */
	}

	strcpy(moveList[forwardMostMove], machine_move);

	ParseMachineMove(machine_move, &move_type, &from_x, &from_y,
			 &to_x, &to_y);

        if (gameMode != PauseGame)
          currentMove = forwardMostMove;  /*display latest move*/
    
	MakeMove(&move_type, from_x, from_y, to_x, to_y);
#if BLINK_COUNT
	if ( gameMode != TwoMachinesPlay ) 
	  BlinkSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
#endif
        if (gameMode != PauseGame && localPlayer.appData.ringBellAfterMoves)
          putc(BELLCHAR, stderr);
          
        if (gameMode == TwoMachinesPlay ||
    	    (gameMode == PauseGame && pausePreviousMode == TwoMachinesPlay)) {
    	    strcat(machine_move, "\n");
    	    if (BlackOnMove(forwardMostMove)) {
    	        Attention(secondProgramPID);
#ifdef SENDTIME
    	        if (secondSendTime) 
	          SendTimeRemaining(toSecondProgFP);
#endif
	        SendToProgram(machine_move, toSecondProgFP);
	        if (firstMove) {
	    	firstMove = False;
	    	SendToProgram(localPlayer.appData.blackString,
	    		      toSecondProgFP);
	        }
	    } else {
	        Attention(firstProgramPID);
#ifdef SENDTIME
	        if (firstSendTime)
	          SendTimeRemaining(toFirstProgFP);
#endif
	        SendToProgram(machine_move, toFirstProgFP);
	        if (firstMove) {
	    	firstMove = False;
	    	SendToProgram(localPlayer.appData.blackString,
	    		      toFirstProgFP);
	        }
	    }
        } 
   
}  



void
ReadGameFile()
{
	for (;;) {
		if (!ReadGameFileProc())
			return;
		if (matchMode == MatchOpening)
			continue;
		readGameXID = XtAppAddTimeOut(appContext,
			(int) (1000 * localPlayer.appData.timeDelay),
			(XtTimerCallbackProc) ReadGameFile, NULL);
		break;
	}
}

int
ReadGameFileProc()
{
	int from_x, from_y, to_x, to_y, done;
	ChessMove move_type;
	char move[MSG_SIZ], buf[MSG_SIZ];

	if (gameFileFP == NULL)
		return (int) False;

	if (gameMode == PauseGame) return True;

	if (gameMode != PlayFromGameFile) {
		fclose(gameFileFP);
		gameFileFP = NULL;
		return (int) False;
	}

	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}       

	fgets(move, MSG_SIZ, gameFileFP);
	move[strlen(move) - 1] = NULLCHAR;
	sprintf(buf, "# %s game file", programName);
	if ( strncmp(move,buf,strlen(buf)) ) {
	  strcat(move,": no xshogi game file");
	  DisplayMessage(move,False);  
	  return (int) False;
	}
        DisplayName(move);
	rewind(gameFileFP);

	parseGameFile ();

	move_type = (ChessMove) 0;

	lastGameMode = gameMode;
	gameMode = ForceMoves;
	ModeHighlight();
	DisplayMessage("End of game file",False);

	if (readGameXID != 0) {
		XtRemoveTimeOut(readGameXID);
		readGameXID = 0;
	}
	fclose(gameFileFP);
	gameFileFP = NULL;

	return ((int) False);
}     




/* Apply a move to the given board.  Oddity: move_type is ignored on
   input unless the move is seen to be a pawn promotion, in which case
   move_type tells us what to promote to.
*/
void ApplyMove(move_type, from_x, from_y, to_x, to_y, currentMove)
     ChessMove *move_type;
     int from_x, from_y, to_x, to_y;
     int currentMove;
{
	ChessSquare piece;
	char pieceChar;

        if ( from_x > 80 ) {
                int i = from_x - 81, c = (BlackOnMove(currentMove) ? 1 : 0);
                ChessSquare cpiece = catchedIndexToPiece[c][i];
                boards[currentMove][to_y][to_x] = cpiece;
                catches[currentMove][c][i]--;
        } else if (PromotionPossible(from_y,to_y,piece = boards[currentMove][from_y][from_x])) {
                ChessSquare cpiece = boards[currentMove][to_y][to_x];
                if ( cpiece != EmptySquare ) {
                  int i = pieceToCatchedIndex[cpiece];
                  int c = (cpiece < WhitePawn );
                  catches[currentMove][c][i]++;
                };
                if ( *move_type == NormalMove ) {
                  boards[currentMove][to_y][to_x] = piece;
                } else {
                  boards[currentMove][to_y][to_x] = piece = pieceToPromoted[piece];
                  pieceChar = '+';
                }
                boards[currentMove][from_y][from_x] = EmptySquare;
        } else {
                ChessSquare piece = boards[currentMove][to_y][to_x];
                if ( piece != EmptySquare ) {
                  int i = pieceToCatchedIndex[piece];
                  int c = (piece < WhitePawn );
                  catches[currentMove][c][i]++;
                };
                *move_type = NormalMove;
                boards[currentMove][to_y][to_x] =
                  boards[currentMove][from_y][from_x];
                boards[currentMove][from_y][from_x] = EmptySquare;
        }
}

/*
 * MakeMove() displays moves.  If they are illegal, GNU shogi will detect
 * this and send an Illegal move message.  XShogi will then retract the move.
 * The clockMode False case is tricky because it displays the player on move.
 */
void
MakeMove(move_type, from_x, from_y, to_x, to_y)
	ChessMove *move_type;
	int from_x, from_y, to_x, to_y;
{
	char message[MSG_SIZ], movestr[MSG_SIZ];
	char promoPiece = NULLCHAR;
        ChessSquare piece;

	forwardMostMove++;

	CopyBoard(boards[forwardMostMove], boards[forwardMostMove - 1]);
	CopyCatches(catches[forwardMostMove], catches[forwardMostMove - 1]);

        ApplyMove(move_type, from_x, from_y, to_x, to_y, forwardMostMove);

        endMessage[0] = NULLCHAR;

        timeRemaining[0][forwardMostMove] = blackTimeRemaining;
        timeRemaining[1][forwardMostMove] = whiteTimeRemaining;

        if (gameMode == PauseGame && pausePreviousMode != PlayFromGameFile)
          return;

	currentMove = forwardMostMove;

	if (gameMode == PlayFromGameFile) {
		sprintf(message, "%d. %s%s", (currentMove + 1) / 2,
			BlackOnMove(currentMove) ? "... " : "", currentMoveString);
		strcpy(parseList[currentMove - 1], currentMoveString);
	} else {
		if ( *move_type == WhitePromotion || *move_type == BlackPromotion )
		  promoPiece = '+';
		else
		  promoPiece = NULLCHAR;
		MakeAlg(from_x, from_y, to_x, to_y, promoPiece,
			currentMove - 1, movestr);
		sprintf(message, "%d. %s%s", (currentMove + 1) / 2,
			BlackOnMove(currentMove) ? "... " : "", movestr);
	  strcpy(parseList[currentMove - 1], movestr);
	}

	DisplayMessage(message, False);

	if ((gameMode == PlayFromGameFile) || (gameMode == ForceMoves) ||
	    (gameMode == PauseGame && pausePreviousMode == PlayFromGameFile))
	  DisplayClocks(ReDisplayTimers);
	else
	  DisplayClocks(SwitchTimers);

	DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);

	XSync(localPlayer.xDisplay, False);
	if ( updateRemotePlayer ) {
	  DisplayMessage(message, True);
	  XSync(remotePlayer.xDisplay, False);
	}
}

void
InitChessProgram(host_name, program_name, pid, to, from, xid, sendTime)
	char *host_name, *program_name;
	int *pid;
	FILE **to, **from;
	XtIntervalId *xid;
	int *sendTime;
{
	char arg_buf[10];
	char *arg1, *arg2;
	int to_prog[2], from_prog[2];
	FILE *from_fp, *to_fp;
	int dummy_source;
	XtInputId dummy_id;
#if     SYSTEM_FIVE || SYSV
	char *pty_name;
#endif

	if (localPlayer.appData.noChessProgram) return;

#if	SYSTEM_FIVE || SYSV
	if ((pty_name = PseudoTTY(&to_prog[1])) == NULL) {
		fprintf(stderr, "%s: can't open pseudo-tty\n", programName);
		perror("");
		exit(1);
	}

	from_prog[0] = to_prog[1];
	to_prog[0] = from_prog[1] = open(pty_name, O_RDWR, 0);
#if	SVR4
	if (ioctl (to_prog[0], I_PUSH, "ptem") == -1 ||
	    ioctl (to_prog[0], I_PUSH, "ldterm") == -1 ||
	    ioctl (to_prog[0], I_PUSH, "ttcompat") == -1) {
		fprintf(stderr, "%s: can't ioctl pseudo-tty\n", programName);
		perror("");
		exit(1);
	}
#endif
#else
	signal(SIGPIPE, CatchPipeSignal);
	pipe(to_prog);
	pipe(from_prog);
#endif

	if ((*pid = fork()) == 0) {
#if	!SYSTEM_FIVE && !SYSV
		signal(SIGPIPE, CatchPipeSignal);
#endif
		dup2(to_prog[0], 0);
		dup2(from_prog[1], 1);
		close(to_prog[0]);
		close(to_prog[1]);
		close(from_prog[0]);
		close(from_prog[1]);
		dup2(1, fileno(stderr));	/* force stderr to the pipe */
		if (localPlayer.appData.searchTime != NULL) {
			sprintf(arg_buf, "%d", searchTime);
			arg1 = arg_buf;
			arg2 = (char *) NULL;
		} else if (localPlayer.appData.searchDepth > 0) {
			sprintf(arg_buf, "%d", localPlayer.appData.searchDepth);
			arg1 = "1";
			arg2 = "9999";
		} else {
			sprintf(arg_buf, "%d", localPlayer.appData.movesPerSession);
			arg1 = arg_buf;
			arg2 = localPlayer.appData.timeControl;
		}
		if (strcmp(host_name, "localhost") == 0) {
			execlp(program_name, program_name, arg1, arg2,
				(char *) NULL);
		} else {
			execlp(localPlayer.appData.remoteShell, localPlayer.appData.remoteShell,
				host_name, program_name, arg1, arg2,
				(char *) NULL);
		}

		perror(program_name);
		exit(1);
	}

	close(to_prog[0]);
	close(from_prog[1]);

	*from = from_fp = fdopen(from_prog[0], "r");
	*to = to_fp = fdopen(to_prog[1], "w");
	setbuf(from_fp, NULL); setbuf(to_fp, NULL);

	ReceiveFromProgram(from_fp, &dummy_source, &dummy_id); /* "GNU Shogi"*/

	if ( !at_least_gnushogi_1_2p03 ) {
	  printf("you must have at least gnushogi-1.2p03\n");
	  exit(1);
	}

	if (*pid == 0) return;

	*xid = XtAppAddInput(appContext, fileno(from_fp), 
			     (XtPointer)XtInputReadMask,
			     (XtInputCallbackProc)ReceiveFromProgram, 
			     (XtPointer)from_fp);

	SendToProgram(localPlayer.appData.initString, *to);

	if ( localPlayer.appData.gameIn )
	  SendToProgram("gamein\n", *to);

	SendSearchDepth(*to);

#ifdef SENDTIME
    	if (*sendTime == 2) {
		/* Does program have "time" command? */
		char buf[MSG_SIZ];
		
		sprintf(buf, "time %d\n", blackTimeRemaining/10);
		SendToProgram(buf, to_fp);
		ReceiveFromProgram(from_fp, &dummy_source, &dummy_id);
		if (*sendTime == 2) {
		  *sendTime = 1;  /* yes! */
		  sprintf(buf, "otime %d\n", whiteTimeRemaining/10);
		  SendToProgram(buf, to_fp);
		  ReceiveFromProgram(from_fp, &dummy_source, &dummy_id);
                }
	}
#else
	*sendTime = 0;  	 
#endif
    
}



#ifdef ISS

void GameEnds(why)
     char *why;
{
    iss_mode = IssIdle;
    iss_gamenum = -1;
    iss_user_moved = False;
    if (readGameXID != 0) {
	XtRemoveTimeOut(readGameXID);
	readGameXID = 0;
    }
    if (gameFileFP != NULL) {
	fclose(gameFileFP);
	gameFileFP = NULL;
    }
    DisplayClocks(StopTimers);
    
    if (why == NULL || strlen(why) == 0) return;
    
    strncpy(endMessage, why, MOVE_LEN * 4);
    endMessage[MOVE_LEN * 4 - 1] = NULLCHAR;
    if (currentMove == forwardMostMove)
      DisplayMove(currentMove - 1);
    
    if (!(gameMode == PlayFromGameFile ||
	  (gameMode == PauseGame && pausePreviousMode == PlayFromGameFile))) {
	if (*localPlayer.appData.saveGameFile != NULLCHAR) {
	    SaveGame(localPlayer.appData.saveGameFile);
	} else if (localPlayer.appData.autoSaveGames) {
	    SaveGameProc(NULL, NULL, NULL, NULL);
	}
	if (localPlayer.appData.savePositionFile[0] != NULLCHAR) 
	  SavePosition(localPlayer.appData.savePositionFile);
    }
}


#endif


void
ShutdownChessPrograms(why)
	char *why;
{ 

#ifdef ISS
	GameEnds(why);

	lastGameMode = gameMode;
	if ( gameMode == PauseGame ) {
	  pausePreviousMode = EndOfGame;
        } else {
	  gameMode = EndOfGame;
	  ModeHighlight();
	}

	if (firstProgramPID != 0) {
          if (kill(firstProgramPID, 0) == 0)
	    SendToProgram("quit\n", toFirstProgFP);
	  fclose(fromFirstProgFP);
	  fclose(toFirstProgFP);
	  fromFirstProgFP = toFirstProgFP = NULL;
	  if (kill(firstProgramPID, SIGTERM)==0)
	    wait((union wait *)0);
        }								    
	firstProgramPID = 0;

	if (firstProgramXID != 0)
		XtRemoveInput(firstProgramXID);
	firstProgramXID = 0;

	if (secondProgramPID != 0) {
          if (kill(secondProgramPID, 0) == 0)
	    SendToProgram("quit\n", toSecondProgFP);
	  fclose(fromSecondProgFP);
	  fclose(toSecondProgFP);
	  fromSecondProgFP = toSecondProgFP = NULL;
	  if (kill(secondProgramPID, SIGTERM)==0)
	    wait((union wait *)0);
	}
	secondProgramPID = 0;

	if (secondProgramXID != 0)
		XtRemoveInput(secondProgramXID);
	secondProgramXID = 0;

 	if(matchMode != MatchFalse) {
 		if (localPlayer.appData.saveGameFile[0] != NULLCHAR) 
		  SaveGame(localPlayer.appData.saveGameFile);
		exit(0);
 	}
#else

	lastGameMode = gameMode;
	gameMode = EndOfGame;
	ModeHighlight();
	CopyBoard(boards[currentMove + 1], boards[currentMove]);
	CopyCatches(catches[currentMove + 1], catches[currentMove]);
	strncpy(parseList[currentMove], why, MOVE_LEN);
	parseList[currentMove][MOVE_LEN-1] = NULLCHAR;
	currentMove++;
	DisplayMessage(why,False);

	if (readGameXID != 0)
		XtRemoveTimeOut(readGameXID);
	readGameXID = 0;

	if (firstProgramPID != 0) {
		fclose(fromFirstProgFP);
		fclose(toFirstProgFP);
		fromFirstProgFP = toFirstProgFP = NULL;
		if (kill(firstProgramPID, SIGTERM)==0) 
		  wait((union wait *)0);

	}
	firstProgramPID = 0;

	if (firstProgramXID != 0)
		XtRemoveInput(firstProgramXID);
	firstProgramXID = 0;

	if (secondProgramPID != 0) {
		fclose(fromSecondProgFP);
		fclose(toSecondProgFP);
		fromSecondProgFP = toSecondProgFP = NULL;
		if (kill(secondProgramPID, SIGTERM)==0) 
		  wait((union wait *)0);
	}
	secondProgramPID = 0;

	if (secondProgramXID != 0)
		XtRemoveInput(secondProgramXID);
	secondProgramXID = 0;

	DisplayClocks(StopTimers);

 	if(matchMode != MatchFalse) {
 		if (localPlayer.appData.saveGameFile[0] != NULLCHAR) 
		  SaveGame(localPlayer.appData.saveGameFile);
		exit(0);
 	}
#endif
}

void
CommentPopUp(label)
	char *label;
{
	Arg args[2];
	Position x, y;
	Dimension bw_width, pw_width;

	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}

	DisplayMessage("Comment",False);

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtGetValues(localPlayer.formWidget, args, 1);

	XtSetArg(args[0], XtNresizable, True);
	XtSetArg(args[1], XtNwidth, bw_width - 8);

	commentShell = XtCreatePopupShell("Comment",
		transientShellWidgetClass, localPlayer.commandsWidget, args, 2);

	XtSetArg(args[0], XtNlabel, label);

	(void) XtCreateManagedWidget("commentLabel", labelWidgetClass,
		commentShell, args, 1);

	XtRealizeWidget(commentShell);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtGetValues(commentShell, args, 1);

	XtTranslateCoords(localPlayer.shellWidget, (bw_width - pw_width) / 2, -50, &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(commentShell, args, 2);

	XtPopup(commentShell, XtGrabNone);
	commentUp = True;
}

void
FileNamePopUp(label, proc)
	char *label;
	Boolean (*proc) P((char *name));
{
	Arg args[2];
	Widget popup, dialog;
	Position x, y;
	Dimension bw_width, pw_width;

	fileProc = proc;

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtGetValues(localPlayer.boardWidget, args, 1);

	XtSetArg(args[0], XtNresizable, True);
	XtSetArg(args[1], XtNwidth, DIALOG_SIZE);

	popup = XtCreatePopupShell("File Name Prompt",
		transientShellWidgetClass, localPlayer.commandsWidget, args, 2);

	XtSetArg(args[0], XtNlabel, label);
	XtSetArg(args[1], XtNvalue, "");

	dialog = XtCreateManagedWidget("dialog", dialogWidgetClass,
		popup, args, 2);

	XawDialogAddButton(dialog, "ok", FileNameCallback, (XtPointer) dialog);
	XawDialogAddButton(dialog, "cancel", FileNameCallback,
			   (XtPointer) dialog);

	XtRealizeWidget(popup);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtGetValues(popup, args, 1);

	XtTranslateCoords(localPlayer.boardWidget, (bw_width - pw_width) / 2, 10, &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(popup, args, 2);

	XtPopup(popup, XtGrabExclusive);
	filenameUp = True;

	XtSetKeyboardFocus(localPlayer.shellWidget, popup);
}

void
FileNameCallback(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	String name;
	Arg args[1];

	XtSetArg(args[0], XtNlabel, &name);
	XtGetValues(w, args, 1);

	if (strcmp(name, "cancel") == 0) {
		XtPopdown(w = XtParent(XtParent(w)));
		XtDestroyWidget(w);
		filenameUp = False;
		ModeHighlight();
		return;
	}

	FileNameAction(w, NULL, NULL, NULL);
}

void
FileNameAction(w, event, prms, nprms)
	Widget w;
	XEvent *event;
	String *prms;
	Cardinal *nprms;
{
	char buf[MSG_SIZ];
	String name;

	name = XawDialogGetValueString(w = XtParent(w));

	if ((name != NULL) && (*name != NULLCHAR)) {
		strcpy(buf, name);
		XtPopdown(w = XtParent(w));
		XtDestroyWidget(w);
		filenameUp = False;
		(*fileProc)(buf);  /* I can't see a way not
				      to use a global here */
		ModeHighlight();
		return;
	}

	XtPopdown(w = XtParent(w));
	XtDestroyWidget(w);
	filenameUp = False;
	ModeHighlight();
}




typedef struct {
	ChessSquare piece;
	int to_x, to_y;
} PromotionMoveInfo;

static PromotionMoveInfo pmi;  /*making this global is gross */
      

void
PromotionPopUp(piece, to_x, to_y, fromRemotePlayer)
         ChessSquare piece;
         int to_x, to_y;
	 int fromRemotePlayer;
{
	Arg args[2];
	Widget dialog;
	Position x, y;
	Dimension bw_width, bw_height, pw_width, pw_height;
	
	player = fromRemotePlayer ? &remotePlayer : &localPlayer;

	pmi.piece = piece;
	pmi.to_x = to_x;
	pmi.to_y = to_y;

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtSetArg(args[1], XtNheight, &bw_height);
	XtGetValues(player->boardWidget, args, 2);

	XtSetArg(args[0], XtNresizable, True);

	player->promotionShell = XtCreatePopupShell("Promotion",
		transientShellWidgetClass, player->commandsWidget, args, 1);
                                      
	XtSetArg(args[0], XtNlabel, "Promote piece?");
	dialog = XtCreateManagedWidget("promotion", dialogWidgetClass,
		player->promotionShell, args, 1);

	XawDialogAddButton(dialog, "Yes", PromotionCallback, 
			   (XtPointer) dialog);
	XawDialogAddButton(dialog, "No", PromotionCallback, 
			   (XtPointer) dialog);
	XawDialogAddButton(dialog, "cancel", PromotionCallback, 
			   (XtPointer) dialog);

	XtRealizeWidget(player->promotionShell);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtSetArg(args[1], XtNheight, &pw_height);
	XtGetValues(player->promotionShell, args, 2);

	XtTranslateCoords(player->boardWidget, (bw_width - pw_width) / 2,
			  LINE_GAP + player->squareSize/3 +
			  ((piece == BlackPawn) ^ (player->flipView) ?
			   0 : 6*(player->squareSize + LINE_GAP)),
			  &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(player->promotionShell, args, 2);

	XtPopup(player->promotionShell, XtGrabNone);

	player->promotionUp = True;
}

void
PromotionCallback(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	String name;
	Arg args[1];
	ChessMove move_type;
	struct DisplayData *player;;

	XtSetArg(args[0], XtNlabel, &name);
	XtGetValues(w, args, 1);

	w = XtParent(XtParent(w));
	player = (w == remotePlayer.promotionShell) ? &remotePlayer : &localPlayer;
	XtPopdown(w);
	XtDestroyWidget(w);
	player->promotionUp = False;

	if (fromX == -1) return;

	if (strcmp(name, "Yes") == 0) {
		if ((int) pmi.piece < (int) WhitePawn)
		  move_type = BlackPromotion;
		else
		  move_type = WhitePromotion;
	} else if (strcmp(name, "No") == 0) {
                move_type = NormalMove;
	} else /* strcmp(name, "cancel") == 0 */ {
		fromX = fromY = -1;
		return;
	}

	MakeMove(&move_type, fromX, fromY, pmi.to_x, pmi.to_y);
#if BLINK_COUNT
        if ( updateRemotePlayer )
          BlinkSquare(pmi.to_y, pmi.to_x, boards[currentMove][pmi.to_y][pmi.to_x]);
#endif
	FinishUserMove(move_type, pmi.to_x, pmi.to_y);
}


typedef struct {
	char mode[2];
	char name[100];
} FileModeInfo;

static FileModeInfo fmi;


void
FileModePopUp (name)
	char *name;
{
	Arg args[2];
	Widget dialog;
	Position x, y;
	Dimension bw_width, bw_height, pw_width, pw_height;

	struct DisplayData *player = &localPlayer;

	strcpy(fmi.name,name);

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtSetArg(args[1], XtNheight, &bw_height);
	XtGetValues(player->boardWidget, args, 2);

	XtSetArg(args[0], XtNresizable, True);
	player->filemodeShell = XtCreatePopupShell("FileMode",
		transientShellWidgetClass, player->commandsWidget, args, 1);
                                      
	XtSetArg(args[0], XtNlabel, "Append to existing file?");
	dialog = XtCreateManagedWidget("filemode", dialogWidgetClass,
		player->filemodeShell, args, 1);

	XawDialogAddButton(dialog, "Yes", FileModeCallback, 
			   (XtPointer) dialog);
	XawDialogAddButton(dialog, "No", FileModeCallback, 
			   (XtPointer) dialog);
	XawDialogAddButton(dialog, "cancel", FileModeCallback, 
			   (XtPointer) dialog);

	XtRealizeWidget(player->filemodeShell);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtSetArg(args[1], XtNheight, &pw_height);
	XtGetValues(player->filemodeShell, args, 2);

	XtTranslateCoords(player->boardWidget, (bw_width - pw_width) / 2,
			  LINE_GAP + player->squareSize/3 +
			  (6*(player->squareSize + LINE_GAP)),
			  &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(player->filemodeShell, args, 2);

	XtPopup(player->filemodeShell, XtGrabNone);

	filemodeUp = True;
}

void
FileModeCallback(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	String name;
	Arg args[1];

	XtSetArg(args[0], XtNlabel, &name);
	XtGetValues(w, args, 1);

	XtPopdown(w = XtParent(XtParent(w)));
	XtDestroyWidget(w);

	if (strcmp(name, "Yes") == 0) {
		strcpy(fmi.mode,"a");
	} else if (strcmp(name, "No") == 0) {
                strcpy(fmi.mode,"w");
	} else /* strcmp(name, "cancel") == 0 */ {
               filemodeUp = False;
	       return;
	}

	XtPopdown(localPlayer.filemodeShell);
	XtDestroyWidget(localPlayer.filemodeShell);

	SaveGame(fmi.name);

	filemodeUp = False;

}







void
SelectCommand(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	Cardinal fromRemotePlayer = (Cardinal)client_data;

	XawListReturnStruct *list_return = XawListShowCurrent(w);

	player = fromRemotePlayer ? &remotePlayer : &localPlayer;

	fromX = fromY = -1;

	if (player->promotionUp) {
		XtPopdown(player->promotionShell);
		XtDestroyWidget(player->promotionShell);
		player->promotionUp = False;
	}

        (*buttonProcs[list_return->list_index])(w, NULL, NULL, &fromRemotePlayer);
    
	if (!filenameUp) ModeHighlight();
}                    




void HighlightProcButton(proc)
     XtActionProc proc;
{
    int i = 0;
    
    if (proc == NULL) {
	XawListUnhighlight(localPlayer.commandsWidget);
        if ( updateRemotePlayer )
          XawListUnhighlight(remotePlayer.commandsWidget);
	return;
    }
    
    for (;;) {
	if (buttonProcs[i] == NULL) {
	    XawListUnhighlight(localPlayer.commandsWidget);
            if ( updateRemotePlayer )
              XawListUnhighlight(remotePlayer.commandsWidget);
	    return;
	}
	if (buttonProcs[i] == proc) {
	    XawListHighlight(localPlayer.commandsWidget, i);
            if ( updateRemotePlayer )
              XawListHighlight(remotePlayer.commandsWidget, i);
	    return;
	}
	i++;
    }
}


void
ModeHighlight ()
{
    switch (gameMode) {
      case BeginningOfGame:
#ifdef ISS
	if (localPlayer.appData.issActive)
	  HighlightProcButton(NULL);
	else
#endif 
        if (localPlayer.appData.noChessProgram)
	  HighlightProcButton(ForceProc);
	else
	  HighlightProcButton(MachineBlackProc);
	break;
      case MachinePlaysBlack:
	HighlightProcButton(MachineBlackProc);
	break;
      case MachinePlaysWhite:
	HighlightProcButton(MachineWhiteProc);
	break;
      case TwoMachinesPlay:
	HighlightProcButton(TwoMachinesProc);
	break;
      case ForceMoves:
#ifdef ISS
	if (localPlayer.appData.issActive)
	  HighlightProcButton(NULL);
	else
#endif
	  HighlightProcButton(ForceProc);
	break;
      case PlayFromGameFile:
	HighlightProcButton(LoadGameProc);
	break;
      case PauseGame:
	HighlightProcButton(PauseProc);
	break;
      case EditPosition:
	HighlightProcButton(EditPositionProc);
	break;
      case EndOfGame:
      default:
	HighlightProcButton(NULL);
	break;
    }
}

/*
 * Button procedures
 */
    
void
QuitRemotePlayerProc()
{               
	/* This should be modified... */
	XCloseDisplay(remotePlayer.xDisplay);
	/* XtDestroyWidget(remotePlayer.shellWidget); */
	updateRemotePlayer = False;
	DisplayMessage("Remote player has pressed Quit",False);
	fromX = fromY = -1;
}
   

#ifdef ISS

void QuitProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    /* Save game if resource set and not already saved */
    if (*endMessage == NULLCHAR) {
	if (localPlayer.appData.saveGameFile[0] != NULLCHAR) 
	  SaveGame(localPlayer.appData.saveGameFile);
	if (localPlayer.appData.savePositionFile[0] != NULLCHAR) 
	  SavePosition(localPlayer.appData.savePositionFile);
    }
    ShutdownChessPrograms(NULL);
    if (telnetPID != 0) {
	if (kill(telnetPID, SIGTERM) == 0)
	  wait((union wait *) 0);
    }
    exit(0);
}

void CallFlagProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    /* Call your opponent's flag (claim a win on time) */
    if (localPlayer.appData.issActive) {
	SendToISS("flag\n");
    } else {
	switch (gameMode) {
	  default:
	    return;
	  case MachinePlaysWhite:
	    if (whiteFlag) {
		if (blackFlag)
		  ShutdownChessPrograms("Draw (both players ran out of time)");
		else
		  ShutdownChessPrograms("Black wins on time");
	    }
	    break;
	  case MachinePlaysBlack:
	    if (blackFlag) {
		if (whiteFlag)
		  ShutdownChessPrograms("Draw (both players ran out of time)");
		else
		  ShutdownChessPrograms("White wins on time");
	    }
	    break;
	}
    }
}

void DrawProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    /* Offer draw or accept pending draw offer from opponent */
    
    if (localPlayer.appData.issActive) {
	/* Note: tournament rules require draw offers to be
	   made after you make your move but before you punch
	   your clock.  Currently ISS doesn't let you do that;
	   instead, you always punch your clock after making a
	   move, but you can offer a draw at any time. */
	
	SendToISS("draw\n");
    } else {
	/* Currently GNU Shogi doesn't offer or accept draws
	   at all, so there is no Draw button in GNU Shogi
	   mode.  */
	
	fprintf(stderr, "Draw function not implemented\n");
    }
}


void DeclineDrawProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    /* Decline a pending draw offer from opponent */
    
    if (localPlayer.appData.issActive) {
	/* Note: ISS also lets you withdraw your own draw
	   offer with this command.  I'm not sure how long
	   your draw offer remains pending if you don't
	   withdraw it. */
	
	SendToISS("decline draw\n");
    } else {
	/* Currently GNU Shogi doesn't offer or accept draws
	   at all, so there is no Decline Draw button in
	   GNU Shogi mode.  */
	
	fprintf(stderr, "Decline Draw function not implemented\n");
    }
}


void ResignProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    /* Resign.  You can do this even if it's not your turn. */
    
    if (localPlayer.appData.issActive) {
	SendToISS("resign\n");
    } else {
	/* This button is not currently used in GNU Chess mode,
	   but it should work. */
	
	switch (gameMode) {
	  case MachinePlaysWhite:
	    ShutdownChessPrograms("Black resigns");
	    break;
	  case MachinePlaysBlack:
	    ShutdownChessPrograms("White resigns");
	    break;
	  default:
	    break;
	}
    }
}


#else
                        
void QuitProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	if ( updateRemotePlayer )
	  QuitRemotePlayerProc();
	ShutdownChessPrograms("Quitting");
	exit(0);
}


#endif



void LoadGameProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{                
	int fromRemotePlayer = *nprms;

        if ( fromRemotePlayer ) {
	  DisplayMessage("only opponent may load game",fromRemotePlayer);
	  return;
        }

	if (gameMode != BeginningOfGame) {
		DisplayMessage("Press Reset first",False);
		return;
	}
	if (localPlayer.appData.loadGameFile == NULL)
		FileNamePopUp("Game file name?", LoadGame);
	else
		(void) LoadGame(localPlayer.appData.loadGameFile);
}



Boolean LoadGame(name)
	char *name;
{
	char buf[MSG_SIZ];
        ChessMove cm;

	if (gameMode != BeginningOfGame) {
		DisplayMessage("Press Reset first",False);
		return (int) False;
	}

	if (localPlayer.appData.loadGameFile != name) {
		if (localPlayer.appData.loadGameFile)
			XtFree(localPlayer.appData.loadGameFile);
		localPlayer.appData.loadGameFile = XtMalloc(strlen(name)+1);
		strcpy(localPlayer.appData.loadGameFile, name);
	}

	if ((gameFileFP = fopen(name, "r")) == NULL) {
		sprintf(buf, "Can't open %s", name);
		DisplayMessage(buf,False);
		XtFree(localPlayer.appData.loadGameFile);
		localPlayer.appData.loadGameFile = NULL;
		return (int) False;
	}

	lastGameMode = gameMode = PlayFromGameFile;
	ModeHighlight();
	InitPosition(True);
	DisplayClocks(StopTimers);
	if (firstProgramXID == 0)
	  InitChessProgram(localPlayer.appData.firstHost, localPlayer.appData.firstChessProgram,
			   &firstProgramPID, &toFirstProgFP,
			   &fromFirstProgFP, &firstProgramXID,
			   &firstSendTime);
	SendToProgram(localPlayer.appData.initString, toFirstProgFP);
	SendSearchDepth(toFirstProgFP);
	SendToProgram("force\n", toFirstProgFP);

	currentMove = forwardMostMove = backwardMostMove = 0;

	ReadGameFile();

	return True;
}

void
ResurrectChessProgram()
     /* Restart the chess program and feed it all the moves made so far.
	Used when the user wants to back up from end of game, when gnuchess 
	has already exited.  Assumes gameMode == EndOfGame. */
{
	char buf[MSG_SIZ];
	int i;

	if (currentMove > 0)
	  currentMove--;  /* delete "Black wins" or the like */

	InitChessProgram(localPlayer.appData.firstHost, localPlayer.appData.firstChessProgram,
		&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
		&firstProgramXID, &firstSendTime);
	SendToProgram(localPlayer.appData.initString, toFirstProgFP);
	SendSearchDepth(toFirstProgFP);
	SendToProgram("force\n", toFirstProgFP);
	gameMode = lastGameMode = ForceMoves;
	ModeHighlight();

	i = whitePlaysFirst ? 1 : 0;

	if (startedFromSetupPosition) {
		SendBoard(toFirstProgFP, boards[i], catches[i]);
	}

	for (; i < currentMove; i++) {
		strcpy(buf, moveList[i]);
		SendToProgram(buf, toFirstProgFP);
	}

#ifdef SENDTIME
        if (!firstSendTime) {
	  /* can't tell gnuchess what its clock should read,
	     so we bow to its notion. */
	    DisplayClocks(ResetTimers);
	    timeRemaining[0][currentMove] = blackTimeRemaining;
	    timeRemaining[1][currentMove] = whiteTimeRemaining;
        }
#else
	DisplayClocks(ResetTimers);
	  /* ugh, but there's no way to tell gnuchess what the clocks
	     should read, so the best we can do is bow to its notion.
	     !!gnuchess 4.0 adds a feature that will let us fix this. */
#endif
}


void MachineWhiteProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	int fromRemotePlayer = *nprms;

	if ( updateRemotePlayer ) {
	  DisplayMessage("no machine moves in challenge mode",fromRemotePlayer);
          return;
   	}

        if (gameMode == PauseGame) PauseProc(w, event, prms, nprms);
        if (gameMode == PlayFromGameFile) ForceProc(w, event, prms, nprms);
	if (gameMode == EditPosition) EditPositionDone();

	if ((gameMode == EndOfGame) || (gameMode == PlayFromGameFile)
	    || (gameMode == TwoMachinesPlay) || localPlayer.appData.noChessProgram
	    || (gameMode == MachinePlaysWhite))
		return;

	if (BlackOnMove(gameMode == ForceMoves ? currentMove : forwardMostMove)) {
		DisplayMessage("It is not White's turn",False);
		return;
	}

        if (gameMode == ForceMoves) forwardMostMove = currentMove;
    
	lastGameMode = gameMode = MachinePlaysWhite;
	ModeHighlight();
	SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);
	DisplayClocks(StartTimers);
}
       

void MachineBlackProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	int fromRemotePlayer = *nprms;

	if ( updateRemotePlayer ) {
	  DisplayMessage("no machine moves in challenge mode",fromRemotePlayer);
          return;
   	}

        if (gameMode == PauseGame) PauseProc(w, event, prms, nprms);
        if (gameMode == PlayFromGameFile) ForceProc(w, event, prms, nprms);
	if (gameMode == EditPosition) EditPositionDone();

	if ((gameMode == EndOfGame) || (gameMode == PlayFromGameFile)
	    || (gameMode == TwoMachinesPlay) || localPlayer.appData.noChessProgram
	    || (gameMode == MachinePlaysBlack))
		return;

        if (!BlackOnMove(gameMode == ForceMoves ? currentMove : forwardMostMove)) {
		DisplayMessage("It is not Black's turn",False);
		return;
	}

        if (gameMode == ForceMoves) forwardMostMove = currentMove;
    
	lastGameMode = gameMode = MachinePlaysBlack;
	ModeHighlight();
	SendToProgram(localPlayer.appData.blackString, toFirstProgFP);
	DisplayClocks(StartTimers);
}



void ForwardProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    char buf[MSG_SIZ];
    int target;
    unsigned int state;
    int i, j;  

    int fromRemotePlayer = *nprms;

    if ( updateRemotePlayer ) {
	  DisplayMessage("Forward button disabled",fromRemotePlayer);
	  return;
    }

    if ((gameMode == EndOfGame) || (gameMode == EditPosition))
      return;
    
    if (gameMode == PlayFromGameFile)
      PauseProc(w, event, prms, nprms);
     
    if (currentMove >= forwardMostMove) {
#ifdef notdef
	if (gameFileFP != NULL)
	  (void) LoadGameOneMove();
#endif
	return;
    }
    
    if (event == NULL) {
	/* Kludge */
	Window root, child;
	int root_x, root_y;
	int win_x, win_y;
	XQueryPointer(localPlayer.xDisplay, localPlayer.xBoardWindow,
		      &root, &child, &root_x, &root_y,
		      &win_x, &win_y, &state);
    } else {
	state = event->xkey.state;
    }
    
    if (state & ShiftMask)
      target = forwardMostMove;
    else
      target = currentMove + 1;
    
    if (gameMode == ForceMoves) {
	while (currentMove < target) {
	    strcpy(buf, moveList[currentMove++]);
	    SendToProgram(buf, toFirstProgFP);
	}
    } else {
	currentMove = target;
    }
    
#ifdef ISS
    if (!localPlayer.appData.issActive && gameMode == ForceMoves) {
#else
    if (gameMode == ForceMoves) {
#endif
	    blackTimeRemaining = timeRemaining[0][currentMove];
	    whiteTimeRemaining = timeRemaining[1][currentMove];
    }

    DisplayClocks(ReDisplayTimers);
    DisplayMove(currentMove - 1);
    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}

void
ResetFileProc()
{
	char *buf = "";

        if ( updateRemotePlayer ) {
	  return;
        }	

	if (localPlayer.appData.loadGameFile);
		XtFree(localPlayer.appData.loadGameFile);
	if (localPlayer.appData.loadPositionFile);
		XtFree(localPlayer.appData.loadPositionFile);
	localPlayer.appData.loadGameFile = localPlayer.appData.loadPositionFile = NULL;
	DisplayName(buf);
	if (gameFileFP != NULL) {
		fclose(gameFileFP);
		gameFileFP = NULL;
	}
}

void
ResetChallenge()
{
	char *buf = "";

	if (localPlayer.appData.challengeDisplay);
		XtFree(localPlayer.appData.challengeDisplay);
	localPlayer.appData.challengeDisplay = NULL;
	DisplayName(buf);
}
    
     

void ResetProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    int fromRemotePlayer = *nprms;

    if ( fromRemotePlayer ) {
	DisplayMessage("only your opponent may reset the game",fromRemotePlayer);
	return;
    }	

    Reset(True);
}
 

void Reset(redraw)
	int /*Boolean*/ redraw;
{
	ResetFileProc();
	ResetChallenge();

	localPlayer.flipView = False;
	remotePlayer.flipView = True;
	startedFromSetupPosition = whitePlaysFirst = False;
	matchMode = MatchFalse;
	firstMove = True;
	blackFlag = whiteFlag = False;
	maybeThinking = False; 

        endMessage[0] = NULLCHAR;
#ifdef ISS
        iss_white[0] = iss_black[0] = NULLCHAR;
        iss_user_moved = iss_getting_history = False;
        iss_mode = IssIdle;
        iss_gamenum = -1;
#endif

	ShutdownChessPrograms("");
	lastGameMode = gameMode = BeginningOfGame;
	ModeHighlight();
	InitPosition(redraw);
	DisplayClocks(ResetTimers);
        timeRemaining[0][0] = blackTimeRemaining;
        timeRemaining[1][0] = whiteTimeRemaining;
	InitChessProgram(localPlayer.appData.firstHost, localPlayer.appData.firstChessProgram,
			 &firstProgramPID, &toFirstProgFP,
			 &fromFirstProgFP, &firstProgramXID, &firstSendTime);
	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}
	if (localPlayer.promotionUp) {
		XtPopdown(localPlayer.promotionShell);
		XtDestroyWidget(localPlayer.promotionShell);
		localPlayer.promotionUp = False;
	}
	if (updateRemotePlayer && remotePlayer.promotionUp) {
		XtPopdown(remotePlayer.promotionShell);
		XtDestroyWidget(remotePlayer.promotionShell);
		remotePlayer.promotionUp = False;
	}
	
}



void
ClearCatches (catches)
	Catched catches;
{
	int c, p;
	for ( c = 0; c <= 1; c++)
		for ( p = 0; p <= 7; p++ )
			catches[c][p] = 0;
}





Boolean
Challenge(name)
	char *name;
{
	char buf[MSG_SIZ];            
	int argc;
	char **argv;
	XrmDatabase database;

	if (gameMode != BeginningOfGame) {
		DisplayMessage("Press Reset first",False);
		return (int) False;
	}

	if (localPlayer.appData.challengeDisplay != name) {
		if (localPlayer.appData.challengeDisplay)
			XtFree(localPlayer.appData.challengeDisplay);
		localPlayer.appData.challengeDisplay = XtMalloc(strlen(name)+1);
		strcpy(localPlayer.appData.challengeDisplay, name);
	}
                                    
        sprintf(buf,"trying to connect to %s.....",name);
	DisplayMessage(buf,False);

	argc = global_argc;
	argv = global_argv;

	if ( (remotePlayer.xDisplay = XtOpenDisplay(appContext, name, "XShogi",
		    "XShogi", 0, 0, &argc, argv)) == NULL ) {
		sprintf(buf, "Can't open display %s", name);
		DisplayMessage(buf,False);
		XtFree(localPlayer.appData.challengeDisplay);
		localPlayer.appData.challengeDisplay = NULL;
		return (int) False;
	}        

	DisplayMessage("connected! creating remote window...",False);

        remotePlayer.xScreen = DefaultScreen(remotePlayer.xDisplay);

	remotePlayer.shellWidget = XtAppCreateShell(NULL, "XShogi",
		applicationShellWidgetClass, remotePlayer.xDisplay, NULL, 0);

	database = XtDatabase(remotePlayer.xDisplay);

	XrmParseCommand(&database,
		shellOptions, XtNumber(shellOptions), "XShogi", &argc, argv);

	XtGetApplicationResources(remotePlayer.shellWidget, &remotePlayer.appData, clientResources,
		XtNumber(clientResources), NULL, 0);
                                  
	player = &remotePlayer;

	CreatePlayerWindow ();

	updateRemotePlayer = True;

	DisplayName("REMOTE");
	DrawPosition(remotePlayer.boardWidget, NULL, NULL, NULL);
	DisplayClocks(ReDisplayTimers);

	DisplayMessage("ready to play",False);
	DisplayMessage("ready to play",True);

	return True;
}
                                   
void ChallengeProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;            
{                           
	int fromRemotePlayer = *nprms;

	if ( updateRemotePlayer ) {
		DisplayMessage("you are already in challenge mode",fromRemotePlayer);
		return;
	} 		

	if (gameMode != BeginningOfGame) {
		DisplayMessage("Press Reset first",False);
		return;
	}             

	if (localPlayer.appData.challengeDisplay == NULL)
		FileNamePopUp("Challenge display?", Challenge);
	else
		(void) Challenge(localPlayer.appData.challengeDisplay);
}



                   
Boolean SelectLevel(command)
	char *command;
{
	char buf[MSG_SIZ];

	sprintf(buf,"level %s\n",command);            
	SendToProgram(buf, toFirstProgFP);

	return True;
}                     

                                   
void SelectLevelProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;            
{
  if ((BlackOnMove(forwardMostMove) && gameMode == MachinePlaysBlack)
    || (!BlackOnMove(forwardMostMove) && gameMode == MachinePlaysWhite)) {
    DisplayMessage("Wait until your turn",False);
  } else {
    FileNamePopUp("#moves #minutes", SelectLevel);
  }
}


void MoveNowProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
  if ((!BlackOnMove(forwardMostMove) && gameMode == MachinePlaysBlack)
    || (BlackOnMove(forwardMostMove) && gameMode == MachinePlaysWhite)) {
    DisplayMessage("Wait until machines turn",False);
  } else {
    Attention(firstProgramPID);
  }
}



void LoadPositionProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{        
    int fromRemotePlayer = *nprms;

    if ( fromRemotePlayer ) {
	  DisplayMessage("only opponent may load position",fromRemotePlayer);
	  return;
    }

    if (gameMode != BeginningOfGame) {
	DisplayMessage("Press Reset first",False);
	return;
    }
    FileNamePopUp("Position file name?", LoadPosition);
}


static int empty_line(line)
char *line;
{
  while ( *line != '\0' ) {
    if ( *line != ' ' )
      return ((*line == '#') ? 1 : 0);
    line++;
  }
  return 1;
}


Boolean
LoadPosition(name)
	char *name;
{
	char *p, line[MSG_SIZ], buf[MSG_SIZ];
	Board initial_position;
	Catched initial_catches;
	FILE *fp;
	int i, j;

	if (gameMode != BeginningOfGame) {
		DisplayMessage("Press Reset first",False);
		return False;
	}

        if (localPlayer.appData.loadPositionFile != name) {
	  if (localPlayer.appData.loadPositionFile)
		XtFree(localPlayer.appData.loadPositionFile);
 	  localPlayer.appData.loadPositionFile = XtMalloc(strlen(name)+1);
	  strcpy(localPlayer.appData.loadPositionFile, name);
	}

	if ((fp = fopen(name, "r")) == NULL) {
		sprintf(buf, "Can't open %s", name);
		DisplayMessage(buf,False);
                XtFree(localPlayer.appData.loadPositionFile);
		localPlayer.appData.loadPositionFile = NULL;
		return False;
	}

	lastGameMode = gameMode = ForceMoves;
	ModeHighlight();
	startedFromSetupPosition = True;

	if (firstProgramXID == 0)
	  InitChessProgram(localPlayer.appData.firstHost, localPlayer.appData.firstChessProgram,
			   &firstProgramPID, &toFirstProgFP,
			   &fromFirstProgFP, &firstProgramXID,
			   &firstSendTime);
	/*
	 * chack and skip header information in position file
	 */
	fgets(line, MSG_SIZ, fp);
	line[strlen(line) - 1] = NULLCHAR;
	sprintf(buf, "# %s position file", programName);
	if ( strncmp(line,buf,strlen(buf)) ) {
	  strcat(line,": no xshogi position file");
	  DisplayMessage(line,False);  
	  return False;
	}
	DisplayName(line);
	fgets(line, MSG_SIZ, fp); /* skip opponents */

	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		fgets(line, MSG_SIZ, fp);
		for (p = line, j = 0; j < BOARD_SIZE; p++) {
		  int promoted;
			if (*p == '+')
				promoted = True;
			if (*p == ' ')
				promoted = False;
			p++;              
			initial_position[i][j++] = CharToPiece(*p,promoted);
		}
	}
        { int color;
	  for ( color = 0; color <= 1; color++ ) {
	    fscanf(fp, "%i%i%i%i%i%i%i%i\n",
		&initial_catches[color][pawn],
		&initial_catches[color][lance],
		&initial_catches[color][knight],
		&initial_catches[color][silver],
		&initial_catches[color][gold],
		&initial_catches[color][bishop],
		&initial_catches[color][rook],
		&initial_catches[color][king]);
	  };
	}

	whitePlaysFirst = False;
        if (!feof(fp)) {
		fgets(line, MSG_SIZ, fp);
		if (strncmp(line, "white", strlen("white"))==0)
		  whitePlaysFirst = True;
	}
	fclose(fp);

	if (whitePlaysFirst) {
		CopyBoard(boards[0], initial_position);
                CopyCatches(catches[0], initial_catches);
		strcpy(moveList[0], " ...\n");
		strcpy(parseList[0], " ...\n");
		currentMove = forwardMostMove = backwardMostMove = 1;
		CopyBoard(boards[1], initial_position);
                CopyCatches(catches[1], initial_catches);
		SendToProgram("white\n", toFirstProgFP);
		SendToProgram("force\n", toFirstProgFP);
		SendCurrentBoard(toFirstProgFP);
		DisplayMessage("White to play",False);
	} else {
		currentMove = forwardMostMove = backwardMostMove = 0;
		CopyBoard(boards[0], initial_position);
                CopyCatches(catches[0], initial_catches);
		SendCurrentBoard(toFirstProgFP);
		SendToProgram("force\n", toFirstProgFP);
		DisplayMessage("Black to play",False);
	}

	DisplayClocks(ResetTimers);
	timeRemaining[0][1] = blackTimeRemaining;
	timeRemaining[1][1] = whiteTimeRemaining;

	DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
	return True;
}    


void EditPositionProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{                                               
	int fromRemotePlayer = *nprms;

	if ( updateRemotePlayer ) {
	  DisplayMessage("Edit button disabled",fromRemotePlayer);
	  return;
	}

	if (gameMode == EditPosition) return;

	ForceProc(w, event, prms, nprms);
	if (gameMode != ForceMoves) return;

	DisplayName("<-- Press to set side to play next");
	DisplayMessage("Mouse: 1=drag, 2=black, 3=white",False);

	lastGameMode = gameMode = EditPosition;
	ModeHighlight();
	if (currentMove > 0)
	  CopyBoard(boards[0], boards[currentMove]);
	
	whitePlaysFirst = !BlackOnMove(forwardMostMove);
	currentMove = forwardMostMove = backwardMostMove = 0;
}

void
EditPositionDone()
{
	startedFromSetupPosition = True;
	SendToProgram(localPlayer.appData.initString, toFirstProgFP);
	SendSearchDepth(toFirstProgFP);
	if (whitePlaysFirst) {
		strcpy(moveList[0], " ...\n");
		strcpy(parseList[0], " ...\n");
		currentMove = forwardMostMove = backwardMostMove = 1;
		CopyBoard(boards[1], boards[0]);
		CopyCatches(catches[1], catches[0]);
		SendToProgram("force\n", toFirstProgFP);
		SendCurrentBoard(toFirstProgFP);
		DisplayName(" ");
		DisplayMessage("White to play",False);
	} else {
		currentMove = forwardMostMove = backwardMostMove = 0;
		SendCurrentBoard(toFirstProgFP);
		SendToProgram("force\n", toFirstProgFP);
		DisplayName(" ");
		DisplayMessage("Black to play",False);
	}
	lastGameMode = gameMode = ForceMoves;
}

void BackwardProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    int target;
    unsigned int state;
    int i, j;
    char buf[MSG_SIZ];
                 
    int fromRemotePlayer = *nprms;

    if ( updateRemotePlayer ) {
	DisplayMessage("Backward button disabled",fromRemotePlayer);
	return;
    }              

    ForceProc(w,event,prms,nprms);

    if ((currentMove <= backwardMostMove) || (gameMode == EditPosition))
      return;

    if (gameMode == EndOfGame)
      ResurrectChessProgram();
    
    if (gameMode == PlayFromGameFile)
      PauseProc(w, event, prms, nprms);
    
    if (event == NULL) {
	/* Kludge */
	Window root, child;
	int root_x, root_y;
	int win_x, win_y;
	XQueryPointer(localPlayer.xDisplay, localPlayer.xBoardWindow,
		      &root, &child, &root_x, &root_y,
		      &win_x, &win_y, &state);
    } else {
	state = event->xkey.state;
    }
    if (state & ShiftMask)
      target = backwardMostMove;
    else
      target = currentMove - 1;
    
    if (gameMode == ForceMoves) {
	Attention(firstProgramPID);
	while (currentMove > target) {
	    SendToProgram("undo\n", toFirstProgFP);
	    currentMove--;
	}
    } else {
	currentMove = target;
    }
    
#ifdef ISS
    if (!localPlayer.appData.issActive && gameMode == ForceMoves) {
#else
    if (gameMode == ForceMoves) {
#endif
	whiteTimeRemaining = timeRemaining[0][currentMove];
	blackTimeRemaining = timeRemaining[1][currentMove];
    } 

    DisplayClocks(ReDisplayTimers);
    DisplayMove(currentMove - 1);
    DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}


void FlipViewProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{                                    
  struct DisplayData *player = *nprms ? &remotePlayer : &localPlayer;

  player->flipView = !player->flipView;
  DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
}


void SaveGameProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    char def[MSG_SIZ];
                         
    int fromRemotePlayer = *nprms;

    if ( fromRemotePlayer ) {
	DisplayMessage("only opponent may save game",fromRemotePlayer);
	return;
    }

#ifdef ISS
    if (localPlayer.appData.issActive && iss_white[0] != NULLCHAR) {
	sprintf(def, "%s-%s.game", iss_white, iss_black);
    } else 
#endif 
    {
	def[0] = NULLCHAR;
    }     
    FileNamePopUp("Filename for saved game?", SaveGame);
}


Boolean
SaveGame(name)
	char *name;
{
	char buf[MSG_SIZ], black_move[MSG_SIZ], white_move[MSG_SIZ];
	int i, len, move = 0;
	time_t tm;

	if ( !filemodeUp) /* if called via FileModeCallback avoid recursion */
	  if ((gameFileFP = fopen(name,"r")) == NULL)
		strcpy(fmi.mode,"w");
	  else {
		fclose(gameFileFP);
		FileModePopUp(name);
		return;
	  }

	if ((gameFileFP = fopen(name, fmi.mode)) == NULL) {
		sprintf(buf, "Can't open %s (mode %s)", name, fmi.mode);
		DisplayMessage(buf,False);
		return;
	}

	tm = time((time_t *) NULL);
	gethostname(buf, MSG_SIZ);
        
	fprintf(gameFileFP, "# %s game file -- %s", programName, ctime(&tm)); 
        PrintOpponents(gameFileFP);

	for (i = 0; i < currentMove;) {
		if ( i % 5 == 0 )
		  fprintf(gameFileFP, "\n");
		fprintf(gameFileFP, "%d. %s ", ++move, parseList[i++]);
		if (i >= currentMove) {
			fprintf(gameFileFP, "\n");
			break;
		}
		if ((len = strlen(parseList[i])) == 0) {
			break;
		}
		fprintf(gameFileFP, "%s ", parseList[i++]);
	}  
	
	fprintf(gameFileFP, "\n");

	fclose(gameFileFP);
	gameFileFP = NULL; 

 	return True;
}


void SwitchProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	if (localPlayer.appData.noChessProgram) return;
	switch (gameMode) {
	default:
		return;
	case MachinePlaysBlack:
		if (BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		lastGameMode = gameMode = MachinePlaysWhite;
		ModeHighlight();
		break;
	case BeginningOfGame:
	case MachinePlaysWhite:
		if (!BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		if (forwardMostMove == 0) {
			MachineBlackProc(w, event, prms, nprms);
			return;
		}
		lastGameMode = gameMode = MachinePlaysBlack;
		ModeHighlight();
		break;
	}

	Attention(firstProgramPID);
	SendToProgram("switch\n", toFirstProgFP);
}


void ForceProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{       
 	int i;

	switch (gameMode) {
	      case MachinePlaysBlack:
		if (BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		Attention(firstProgramPID);
		SendToProgram("force\n", toFirstProgFP);
		break;
	      case MachinePlaysWhite:
		if (!BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		Attention(firstProgramPID);
		SendToProgram("force\n", toFirstProgFP);
		break;
	      case BeginningOfGame:
		SendToProgram("force\n", toFirstProgFP);
		break;
	      case PlayFromGameFile:
		if (readGameXID != 0) {
			XtRemoveTimeOut(readGameXID);
			readGameXID = 0;
		}
		if (gameFileFP != NULL) {
			fclose(gameFileFP);
			gameFileFP = NULL;
		}
		break;
	      case EndOfGame:
		ResurrectChessProgram();
		break;
	      case EditPosition:
		EditPositionDone();
		break;
	      case TwoMachinesPlay:
		ShutdownChessPrograms("");
		ResurrectChessProgram();
		return;
	      default:
		return;
	}

        if (gameMode == MachinePlaysWhite ||
	    gameMode == MachinePlaysBlack ||
	    gameMode == TwoMachinesPlay ||
	    gameMode == PlayFromGameFile) {
	    i = forwardMostMove;
	    while (i > currentMove) {
	        SendToProgram("undo\n", toFirstProgFP);
	        i--;
	    }
	    blackTimeRemaining = timeRemaining[0][currentMove];
	    whiteTimeRemaining = timeRemaining[1][currentMove];
	    if (whiteFlag || blackFlag) {
	        whiteFlag = blackFlag = 0;
	    }
	    DisplayTitle("");
        }		
    
	lastGameMode = gameMode = ForceMoves;
	ModeHighlight();
	DisplayClocks(StopTimers);
}


#ifdef ISS

void NothingProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    return;
}

#endif


void HintProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{                                      
	int fromRemotePlayer = *nprms;

	if ( updateRemotePlayer ) {
	  DisplayMessage("no hints in challenge mode",fromRemotePlayer);
   	  return;
   	}

	if (localPlayer.appData.noChessProgram) return;
	switch (gameMode) {
	case MachinePlaysBlack:
		if (BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		break;
	case BeginningOfGame:
	case MachinePlaysWhite:
		if (!BlackOnMove(forwardMostMove)) {
			DisplayMessage("Wait until your turn",False);
			return;
		}
		break;
	default:
		DisplayMessage("No hint available",False);
		return;
	}
	Attention(firstProgramPID);
	SendToProgram("hint\n", toFirstProgFP);
}


void PrintPosition(fp, move)
     FILE *fp;
     int move;
{
    int i, j;
    
	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if ( pieceIsPromoted[(int) boards[currentMove][i][j]] )
				fprintf(fp,"%c",'+');
			else
				fprintf(fp,"%c",' ');
			fprintf(fp, "%c",
				pieceToChar[(int) boards[currentMove][i][j]]);
			if (j == BOARD_SIZE - 1) 
			  fputc('\n', fp);
		}
	}
	{ int color;
	  for (color = 0; color <= 1; color++)
	    { 
	 	fprintf(fp,"%i %i %i %i %i %i %i %i\n",
			catches[currentMove][color][pawn],
			catches[currentMove][color][lance],
			catches[currentMove][color][knight],
			catches[currentMove][color][silver],
			catches[currentMove][color][gold],
			catches[currentMove][color][bishop],
			catches[currentMove][color][rook],
			catches[currentMove][color][king]);
            };
        }

	if ((gameMode == EditPosition) ? !whitePlaysFirst : BlackOnMove(forwardMostMove))
	  fprintf(fp, "black to play\n");
	else
	  fprintf(fp, "white to play\n");

}


void PrintOpponents(fp)
     FILE *fp;
{
    char host_name[MSG_SIZ];
    
#if defined(SVR4) && defined(sun)
    sysinfo(SI_HOSTNAME, host_name, MSG_SIZ);
#else
    gethostname(host_name, MSG_SIZ);
#endif
    switch (lastGameMode) {
      case MachinePlaysWhite:
	fprintf(fp, "# %s@%s vs. %s@%s\n", localPlayer.appData.firstChessProgram,
		localPlayer.appData.firstHost, getpwuid(getuid())->pw_name,
		host_name);
	break;
      case MachinePlaysBlack:
	fprintf(fp, "# %s@%s vs. %s@%s\n", getpwuid(getuid())->pw_name,
		host_name, localPlayer.appData.firstChessProgram,
		localPlayer.appData.firstHost);
	break;
      case TwoMachinesPlay:
	fprintf(fp, "# %s@%s vs. %s@%s\n", localPlayer.appData.secondChessProgram,
		localPlayer.appData.secondHost, localPlayer.appData.firstChessProgram,
		localPlayer.appData.firstHost);
	break;
      default:
#ifdef ISS
	if (localPlayer.appData.issActive && iss_white[0] != NULLCHAR) {
	    fprintf(fp, "# %s vs. %s\n",
		    iss_white, iss_black);
	} else 
#endif
	{
	    fprintf(fp, "#\n");
	}
	break;
    }
}




void SavePositionProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
    char def[MSG_SIZ];

    int fromRemotePlayer = *nprms;                         

    if ( fromRemotePlayer ) {
	DisplayMessage("only opponent may save game",fromRemotePlayer);
	return;
    }

#ifdef ISS
    if (localPlayer.appData.issActive && iss_black[0] != NULLCHAR) {
	sprintf(def, "%s-%s.pos", iss_black, iss_white);
    } else
#endif 
    {
	def[0] = NULLCHAR;
    }
    FileNamePopUp("Filename for saved position?", SavePosition);
}

Boolean
SavePosition(name)
	char *name;
{
	char buf[MSG_SIZ], host_name[MSG_SIZ];
	FILE *fp;
	time_t tm;
	int i, j;

	if ((fp = fopen(name, "w")) == NULL) {
		sprintf(buf, "Can't open %s", name);
		DisplayMessage(buf,False);
		return;
	}

	tm = time((time_t *) NULL);
	gethostname(host_name, MSG_SIZ);

	fprintf(fp, "# %s position file -- %s", programName, ctime(&tm)); 
	PrintOpponents(fp); 
	PrintPosition(fp, currentMove);
	fclose(fp);   

	return True;
}          



void TwoMachinesProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	int i;
	MatchMode matchKind;

	int fromRemotePlayer = *nprms;                                                                    

	if ( updateRemotePlayer ) {
		DisplayMessage("no machine moves in challenge mode",fromRemotePlayer);
		return;
        };
 		
    if (gameMode == PauseGame) PauseProc(w, event, prms, nprms);
    if (gameMode == PlayFromGameFile) ForceProc(w, event, prms, nprms);
 	if ((gameMode == EndOfGame) || (gameMode == TwoMachinesPlay)
	    || localPlayer.appData.noChessProgram)
		return;

	if (matchMode == MatchFalse) {
		switch (gameMode) {
		      case PauseGame:
		      case PlayFromGameFile:
			return;
		      case MachinePlaysBlack:
		      case MachinePlaysWhite:
			ForceProc(w, event, prms, nprms);
			if (gameMode != ForceMoves) return;
			matchKind = MatchOpening;
			break;
		      case ForceMoves:
			matchKind = MatchOpening;
			break;
		      case EditPosition:
			EditPositionDone();
			matchKind = MatchPosition;
			break;
		      case BeginningOfGame:
		      default:
			matchKind = MatchInit;
			break;
		}
	} else {
		matchKind = matchMode;
	}

        forwardMostMove = currentMove;
    
	localPlayer.flipView = False;
	remotePlayer.flipView = True;
	firstMove = False;
	DisplayClocks(ResetTimers);
	DisplayClocks(StartTimers);

	switch (matchKind) {
	      case MatchOpening:
		if (firstProgramXID == 0) {
			if (localPlayer.appData.loadGameFile == NULL) {
				DisplayMessage("Select game file first",False);
				return;
			}
			InitChessProgram(localPlayer.appData.firstHost,
					 localPlayer.appData.firstChessProgram,
					 &firstProgramPID, &toFirstProgFP,
					 &fromFirstProgFP, &firstProgramXID,
   					 &firstSendTime);
			if (!LoadGame(localPlayer.appData.loadGameFile)) {
				ShutdownChessPrograms("Bad game file");
				return;
			}
			DrawPosition(localPlayer.boardWidget, NULL, NULL, NULL);
		}
		InitChessProgram(localPlayer.appData.secondHost,
				 localPlayer.appData.secondChessProgram,
				 &secondProgramPID, &toSecondProgFP,
				 &fromSecondProgFP, &secondProgramXID,
				 &secondSendTime);
		if (startedFromSetupPosition) {
			if (whitePlaysFirst) {
				i = 1;
				SendToProgram("force\n", toSecondProgFP);
				SendBoard(toSecondProgFP, boards[i], catches[i]);
			} else {
				i = 0;
				SendBoard(toSecondProgFP, boards[i], catches[i]);
				SendToProgram("force\n", toSecondProgFP);
			}
		} else {
			i = 0;
			SendToProgram("force\n", toSecondProgFP);
		}
	        for (i = backwardMostMove; i < forwardMostMove; i++)
			SendToProgram(moveList[i], toSecondProgFP);
		lastGameMode = gameMode = TwoMachinesPlay;
		ModeHighlight();
		firstMove = True;
		if (BlackOnMove(forwardMostMove))
		  SendToProgram(localPlayer.appData.blackString, toSecondProgFP);
		else
		  SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);
		break;

	      case MatchPosition:
		if (firstProgramXID == 0) {
			if (localPlayer.appData.loadPositionFile == NULL) {
				DisplayMessage("Select position file first",False);
				return;
			}
			InitChessProgram(localPlayer.appData.firstHost,
					 localPlayer.appData.firstChessProgram,
					 &firstProgramPID, &toFirstProgFP,
					 &fromFirstProgFP, &firstProgramXID,
   					 &firstSendTime);
			if (!LoadPosition(localPlayer.appData.loadPositionFile))
				return;
		}
		InitChessProgram(localPlayer.appData.secondHost,
				 localPlayer.appData.secondChessProgram,
				 &secondProgramPID, &toSecondProgFP,
				 &fromSecondProgFP, &secondProgramXID,
   				 &secondSendTime);
		if (whitePlaysFirst)
		  SendToProgram("force\n", toSecondProgFP);
		SendCurrentBoard(toSecondProgFP);
		lastGameMode = gameMode = TwoMachinesPlay;
		ModeHighlight();
		firstMove = True;
		if (BlackOnMove(forwardMostMove))
		  SendToProgram(localPlayer.appData.blackString, toSecondProgFP);
		else
		  SendToProgram(localPlayer.appData.whiteString, toFirstProgFP);
		break;

	      case MatchInit:
		InitPosition(True);
		if (firstProgramXID == 0)
			InitChessProgram(localPlayer.appData.firstHost,
					 localPlayer.appData.firstChessProgram,
					 &firstProgramPID, &toFirstProgFP,
					 &fromFirstProgFP, &firstProgramXID,
   					 &firstSendTime);
		InitChessProgram(localPlayer.appData.secondHost,
				 localPlayer.appData.secondChessProgram,
				 &secondProgramPID, &toSecondProgFP,
				 &fromSecondProgFP, &secondProgramXID,
				 &secondSendTime);
		lastGameMode = gameMode = TwoMachinesPlay;
		ModeHighlight();
		SendToProgram(localPlayer.appData.blackString, toSecondProgFP);

	      default:
		break;
	}      

    if (!firstSendTime || !secondSendTime) {
	DisplayClocks(ResetTimers);
	timeRemaining[0][forwardMostMove] = blackTimeRemaining;
	timeRemaining[1][forwardMostMove] = whiteTimeRemaining;
    }
    DisplayClocks(StartTimers);
}

void PauseProc(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	static GameMode previous_mode = PauseGame;

	switch (gameMode) {
	      case ForceMoves:
#ifdef ISS
		if (localPlayer.appData.issActive) {
		    pausePreviousMode = gameMode;
		    gameMode = PauseGame;
		    ModeHighlight();
		}
		return;
#endif
	      case EndOfGame:
	      case EditPosition:
	      default:
		return;
	      case PauseGame:
		gameMode = previous_mode;
		ModeHighlight();
		previous_mode = PauseGame;
		DisplayClocks(StartTimers);
		DisplayMessage("",False);
		if ( updateRemotePlayer )
		  DisplayMessage("",True);
		break;
	      case PlayFromGameFile:
		if (readGameXID == 0) {
			readGameXID =
			  XtAppAddTimeOut(appContext,
			    (int) (1000 * localPlayer.appData.timeDelay),
			    (XtTimerCallbackProc) ReadGameFile, NULL);
		} else {
			XtRemoveTimeOut(readGameXID);
			readGameXID = 0;
		}
		DisplayMessage("Pausing",False);
		if ( updateRemotePlayer )
		  DisplayMessage("Pausing",True);
		break;
	      case BeginningOfGame:
	      case MachinePlaysBlack:
	      case MachinePlaysWhite:
	      case TwoMachinesPlay:
		if (forwardMostMove == 0)	/* don't pause if no one has moved */
			return;
		if ((gameMode == MachinePlaysWhite &&
		     !BlackOnMove(forwardMostMove)) ||
		    (gameMode == MachinePlaysBlack &&
		     BlackOnMove(forwardMostMove))) {
		    DisplayClocks(StopTimers);
		}
		previous_mode = gameMode;
		gameMode = PauseGame;
		ModeHighlight();
		DisplayClocks(StopTimers);
		DisplayMessage("Pausing",False);
		if ( updateRemotePlayer )
		  DisplayMessage("Pausing",True);
		break;
	}
}

void Iconify(w, event, prms, nprms)
     Widget w;
     XEvent *event;
     String *prms;
     Cardinal *nprms;
{
	Arg args[1];


	fromX = fromY = -1;

	XtSetArg(args[0], XtNiconic, True);
	XtSetValues(localPlayer.shellWidget, args, 1);
}

void
SendToProgram(message, fp)
	char *message;
	FILE *fp;
{
	if (fp == NULL) return;
	lastMsgFP = fp;

	if (xshogiDebug)
		fprintf(stderr, "Sending to %s: %s\n",
			fp == toFirstProgFP ? "first" : "second", message);

	if (message[strlen(message) - 1] != '\n')
		fprintf(fp, "\n%s\n", message);
	else
		fputs(message, fp);
	fflush(fp);
}

void
ReceiveFromProgram(fp, source, id)
	FILE *fp;
        int *source;
        XtInputId *id;
{
	char message[MSG_SIZ], *end_str, *number, *name;
        extern char *sys_errlist[];

	if (fgets(message, MSG_SIZ, fp) == NULL) {
		if (fp == fromFirstProgFP) {
		    number = "first";
		    name = localPlayer.appData.firstChessProgram;
		} else if (fp == fromSecondProgFP) {
		    number = "second";
		    name = localPlayer.appData.secondChessProgram;
		} else {
		    return;
		}
		if (ferror(fp) == 0) {
		    sprintf(message, "%s shogi program (%s) exited unexpectedly",
			    number, name);
		    fprintf(stderr, "%s: %s\n", programName, message);
		} else {
		    sprintf(message,
			    "error reading from %s shogi program (%s): %s",
			    number, name, sys_errlist[ferror(fp)]);
		    fprintf(stderr, "%s: %s\n", programName, message);
		}
		return;
	}

	if ((end_str = (char *) strchr(message, '\r')) != NULL)
		*end_str = NULLCHAR;
	if ((end_str = (char *) strchr(message, '\n')) != NULL)
		*end_str = NULLCHAR;

	if (xshogiDebug || localPlayer.appData.debugMode)
		fprintf(stderr, "Received from %s: %s\n",
			fp == fromFirstProgFP ? "first" : "second", message);
	HandleMachineMove(message, fp);
}

void
SendSearchDepth(fp)
	FILE *fp;
{
	char message[MSG_SIZ];

	if (localPlayer.appData.searchDepth <= 0) return;

	sprintf(message, "depth\n%d\nhelp\n", localPlayer.appData.searchDepth);
	  /* note kludge: "help" command forces gnuchessx to print
	     out something that ends with a newline. */
	SendToProgram(message, fp);
}

void
DisplayMessage(message, toRemotePlayer)
	char *message;
	int toRemotePlayer;
{
	Arg arg;

	XtSetArg(arg, XtNlabel, message);
	if ( !toRemotePlayer )
	  XtSetValues(localPlayer.messageWidget, &arg, 1);
	if ( updateRemotePlayer && toRemotePlayer)
	  XtSetValues(remotePlayer.messageWidget, &arg, 1);
}

void
DisplayName(name)
	char *name;
{
	Arg arg;

	XtSetArg(arg, XtNlabel, name);
	XtSetValues(localPlayer.titleWidget, &arg, 1);
	if ( updateRemotePlayer )
  	  XtSetValues(remotePlayer.titleWidget, &arg, 1);
}

void SendTimeRemaining(fp)
     FILE *fp;
{
    char message[MSG_SIZ];
    long comtime, opptime;

    if (BlackOnMove(forwardMostMove) == (fp == toFirstProgFP)) { 
      comtime = blackTimeRemaining;
      opptime = whiteTimeRemaining;
    } else {
      comtime = whiteTimeRemaining;
      opptime = blackTimeRemaining;
    }

    if (comtime <= 0) comtime = 1000;
    if (opptime <= 0) opptime = 1000;
    
    sprintf(message, "time %d\n", comtime/10);
    SendToProgram(message, fp);
    sprintf(message, "otime %d\n", opptime/10);
    SendToProgram(message, fp);
}


void DisplayMove(moveNumber)
     int moveNumber;
{
    char message[MSG_SIZ];
    
    if (moveNumber < 0) {
	if (moveNumber == forwardMostMove - 1)
	  DisplayMessage(endMessage,False);
	else
	  DisplayMessage("",False);
    } else {
	sprintf(message, "%d. %s%s  %s", moveNumber / 2 + 1,
		BlackOnMove(moveNumber) ? "" : "... ",
		parseList[moveNumber],
		moveNumber == forwardMostMove - 1 ? endMessage : "");
	DisplayMessage(message,False);
    }
}

void DisplayTitle(title)
     char *title;
{
    Arg arg;
    
    XtSetArg(arg, XtNlabel, title);
    XtSetValues(localPlayer.titleWidget, &arg, 1);
}

/*
 * This routine sends a SIGINT (^C interrupt) to gnuchess to awaken it
 * if it might be busy thinking on our time.  This normally isn't needed,
 * but is useful on systems where the FIONREAD ioctl doesn't work (such 
 * as ESIX), since on those systems the gnuchess feature that lets you 
 * interrupt its thinking just by typing a command does not work.
 *
 * In the future, similar code could be used to stop gnuchess and make
 * it move immediately when it is thinking about its own move; this could
 * be useful if we want to make Backward or ForceMoves work while gnuchess
 * is thinking. --t.mann
 */
void
Attention(pid)
     int pid;
{
#if defined(ATTENTION) || defined(ESIX) || !defined(FIONREAD)
	if (localPlayer.appData.noChessProgram || (pid == 0)) return;
	switch (gameMode) {
	      case MachinePlaysBlack:
	      case MachinePlaysWhite:
	      case TwoMachinesPlay:
		if (forwardMostMove > backwardMostMove + 1 && maybeThinking) {
			if (xshogiDebug || localPlayer.appData.debugMode)
			  fprintf(stderr, "Sending SIGINT to %s\n",
			    pid == firstProgramPID ? "first" : "second");

			(void) kill(pid, SIGINT); /* stop it thinking */
		}
		break;
	}
#endif /*ATTENTION*/
}

void
CheckFlags()
{
	if (blackTimeRemaining <= 0) { 
#ifdef notdef
		blackTimeRemaining = 0;
#endif
		if (!blackFlag) {
		        blackFlag = True;
#ifdef ISS
		        if (localPlayer.appData.issActive) {
			  if (localPlayer.appData.autoCallFlag &&
			    iss_mode == IssPlayingWhite && !whiteFlag)
			    SendToISS("flag\n");
	    		} else
#endif
			{
			  if (whiteFlag)
			    DisplayName("  Both flags have fallen");
			  else
			    DisplayName("  Black's flag has fallen");
			}
		}
	}
	if (whiteTimeRemaining <= 0) {
#ifdef notdef
		whiteTimeRemaining = 0;
#endif
		if (!whiteFlag) {
			whiteFlag = True;
#ifdef ISS
	   		if (localPlayer.appData.issActive) {
			   if (localPlayer.appData.autoCallFlag &&
			      iss_mode == IssPlayingBlack && !blackFlag)
			    SendToISS("flag\n");
		        } else
#endif
			{
			  if (blackFlag)
			    DisplayName("  Both flags have fallen");
			  else
			    DisplayName("  White's flag has fallen");
			}
		}
	}
}

void
CheckTimeControl()
{
	if (!localPlayer.appData.clockMode) return;
#ifdef ISS
        if (localPlayer.appData.issActive) return;
#endif
	if (forwardMostMove == 0) return;
	/*
	 * add time to clocks when time control is achieved
	 */
	if ((forwardMostMove % (localPlayer.appData.movesPerSession * 2)) == 0) {

#ifdef notdef
	    if (blackTimeRemaining > 0)
#endif
		blackTimeRemaining += timeControl;
      
#ifdef notdef
	    if (whiteTimeRemaining > 0)
#endif
		whiteTimeRemaining += timeControl;
	}
}

void
DisplayLabels()
{
	DisplayTimerLabel(localPlayer.blackTimerWidget, "Black",
			  blackTimeRemaining);
	DisplayTimerLabel(localPlayer.whiteTimerWidget, "White",
			  whiteTimeRemaining);
	if ( updateRemotePlayer ) {
	  DisplayTimerLabel(remotePlayer.blackTimerWidget, "Black",
			  blackTimeRemaining);
	  DisplayTimerLabel(remotePlayer.whiteTimerWidget, "White",
			  whiteTimeRemaining);
	}
}

#ifdef HAS_GETTIMEOFDAY
static struct timeval tickStartTV;
static int tickLength;

int
PartialTickLength()
{
	struct timeval tv;
	struct timezone tz;
	int ptl;

	gettimeofday(&tv, &tz);
	ptl = ( (tv.tv_sec - tickStartTV.tv_sec)*1000000 +
	        (tv.tv_usec - tickStartTV.tv_usec) + 500 ) / 1000;
	if (ptl > tickLength) ptl = tickLength;
	return ptl;
}
#else /*!HAS_GETTIMEOFDAY*/
#define tickLength 1000
#endif /*HAS_GETTIMEOFDAY*/

/*
 * DisplayClocks manages the game clocks.
 *
 * In tournament play, white starts the clock and then black makes a move.
 * We give the human user a slight advantage if he is playing black---the
 * clocks don't run until he makes his first move, so it takes zero time.
 * Also, DisplayClocks doesn't account for network lag so it could get
 * out of sync with GNU Shogi's clock -- but then, referees are always right.
 */
void
DisplayClocks(clock_mode)
	int clock_mode;
{
#ifdef HAS_GETTIMEOFDAY
	struct timezone tz;
#endif /*HAS_GETTIMEOFDAY*/
	long timeRemaining;

	switch (clock_mode) {
	      case ResetTimers:
		/* Stop clocks and reset to a fresh time control */
		if (timerXID != 0) {
			XtRemoveTimeOut(timerXID);
			timerXID = 0;
		}
		blackTimeRemaining = timeControl;
		whiteTimeRemaining = timeControl;
		if (blackFlag || whiteFlag) {
			DisplayName("");
			blackFlag = whiteFlag = False;
		}
		DisplayLabels();
		break;

	      case DecrementTimers:
		/* Decrement running clock to next 1-second boundary */ 
		if (gameMode == PauseGame) return;
		timerXID = 0;
		if (!localPlayer.appData.clockMode) return;

		if (BlackOnMove(forwardMostMove)) { 
		  timeRemaining = blackTimeRemaining -= tickLength;
		} else { 
		  timeRemaining = whiteTimeRemaining -= tickLength;
	        }

		DisplayLabels();
		CheckFlags(); 
#ifdef notdef
		if (timeRemaining == 0) return;
#endif

#ifdef HAS_GETTIMEOFDAY
		tickLength = (timeRemaining <= 1000 && timeRemaining > 0) ? 100 : 1000;
		gettimeofday(&tickStartTV, &tz);
#endif /*HAS_GETTIMEOFDAY*/
		timerXID =
		  XtAppAddTimeOut(appContext, tickLength,
		    (XtTimerCallbackProc) DisplayClocks,
		    (XtPointer) DecrementTimers);
		break;

	      case SwitchTimers:
		/* A player has just moved, so stop the previously running
		   clock and start the other one. */
		if (timerXID != 0) {
			XtRemoveTimeOut(timerXID);
			timerXID = 0;
#ifdef HAS_GETTIMEOFDAY
			if (localPlayer.appData.clockMode) {
				if (BlackOnMove(forwardMostMove))
				  whiteTimeRemaining -= PartialTickLength();
				else
				  blackTimeRemaining -= PartialTickLength();
				CheckFlags();
			}
#endif /*HAS_GETTIMEOFDAY*/
		}

		CheckTimeControl();
		DisplayLabels();
		if (!localPlayer.appData.clockMode) return;

		if (gameMode == PauseGame &&
	  	  (pausePreviousMode == MachinePlaysBlack ||
	   	  pausePreviousMode == MachinePlaysWhite)) return;
	
		timeRemaining = BlackOnMove(forwardMostMove) ?
		  blackTimeRemaining : whiteTimeRemaining; 
                
#ifdef notdef
		if (timeRemaining == 0) return;
#endif

#ifdef HAS_GETTIMEOFDAY
		tickLength = (timeRemaining <= 1000 && timeRemaining > 0) ?
		  ((timeRemaining-1) % 100) + 1 :
		  ((timeRemaining-1) % 1000) + 1;
		if (tickLength <= 0) tickLength += 1000;
		gettimeofday(&tickStartTV, &tz);
#endif /*HAS_GETTIMEOFDAY*/
		timerXID =
		  XtAppAddTimeOut(appContext, tickLength,
		    (XtTimerCallbackProc) DisplayClocks,
		    (XtPointer) DecrementTimers);
		break;

	      case ReDisplayTimers:
		/* Display current clock values */
		DisplayLabels();
		break;

	      case StopTimers:
		/* Stop both clocks */
		if (timerXID == 0)
			return;
		XtRemoveTimeOut(timerXID);
		timerXID = 0;
		if (!localPlayer.appData.clockMode) return;
#ifdef HAS_GETTIMEOFDAY
		if (BlackOnMove(forwardMostMove))
		  blackTimeRemaining -= PartialTickLength();
		else
		  whiteTimeRemaining -= PartialTickLength();
		CheckFlags();
		DisplayLabels();
#endif /*HAS_GETTIMEOFDAY*/
		break;

	      case StartTimers:
		/* Start clock of player on move, if not already running. */
		if (timerXID != 0)
			return;

		DisplayLabels();
		if (!localPlayer.appData.clockMode) return;

		timeRemaining = BlackOnMove(forwardMostMove) ?
		  blackTimeRemaining : whiteTimeRemaining;
		if (timeRemaining == 0) return;
#ifdef HAS_GETTIMEOFDAY
		tickLength = (timeRemaining <= 1000 && timeRemaining > 0) ?
		  ((timeRemaining-1) % 100) + 1 :
		  ((timeRemaining-1) % 1000) + 1;
		if (tickLength <= 0) tickLength += 1000;
		gettimeofday(&tickStartTV, &tz);
#endif /*HAS_GETTIMEOFDAY*/
		timerXID =
		  XtAppAddTimeOut(appContext, tickLength,
		    (XtTimerCallbackProc) DisplayClocks,
		    (XtPointer)DecrementTimers);
		break;
	}
}

void
DisplayTimerLabel(w, color, timer)
	Widget w;
	char *color;
	long timer;
{
	char buf[MSG_SIZ];
	Arg args[3];
	struct DisplayData *player;

	player = (w == localPlayer.blackTimerWidget || w == localPlayer.whiteTimerWidget)
			? &localPlayer : &remotePlayer;

	if (localPlayer.appData.clockMode) {
		sprintf(buf, "%s: %s", color, TimeString(timer));
		XtSetArg(args[0], XtNlabel, buf);
	} else
		XtSetArg(args[0], XtNlabel, color);

	if (((color[0] == 'W') && BlackOnMove(forwardMostMove))
		|| ((color[0] == 'B') && !BlackOnMove(forwardMostMove))) {
		XtSetArg(args[1], XtNbackground, player->timerForegroundPixel);
		XtSetArg(args[2], XtNforeground, player->timerBackgroundPixel);
	} else {
		XtSetArg(args[1], XtNbackground, player->timerBackgroundPixel);
		XtSetArg(args[2], XtNforeground, player->timerForegroundPixel);
	}

	XtSetValues(w, args, 3);
}

char *TimeString(tm)
     long tm;
{
    int second, minute, hour, day;
    char *sign = "";
    static char buf[32];
    
    if (tm > 0 && tm <= 900) {
	/* convert milliseconds to tenths, rounding up */
	sprintf(buf, " 0.%1d ", (tm+99)/100);
	return buf;
    }

    /* convert milliseconds to seconds, rounding up */
    tm = (tm + 999) / 1000; 

    if (tm < 0) {
	sign = "-";
	tm = -tm;
    }
    
    if (tm >= (60 * 60 * 24)) {
	day = (int) (tm / (60 * 60 * 24));
	tm -= day * 60 * 60 * 24;
    } else {
	day = 0;
    }
    
    if (tm >= (60 * 60)) {
	hour = (int) (tm / (60 * 60));
	tm -= hour * 60 * 60;
    } else {
	hour = 0;
    }
    
    if (tm >= 60) {
	minute = (int) (tm / 60);
	tm -= minute * 60;
    } else {
	minute = 0;
    }
    
    second = tm % 60;
    
    if (day > 0)
      sprintf(buf, " %s%d:%02d:%02d:%02d ", sign, day, hour, minute, second);
    else if (hour > 0)
      sprintf(buf, " %s%d:%02d:%02d ", sign, hour, minute, second);
    else
      sprintf(buf, " %s%2d:%02d ", sign, minute, second);
    
    return buf;
}

void
Usage()
{
	fprintf(stderr, "Usage: %s\n", programName);
	fprintf(stderr, "\tstandard Xt options\n");
	fprintf(stderr, "\t-iconic\n");
	fprintf(stderr, "\t-tc or -timeControl minutes[:seconds]\n");
	fprintf(stderr, "\t-gi or -gameIn (True | False)\n");
	fprintf(stderr, "\t-mps or -movesPerSession moves\n");
	fprintf(stderr, "\t-st or -searchTime minutes[:seconds]\n");
	fprintf(stderr, "\t-sd or -searchDepth number\n");
	fprintf(stderr, "\t-clock or -clockMode (True | False)\n");
	fprintf(stderr, "\t-td or -timeDelay seconds\n");
#ifdef ISS
    	fprintf(stderr, "\t-iss or -internetShogiServerMode (True | False)\n");
    	fprintf(stderr, "\t-isshost or -internetShogiServerHost host_name\n");
    	fprintf(stderr, "\t-issport or -internetShogiServerPort port_number\n");
#endif
	fprintf(stderr, "\t-ncp or -noChessProgram (True | False)\n");
	fprintf(stderr, "\t-fcp or -firstChessProgram program_name\n");
	fprintf(stderr, "\t-scp or -secondChessProgram program_name\n");
	fprintf(stderr, "\t-fh or -firstHost host_name\n");
	fprintf(stderr, "\t-sh or -secondHost host_name\n");
	fprintf(stderr, "\t-rsh or -remoteShell shell_name\n");
	fprintf(stderr,
		"\t-mm or -matchMode (False | Init | Position | Opening)\n");
	fprintf(stderr, "\t-lgf or -loadGameFile file_name\n");
	fprintf(stderr, "\t-lpf or -loadPositionFile file_name\n");
	fprintf(stderr, "\t-sgf or -saveGameFile file_name\n");
	fprintf(stderr, "\t-spf or -savePositionFile file_name\n");
	fprintf(stderr, "\t-size or -boardSize (Large | Medium | Small)\n");
	fprintf(stderr, "\t-coords or -showCoords (True | False)\n");
	fprintf(stderr, "\t-mono or -monoMode (True | False)\n");
	fprintf(stderr, "\t-bpc or -blackPieceColor color\n");
	fprintf(stderr, "\t-wpc or -whitePieceColor color\n");
	fprintf(stderr, "\t-lsc or -lightSquareColor color\n");
	fprintf(stderr, "\t-dsc or -darkSquareColor color\n");
#ifdef BOTH_BITMAPS
	fprintf(stderr, "\t-wps or -westernPieceSet (True | False)\n");
#endif
	fprintf(stderr, "\t-debug or -debugMode (True | False)\n");
	exit(2);
}

/*
 * This is necessary because some C libraries aren't ANSI C compliant yet.
 */
char *
StrStr(string, match)
	char *string, *match;
{
	int i, length;

	length = strlen(match);

	for (i = strlen(string) - length; i >= 0; i--, string++)
		if (!strncmp(match, string, (size_t) length))
			return string;

	return NULL;
}

int
StrCaseCmp(s1, s2)
     char *s1, *s2;
{
	char c1, c2;

	for (;;) {
		c1 = ToLower(*s1++);
		c2 = ToLower(*s2++);
		if (c1 > c2) return 1;
		if (c1 < c2) return -1;
		if (c1 == NULLCHAR) return 0;
	}
}


int
ToLower(c)
     int c;
{
	return isupper(c) ? tolower(c) : c;
}


int
ToUpper(c)
     int c;
{
	return islower(c) ? toupper(c) : c;
}


#if	SYSTEM_FIVE || SYSV
#ifdef	IRIX /*??*/
char *
PseudoTTY(ptyv)
	int *ptyv;
     {
         char *line;

         line = (char *)_getpty(ptyv, O_RDWR, 0600, 0);
         if (0 == line)
             return NULL;
         return line;
     }
#else	/*!IRIX*/
char *
PseudoTTY(ptyv)
	int *ptyv;
{
#if	SVR4
	char *ptsname(), *ptss;

	*ptyv = open("/dev/ptmx", O_RDWR);
	if (*ptyv > 0 ) {
		if (grantpt(*ptyv) == -1)
			return NULL;
		if (unlockpt(*ptyv) == -1)
			return NULL;
		if (!(ptss = ptsname(*ptyv)))
			return NULL;
		strncpy(ptyname, ptss, sizeof(ptyname));
		ptyname[sizeof(ptyname) -1] = 0;
		return ptyname;
	}
	
#else	/* !SVR4 */
	struct stat stb;
	int c, i;

	for (c = 'p'; c <= 'z'; c++)
#if	defined(HPUX) || defined(hpux)
		for (i = 0; i < 15 /*??*/; i++) {
			sprintf(ptyname, "/dev/ptym/pty%c%x", c, i);
#else /* !HPUX */
		for (i = 0; i < 16; i++) {
#ifdef	RTU
			sprintf(ptyname, "/dev/pty%x", i);
#else /* !RTU */
			sprintf(ptyname, "/dev/pty%c%x", c, i);
#endif /* RTU */
#endif /* HPUX */

#ifdef	IRIS
			*ptyv = open("/dev/ptc", O_RDWR, 0);
			if (*ptyv < 0)
				return NULL;
			if (fstat(*ptyv, &stb) < 0)
				return NULL;
#else /* !IRIS */
			if (stat(ptyname, &stb) < 0)
				return NULL;
			*ptyv = open(ptyname, O_RDWR, 0);
#endif /* IRIS */

			if (*ptyv >= 0) {
#if	defined(HPUX) || defined(hpux)
				sprintf(ptyname, "/dev/pty/tty%c%x", c, i);
#else /* !HPUX */
#ifdef	RTU
				sprintf(ptyname, "/dev/ttyp%x", i);
#else /* !RTU */
#ifdef	IRIS
				sprintf(ptyname, "/dev/ttyq%d",
					minor(stb.st_rdev));
#else /* !IRIS, !RTU, !HPUX */
				sprintf(ptyname, "/dev/tty%c%x", c, i);
#endif /* IRIS */
#endif /* RTU */
#endif /* HPUX */

#ifndef	UNIPLUS
				if (access(ptyname, 6) != 0) {
					close(*ptyv);
#ifdef	IRIS
					return NULL;
#else /* !IRIS */
					continue;
#endif /* IRIS */
				}
#endif /* !UNIPLUS */

#ifdef	IBMRTAIX
			signal(SIGHUP, SIG_IGN);
#endif /* IBMRTAIX */
			return ptyname;
		}
	}

#endif
	return NULL;
}
#endif /*IRIX*/

#else /* !SYSTEM_FIVE */

void
CatchPipeSignal(dummy)
     int dummy;
{
	char message[MSG_SIZ];

	sprintf(message,
		"%s chess program (%s) exited unexpectedly",
		lastMsgFP == toFirstProgFP ? "first" : "second",
		lastMsgFP == toFirstProgFP ? localPlayer.appData.firstChessProgram
		    : localPlayer.appData.secondChessProgram);
	fprintf(stderr, "%s: %s\n", programName, message);
	ShutdownChessPrograms(message);
	return;
}
#endif
