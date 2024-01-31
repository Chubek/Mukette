%{
#include "mukette.h"
#include <getopt.h>

#ifndef LEX_STACK_MAX
#define LEX_STACK_MAX 4096
#endif

static int stack[LEX_STACK_MAX] = {0};
static int stack_top = 1;

static inline int push_stack(int state) {
     return (stack[stack_top++] = state);
}

static inline int pop_stack(void) {
    return stack[--stack_top];
}


int yyerror(const char *msg);

int yywrap(void);

%}

%s BOLD
%s ITALIC
%s BOLD_ITALIC
%s UNDERLINE
%s STRIKETHROUGH
%s HYPERLINK
%s URL
%s TABLE_HEAD
%s TABLE_BODY

web_url ((http|https):\/\/)?[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}\/?[a-zA-Z0-9._\/%&=?-]*

%%

<ITALIC>[\\]?[*_]{1}    {
    if (yytext[0] != '\\') {
        PRINT_ITALIC(&yytext[1]);
        BEGIN(pop_stack());
    } else {
        PRINT_ITALIC(&yytext[1]);
    }
}

<ITALIC>.+              {
    PRINT_ITALIC(yytext);
    BEGIN(pop_stack());
}


<BOLD>[\\]?[*_]{2}      {
    if (yytext[0] != '\\') {
        PRINT_BOLD(&yytext[1]);
        BEGIN(pop_stack());
    } else {
        PRINT_BOLD(&yytext[1]);
    }
} 

<BOLD>.+                {
    PRINT_BOLD(yytext);
    BEGIN(pop_stack());
}

<BOLD_ITALIC>[\\]?[*_]{3}   {
    if (yytext[0] != '\\') {
        PRINT_BOLD_ITALIC(&yytext[1]);
        BEGIN(pop_stack());
    } else {
        PRINT_BOLD_ITALIC(&yytext[1]);
    }
}

<BOLD_ITALIC>.+             {
    PRINT_BOLD_ITALIC(yytext);
    BEGIN(pop_stack());
}

<STRIKETHROUGH>[\\]?[~]{2}  {
    if (yytext[0] != '\\') {
        PRINT_BOLD_UNDERLINE(&yytext[1]);
        BEGIN(pop_stack());
    } else {
        PRINT_BOLD_UNDERLINE(&yytext[1]);
    }
}

<STRIKETHROUGH>.+       {
    PRINT_BOLD_UNDERLINE(yytext);
    BEGIN(pop_stack());
}


<UNDERLINE>[\\]?[~]{3}  {
    if (yytext[0] != '\\') {
        PRINT_UNDERLINE(&yytext[1]);
        BEGIN(pop_stack());
    } else {
        PRINT_UNDERLINE(&yytext[1]);
    }
}

<UNDERLINE>.+           {
    PRINT_UNDERLINE(yytext);
    BEGIN(pop_stack());
}

<HYPERLINK>[\\]?[\]\(]	{
    if (yytext[0] != '\\') {
        BEGIN(URL);
    } else {
        PRINT_ITALIC_UNDERLINE(&yytext[1]);
    }
}

<HYPERLINK>.+		{
    PRINT_ITALIC_UNDERLINE(yytext);
    BEGIN(pop_stack());
}

<URL>{web_url}[)]	{
    add_hyperlink(&yytext[0]);
    BEGIN(pop_stack());
}

<TABLE_HEAD,TABLE_BODY>[\\]?[|]	{
    if (yytext[0] != '\\') {
        TAB();
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<TABLE_HEAD>[\r\n][|][-|]+ {
    NEWLINE();
    BEGIN(push_stack(TABLE_BODY));
}

<TABLE_BODY>[|][ \r\n]+	{
    NEWLINE();
    BEGIN(pop_stack());
}

<TABLE_HEAD>.+		{
    PRINT_BOLD_ITALIC_UNDERLINE(&yytext[0]);
    BEGIN(pop_stack());
}

<TABLE_BODY>.+		{
    PRINT_BOLD_UNDERLINE(&yytext[0]);
    BEGIN(pop_stack());
}

<*>[\\]?[*_]{1}   	{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(ITALIC));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\\]?[*_]{2}  	{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(BOLD));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\\]?[*_]{3}  	{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(BOLD_ITALIC));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\\]?[~]{2}    	{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(STRIKETHROUGH));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\\]?[~]{3}    	{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(UNDERLINE));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}


<*>[\\]?[\[]  		{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(HYPERLINK));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\\]?[|]		{
    if (yytext[0] != '\\') {
        BEGIN(push_stack(TABLE_HEAD));
    } else {
        PRINT_NORMAL(&yytext[1]);
    }
}

<*>[\r\n]+	{
    NEWLINE();
    BEGIN(pop_stack());
}

<*>[ \t]	{
    WHITESPACE();
}

<*>[^ \t\r\n]+  {
    PRINT_NORMAL(&yytext[0]);
}

%%

int yywrap(void) {
  return 1;
}

int yyerror(const char* msg) {
   fprintf(stderr, "%s\n", msg);
   fprintf(stderr, "Markdown parse failed\n");
   exit(EXIT_FAILURE);
}

int load_yyin(int argc, char **argv) {
   if (argc == 1)
   	yyin = stdin;
   else
   	yyin = fopen(argv[1], "r");
}

int main(int argc, char **argv) {
  initialize_muk_config();

  INIT_SCREEN();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  load_yyin(argc, argv);
  yylex();
  poll_and_navigate();

  END_SCREEN();

  yyin != stdin 
  	? fclose(yyin)
	: 0;

  return 0;
}
