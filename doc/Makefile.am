#
# Documentation Makefile for GNU Shogi
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

info_TEXINFOS = gnushogi.texi
AM_MAKEINFOHTMLFLAGS = --split=section

dist_man_MANS = gnushogi.6 @XSHOGIMAN@
EXTRA_MANS = xshogi.6

dist_doc_DATA = \
	BOOKFILES \
	PORTING \
	shogi.rules \
	tutorial1.gam \
	tutorial2.gam

spell:
	tr '[A-Z]' '[a-z]' < $(srcdir)/gnushogi.texi | tr -cd '[A-Za-z0-9_ \012]' | \
	tr -s '[ ]' '\012' | sort | uniq -c | sort -nr | spell | sort | uniq
