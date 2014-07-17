#
# Makefile for GNU Shogi
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

pkgbindatadir = $(pkglibdir)

bin_PROGRAMS = gnushogi gnuminishogi
noinst_PROGRAMS = @PAT2INC@ sizetest
EXTRA_PROGRAMS = pat2inc

pkgbindata_DATA = $(top_srcdir)/misc/gnushogi.tbk
# gnuminishogi.bbk would be empty

AM_CFLAGS = @CEXTRAFLAGS@ @WARNINGS@

# The pattern file contains various opening patterns. The program tries to
# obtain such a pattern in the opening stage. Sequences of opening
# patterns may be described in order to support the correct order of moves.

gnushogi_CPPFLAGS = \
	-DHASHFILE=\"$(pkglibdir)/gnushogi.hsh\" \
	-DBOOK=\"$(pkglibdir)/gnushogi.tbk\" \
	-DBINBOOK=\"$(pkglibdir)/gnushogi.bbk\"
gnuminishogi_CPPFLAGS = \
	-DMINISHOGI \
	-DHASHFILE=\"$(pkglibdir)/gnuminishogi.hsh\" \
	-DBOOK=\"$(pkglibdir)/gnuminishogi.tbk\" \
	-DBINBOOK=\"$(pkglibdir)/gnuminishogi.bbk\"

BUILTHEADERS = \
	gnushogi-pattern.inc \
	gnuminishogi-pattern.inc

noinst_HEADERS = \
	$(BUILTHEADERS) \
	book.h \
	debug.h \
	eval.h \
	gnushogi.h \
	opts.h \
	pattern.h

COMMONSOURCES =         \
	globals.c      	\
	init-common.c   \
	pattern-common.c

NOTCOMMONFILES =       \
	attacks.c      \
	book.c         \
	commondsp.c    \
	eval.c         \
	genmove.c      \
	init.c         \
	pattern.c      \
	rawdsp.c       \
	search.c       \
	tcontrl.c      \
	util.c

gnushogi_SOURCES = \
	main.c \
	$(NOTCOMMONFILES) \
	$(COMMONSOURCES)
gnuminishogi_SOURCES = \
	main.c \
	$(NOTCOMMONFILES) \
	$(COMMONSOURCES)

EXTRA_gnushogi_SOURCES = \
	cursesdsp.c
EXTRA_gnuminishogi_SOURCES = \
	cursesdsp.c

BUILT_SOURCES = $(BUILTHEADERS)

gnushogi_DEPENDENCIES = @CURSESDSP@
gnushogi_LDADD = @CURSESDSP@ @LIBCURSES@
gnuminishogi_DEPENDENCIES = @CURSESDSPMINI@
gnuminishogi_LDADD = @CURSESDSPMINI@ @LIBCURSES@

pat2inc_SOURCES = \
	pat2inc.c \
	makepattern.c \
	$(COMMONSOURCES)

pat2inc_LDADD = @LIBCURSES@

gnushogi-pattern.inc: $(top_srcdir)/misc/gnushogi.pat $(pat2inc_SOURCES)
	if [ "x@PAT2INCEXE@" = "x" ]; then \
	  echo "$@ needs rebuilding, which can only be done when NOT cross-compiling"; \
	  exit 1; \
	fi
	$(MAKE) $(AM_MAKEFLAGS) @PAT2INCEXE@
	./pat2inc $< $@
gnuminishogi-pattern.inc: $(top_srcdir)/misc/gnuminishogi.pat $(pat2inc_SOURCES)
	if [ "x@PAT2INCEXE@" = "x" ]; then \
	  echo "$@ needs rebuilding, which can only be done when NOT cross-compiling"; \
	  exit 1; \
	fi
	$(MAKE) $(AM_MAKEFLAGS) @PAT2INCEXE@
	./pat2inc $< $@

# just get *.inc generated
gnushogi_bootstrap: $(BUILTHEADERS)
	:

# optional support for prebuilding a bbk

gnushogi.bbk: $(top_srcdir)/misc/gnushogi.tbk gnushogi$(EXEEXT)
	rm -f $@
	cat $< > _tmp_t
	echo quit | ./gnushogi -R -b _tmp_t -B $@ -S 8000 -P 40 3 0
	rm _tmp_t
gnuminishogi.bbk: $(top_srcdir)/misc/gnuminishogi.tbk gnuminishogi$(EXEEXT)
	rm -f $@
	cat $< > _tmp_t
	echo quit | ./gnuminishogi -R -b _tmp_t -B $@ -S 8000 -P 40 3 0
	rm _tmp_t
