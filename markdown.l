%{

#include "mukette.h"

#ifndef LEX_STACK_MAX
#define LEX_STACK_MAX 4096
#endif

static int stack[LEX_STACK_MAX] = {-1};
static int stack_top = 0;

static inline int push_stack(int state) 
{ stack[stack_top++] = state; return state; }
static inline int pop_stack(void) 
{ int state = (stack[stack_top--] = -1); return stack[stack_top]; }


%}

%s BOLD
%s ITALIC
%s BOLD_ITALIC
%s UNDERLINE
%s STRIKETHROUGH
%s HYPERLINK
%s URL

web_url ((http|https):\/\/)?[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}\/?[a-zA-Z0-9._\/%&=?-]*

%%

<BOLD>[\\]?[*_]{2}      { if (yytext[0] != '\\') 
                                        { BEGIN(stack_pop()); } 
                                        else { PRINT_BOLD(&yytext[1]); } } 
<BOLD>.+                { PRINT_BOLD(yytext); }



<ITALIC>[\\]?[*_]{1}    { if (yytext[0] != '\\')                      
                                        { BEGIN(stack_pop()); } 
                                        else { PRINT_ITALIC(&yytext[1]); } } 
<ITALIC>.+              { PRINT_ITALIC(yytext); }



<BOLD,ITALIC>[\\]?[*_]{3}   { if (yytext[0] != '\\') 
					{ BEGIN(stack_pop()); }
					else { BOLD_PRINT_ITALIC(&yytext[1]); } }
<BOLD,ITALIC>.+             { BOLD_PRINT_ITALIC(yytext); }



<UNDERLINE>[\\]?[_]{2}  { if (yytext[0] != '\\') 
                                        { BEGIN(stack_pop()); } 
                                        else { PRINT_UNDERLINE(&yytext[1]); } } 
<UNDERLINE>.+           { PRINT_UNDERLINE(yytext); }



<STRIKETHROUGH>[\\]?[~]{2}  { if (yytext[0] != '\\') 
                                        { BEGIN(stack_pop()); } 
                                        else { PRINT_BOLD_UNDERLINE(&yytext[1]); } } 
<STRIKETHROUGH>.+       { PRINT_BOLD_UNDERLINE(yytext); }

<HYPERLINK>[\\]?[\]\(]	{ if (yytext[0] != '\\')
				{ BEGIN(URL); }
				else { PRINT_ITALIC_UNDERLINE(&yytext[1]); } }
<HYPERLINK>.+		{ PRINT_ITALIC_UNDERLINE(yytext); }

<URL>{web_url} ")"	{ add_hyperlink(&yytext[0]); BEGIN(stack_pop()); }

<TABLE>///implement



<*>[\\]?[*_]{1}   { if (yytext[0] != '\\') 
					{ BEGIN(push_stack(ITALIC)); }
					else { PRINT_NORMAL(&yytext[1]); } }
<*>[\\]?[*_]{2}   { if (yytext[0] != '\\') 
					{ BEGIN(push_stack(UNDERLINE)); }
					else { PRINT_NORMAL(&yytext[1]); } }
<*>[\\]?[~]{2}    { if (yytext[0] != '\\') 
					{ BEGIN(push_stack(STRIKETHROUGH)); }
					else { PRINT_NORMAL(&yytext[1]); } }

<*>[\\]?[\[]  	 { if (yytext[0] != '\\')
					{ BEGIN(push_stack(HYPERLINK)); }
					else { PRINT_NORMAL(&yytext[1]); } }

<*>[\\]?[|]	 { if (yytext[0] != '\\')
					{ BEGIN(push_stack(TABLE));	}
					else { PRINT_NORMAL(&yytext[1]); } }




<*>[\r\n]+	{ NEWLINE(); BEGIN(pop_stack()); }
<*>[ \t]	{ WHITESPACE();	}		
<*>[^ \t\r\n]+  { PRINT_NORMAL(&yytext[0]); }



