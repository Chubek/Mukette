%{
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "pager.h"

char *last_table_cell = NULL;
char *last_list_point = NULL;
char *last_hyperlink_name = NULL;

char **code = NULL;
size_t num_code = 0;

void yyerror(const char*);

%}

%option noyywrap

%x Line ImagePath Header CodeListing Image ListNumber ListBullet ListItem Hyperlink LinkURL FormatStart Italic Bold BoldItalic InlineCode Strikethrough TableCell TableSep

%%

	if (feof(yyin)) return 0;


^[\\].		   { addch(*(yytext + 1)); }
^[#]{1,6}[ \t]+    { BEGIN Header; turn_on_color(COLOR_PAIR_HEADER); turn_on_bold(); }
^```$          	   { BEGIN CodeListing; }
^"|"                { BEGIN TableCell; }
^[1-9]+[ \t]+"-"   { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng);  }
1^"*"[ \t]+|"-"[ \t]+ { BEGIN ListItem; last_list_point = strndup(&yytext[0], yyleng); }
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

<Header>{
    .+          { print_header(&yytext[0]); }
    [\n]+       { BEGIN INITIAL;
    				turn_off_bold();
    			        turn_off_color(COLOR_PAIR_HEADER); 
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
		     BEGIN INITIAL;
  		  }
  [\n]+		  { BEGIN INITIAL; }
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
    .          { addch(*yytext); }
}

<Bold>{
    "**"        { BEGIN Line; turn_off_bold(); }
    "__"        { BEGIN Line; turn_off_bold(); }
    .           { addch(*yytext); }
}

<InlineCode>{
    "`"         { BEGIN Line; turn_off_bold_underline(); }
    .           { addch(*yytext); }
}

<Strikethrough>{
    "~~"        { BEGIN Line; turn_off_underline(); }
    .           { addch(*yytext); }
}

<BoldItalic>{
    "___"	{ BEGIN Line; turn_off_bold(); turn_off_italic(); }
    "***"	{ BEGIN Line; turn_off_bold(); turn_off_italic(); }
    .		{ addch(*yytext); }
}

<TableCell>{
    "|\n"	{ add_table_cell(last_table_cell, true); free(last_table_cell);  }
    "|"         { add_table_cell(last_table_cell, false); free(last_table_cell); }
    [\n]+       { BEGIN INITIAL;  }
    .+          { last_table_cell = strndup(&yytext[0], yyleng); }
}

<TableSep>{
    [\n]+       { BEGIN INITIAL; }
}


%%


FILE *bind_yyin(int argc, char **argv) {
    if (argc < 2 && isatty(STDIN_FILENO)) {
        fprintf(stderr, "File error\n");
        exit(EXIT_FAILURE);
    } else if ((argc > 1 && strcmp(argv[1], "-") == 0) || !isatty(STDIN_FILENO)) {
        return stdin;
    } else if (argc > 1 && access(argv[1], F_OK) != -1) {
        return fopen(argv[1], "r");
    } else {
        fprintf(stderr, "You must pass an argument\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    atexit(free_subwins);

    yyin = fopen(argv[1], "r");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    initialize_colors();

    yylex();
    refresh();

    for (;;) {
        int ch = getch();
        refresh();

        switch (ch) {
            case KEY_LEFT:
                move_left();
                break;
            case KEY_RIGHT:
                move_right();
                break;
            case KEY_UP:
                move_up();
                break;
            case KEY_DOWN:
                move_down();
                break;
            case KEY_EXIT:
            case KEY_F(1):
                goto done;
            default:
                break;
        }
    }

done:
    if (yyin != stdin)
        fclose(yyin);

    refresh();
    endwin();

    return 0;
}
