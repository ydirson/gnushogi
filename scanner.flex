%{
static int lines=1,cols=1;
%}
PIECE  [PLNSGBRK]               
SQUARE [1-9][a-i]     
NUMBER [1-9]([0-9])*
COMMENT ["#"]([^\n])*
%%       
"\n"	  { lines++; cols=1; }
"+"	  { cols++;  return PROMOTE; }
"*"	  { cols++;  return DROPS; }
"'"	  { cols++;  return DROPS; }
"."	  { cols++;  return COLON; }
{PIECE}   { yylval.string = yytext; cols+=strlen(yytext); return PIECE; }
{SQUARE}  { yylval.string = yytext; cols+=strlen(yytext); return SQUARE; }
{NUMBER}  { yylval.string = yytext; cols+=strlen(yytext); return NUMBER; }
{COMMENT} { yylval.string = yytext; lines++; cols=1; return COMMENT; }
.	  { cols++; }
%%	



