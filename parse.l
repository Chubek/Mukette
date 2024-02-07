%{
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pager.h"

void yyerror(const char*);

%}

%option noyywrap

%x Line ImagePath Header CodeListing Image ListNumber ListBullet ListItem Hyperlink LinkURL FormatStart Italic Bold BoldItalic InlineCode Strikethrough TableCell TableSep

%%

<*><<EOF>>	   return 0;
<*>[\n]       { BEGIN INITIAL; print_newline(); fflush(yyin); }

^[\\].		   { addch(*(yytext + 1)); }
^[#]{1,6}[ \t]+    { BEGIN Header; turn_on_color(COLOR_PAIR_HEADER); turn_on_bold(); }
^```$          	   { BEGIN CodeListing; turn_on_bold(); }
^"|"[ \t]+         { BEGIN TableCell; addch('\t'); }
^"|-"[|-]+$         { hline('-', yyleng); };
^[1-9]+[ \t]+[.-]   { BEGIN ListItem; 
				char const * const num = 
					strndup(&yytext[0], yyleng - 1); 
				addstr(num); 
				free((void*)num); 
		    }
^[ \t]?[*.-]+[ \t]+ { BEGIN ListItem; turn_on_bold(); 
				char const * const orig =
					strndup(&yytext[0], yyleng - 1);
				char *bullet = strpbrk(orig, "*-.");
				size_t non_bullet = strcspn(bullet, "*-.");
				bullet[non_bullet] = '\0';
				addstr(bullet);
				free((void*)orig);
		    }
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
    .           { addch(*yytext);  }
    [\n]+       { BEGIN INITIAL; addch('\n'); }
}

<Image,Hyperlink>{
  [^\]]+"]("[^ \)]+")" {
  		        char *hl, *orig, *name, *path;
		        hl = orig = strndup(&yytext[0], yyleng);
		        name = strtok(hl, "](");
		        path = strtok(NULL, "](");
			path[strlen(path) - 1] = '\0';
		        print_hyperlink(name, path);
		        free(orig);
		        BEGIN Line;
  		      }
}


<CodeListing>{
    ^.+$        { printw("\t%s", &yytext[0]); }
    "```"       { BEGIN INITIAL; turn_off_bold();     }
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
    "|\n"	{ BEGIN INITIAL;  }
    "|"         { addch('\t'); }
    .           { addch(*yytext);  }
}

<TableSep>[-|] { }


%%


FILE *bind_yyin(int argc, char **argv) {
    bool stdin_is_tty = isatty(STDIN_FILENO);
    bool file_passed = argc > 1;
    
    if (!stdin_is_tty && !file_passed)
    	return stdin;
    else if (file_passed)
        return access(argv[1], F_OK) != -1 ? fopen(argv[1], "r") : NULL;
    else
     	return NULL;
}

int main(int argc, char **argv) {
    yyin = bind_yyin(argc, argv);

    if (yyin == NULL) {
	fprintf(stderr, "Input/Output error occurred\n");
	exit(EXIT_FAILURE);
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    initialize_colors();

    yylex();
    fclose(yyin);
    refresh();

    int link_at = 0;
    for (;;) {
        int c = getch();
        refresh();
        
	if (c == KEY_STAB || c == '\t' || c == KEY_NEXT)
		jump_to_next_link(&link_at);
	else if (c == KEY_EXIT || c == KEY_F(1) || c == 'q' || c == 'Q')
		goto done;
    }

done:
    refresh();
    endwin();

    return 0;
}
