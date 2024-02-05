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

%}

%option noyywrap

%x LineStart Line ImagePath Header1 Header2 Header3 Header4 Header5 Header6 CodeListing ImageAlt ListNumber ListBullet ListItem LinkName LinkURL FormatStart Italic Bold InlineCode Strikethrough TableCell TableSep Error

%%

^\.                { BEGIN LineStart;  }
^#[ \t]+           { BEGIN Header1; turn_on_color(COLOR_PAIR_H1); }
^##[ \t]+          { BEGIN Header2; turn_on_color(COLOR_PAIR_H2); }
^###[ \t]+         { BEGIN Header3; turn_on_color(COLOR_PAIR_H3); }
^####[ \t]+        { BEGIN Header4; turn_on_color(COLOR_PAIR_H4); }
^#####[ \t]+       { BEGIN Header5; turn_on_color(COLOR_PAIR_H5); }
^######[ \t]+      { BEGIN Header6; turn_on_color(COLOR_PAIR_H6);  }
^```$          	   { BEGIN CodeListing; }
^!\[               { BEGIN ImageAlt; }
^[1-9]+[ \t]+"-"   { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng);  }
^[*-]              { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng); }
^\|                { BEGIN TableCell; }
.              	   { BEGIN Line; }
\n	       	   { print_newline(); }

<Line>{
    "["         { BEGIN LinkName; }
    [-~_*`]     { BEGIN FormatStart; }
    [\n]+       { BEGIN LineStart; print_newline(); }
    .+          { print_text(&yytext[0]); }
}

<Header1>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H1); print_newline(); }
}

<Header2>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H2); print_newline(); }
}

<Header3>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H3); print_newline(); }
}

<Header4>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H4); print_newline(); }
}

<Header5>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H5); print_newline(); }
}

<Header6>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN LineStart; turn_off_color(COLOR_PAIR_H6); print_newline(); }
}

<ListItem>{
    .+          { print_list_item(last_list_point, &yytext[0]); }
    [\n]+       { BEGIN LineStart; }
}

<ImageAlt>{
    "]("        { BEGIN ImagePath; }
    .+          { last_hyperlink_name = strndup(&yytext[0], yyleng); }
}

<ImagePath>{
    ")"         { BEGIN LineStart; }
    .+          { const char *addr = strndup(&yytext[0], yyleng);
    				print_hyperlink(last_hyperlink_name, addr);
				free(addr); free(last_hyperlink_name); }
    [\n]+       { BEGIN Error; }
}

<LinkName>{
    "]("        { BEGIN LinkURL; }
    .+          { last_hyperlink_name = strndup(&yytext[0], yyleng); }
}

<LinkURL>{
    ")"        { BEGIN Line; }
   .+          { const char *addr = strndup(&yytext[0], yyleng);
   				print_hyperlink(last_hyperlink_name, addr);
				free(addr); free(last_hyperlink_name);  }
   [\n]+       { BEGIN Error; }
}

<CodeListing>{
    ^.+$        { code = realloc(code, (num_code + 1) * sizeof(char*));
    			code[num_code++] = strndup(&yytext[0], yyleng); }
    "```"       {BEGIN LineStart; display_code_listing(code, num_code);
	    			free(code); code = NULL; num_code = 0; }
}

<FormatStart>{
    "*"         { BEGIN Italic; turn_on_italic(); }
    "_"         { BEGIN Italic; turn_on_italic(); }
    "__"        { BEGIN Bold; turn_on_bold();  }
    "**"        { BEGIN Bold; turn_on_bold(); }
    "`"         { BEGIN InlineCode; turn_on_bold_underline(); }
    "~~"        { BEGIN Strikethrough; turn_on_underline(); }
    .+          { print_text(&yytext[0]);}
    [\n]+       { BEGIN Error; printf("-\n"); }
}

<Italic>{
    "*"         { BEGIN Line; turn_off_italic(); }
    "_"         { BEGIN Line; turn_off_italic(); }
    [\n]+       { BEGIN Error; }
    .+          { print_text(&yytext[0]); }
}

<Bold>{
    "**"        { BEGIN Line; turn_off_bold(); }
    "__"        { BEGIN Line; turn_off_bold(); }
    [\n]+       { BEGIN Error; }
    .+          { print_text(&yytext[0]); }
}

<InlineCode>{
    "`"         { BEGIN Line; turn_off_bold_underline(); }
    [\n]+       { BEGIN Error; }
    .+          { print_text(&yytext[0]); }
}

<Strikethrough>{
    "~~"        { BEGIN Line; turn_off_underline(); }
    [\n]+       { BEGIN Error; }
    .+          { print_text(&yytext[0]); }
}

<TableCell>{
    "|\n"	{ add_table_cell(last_table_cell, true); free(last_table_cell);  }
    "|"         { add_table_cell(last_table_cell, false); free(last_table_cell); }
    [\n]+       { BEGIN LineStart;  }
    .+          { last_table_cell = strndup(&yytext[0], yyleng); }
}

<TableSep>{
    [\n]+       { BEGIN LineStart;}
}

<Error>{
    .		{ yyerror("Parse error ocurred"); }
}

%%

void yyerror(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

int main() {
    atexit(free_subwins);

    initscr();
    yylex();
    poll_and_navigate();
    
    return 0;
}

