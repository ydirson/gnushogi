
%{

/*
 * parser.c - C source for Xshogi
 *
 * Copyright (c) 1993 Matthias Mutz
 *
 * XShogi is based on XBoard 2.0
 *
 * Copyright (c) 1992 Free Software Foundation
 *
 * This file is part of XShogi.
 *
 * XShogi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * XShogi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XShogi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include "xshogi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
                  
enum { False, True };

static void yyerror();
           
static ChessMove move_type;
static int 	 from_x, from_y, to_x, to_y;
static char      piece;

char currentMoveString[MSG_SIZ];
static char token[20];

FILE *outfile = (FILE *)0;

extern FILE	 *gameFileFP, *toFirstProgFP;
extern int	 currentMove;
extern char 	 moveList[MAX_MOVES][MOVE_LEN];

extern void SendToProgram P((char *message, FILE *fp));
extern void MakeMove P((ChessMove *move_type, int from_x, int from_y, int to_x, int to_y));

%}

%start goal

%token PROMOTE DROPS PIECE SQUARE NUMBER COMMENT COLON
      
%union { int val; char* string; }

%%         

 goal:
   comment_list move_elem move_list
 ;                           

 comment_list:
  | comment_list COMMENT
    	{ fprintf(stderr,"%s\n",$<string>2); } 
 ;

 move_list:
  | move_list move_elem
 ;

 move_elem:
    { strcpy(token, "number"); } number 
    { strcpy(token, "promoted"); } promoted 
    { strcpy(token, "move"); } move
 ;

 number:
  | NUMBER { strcpy(token, "colon"); } COLON
 ;

 promoted:
  | PROMOTE
 ;

 move:
   SQUARE  { from_x = '9' - $<string>1[0]; 
	     from_y = 'i' - $<string>1[1];
	     strcpy(currentMoveString,$<string>1);
    	     strcpy(token, "square"); }
   SQUARE  { to_x = '9' - $<string>3[0]; 
	     to_y = 'i' - $<string>3[1];
	     strcat(currentMoveString,$<string>3); }
   { move_type = NormalMove; }
   promotion
	{ 
	  SendToProgram(currentMoveString, toFirstProgFP);
	  strcpy(moveList[currentMove], currentMoveString);
	  MakeMove(&move_type, from_x, from_y, to_x, to_y);
	}
  |
   PIECE   { piece = $<string>1[0]; 
	     strcpy(currentMoveString,$<string>1); 
	     strcpy(token,"'*'"); }
   DROPS   { strcat(currentMoveString,"*"); 
	     strcpy(token, "square"); }
   SQUARE  { to_x = '9' - $<string>5[0]; 
	     to_y = 'i' - $<string>5[1];
	     strcat(currentMoveString,$<string>5); }
	{ 
	  move_type = (BlackOnMove(currentMove) ? BlackDrop : WhiteDrop );
	  switch ( piece ) {
		case 'P': from_x = 81; break;
		case 'L': from_x = 82; break;
		case 'N': from_x = 83; break;
		case 'S': from_x = 84; break;
		case 'G': from_x = 85; break;
		case 'B': from_x = 86; break;
		case 'R': from_x = 87; break;
		case 'K': from_x = 88; break;
		default:  from_x = -1;
	  };
	  from_y = from_x;
	  SendToProgram(currentMoveString, toFirstProgFP);
	  strcpy(moveList[currentMove], currentMoveString);
	  MakeMove(&move_type, from_x, from_y, to_x, to_y);
	}

 ;         

 promotion:
   | PROMOTE 
	{ move_type = (BlackOnMove(currentMove) ? BlackPromotion : WhitePromotion ); 
	  strcat(currentMoveString,"+"); }
 ;

%%
 


#include "scanner.c"


static void yyerror (errmsg)
char *errmsg;
{                               
  if ( strlen(token) > 0 ) {
    	fprintf(stderr,"parse error line %d column %d : %s expected\n",lines,cols,token); 
 	token[0] = '\0';
  } else
    	fprintf(stderr,"parse error line %d column %d : %s\n",lines,cols,errmsg); 
  exit(-1); 
}
       

void parseGameFile ()
{ 
  yyin = gameFileFP;
  outfile = stderr;
  yyparse();
}

