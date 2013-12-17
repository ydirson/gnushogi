#
# Makefile for XShogi
#
# Copyright (c) 2013 Yann Dirson and the Free Software Foundation
#
# This file is part of GNU Shogi.
#
# GNU Shogi is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# GNU Shogi is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Shogi; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#

bin_PROGRAMS = @XSHOGI@
EXTRA_PROGRAMS = xshogi
xdocdir = ${datarootdir}/doc/xshogi

xshogi_SOURCES = \
	xshogi.c \
	parser.y \
	scanner.l

xshogi_CPPFLAGS = \
	@OPT@ \
	-DFIRST_SHOGI_PROGRAM=\"@FIRST_SHOGI_PROGRAM@\" \
	-DSECOND_SHOGI_PROGRAM=\"@SECOND_SHOGI_PROGRAM@\" \
	-DFIRST_HOST=\"@FIRST_HOST@\" \
	-DSECOND_HOST=\"@SECOND_HOST@\"

AM_CFLAGS = @CEXTRAFLAGS@ @WARNINGS@

xshogi_LDADD = @X_LIBS@ -lXaw -lXmu -lXt -lXext -lX11

BUILT_SOURCES = parser.h
AM_YFLAGS = -d

XDOCS = \
	README.xboard \
	BUGS
dist_xdoc_DATA = @XDOCS@
EXTRA_DATA = $(XDOCS)

dist_noinst_DATA = \
	REMOTEHOSTS

noinst_HEADERS = \
	bitmaps.h \
	sysdeps.h \
	xshogi.h \
	xshogifn.h \
	\
	eastern_bitmaps/bigblank.xbm \
	eastern_bitmaps/bigblankR.xbm \
	eastern_bitmaps/biglight.xbm \
	eastern_bitmaps/bigsolid.xbm \
	eastern_bitmaps/bigsolidR.xbm \
	eastern_bitmaps/bigsolidR_l.xbm \
	eastern_bitmaps/bigsolidR_m.xbm \
	eastern_bitmaps/bigsolid_l.xbm \
	eastern_bitmaps/bigsolid_m.xbm \
	eastern_bitmaps/bishop.xbm \
	eastern_bitmaps/bishopP.top.xbm \
	eastern_bitmaps/bishopP.xbm \
	eastern_bitmaps/bishopPR.top.xbm \
	eastern_bitmaps/bishopPR.xbm \
	eastern_bitmaps/bishopPR_l.top.xbm \
	eastern_bitmaps/bishopPR_l.xbm \
	eastern_bitmaps/bishopPR_m.top.xbm \
	eastern_bitmaps/bishopPR_m.xbm \
	eastern_bitmaps/bishopP_l.top.xbm \
	eastern_bitmaps/bishopP_l.xbm \
	eastern_bitmaps/bishopP_m.top.xbm \
	eastern_bitmaps/bishopP_m.xbm \
	eastern_bitmaps/bishopR.xbm \
	eastern_bitmaps/bishopR_l.xbm \
	eastern_bitmaps/bishopR_m.xbm \
	eastern_bitmaps/bishop_l.xbm \
	eastern_bitmaps/bishop_m.xbm \
	eastern_bitmaps/gold.xbm \
	eastern_bitmaps/goldR.xbm \
	eastern_bitmaps/goldR_l.xbm \
	eastern_bitmaps/goldR_m.xbm \
	eastern_bitmaps/gold_l.xbm \
	eastern_bitmaps/gold_m.xbm \
	eastern_bitmaps/icon.xbm \
	eastern_bitmaps/king.xbm \
	eastern_bitmaps/kingR.xbm \
	eastern_bitmaps/kingR_l.xbm \
	eastern_bitmaps/kingR_m.xbm \
	eastern_bitmaps/king_l.xbm \
	eastern_bitmaps/king_m.xbm \
	eastern_bitmaps/knight.xbm \
	eastern_bitmaps/knightP.xbm \
	eastern_bitmaps/knightPR.xbm \
	eastern_bitmaps/knightPR_l.xbm \
	eastern_bitmaps/knightPR_m.xbm \
	eastern_bitmaps/knightP_l.xbm \
	eastern_bitmaps/knightP_m.xbm \
	eastern_bitmaps/knightR.xbm \
	eastern_bitmaps/knightR_l.xbm \
	eastern_bitmaps/knightR_m.xbm \
	eastern_bitmaps/knight_l.xbm \
	eastern_bitmaps/knight_m.xbm \
	eastern_bitmaps/lance.xbm \
	eastern_bitmaps/lanceP.xbm \
	eastern_bitmaps/lancePR.xbm \
	eastern_bitmaps/lancePR_l.xbm \
	eastern_bitmaps/lancePR_m.xbm \
	eastern_bitmaps/lanceP_l.xbm \
	eastern_bitmaps/lanceP_m.xbm \
	eastern_bitmaps/lanceR.xbm \
	eastern_bitmaps/lanceR_l.xbm \
	eastern_bitmaps/lanceR_m.xbm \
	eastern_bitmaps/lance_l.xbm \
	eastern_bitmaps/lance_m.xbm \
	eastern_bitmaps/none.xbm \
	eastern_bitmaps/none_l.xbm \
	eastern_bitmaps/none_m.xbm \
	eastern_bitmaps/pawn.xbm \
	eastern_bitmaps/pawnP.xbm \
	eastern_bitmaps/pawnPR.xbm \
	eastern_bitmaps/pawnPR_l.xbm \
	eastern_bitmaps/pawnPR_m.xbm \
	eastern_bitmaps/pawnP_l.xbm \
	eastern_bitmaps/pawnP_m.xbm \
	eastern_bitmaps/pawnR.xbm \
	eastern_bitmaps/pawnR_l.xbm \
	eastern_bitmaps/pawnR_m.xbm \
	eastern_bitmaps/pawn_l.xbm \
	eastern_bitmaps/pawn_m.xbm \
	eastern_bitmaps/rook.xbm \
	eastern_bitmaps/rookP.top.xbm \
	eastern_bitmaps/rookP.xbm \
	eastern_bitmaps/rookPR.top.xbm \
	eastern_bitmaps/rookPR.xbm \
	eastern_bitmaps/rookPR_l.top.xbm \
	eastern_bitmaps/rookPR_l.xbm \
	eastern_bitmaps/rookPR_m.top.xbm \
	eastern_bitmaps/rookPR_m.xbm \
	eastern_bitmaps/rookP_l.top.xbm \
	eastern_bitmaps/rookP_l.xbm \
	eastern_bitmaps/rookP_m.top.xbm \
	eastern_bitmaps/rookP_m.xbm \
	eastern_bitmaps/rookR.xbm \
	eastern_bitmaps/rookR_l.xbm \
	eastern_bitmaps/rookR_m.xbm \
	eastern_bitmaps/rook_l.xbm \
	eastern_bitmaps/rook_m.xbm \
	eastern_bitmaps/silver.xbm \
	eastern_bitmaps/silverP.xbm \
	eastern_bitmaps/silverPR.xbm \
	eastern_bitmaps/silverPR_l.xbm \
	eastern_bitmaps/silverPR_m.xbm \
	eastern_bitmaps/silverP_l.xbm \
	eastern_bitmaps/silverP_m.xbm \
	eastern_bitmaps/silverR.xbm \
	eastern_bitmaps/silverR_l.xbm \
	eastern_bitmaps/silverR_m.xbm \
	eastern_bitmaps/silver_l.xbm \
	eastern_bitmaps/silver_m.xbm \
	eastern_bitmaps/smallblank.xbm \
	eastern_bitmaps/smallblankR.xbm \
	eastern_bitmaps/smalllight.xbm \
	eastern_bitmaps/smallsolid.xbm \
	eastern_bitmaps/smallsolidR.xbm \
	eastern_bitmaps/smallsolidR_l.xbm \
	eastern_bitmaps/smallsolidR_m.xbm \
	eastern_bitmaps/smallsolid_l.xbm \
	eastern_bitmaps/smallsolid_m.xbm \
	\
	western_bitmaps/bishopPRW.xbm \
	western_bitmaps/bishopPW.xbm \
	western_bitmaps/bishopRW.xbm \
	western_bitmaps/bishopW.xbm \
	western_bitmaps/goldRW.xbm \
	western_bitmaps/goldW.xbm \
	western_bitmaps/kingRW.xbm \
	western_bitmaps/kingW.xbm \
	western_bitmaps/knightPRW.xbm \
	western_bitmaps/knightPW.xbm \
	western_bitmaps/knightRW.xbm \
	western_bitmaps/knightW.xbm \
	western_bitmaps/lancePRW.xbm \
	western_bitmaps/lancePW.xbm \
	western_bitmaps/lanceRW.xbm \
	western_bitmaps/lanceW.xbm \
	western_bitmaps/noneW.xbm \
	western_bitmaps/pawnPRW.xbm \
	western_bitmaps/pawnPW.xbm \
	western_bitmaps/pawnRW.xbm \
	western_bitmaps/pawnW.xbm \
	western_bitmaps/rookPRW.xbm \
	western_bitmaps/rookPW.xbm \
	western_bitmaps/rookRW.xbm \
	western_bitmaps/rookW.xbm \
	western_bitmaps/silverPRW.xbm \
	western_bitmaps/silverPW.xbm \
	western_bitmaps/silverRW.xbm \
	western_bitmaps/silverW.xbm \
	 \
	new_eastern_bitmaps/bigsolidR_l.xbm \
	new_eastern_bitmaps/bigsolid_l.xbm \
	new_eastern_bitmaps/bishopPR_l.xbm \
	new_eastern_bitmaps/bishopP_l.xbm \
	new_eastern_bitmaps/bishopR_l.xbm \
	new_eastern_bitmaps/bishop_l.xbm \
	new_eastern_bitmaps/goldR_l.xbm \
	new_eastern_bitmaps/gold_l.xbm \
	new_eastern_bitmaps/kingR_l.xbm \
	new_eastern_bitmaps/king_l.xbm \
	new_eastern_bitmaps/knightPR_l.xbm \
	new_eastern_bitmaps/knightP_l.xbm \
	new_eastern_bitmaps/knightR_l.xbm \
	new_eastern_bitmaps/knight_l.xbm \
	new_eastern_bitmaps/lancePR_l.xbm \
	new_eastern_bitmaps/lanceP_l.xbm \
	new_eastern_bitmaps/lanceR_l.xbm \
	new_eastern_bitmaps/lance_l.xbm \
	new_eastern_bitmaps/none_l.xbm \
	new_eastern_bitmaps/pawnPR_l.xbm \
	new_eastern_bitmaps/pawnP_l.xbm \
	new_eastern_bitmaps/pawnR_l.xbm \
	new_eastern_bitmaps/pawn_l.xbm \
	new_eastern_bitmaps/rookPR_l.xbm \
	new_eastern_bitmaps/rookP_l.xbm \
	new_eastern_bitmaps/rookR_l.xbm \
	new_eastern_bitmaps/rook_l.xbm \
	new_eastern_bitmaps/silverPR_l.xbm \
	new_eastern_bitmaps/silverP_l.xbm \
	new_eastern_bitmaps/silverR_l.xbm \
	new_eastern_bitmaps/silver_l.xbm \
	new_eastern_bitmaps/smallsolidR_l.xbm \
	new_eastern_bitmaps/smallsolid_l.xbm \
	\
	new_western_bitmaps/bishopPRW.xbm \
	new_western_bitmaps/bishopPW.xbm \
	new_western_bitmaps/bishopRW.xbm \
	new_western_bitmaps/bishopW.xbm \
	new_western_bitmaps/goldRW.xbm \
	new_western_bitmaps/goldW.xbm \
	new_western_bitmaps/kingRW.xbm \
	new_western_bitmaps/kingW.xbm \
	new_western_bitmaps/knightPRW.xbm \
	new_western_bitmaps/knightPW.xbm \
	new_western_bitmaps/knightRW.xbm \
	new_western_bitmaps/knightW.xbm \
	new_western_bitmaps/lancePRW.xbm \
	new_western_bitmaps/lancePW.xbm \
	new_western_bitmaps/lanceRW.xbm \
	new_western_bitmaps/lanceW.xbm \
	new_western_bitmaps/noneW.xbm \
	new_western_bitmaps/pawnPRW.xbm \
	new_western_bitmaps/pawnPW.xbm \
	new_western_bitmaps/pawnRW.xbm \
	new_western_bitmaps/pawnW.xbm \
	new_western_bitmaps/rookPRW.xbm \
	new_western_bitmaps/rookPW.xbm \
	new_western_bitmaps/rookRW.xbm \
	new_western_bitmaps/rookW.xbm \
	new_western_bitmaps/silverPRW.xbm \
	new_western_bitmaps/silverPW.xbm \
	new_western_bitmaps/silverRW.xbm \
	new_western_bitmaps/silverW.xbm
