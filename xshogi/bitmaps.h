/*
 * FILE: bitmaps.h
 *
 *     Piece bitmaps for xshogi.
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
 * XShogi borrows its piece bitmaps from CRANES Shogi.
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

#ifndef _BITMAPS_H_
#define _BITMAPS_H_

/* 
 * Naming conventions:
 *
 * R  -- reverse orientation
 * P  -- promoted piece
 * W  -- westernized bitmap
 * _m -- medium-sized piece (49x49)
 * _l -- large-sized piece  (64x64)
 * (default is small-sized piece) (32x32)
 *
 */

#include "eastern_bitmaps/icon.xbm"

#include "eastern_bitmaps/bigsolid.xbm"
#include "eastern_bitmaps/smallsolid.xbm"
#include "eastern_bitmaps/bigsolidR.xbm"
#include "eastern_bitmaps/smallsolidR.xbm"

/* 
 * Westernized piece bitmaps.
 * Thanks to Paul Raines for making the "westernized" bitmaps available.
 */

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


/* Eastern piece bitmaps. */

#include "eastern_bitmaps/king.xbm"
#include "eastern_bitmaps/rook.xbm"
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

#include "eastern_bitmaps/bigsolid_m.xbm"
#include "eastern_bitmaps/smallsolid_m.xbm"
#include "eastern_bitmaps/bigsolidR_m.xbm"
#include "eastern_bitmaps/smallsolidR_m.xbm"

#include "eastern_bitmaps/king_m.xbm"
#include "eastern_bitmaps/rook_m.xbm"
#include "eastern_bitmaps/bishop_m.xbm"
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
#include "eastern_bitmaps/bishopR_m.xbm"
#include "eastern_bitmaps/goldR_m.xbm"
#include "eastern_bitmaps/silverR_m.xbm"
#include "eastern_bitmaps/silverPR_m.xbm"
#include "eastern_bitmaps/knightR_m.xbm"
#include "eastern_bitmaps/knightPR_m.xbm"
#include "eastern_bitmaps/lanceR_m.xbm"
#include "eastern_bitmaps/lancePR_m.xbm"
#include "eastern_bitmaps/pawnR_m.xbm"
#include "eastern_bitmaps/pawnPR_m.xbm"

#include "eastern_bitmaps/bigsolid_l.xbm"
#include "eastern_bitmaps/smallsolid_l.xbm"
#include "eastern_bitmaps/bigsolidR_l.xbm"
#include "eastern_bitmaps/smallsolidR_l.xbm"

#include "eastern_bitmaps/king_l.xbm"
#include "eastern_bitmaps/rook_l.xbm"
#include "eastern_bitmaps/bishop_l.xbm"
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
#include "eastern_bitmaps/bishopR_l.xbm"
#include "eastern_bitmaps/goldR_l.xbm"
#include "eastern_bitmaps/silverR_l.xbm"
#include "eastern_bitmaps/silverPR_l.xbm"
#include "eastern_bitmaps/knightR_l.xbm"
#include "eastern_bitmaps/knightPR_l.xbm"
#include "eastern_bitmaps/lanceR_l.xbm"
#include "eastern_bitmaps/lancePR_l.xbm"
#include "eastern_bitmaps/pawnR_l.xbm"
#include "eastern_bitmaps/pawnPR_l.xbm"

/* 
 * Alternative top bitmaps for eastern pieces (promoted rook and bishop
 * only).  Thanks to Ken'ichi Nakayama for providing these bitmaps.  
 */

#if defined(TOP_PART_BITMAPS)

#include "eastern_bitmaps/rookP.top.xbm"
#include "eastern_bitmaps/bishopP.top.xbm"

#include "eastern_bitmaps/rookPR.top.xbm"
#include "eastern_bitmaps/bishopPR.top.xbm"

#include "eastern_bitmaps/rookP_m.top.xbm"
#include "eastern_bitmaps/bishopP_m.top.xbm"

#include "eastern_bitmaps/rookPR_m.top.xbm"
#include "eastern_bitmaps/bishopPR_m.top.xbm"

#include "eastern_bitmaps/rookP_l.top.xbm"
#include "eastern_bitmaps/bishopP_l.top.xbm"

#include "eastern_bitmaps/rookPR_l.top.xbm"
#include "eastern_bitmaps/bishopPR_l.top.xbm"

#else   /* !TOP_PART_BITMAPS */

#include "eastern_bitmaps/rookP.xbm"
#include "eastern_bitmaps/bishopP.xbm"

#include "eastern_bitmaps/rookPR.xbm"
#include "eastern_bitmaps/bishopPR.xbm"

#include "eastern_bitmaps/rookP_m.xbm"
#include "eastern_bitmaps/bishopP_m.xbm"

#include "eastern_bitmaps/rookPR_m.xbm"
#include "eastern_bitmaps/bishopPR_m.xbm"

#include "eastern_bitmaps/rookP_l.xbm"
#include "eastern_bitmaps/bishopP_l.xbm"

#include "eastern_bitmaps/rookPR_l.xbm"
#include "eastern_bitmaps/bishopPR_l.xbm"

#endif  /* TOP_PART_BITMAPS */

#endif /* _BITMAPS_H_ */

