%{
    #include "parser.h"
    #include <iostream>
%}
 
%option reentrant
%option noyywrap
 
%x SINGLE_QUOTED
%x DOUBLE_QUOTED

%%

"=="   {return EQUALS;}
"="    {return ASSIGN;}
"+"    {return ADD;}
"-"    {return SUB;}
"*"    {return MULT;}
"/"    {return DIV;}
"%"    {return MOD;}
"{"    {return LBRACE;}
"}"    {return RBRACE;}

"if"     {return IF;}

[ \t\r\n]    {}

[0-9]+.[0-9]+ {return DOUBLE;}
[0-9]+ {return INTEGER;}

%%