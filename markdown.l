%{

#include "mukette.h"

%}

%s BOLD
%s ITALIC
%s BOLD_ITALIC
%s UNDERLINE
%s STRIKETHROUGH
%s HYPERLINK

%%

<BOLD>[\\]?[*_]{2}      { if (yytext[0] != '\\') 
                                        { BEGIN(INITIAL); } 
                                        else { PRINT_BOLD(&yytext[1]); } } 
<BOLD>.+                { PRINT_BOLD(yytext); }

<ITALIC>[\\]?[*_]{1}    { if (yytext[0] != '\\')                      
                                        { BEGIN(INITIAL); } 
                                        else { PRINT_ITALIC(&yytext[1]); } } 
<ITALIC>.+              { PRINT_ITALIC(yytext); }

<BOLD_ITALIC>[\\]?[*_]{3}   { if (yytext[0] != '\\') 
					{ BEGIN(INITIAL); }
					else { BOLD_PRINT_ITALIC(&yytext[1]); } }
<BOLD_ITALIC>.+             { BOLD_PRINT_ITALIC(yytext); }

<UNDERLINE>[\\]?[_]{2}  { if (yytext[0] != '\\') 
                                        { BEGIN(INITIAL); } 
                                        else { PRINT_UNDERLINE(&yytext[1]); } } 
<UNDERLINE>.+           { PRINT_UNDERLINE(yytext); }

<STRIKETHROUGH>[\\]?[~]{2}  { if (yytext[0] != '\\') 
                                        { BEGIN(INITIAL); } 
                                        else { PRINT_BOLD_UNDERLINE(&yytext[1]); } } 
<STRIKETHROUGH>.+       { PRINT_BOLD_UNDERLINE(yytext); }

<INITIAL>[\\]?[*_]{1}	 { if (yytext[0] != '\\') 
					{ BEGIN(ITALIC); }
					else { PRINT_NORMAL(&yytext[1]); } }
<INITIAL>[\\]?[*_]{2}    { if (yytext[0] != '\\') 
					{ BEGIN(UNDERLINE); }
					else { PRINT_NORMAL(&yytext[1]); } }
<INITIAL>[\\]?[~]{2}     { if (yytext[0] != '\\') 
					{ BEGIN(STRIKETHROUGH); }
					else { PRINT_NORMAL(&yytext[1]); } }