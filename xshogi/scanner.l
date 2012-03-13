%{
/*
 * FILE: scanner.l
 *
 *     Lexer for xshogi.
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
 
static int lines = 1, cols = 1;
%}

PIECE   [PLNSGBRK]               
SQUARE  [1-9][a-i]     
NUMBER  [1-9]([0-9])*
COMMENT ["#"]([^\n])*

%%

"\n"      { lines++; cols = 1; }
"+"       { cols++;  return PROMOTE; }
"*"       { cols++;  return DROPS; }
"'"       { cols++;  return DROPS; }
"."       { cols++;  return COLON; }
{PIECE}   { yylval.string = yytext; cols += strlen(yytext); return PIECE;  }
{SQUARE}  { yylval.string = yytext; cols += strlen(yytext); return SQUARE; }
{NUMBER}  { yylval.string = yytext; cols += strlen(yytext); return NUMBER; }
{COMMENT} { yylval.string = yytext; lines++; cols = 1; return COMMENT; }
.         { cols++; }

%%

/*
 * This is to avoid having to link in a lex library;
 * flex also allows the "%option noyywrap" option but
 * I don't think that's generally true of other lex
 * implementations.
 */

int
yywrap()
{
    return 1;
}



