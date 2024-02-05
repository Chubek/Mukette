%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pager.h"

char *last_table_cell = NULL;
char *last_list_point = NULL;
char *last_hyperlink_name = NULL;

char **code = NULL;
size_t num_code = 0;

void yyerror(const char*);

%}

%option noyywrap

%x Line ImagePath Header1 Header2 Header3 Header4 Header5 Header6 CodeListing Image ListNumber ListBullet ListItem Hyperlink LinkURL FormatStart Italic Bold BoldItalic InlineCode Strikethrough TableCell TableSep Error

%%

<<EOF>>		   { return 0; }

^[\\].		   { addch(*(yytext + 1)); }
^#[ \t]+           { BEGIN Header1; turn_on_color(COLOR_PAIR_H1); turn_on_bold(); }
^##[ \t]+          { BEGIN Header2; turn_on_color(COLOR_PAIR_H2); turn_on_bold(); }
^###[ \t]+         { BEGIN Header3; turn_on_color(COLOR_PAIR_H3); turn_on_bold(); }
^####[ \t]+        { BEGIN Header4; turn_on_color(COLOR_PAIR_H4); turn_on_bold(); }
^#####[ \t]+       { BEGIN Header5; turn_on_color(COLOR_PAIR_H5); turn_on_bold(); }
^######[ \t]+      { BEGIN Header6; turn_on_color(COLOR_PAIR_H6); turn_on_bold(); }
^```$          	   { BEGIN CodeListing; }
^"|"                { BEGIN TableCell; }
^[1-9]+[ \t]+"-"   { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng);  }
^"*"[ \t]+|"-"[ \t]+ { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng); }
^[^#`0-9\*\-]	{ BEGIN Line; addch(*yytext); }

<Line>{
    [\\].	{ addch(*(yytext + 1)); }
    "["         { BEGIN Hyperlink; }
    "!["	{ BEGIN Image;     }
    [-~_*`]{1,3}   { 
    
			if (!strncmp(&yytext[0], "***", 3)) {
					turn_on_bold(); turn_on_italic();
					BEGIN(BoldItalic);
				}
			else if (!strncmp(&yytext[0], "___", 3)) {
					turn_on_bold(); turn_on_italic();
					BEGIN(BoldItalic);
				}

			else if (!strncmp(&yytext[0], "**", 2)) {
					turn_on_bold();
					BEGIN(Bold);
				}

			else if (!strncmp(&yytext[0], "__", 2)) {
					turn_on_bold();
					BEGIN(Bold);
				}		

			else if (!strncmp(&yytext[0], "*", 1)) { 
					turn_on_italic();	
					BEGIN(Italic); 
				}
			else if (!strncmp(&yytext[0], "_", 1)) {
					turn_on_italic();
					BEGIN(Italic);
				}
			else if (!strncmp(&yytext[0], "`", 1)) {
					turn_on_bold_underline();
					BEGIN(InlineCode);
				}
			else if (!strncmp(&yytext[0], "~~", 2)) {
					turn_on_underline();
					BEGIN(Strikethrough);
				}
			else addstr(&yytext[0]);
		   }
    [\n]+       { BEGIN INITIAL; print_newline(); }
    .          { addch(*yytext); }
}

<Header1>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL;
    				turn_off_bold();
    			        turn_off_color(COLOR_PAIR_H1); 
    				print_newline(); }
}

<Header2>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL; 
    				turn_off_bold();
				turn_off_color(COLOR_PAIR_H2); 
				print_newline(); }
}

<Header3>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL; 
    				turn_off_bold();
				turn_off_color(COLOR_PAIR_H3); 
				print_newline(); }
}

<Header4>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL; 
    				turn_off_bold();
				turn_off_color(COLOR_PAIR_H4); 
				print_newline(); }
}

<Header5>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL; 
				turn_off_bold();
    				turn_off_color(COLOR_PAIR_H5); 
				print_newline(); }
}

<Header6>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL;
    				turn_off_bold();
    				turn_off_color(COLOR_PAIR_H6); 
				print_newline(); }
}

<ListItem>{
    .+          { print_list_item(last_list_point, &yytext[0]); }
    [\n]+       { BEGIN INITIAL; }
}

<Image,Hyperlink>{
  [^\]]+"]("[^\)]+ {
  		     char *hl, *orig, *name, *path;
		     hl = orig = strndup(&yytext[0], yyleng);
		     name = strtok(hl, "](");
		     path = strtok(NULL, "](");
		     print_hyperlink(name, path);
		     free(orig);
  		  }
  [\n]+		  { BEGIN Error; }
}


<CodeListing>{
    ^.+$        { code = realloc(code, (num_code + 1) * sizeof(char*));
    			code[num_code++] = strndup(&yytext[0], yyleng); }
    "```"       {BEGIN INITIAL; display_code_listing(code, num_code);
	    			free(code); code = NULL; num_code = 0; }
}

<Italic>{
    "*"         { BEGIN Line; turn_off_italic(); }
    "_"         { BEGIN Line; turn_off_italic(); }
    [\n]+       { BEGIN Error; }
    .          { addch(*yytext); }
}

<Bold>{
    "**"        { BEGIN Line; turn_off_bold(); }
    "__"        { BEGIN Line; turn_off_bold(); }
    [\n]+       { BEGIN Error; }
    .          { addch(*yytext); }
}

<InlineCode>{
    "`"         { BEGIN Line; turn_off_bold_underline(); }
    [\n]+       { BEGIN Error; }
    .          { addch(*yytext); }
}

<Strikethrough>{
    "~~"        { BEGIN Line; turn_off_underline(); }
    [\n]+       { BEGIN Error; }
    .          { addch(*yytext); }
}

<BoldItalic>{
    "___"	{ BEGIN Line; turn_off_bold(); turn_off_italic(); }
    "***"	{ BEGIN Line; turn_off_bold(); turn_off_italic(); }
    [\n]+	{ BEGIN Error; }
    .		{ addch(*yytext); }
}

<TableCell>{
    "|\n"	{ add_table_cell(last_table_cell, true); free(last_table_cell);  }
    "|"         { add_table_cell(last_table_cell, false); free(last_table_cell); }
    [\n]+       { BEGIN INITIAL;  }
    .+          { last_table_cell = strndup(&yytext[0], yyleng); }
}

<TableSep>{
    [\n]+       { BEGIN INITIAL;}
}

<Error>{
    .		{ return -1; }
}

%%

static inline bool file_exits(const char *path) {
  return access(path, F_OK) == 0;
}

FILE *bind_yyin(int argc, char **argv) {
   return (argc > 1 && !file_exits(argv[1])) || argc < 2 
   			? stdin 
			: fopen(argv[1], "r");
}

int main(int argc, char **argv) {
    atexit(free_subwins);

    yyin = bind_yyin(argc, argv);

   initialize_screen();
   if (yylex() >= 0) {
    	yyin == stdin ? 0 : fclose(yyin);
	poll_and_navigate();
   } else {
        fprintf(stderr, "Error occurred parsing markdown file");
        exit(EXIT_FAILURE);
    }
    
    
    return 0;
}

