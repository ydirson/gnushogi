# Makefile for GNU Shogi
# Copyright (c) 1993 Matthias Mutz
#
# GNU Shogi is based on GNU Chess
# Copyright (c) 1992 Free Software Foundation
#
# This file is part of GNU SHOGI.
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

#
# gnushogi   is a curses-based shogi.
# gnushogix  is the xshogi based shogi.
# gnushogir  is a plain dumb-terminal shogi (but with full variation output).
# gnupdisp   display patterns in pattern textfile
#

#PATH = (. ~ ~/bin /mount/public.bones/bin /usr/local /usr/ucb /usr/bin /usr/etc)

# The version number of this GNU Shogi and XShogi release
VERS=	1.1

# Relevant file areas.
DIST=   ../README ../doc ../misc ../src

# Home directory
#HOME=/home/staff/mutz/capella
HOME=//capella/u/mutz

# Distribution directory
DISTDIR=$(HOME)

# Programs being distributed
PROGS=gnushogi-$(VERS)

# For _pow external 
LIBS= -lm

# Change these to something less transitory, like /usr/games, and then
# compile. Ask your system admin / unix guru to put gnushogi.{hash,lang,book}
# in $(LIBDIR).
# Where the binaries live.
BINDIR= $(HOME)/bin

# Where language description, our book, and the persistent hash live.
#LIBDIR= /home/staff/mutz/gnu/lib
LIBDIR= $(HOME)/lib

# Display routines.
LCURSES=-lcurses -ltermcap

# The compiler used for compiling this software.
# Use this for a plain C compiler 
#CC= cc $(OPT)
# Use this for DEC's ANSI C compiler on Ultrix
#CC= c89 $(OPT)
# Use this if you are lucky enough to have GNU CC.
CC=	gcc -W  $(OPT)
#CC=	cc $(OPT)

# Miscellaneous CFLAGS. Uncomment the one you need and comment the other
#CFLAGS= -p -Dinline=""	 -traditional-cpp
#CFLAGS= -O4 -Qpath . # SunOS cc using unprotoize
#CFLAGS= -O4 # Sun acc
#CFLAGS= -g -traditional-cpp  # debug
#CFLAGS= -O2 # DEC ANSI C (c89) on Ultrix.
#CFLAGS= +O3 -Aa -D_HPUX_SOURCE -DSYSV # HPUX cc 
#CFLAGS= -O   -finline-functions -fstrength-reduce -D__mips -D__LANGUAGE_C # gnu cc 1.40 on DS5000
#CFLAGS= -O   -finline-functions -fstrength-reduce  # gnu cc 1.40 on others
#CFLAGS= -g -w -finline-functions -fstrength-reduce # gnu cc 1.40 on others
#CFLAGS= -O2 -funroll-loops -D__mips -D__LANGUAGE_C # gnu cc 2.00 on DS5000
#CFLAGS= -O2 -D__alpha -D__LANGUAGE_C # gnu cc 2.00 on Flamingo
#CFLAGS= -O2 -funroll-loops -traditional-cpp # gnu cc  2.00 on SunOS
#CFLAGS= -g -O -finline-functions -fstrength-reduce -DOLDTIME -DNOFIONREAD # gnu cc 1.40 on sun4 (acrux)
CFLAGS= -O2 -funroll-loops -traditional-cpp -I/usr/local/include/ansi # gnu cc  2.00 on apollo (capella)

# Compression program for building distribution files
COMPRESS= gzip
Z= z
#COMPRESS= compress
#Z= Z

# compile options for gnushogi
# -DAG[0-4]  selects a set of values for SpaceBonus tables
# -DQUIETBACKGROUND don't print post information in background ( easy OFF)
# -DNOMEMSET if your machine does not support memset
# -DNODYNALPHA don't dynamically adjust alpha
# -DHISTORY use history killer hueristic 
# -DHASGETTIMEOFDAY use gettimeofday for more accurate timing
# -DLONG64 if you have 64bit longs
# -DSYSV if you are using SYSV
# -DCACHE  Cache static evaluations 
# -DQUIETBOOKGEN Don't print errors while loading a book or generating a binbook
# -DSEMIQUIETBOOKGEN Print less verbose errors while reading book or generating binbook
# -DGDX  use random file based book
# -DUSE_PATTERN use pattern directed scoring in openings
# -DNULLMOVE include null move heuristic
# -DXSHOGI create xshogi version
# -DBAREBONES do not print statistics (for gnushogix version)
# -DHARDTIMELIMIT reaching the time control means loosing the game
# -DCHECKTRP check for search tree overflow
# -DDEEPSEARCHCUT check for conditions that prevent move consideration for big plys
# some debug options
# -DDEBUG8 dump board,movelist,input move to /tmp/DEBUG if illegal move
# -DDEBUG9 dump move list from test command
# -DDEBUG10 dump board and move after search before !easy begins
# -DDEBUG11 dump board when the move is output
# -DDEBUG12 dump boards between moves
# -DDEBUG13 dump search control information for each move to /tmp/DEBUG
# -DDEBUG40 include extra values of variables for debugging  in game list
# -DDEBUG41 dump post output to /tmp/DEBUG
# -DDEBUG_EVAL dump evaluation of position to /tmp/EVAL on input command "eval"
# the rest of the debug options are tied to the debuglevel command
# -DDEBUG -DDEBUG4 set up code for debuglevel command
#          debuglevel
#               1 always force evaluation in evaluate
#               4 print move list after search
#               8 print move list after book before search
#              16 print move list after each ply of search
#              32 print adds to transposition table
#              64 print returns from transposition table lookups
#	      256 print search tree as it is generated
#	      512 debug trace
#	     1024 interactive tree print
#			debug? p#  where # is no. of plys to print from top of tree (default all plys)
#			       XXXX moves specifying branch of tree to print (default all branches)
#			       return terminates input
#	     2048 non-interactive trace print

# normal  
OPT= -DUSE_PATTERN -DGDX -DDEBUG9 -DVERYBUGGY -DCHECKTRP -DAG4 -DQUIETBACKGROUND -DHARDTIMELIMIT -DHASGETTIMEOFDAY -DCACHE -DHISTORY -DEXACTHISTORY # -DDEEPSEARCHCUT -DSEMIQUIETBOOKGEN 

# options for genmoves.c in order to support move ordering at cost of speed
# -DTESUJIBONUS  add bonus to a move that semms to be a tesuji
# -DFIELDBONUS   add bonus to regular moves
# -DDROPBONUS    add bonus to drops

GENOPT= -DTESUJIBONUS -DFIELDBONUS -DDROPBONUS
#GENOPT= -DTESUJIBONUS -DDROPBONUS

# Special options for gnushogix
XSHOGI= -DXSHOGI -DBAREBONES -DHARDTIMELIMIT

# Debug options for gnushogir             
DEBUG= -DDEBUG -DDEBUG41 -DDEBUG9 -DDEBUG10 # -DBOOKTEST -DCACHETEST -DHASHTEST

# The hashfile is a record of positions seen. It is used by
# GNU Shogi to avoid making the same mistakes, a form of learning.
HASH=	-DHASHFILE=\"$(LIBDIR)/gnushogi.hash\"

# The "book" is a record of the first few moves, for playing good
# moves easily and quickly, saving time, and irritating the human
# opponent.
BOOK=	-DBOOK=\"$(LIBDIR)/gnushogi.book\"
#BOOK=	
BINBOOK = -DBINBOOK=\"$(LIBDIR)/gnushogi.book.data\"

# The language file describes capabilities of the program. Perhaps
# it is useful for non-English speaking countries and customizing
# for their convenience and shogi happiness.
LANG= -DLANGFILE=\"$(LIBDIR)/gnushogi.lang\"

# The pattern file contains various opening patterns. The program tries to
# obtain such a pattern in the opening stage. Sequences of opening
# patterns may be described in order to support the correct order of moves.
PATTERN= -DPATTERNFILE=\"$(LIBDIR)/gnushogi.pattern\"

all : gnushogi gnushogix gnushogir 

gnushogi: mainN.o bookN.o genmovesN.o patternN.o ataks.o utilN.o evalN.o initN.o searchN.o dspcomN.o uxdsp.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnushogi mainN.o bookN.o genmovesN.o patternN.o ataks.o utilN.o evalN.o initN.o searchN.o dspcomN.o uxdsp.o $(LCURSES) $(LIBS)

gnushogix: mainX.o bookX.o genmovesX.o patternX.o ataks.o utilX.o evalX.o initX.o searchX.o dspcomX.o nondspX.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnushogix mainX.o bookX.o genmovesX.o patternX.o ataks.o utilX.o evalX.o initX.o searchX.o dspcomX.o nondspX.o $(LIBS)

gnushogir: mainN.o bookN.o genmovesR.o patternR.o ataks.o utilN.o evalN.o initN.o searchR.o dspcomR.o nondspR.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnushogir mainN.o bookN.o genmovesR.o patternR.o ataks.o utilN.o evalN.o initN.o searchR.o dspcomR.o nondspR.o $(LIBS)

Dgnushogir: mainDR.o bookDR.o genmovesDR.o patternDR.o ataks.o utilDR.o evalDR.o initDR.o searchDR.o dspcomDR.o nondspDR.o
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnushogir mainDR.o bookDR.o genmovesDR.o patternDR.o ataks.o utilDR.o evalDR.o initDR.o searchDR.o dspcomDR.o nondspDR.o $(LIBS) $(LIBS)

gnuan: mainN.o bookN.o genmovesN.o patternN.o ataks.o utilN.o evalN.o initN.o searchN.o gnuan.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnuan mainN.o bookN.o genmovesN.o pattern.o ataks.o utilN.o evalN.o initN.o searchN.o gnuan.o $(LIBS)

gdxbkstats: gdxbkstats.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) -o gdxbkstats gdxbkstats.o

gnupdisp: nondspDR.o dspcomDR.o bookN.o genmovesN.o patternDR.o ataks.o utilN.o evalN.o initN.o searchN.o gnupdisp.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o gnupdisp patternDR.o bookN.o genmovesN.o ataks.o utilN.o evalN.o searchN.o initN.o nondspDR.o dspcomDR.o gnupdisp.o $(LIBS)

size_test: nondspR.o dspcomR.o bookN.o genmovesN.o patternR.o ataks.o utilN.o evalN.o initN.o searchN.o pattern.o size_test.o
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -o size_test patternR.o bookN.o genmovesN.o ataks.o utilN.o evalN.o searchN.o initN.o nondspR.o dspcomR.o size_test.o $(LIBS)

gnupdisp.o: gnupdisp.c pattern.h gnushogi.h 
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) $(PATTERN) -c gnupdisp.c

mainN.o: main.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) -c main.c
	mv main.o mainN.o

mainX.o: main.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) $(XSHOGI) -c main.c
	mv main.o mainX.o

mainDR.o: main.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) -c main.c
	mv main.o mainDR.o

genmovesN.o: genmoves.c gnushogi.h version.h
	$(CC) $(GENOPT) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c genmoves.c
	mv genmoves.o genmovesN.o

genmovesR.o: genmoves.c gnushogi.h version.h
	$(CC) -DNONDSP $(GENOPT) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c genmoves.c
	mv genmoves.o genmovesR.o

genmovesDR.o: genmoves.c gnushogi.h version.h
	$(CC) $(DEBUG) $(GENOPT) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c genmoves.c
	mv genmoves.o genmovesDR.o

genmovesX.o: genmoves.c gnushogi.h version.h
	$(CC) $(GENOPT) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(XSHOGI) \
		-c genmoves.c
	mv genmoves.o  genmovesX.o

bookN.o: book.c gnushogi.h version.h ataks.h
	$(CC) -DNONDSP $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) -c book.c 
	mv book.o bookN.o

bookX.o: book.c gnushogi.h version.h ataks.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(XSHOGI) $(BINBOOK) \
		-c book.c
	mv book.o  bookX.o

bookDR.o: book.c gnushogi.h version.h ataks.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(BINBOOK) \
		-c book.c
	mv book.o  bookDR.o

patternN.o: pattern.c gnushogi.h pattern.h
	$(CC) $(CFLAGS) $(PATTERN) -c pattern.c
	mv pattern.o  patternN.o

patternX.o: pattern.c gnushogi.h pattern.h
	$(CC) $(CFLAGS) $(PATTERN) $(XSHOGI) -c pattern.c
	mv pattern.o  patternX.o

patternR.o: pattern.c gnushogi.h pattern.h
	$(CC) $(CFLAGS) $(PATTERN) -DNONDSP -c pattern.c
	mv pattern.o  patternR.o

patternDR.o: pattern.c gnushogi.h pattern.h
	$(CC) $(CFLAGS) $(PATTERN) $(DEBUG) -c pattern.c
	mv pattern.o  patternDR.o

ataks.o: ataks.h ataks.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c ataks.c

utilN.o: util.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c util.c
	mv util.o utilN.o

utilX.o: util.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(XSHOGI) -c util.c
	mv util.o utilX.o

utilDR.o: util.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c util.c
	mv util.o utilDR.o

evalN.o: eval.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c eval.c
	mv eval.o evalN.o

evalX.o: eval.c gnushogi.h version.h pattern.h
	$(CC) $(XSHOGI) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c eval.c
	mv eval.o evalX.o

evalDR.o: eval.c gnushogi.h version.h pattern.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c eval.c
	mv eval.o evalDR.o

initN.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP -c init.c
	mv init.o initN.o

initX.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) $(XSHOGI) -c init.c
	mv init.o  initX.o

initDR.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c init.c
	mv init.o initDR.o

searchN.o: search.c gnushogi.h version.h debug512.h debug10.h debug13.h debug16.h debug256.h debug4.h debug40.h debug41.h debug64.h debug8.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c search.c
	mv search.o searchN.o

searchX.o: search.c gnushogi.h version.h debug512.h debug10.h debug13.h debug16.h debug256.h debug4.h debug40.h debug41.h debug64.h debug8.h
	$(CC) $(XSHOGI) $(CFLAGS) $(HASH) $(LANG) $(BOOK) \
		-c search.c 
	mv search.o searchX.o

searchR.o: search.c gnushogi.h version.h debug512.h debug10.h debug13.h debug16.h debug256.h debug4.h debug40.h debug41.h debug64.h debug8.h
	$(CC) -DNONDSP $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c search.c
	mv search.o searchR.o

searchDR.o: search.c gnushogi.h version.h debug512.h debug10.h debug13.h debug16.h debug256.h debug4.h debug40.h debug41.h debug64.h debug8.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c search.c
	mv search.o searchDR.o

uxdsp.o: uxdsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c uxdsp.c

nondspX.o: nondsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP $(XSHOGI) \
		-c nondsp.c 
	mv nondsp.o nondspX.o

nondspR.o: nondsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP \
		-c nondsp.c 
	mv nondsp.o nondspR.o

nondspDR.o: nondsp.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP \
		-c nondsp.c 
	mv nondsp.o nondspDR.o

dspcomN.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -c dspcom.c
	mv dspcom.o dspcomN.o

dspcomX.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP $(XSHOGI) \
		-c dspcom.c 
	mv dspcom.o dspcomX.o

dspcomR.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP -DNOFIONREAD \
		-c dspcom.c 
	mv dspcom.o dspcomR.o

dspcomDR.o: dspcom.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BOOK) -DNONDSP -DNOFIONREAD \
		-c dspcom.c 
	mv dspcom.o dspcomDR.o

gnushogi.book.data:
	-rm ./gnushogi.book.data
	echo quit >test
#	cat ../misc/new.games/game?? > /tmp/t
	cat ../misc/SHOGI.book > /tmp/t
	cat ../misc/basic.book >> /tmp/t
	cat ../misc/some.openings/opening?? >> /tmp/t
	cat ../misc/shogi-l.games/*match-?? ../misc/shogi-l.games/game?? >> /tmp/t
	cat test| ./gnushogir -b /tmp/t -B ./gnushogi.book.data -S 8000 -P 40 3 0
	rm test /tmp/t


distribution:
	-patchlevel=`cat $(DISTDIR)/gnushogi-$(VERS)/src/version.h|grep patchlevel|sed -e 's/[^0-9]//g'` ;\
	echo "GNU patchlevel is $$patchlevel" ;\
	cd $(DISTDIR) ;\
	rm -f gnushogi.tar.$(VERS).$(Z)* ;\
	tar cf - $(PROGS) | $(COMPRESS) > $(DISTDIR)/gnushogi-$(VERS).pl$$patchlevel.tar.$(Z) ;\
#	rm -f gnushogi.tar.$(VERS).$(Z).uu* ;\
#	uuencode gnushogi-$(VERS).pl$$patchlevel.tar.$(Z) gnushogi-$(VERS).pl$$patchlevel.tar.$(Z) > gnushogi-$(VERS).pl$$patchlevel.tar.$(Z).uu ;\
#	rm -f x?? ;\
#	split -2500 gnushogi-$(VERS).pl$$patchlevel.tar.$(Z).uu ;\
#	for i in x??; do \
#	  mv $$i $(DISTDIR)/GNU_Shogi_$$i; \
#	done

install: 
	-cp gnushogix $(BINDIR)/gnushogix
	-cp gnushogir $(BINDIR)/gnushogir
	-cp gnushogi $(BINDIR)/gnushogi
	-cp gnupdisp $(BINDIR)/gnupdisp
#	-cp gnuan $(BINDIR)/gnuan
	-cp ./gnushogi.book.data $(LIBDIR)/gnushogi.book.data
	-cp ../misc/gnushogi.lang $(LIBDIR)/gnushogi.lang
	-cp ../misc/gnushogi.pattern $(LIBDIR)/gnushogi.pattern

clean:
	-rm -f gnushogix gnushogi gnushogir gnuan gnupdisp size_test 
	-echo $(DISTDIR)/gnushogi-$(VERS)
	-rm -f $(DISTDIR)/gnushogi-$(VERS)/src/gnushogi.book.data
	-find $(DISTDIR)/gnushogi-$(VERS) \( -name '*.o' -o -name '*~' -o -name 'CL*' -o -name 'PATCH*' -o -name '#*#' -o -name '%*%' -o -name '*orig' -o -name 'CL*' -o -name '*prt' -o -name '*bak' -o -name '*BAK' \) -exec rm -f {} \;

