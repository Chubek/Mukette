%{
#include "mukette.h"
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>

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

static inline char *trim_text(char *text, int pos) {
   text[pos] = '\0';
   return text;
}

static inline bool file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
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
%s MULTILINE_CODE

web_url ((http|https):\/\/)?[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}\/?[a-zA-Z0-9._\/%&=?-]*

%%

<*>[_*]{1}[^_*]+[_*]{1}.      {
    PRINT_ITALIC(trim_text(&yytext[1], yyleng - 1));
    BEGIN(pop_stack());
}


<*>[_*]{2}[^_*]+[_*]{2}       {
    PRINT_BOLD(trim_text(&yytext[2], yyleng - 2));
    BEGIN(pop_stack());
}

<*>[_*]{3}[^_*]+[_*]{3}   {
    PRINT_BOLD_ITALIC(trim_text(&yytext[3], yyleng - 3));
    BEGIN(pop_stack());
}

<*>[~]{2}[^~]+[~]{2} {
    PRINT_BOLD_UNDERLINE(trim_text(yytext, yyleng - 2));
    BEGIN(pop_stack());
}


<*>[~]{3}[^~]+[~]{3}     {
    PRINT_UNDERLINE(trim_text(yytext, yyleng - 3));
    BEGIN(pop_stack());
}

<*>[\[][^\]]+[\](][^)]+[[)]	 {
    char *dup1, *dup2, *original; 
    int len = 0;
    
    dup1 = dup2 = original = strndup(&yytext[1], yyleng - 1);

    while (*dup1++ != ']')
    	len++;

    dup2[len] = '\0';

    PRINT_BOLD_UNDERLINE(&dup2[0]);
    
    add_hyperlink(&dup1[0]);
    BEGIN(pop_stack());

    free(original);
}

<TABLE_HEAD>[^|\n]+        {
    PRINT_COLOR(COLOR_TABLE_HEADER, &yytext[0]);
}

<TABLE_HEAD>[|-]+[\n]	  {
   NEWLINE();
   BEGIN(TABLE_BODY);
}

<TABLE_BODY>[^|\n]+	  {
   PRINT_COLOR(COLOR_TABLE_BODY, &yytext[0]);
}

<TABLE_BODY>[\n]{1}	  {
  NEWLINE();
}

<TABLE_BODY>[\n]{2}	  {
  NEWLINE();
  BEGIN(pop_stack());
}


<TABLE_HEAD,TABLE_BODY>[|] {
    TAB();
}

<MULTILINE_CODE>^"```"$ {
  attroff(A_REVERSE);
  BEGIN(pop_stack());
}

<MULTILINE_CODE>.	{
   addch(*yytext);
}

<MULTILINE_CODE>[\n]	{
   NEWLINE();
}

<*>^[-*_]{3}$		{
     horiz_rule();
}

<*>^"```"$  {
  attron(A_REVERSE);
  BEGIN(MULTILINE_CODE);
}

<*>^[#]{5,6}[ \t][^\n\r]+$ {
  char *header, *original;
  header = original = strndup(yytext, yyleng);
  while (*header++ == '#');
  print_header(COLOR_HEADER_SMALL, &header[0]);
  free(original);
}

<*>^[#]{3,4}[ \t][^\n\r]+$ {
  char *header, *original;
  header = original = strndup(yytext, yyleng);
  while (*header++ == '#');
  print_header(COLOR_HEADER_MEDIUM, &header[0]);
  free(original);
}


<*>^[#]{1,2}[ \t][^\n\r]+$   {
  char *header, *original;
  header = original = strndup(yytext, yyleng);
  while (*header++ == '#');
  print_header(COLOR_HEADER_BIG, &header[0]);
  free(original);
}

<*>[`].+[`]		{
    PRINT_BOLD_REVERSE(trim_text(&yytext[1], yyleng - 1));
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

<*>[\n]  	{
    NEWLINE();
}

<*>[ \t]+	{
    WHITESPACE();
}

<*>[^ \t\r\n*_\[|~]+  {
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
   	yyin = file_exists(argv[1]) 
		? fopen(argv[1], "r")
		: NULL;

   if (yyin == NULL)
   	yyerror("Error opening file");
}

int main(int argc, char **argv) {
  initialize_muk_config();
  
  INIT_SCREEN();
  START_COLOR();
  ATTR_ADDCOLOR(COLOR_TABLE_HEADER, COLOR_RED, COLOR_BLACK);
  ATTR_ADDCOLOR(COLOR_TABLE_BODY, COLOR_WHITE, COLOR_BLACK);
  ATTR_ADDCOLOR(COLOR_HEADER_BIG, COLOR_YELLOW, COLOR_BLACK);
  ATTR_ADDCOLOR(COLOR_HEADER_MEDIUM, COLOR_WHITE, COLOR_BLACK);
  ATTR_ADDCOLOR(COLOR_HEADER_SMALL, COLOR_GREEN, COLOR_BLACK);
  
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  load_yyin(argc, argv);
  yylex();
  REFRESH();

  poll_and_navigate();  
  END_SCREEN();

  yyin != stdin 
  	? fclose(yyin)
	: 0;

  return 0;
}
