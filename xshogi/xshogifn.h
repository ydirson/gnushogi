/*
 * FILE: xshogifn.h
 *
 *     Function declarations for xshogi.
 *
 * ------------------------------------------------------------------------
 * xshogi is based on XBoard -- an Xt/Athena user interface for GNU Chess.
 *
 * Original authors:                                Dan Sears, Chris Sears
 * Enhancements (Version 2.0 and following):        Tim Mann
 * Modifications to XShogi (Version 1.0):           Matthias Mutz
 * Enhancements to XShogi (Version 1.1):            Matthias Mutz
 * Modified implementation of ISS mode for XShogi:  Matthias Mutz
 * Current maintainer:                              Michael C. Vanier
 *
 * XShogi borrows some of its piece bitmaps from CRANES Shogi.
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.
 * Enhancements Copyright 1992 Free Software Foundation, Inc.
 * Enhancements for XShogi Copyright 1993, 1994, 1995 Matthias Mutz
 * Copyright (c) 1999 Michael Vanier and the Free Software Foundation
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
 * This file is part of GNU shogi.
 *
 * GNU shogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * GNU shogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU shogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ------------------------------------------------------------------------
 *
 */

#ifndef _XSHOGIFN_H_
#define _XSHOGIFN_H_


int  main(int argc, char **argv);
void CreateGCs(void);
void CreatePieces(void);
void CreatePieceMenus(void);
char *FindFont(char *pattern, int targetPxlSize);
void PieceMenuPopup(Widget w, XEvent *event, String *params,
                    Cardinal *num_params);
static void PieceMenuSelect(Widget w, ShogiSquare piece, char *junk);
static void SetBlackToPlay(void);
static void SetWhiteToPlay(void);
void ReadBitmap(String name, Pixmap *pm, Pixmap *qm,
                unsigned char small_bits[], unsigned char medium_bits[],
                unsigned char large_bits[]);
void CreateGrid(void);
int  EventToSquare(int x);
int  EventToXSquare(int x);
ShogiSquare CharToPiece(int c, int p);
void DrawSquare(int row, int column, ShogiSquare piece);
void BlinkSquare(int row, int column, ShogiSquare piece);
void EventProc(Widget widget, XtPointer client_data, XEvent *event);
void DrawPosition(Widget w, XEvent *event, String *prms, Cardinal *nprms);
void InitPosition(int redraw);
void CopyBoard(Board to, Board from);
void CopyCatches(Catched to, Catched from);
void ClearCatches(Catched to);
void UpdateCatched(int c, int f, int d, int a, int cm);
void SendCurrentBoard(FILE *fp);
void SendBoard(FILE *fp, Board board, Catched catches);
void HandleUserMove(Widget w, XEvent *event);
void FinishUserMove(ShogiMove move_type, int to_x, int to_y);
void HandleMachineMove(char *message, FILE *fp);
void ReadGameFile(void);
int  ReadGameFileProc(void);
void ApplyMove(ShogiMove *move_type, int from_x, int from_y,
               int to_x, int to_y, int currentMove);
void MakeMove(ShogiMove *move_type, int from_x, int from_y,
              int to_x, int to_y);
void InitShogiProgram(char *host_name, char *program_name, int *pid,
                      FILE **to, FILE **from, XtIntervalId *xid,
                      int *sendTime);
void ShutdownShogiPrograms(char *message);
void CommentPopUp(char *label);
void FileNamePopUp(char *label, Boolean (*proc)(char *name));
void FileNameCallback(Widget w, XtPointer client_data,
                      XtPointer call_data);
void FileNameAction(Widget w, XEvent *event, String *prms, Cardinal *nprms);
void PromotionPopUp(ShogiSquare piece, int to_x, int to_y, int frp);
void PromotionCallback(Widget w, XtPointer client_data,
                       XtPointer call_data);
void FileModePopUp(char *name);
void FileModeCallback(Widget w, XtPointer client_data,
                      XtPointer call_data);
void SelectCommand(Widget w, XtPointer client_data, XtPointer call_data);
void ModeHighlight(void);
void QuitProc(Widget w, XEvent *event, String *prms, Cardinal *nprms);
void QuitRemotePlayerProc(void);

Boolean LoadGame(char *name);
void LoadGameProc(Widget w, XEvent *event, String *prms, Cardinal *nprms);
Boolean LoadPosition(char *name);
void LoadPositionProc(Widget w, XEvent *event,
                      String *prms, Cardinal *nprms);
void MachineBlackProc(Widget w, XEvent *event,
                      String *prms, Cardinal *nprms);
void MachineWhiteProc(Widget w, XEvent *event,
                      String *prms, Cardinal *nprms);
void ForwardProc(Widget w, XEvent *event,
                 String *prms, Cardinal *nprms);
void MoveNowProc(Widget w, XEvent *event,
                 String *prms, Cardinal *nprms);
void ResetFileProc(void);
void ResetChallenge(void);
void ResetProc(Widget w, XEvent *event,
               String *prms, Cardinal *nprms);
void Reset(int /* Boolean */ redraw);
Boolean Challenge(char *name);
void ChallengeProc(Widget w, XEvent *event,
                   String *prms, Cardinal *nprms);
Boolean SelectLevel(char *command);
void SelectLevelProc(Widget w, XEvent *event,
                     String *prms, Cardinal *nprms);
void BackwardProc(Widget w, XEvent *event,
                  String *prms, Cardinal *nprms);
void FlipViewProc(Widget w, XEvent *event,
                  String *prms, Cardinal *nprms);
void ForceProc(Widget w, XEvent *event,
               String *prms, Cardinal *nprms);
void SaveGameProc(Widget w, XEvent *event,
                  String *prms, Cardinal *nprms);
Boolean SaveGame(char *name);
void SavePositionProc(Widget w, XEvent *event,
                      String *prms, Cardinal *nprms);
Boolean SavePosition(char *name);
void SwitchProc(Widget w, XEvent *event,
                String *prms, Cardinal *nprms);
void HintProc(Widget w, XEvent *event,
              String *prms, Cardinal *nprms);
void EditPositionProc(Widget w, XEvent *event,
                      String *prms, Cardinal *nprms);
void EditPositionDone(void);

void TwoMachinesProc(Widget w, XEvent *event, String *prms,
                     Cardinal *nprms);
void PauseProc(Widget w, XEvent *event, String *prms, Cardinal *nprms);
void Iconify(Widget w, XEvent *event, String *prms, Cardinal *nprms);
void PrintOpponents(FILE *fp);
void PrintPosition(FILE *fp, int move);
void SendToProgram(char *message, FILE *fp);
void ReceiveFromProgram(FILE *fp, int *source, XtInputId *id);
void SendSearchDepth(FILE *fp);
void SendTimeRemaining(FILE *fp);
void DisplayMessage(char *message, int toRemotePlayer);
void DisplayName(char *name);
void DisplayMove(int moveNumber);
void DisplayTitle(char *title);
void Attention(int pid);
void DisplayClocks(int clock_mode);
void DisplayTimerLabel(Widget w, char *color, long timer);
char *TimeString(long tm);
void Usage(void);
extern void parseGameFile(void);
Boolean ParseBoard(char *string);
void ParseMachineMove(char *machine_move, ShogiMove *move_type,
                      int *from_x, int *from_y, int *to_x, int *to_y);
void CatchPipeSignal(int dummy);

#endif /* _XSHOGIFN_H_ */

