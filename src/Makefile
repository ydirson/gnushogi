# Makefile for GNU Shogi
# Copyright (c) 1993, 1994 Matthias Mutz
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
# pat2inc    converts pattern textfile to include file
# sizetest   shows memory usage of main data structures
#

# change this if you need shorter filenames
GNUSHOGI= gnushogi
GNUSHOGIR= gnushogir
GNUSHOGIX= gnushogix

# The version number of this GNU Shogi and XShogi release
VERS=	1.2p03

# Relevant file areas.
DIST=   ../README-$(VERS) ../doc ../misc ../src

# Installation directory
prefix=/usr/local
#prefix=/public/projects/shogi

# Change these to something less transitory, like /usr/games, and then
# compile. Ask your system admin / unix guru to put gnushogi.{hsh,lng,tbk}
# in $(LIBDIR).
# Where the binaries live.
BINDIR= $(prefix)/games
#BINDIR=/public/projects/shogi/bin

# Where language description, our book, and the persistent hash live.
LIBDIR= $(prefix)/games/lib/gnushogi
#LIBDIR=/public/projects/shogi/lib

# Distribution directory
DISTDIR=/public/projects/shogi

# Programs being distributed
PROGS=gnushogi-$(VERS)

# For _pow external 
LIBS= -lm
# For _pow and _conio_kbhit external
#LIBS= -lm -lpc

# Display routines.
LCURSES=-lcurses -ltermcap

# The compiler used for compiling this software.
# Use this for a plain C compiler 
#CC= cc $(OPT) $(GENOPT)
# Use this for DEC's ANSI C compiler on Ultrix
#CC= c89 $(OPT) $(GENOPT)
# Use this if you are lucky enough to have GNU CC.
#CC=	/usr/pub/bin/gcc -fsigned-char $(OPT) $(GENOPT)
CC=	gcc -W -fsigned-char $(OPT) $(GENOPT)
#CC=	cc $(OPT) $(GENOPT)

# Miscellaneous CFLAGS. Uncomment the one you need and comment the other
#CFLAGS= -p -Dinline=""	 -traditional-cpp
#CFLAGS= -O4 -Qpath . # SunOS cc using unprotoize
#CFLAGS= -O4 # Sun acc
#CFLAGS= -g -traditional-cpp  # debug
#CFLAGS= -O2 # DEC ANSI C (c89) on Ultrix.
#CFLAGS= +O3 -Aa -D_HPUX_SOURCE -DSYSV # HPUX cc 
#CFLAGS= -O -finline-functions -fstrength-reduce -D__mips -D__LANGUAGE_C # gnu cc 1.40 on DS5000
#CFLAGS= -O -finline-functions -fstrength-reduce  # gnu cc 1.40 on others
#CFLAGS= -g -w -finline-functions -fstrength-reduce # gnu cc 1.40 on others
#CFLAGS= -O2 -funroll-loops -D__mips -D__LANGUAGE_C # gnu cc 2.00 on DS5000
#CFLAGS= -O2 -D__alpha -D__LANGUAGE_C # gnu cc 2.00 on Flamingo
#CFLAGS= -g -finline-functions -fstrength-reduce -DOLDTIME -DNOFIONREAD # gnu cc 1.40 on sun4
#CFLAGS= -O2 -funroll-loops -traditional-cpp -I/usr/local/include/ansi # gnu cc  2.00 on apollo (capella)
#CFLAGS= -O2 -funroll-loops -traditional-cpp -DNO_STRING_INCLUDE -DNOFIONREAD # gnu cc  2.00 (hawk)
#CFLAGS= -O2 -funroll-loops -DSEVENBIT # djgpp
#CFLAGS= -g -funroll-loops # gnu cc 2.00 on SunOS with debug
CFLAGS= -O2 -funroll-loops # gnu cc 2.00 on SunOS

# flags for DJGPP package
#COFF2EXE= coff2exe $(GNUSHOGI)
#COFF2EXER= coff2exe $(GNUSHOGIR)

# Compression program for building distribution files
COMPRESS= gzip
Z= gz
#COMPRESS= compress
#Z= Z

# lines per uuencoded file
UULINES=1500

# compile options for gnushogi
# -DQUIETBACKGROUND don't print post information in background ( easy OFF)
# -DNOLIST don't list game after exit
# -DNOBYTEOPS if your machine does not support bzero and bcopy
# -DNOMEMOPS if your machine does not support memset and memcpy
# -DNODYNALPHA don't dynamically adjust alpha
# -DHISTORY use history killer hueristic 
# -DHASGETTIMEOFDAY use gettimeofday for more accurate timing
# -DLONG64 if you have 64bit longs
# -DSYSV if you are using SYSV
# -DCACHE  Cache static evaluations 
# -DQUIETBOOKGEN Don't print errors while loading a book or generating a binbook
# -DSEMIQUIETBOOKGEN Print less verbose errors while reading book or generating binbook
# -DNULLMOVE include null move heuristic
# -DXSHOGI create xshogi version
# -DBAREBONES do not print statistics (for gnushogix version)
# -DHARDTIMELIMIT reaching the time control means loosing the game
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
OPT= -DVERYBUGGY -DQUIETBACKGROUND -DHARDTIMELIMIT -DHASGETTIMEOFDAY -DCACHE -DNULLMOVE -DHISTORY -DEXACTHISTORY # -DSEMIQUIETBOOKGEN 
# small memory  
#OPT= -DEASY_OPENINGS -DSMALL_MEMORY -DSLOW_CPU -DSAVE_PTYPE_DISTDATA -DSAVE_NEXTPOS -DVERYBUGGY -DQUIETBACKGROUND -DHARDTIMELIMIT -DHASGETTIMEOFDAY -DNULLMOVE -DCACHE -DHISTORY # -DSAVE_SSCORE -DEXACTHISTORY -DSEMIQUIETBOOKGEN 
#OPT= -DSMALL_MEMORY -DVERY_SLOW_CPU -DSAVE_PTYPE_DISTDATA -DSAVE_NEXTPOS -DVERYBUGGY -DQUIETBACKGROUND -DHARDTIMELIMIT -DHASGETTIMEOFDAY -DNULLMOVE -DCACHE -DHISTORY # -DSAVE_SSCORE -DEXACTHISTORY -DSEMIQUIETBOOKGEN 

# options for genmove.c in order to support move ordering at cost of speed
# -DTESUJIBONUS   add bonus to a move that semms to be a tesuji
# -DFIELDBONUS    add bonus to regular moves
# -DDROPBONUS     add bonus to drops
# -DDEEPSEARCHCUT check for moves not to consider at deep plys
# slow CPU
#GENOPT= -DCHECKBONUS -DDEEPSEARCHCUT -DTESUJIBONUS
# fast CPU
GENOPT= -DCHECKBONUS -DDEEPSEARCHCUT -DTESUJIBONUS -DDROPBONUS -DFIELDBONUS

# Special options for gnushogix
XSHOGI= -DXSHOGI -DBAREBONES -DHARDTIMELIMIT

# Debug options for Dgnushogir             
DEBUG= -DNONDSP -DDEBUG_EVAL -DDEBUG -DDEBUG4 -DHASHKEYTEST -DHASHTEST -DCACHETEST -DDEBUG41 -DDEBUG9 -DDEBUG10 # -DBOOKTEST

# The hashfile is a record of positions seen. It is used by
# GNU Shogi to avoid making the same mistakes, a form of learning.
HASH=	-DHASHFILE=\"$(LIBDIR)/gnushogi.hsh\"

# The "book" is a record of the first few moves, for playing good
# moves easily and quickly, saving time, and irritating the human
# opponent.
TEXTBOOK= -DBOOK=\"$(LIBDIR)/gnushogi.tbk\"
BINBOOK = -DBINBOOK=\"$(LIBDIR)/gnushogi.bbk\"

# The language file describes capabilities of the program. Perhaps
# it is useful for non-English speaking countries and customizing
# for their convenience and shogi happiness.
LANG= -DLANGFILE=\"../misc/gnushogi.lng\"

# The pattern file contains various opening patterns. The program tries to
# obtain such a pattern in the opening stage. Sequences of opening
# patterns may be described in order to support the correct order of moves.
PATTERN= -DPATTERNFILE=\"../misc/gnushogi.pat\"

all : $(GNUSHOGI) $(GNUSHOGIX) gnushogi.bbk # sizetest pat2inc 

$(GNUSHOGI): mainN.o globals.o bookN.o genmoveN.o patternN.o ataksN.o utilN.o evalN.o initN.o searchN.o tcontrlN.o dspcomN.o uxdsp.o
	$(CC) $(CFLAGS) -o $(GNUSHOGI) mainN.o globals.o bookN.o genmoveN.o patternN.o ataksN.o utilN.o evalN.o initN.o searchN.o tcontrlN.o dspcomN.o uxdsp.o $(LCURSES) $(LIBS)
	$(COFF2EXE)

$(GNUSHOGIX): mainX.o globals.o bookX.o genmoveX.o patternX.o ataksN.o utilX.o evalX.o initX.o searchX.o tcontrlX.o dspcomX.o nondspX.o
	$(CC) $(CFLAGS) -o $(GNUSHOGIX) mainX.o globals.o bookX.o genmoveX.o patternX.o ataksN.o utilX.o evalX.o initX.o searchX.o tcontrlX.o dspcomX.o nondspX.o $(LIBS)

$(GNUSHOGIR): mainN.o globals.o bookN.o genmoveR.o patternR.o ataksR.o utilR.o evalR.o initR.o searchR.o tcontrlN.o dspcomR.o nondspR.o
	$(CC) $(CFLAGS) -o $(GNUSHOGIR) mainN.o globals.o bookN.o genmoveR.o patternR.o ataksR.o utilR.o evalR.o initR.o searchR.o tcontrlN.o dspcomR.o nondspR.o $(LIBS)
	$(COFF2EXER)

D$(GNUSHOGIR): mainDR.o globals.o bookDR.o genmoveDR.o patternDR.o ataksDR.o utilDR.o evalDR.o initDR.o searchDR.o tcontrlDR.o dspcomDR.o nondspDR.o
	$(CC) $(DEBUG) $(CFLAGS) -o D$(GNUSHOGIR) mainDR.o globals.o bookDR.o genmoveDR.o patternDR.o ataksDR.o utilDR.o evalDR.o initDR.o searchDR.o tcontrlDR.o dspcomDR.o nondspDR.o $(LIBS) $(LIBS)

pat2inc: nondspDR.o dspcomDR.o globals.o bookDR.o genmoveDR.o patternP.o ataksDR.o utilDR.o evalDR.o initDR.o searchDR.o tcontrlDR.o pat2inc.o
	$(CC) $(CFLAGS) -o pat2inc patternP.o globals.o bookDR.o genmoveDR.o ataksDR.o utilDR.o evalDR.o searchDR.o tcontrlDR.o initDR.o nondspDR.o dspcomDR.o pat2inc.o $(LIBS)

sizetest: sizetest.o 
	$(CC) $(CFLAGS) -o sizetest sizetest.o $(LIBS)

pat2inc.o: pat2inc.c pattern.h gnushogi.h 
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) $(BINBOOK) $(PATTERN) -c pat2inc.c

sizetest.o: sizetest.c gnushogi.h eval.h
	$(CC) $(CFLAGS) $(HASH) -c sizetest.c

globals.o: globals.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c globals.c

mainN.o: main.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(BINBOOK) -c main.c
	mv main.o mainN.o

mainX.o: main.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(BINBOOK) $(XSHOGI) -c main.c
	mv main.o mainX.o

mainDR.o: main.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(BINBOOK) $(HASH) -c main.c
	mv main.o mainDR.o

genmoveN.o: genmove.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c genmove.c
	mv genmove.o genmoveN.o

genmoveR.o: genmove.c gnushogi.h version.h
	$(CC) -DNONDSP -DDEBUG_EVAL $(CFLAGS) $(HASH) -c genmove.c
	mv genmove.o genmoveR.o

genmoveDR.o: genmove.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -c genmove.c
	mv genmove.o genmoveDR.o

genmoveX.o: genmove.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(XSHOGI) \
		-c genmove.c
	mv genmove.o  genmoveX.o

bookN.o: book.c gnushogi.h version.h 
	$(CC) -DNONDSP $(CFLAGS) $(HASH) $(TEXTBOOK) $(BINBOOK) -c book.c 
	mv book.o bookN.o

bookX.o: book.c gnushogi.h version.h 
	$(CC) $(CFLAGS) $(HASH) $(TEXTBOOK) $(BINBOOK) $(XSHOGI) \
		-c book.c
	mv book.o  bookX.o

bookDR.o: book.c gnushogi.h version.h 
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(TEXTBOOK) $(BINBOOK) \
		-c book.c
	mv book.o  bookDR.o

patternN.o: pattern.c gnushogi.h pattern.h pattern.inc
	$(CC) $(CFLAGS) -c pattern.c
	mv pattern.o  patternN.o

patternX.o: pattern.c gnushogi.h pattern.h pattern.inc
	$(CC) $(CFLAGS) $(XSHOGI) -c pattern.c
	mv pattern.o  patternX.o

patternR.o: pattern.c gnushogi.h pattern.h pattern.inc
	$(CC) $(CFLAGS) -DNONDSP -c pattern.c
	mv pattern.o  patternR.o

patternDR.o: pattern.c gnushogi.h pattern.h pattern.inc
	$(CC) $(CFLAGS) $(DEBUG) -c pattern.c
	mv pattern.o  patternDR.o

# to create "pattern.inc" with "pat2inc", the external
# pattern textfile must be used.

patternP.o: pattern.c gnushogi.h pattern.h
	$(CC) $(DEBUG) $(CFLAGS) -DEXTPATTERNFILE $(PATTERN) -c pattern.c
	mv pattern.o  patternP.o

ataksN.o: ataks.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c ataks.c
	mv ataks.o ataksN.o

ataksR.o: ataks.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -DDEBUG_EVAL -DNONDSP -c ataks.c
	mv ataks.o ataksR.o

ataksDR.o: ataks.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -c ataks.c
	mv ataks.o ataksDR.o

utilN.o: util.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c util.c
	mv util.o utilN.o

utilX.o: util.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) $(XSHOGI) -c util.c
	mv util.o utilX.o

utilR.o: util.c gnushogi.h version.h
	$(CC) -DNONDSP -DDEBUG_EVAL $(CFLAGS) $(HASH) $(XSHOGI) -c util.c
	mv util.o utilR.o

utilDR.o: util.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -c util.c
	mv util.o utilDR.o

evalN.o: eval.c eval.h gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) -c eval.c
	mv eval.o evalN.o

evalX.o: eval.c eval.h gnushogi.h version.h pattern.h
	$(CC) $(XSHOGI) $(CFLAGS) $(HASH) -c eval.c
	mv eval.o evalX.o

evalR.o: eval.c eval.h gnushogi.h version.h pattern.h
	$(CC) -DDEBUG_EVAL $(CFLAGS) $(HASH) -c eval.c
	mv eval.o evalR.o

evalDR.o: eval.c eval.h gnushogi.h version.h pattern.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -c eval.c
	mv eval.o evalDR.o

initN.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) -c init.c
	mv init.o initN.o

initX.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) $(XSHOGI) -c init.c
	mv init.o  initX.o

initR.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(CFLAGS) $(HASH) $(LANG) -DNONDSP -c init.c
	mv init.o initR.o

initDR.o: init.c gnushogi.h version.h pattern.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) $(LANG) -c init.c
	mv init.o initDR.o

searchN.o: search.c gnushogi.h version.h 
	$(CC) $(CFLAGS) $(HASH) -c search.c
	mv search.o searchN.o

searchX.o: search.c gnushogi.h version.h 
	$(CC) $(XSHOGI) $(CFLAGS) $(HASH) \
		-c search.c 
	mv search.o searchX.o

searchR.o: search.c gnushogi.h version.h
	$(CC) -DNONDSP -DDEBUG_EVAL $(CFLAGS) $(HASH) -c search.c
	mv search.o searchR.o

searchDR.o: search.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -c search.c
	mv search.o searchDR.o

tcontrlN.o: tcontrl.c gnushogi.h version.h 
	$(CC) $(CFLAGS) -c tcontrl.c
	mv tcontrl.o tcontrlN.o

tcontrlX.o: tcontrl.c gnushogi.h version.h 
	$(CC) $(XSHOGI) $(CFLAGS) -c tcontrl.c
	mv tcontrl.o tcontrlX.o

tcontrlDR.o: tcontrl.c gnushogi.h version.h 
	$(CC) $(DEBUG) $(CFLAGS) -c tcontrl.c
	mv tcontrl.o tcontrlDR.o

uxdsp.o: uxdsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c uxdsp.c

nondspX.o: nondsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -DNONDSP $(XSHOGI) \
		-c nondsp.c 
	mv nondsp.o nondspX.o

nondspR.o: nondsp.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -DNONDSP \
		-c nondsp.c 
	mv nondsp.o nondspR.o

nondspDR.o: nondsp.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -DNONDSP \
		-c nondsp.c 
	mv nondsp.o nondspDR.o

dspcomN.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -c dspcom.c
	mv dspcom.o dspcomN.o

dspcomX.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -DNONDSP $(XSHOGI) \
		-c dspcom.c 
	mv dspcom.o dspcomX.o

dspcomR.o: dspcom.c gnushogi.h version.h
	$(CC) $(CFLAGS) $(HASH) -DDEBUG_EVAL -DNONDSP -DNOFIONREAD \
		-c dspcom.c 
	mv dspcom.o dspcomR.o

dspcomDR.o: dspcom.c gnushogi.h version.h
	$(CC) $(DEBUG) $(CFLAGS) $(HASH) -DNONDSP -DNOFIONREAD \
		-c dspcom.c 
	mv dspcom.o dspcomDR.o

gnushogi.bbk: $(GNUSHOGIR)
	-rm ./gnushogi.bbk
	echo quit >test
	cat ../misc/gnushogi.tbk > _tmp_t
	cat test| ./$(GNUSHOGIR) -b _tmp_t -B ./gnushogi.bbk -S 8000 -P 40 3 0
	rm test _tmp_t


distribution:
	-patchlevel=`cat $(DISTDIR)/gnushogi-$(VERS)/src/version.h|grep patchlevel|sed -e 's/[^0-9]//g'` ;\
	echo "GNU patchlevel is $$patchlevel" ;\
	cd $(DISTDIR) ;\
	rm -f gnushogi.tar.$(VERS).$(Z)* ;\
	tar cf - $(PROGS) | $(COMPRESS) > $(DISTDIR)/gnushogi-$(VERS).tar.$(Z) ;\
#	rm -f gnushogi.tar.$(VERS).$(Z).uu* ;\
#	uuencode gnushogi-$(VERS).tar.$(Z) gnushogi-$(VERS).tar.$(Z) > gnushogi-$(VERS).tar.$(Z).uu ;\
#	rm -f x?? ;\
#	split -$(UULINES) gnushogi-$(VERS).tar.$(Z).uu ;\
#	for i in x??; do \
#	  mv $$i $(DISTDIR)/GNU_Shogi_$$i; \
#	done

install:
	-cp gnushogi $(BINDIR)/gnushogi
	-cp gnushogix $(BINDIR)/gnushogix
	-cp gnushogir $(BINDIR)/gnushogir
	-cp ./gnushogi.bbk $(LIBDIR)/gnushogi.bbk

clean:
	-rm -f gnushogix gnushogi gnushogir Dgnushogir sizetest pat2inc
	-echo $(DISTDIR)/gnushogi-$(VERS)
	-rm -f $(DISTDIR)/gnushogi-$(VERS)/src/gnushogi.bbk
	-find $(DISTDIR)/gnushogi-$(VERS) \( -name '*.o' -o -name '*~' -o -name 'CL*' -o -name 'PATCH*' -o -name '#*#' -o -name '%*%' -o -name '*orig' -o -name 'CL*' -o -name '*prt' -o -name '*bak' -o -name '*BAK' \) -exec rm -f {} \;

