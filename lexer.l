%{
    #include "parser.h"
    #include <iostream>
%}
 
%option reentrant
%option noyywrap
 
%x SINGLE_QUOTED
%x DOUBLE_QUOTED

%%

"="    {return EQUALS;}
"+"    {return ADD;}
"-"    {return SUB;}
"*"    {return MULT;}
"/"    {return DIV;}
"%"    {return MOD;}

[ \t\r\n]    {}

[0-9]+.[0-9]+ {return DOUBLE;}
[0-9]+ {return INTEGER;}

%%