%{
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "pager.h"

extern int yywrap(void);
Navible *navs = NULL;

char *last_tag = NULL;

static inline char *alloc_sh(void) { 
   char *b = calloc(1, 3);
   strncat(&b[0], "sh", 2);
   return b;
}

%}

%x FmtLine ImagePath Header CodeListing Image Hyperlink LinkURL FormatStart Italic Bold BoldItalic InlineCode Strikethrough TableCell TableSep

%%

<*><<EOF>>	   return 0;
<Italic,Bold,BoldItalic,Strikethrough,InlineCode>[\n] { print_newline(); 
						turn_off_all(); BEGIN INITIAL; }

[\n]		   { print_newline(); }
^[\\]{1}.	   { addch(*(yytext + 1)); }
^[#]{1,6}[ \t]+    { turn_on_standout(); print_tab(); addch('*'); BEGIN Header; }
^"|"[ \t]+         { BEGIN TableCell; print_tab(); }
^"|-"[|-]+$         { print_newline();
		      print_tab();
				for (int i = 0; i < yyleng; i++) { addch('-'); }
					print_newline(); };

^[`]{3}$	     { last_tag = alloc_sh();
				BEGIN CodeListing; }
^[`]{3}.+$	     { last_tag = strndup(&yytext[3], yyleng - 3);
				BEGIN CodeListing; }
$[^*~`_-]+$	     { addstr(&yytext[0]); print_newline(); }
^.		     { BEGIN FmtLine; PRINT_BUFF(); }



<FmtLine>{
    [\\]{1}.	{ addch(*(yytext + 1)); }
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
    .          { PRINT_BUFF(); }
    [\n]+      { addstr(&yytext[0]); BEGIN INITIAL; }
}

<Header>{
    .          { PRINT_BUFF(); }
    [\n]       { BEGIN INITIAL;
    				turn_off_standout();
    				print_newline(); }
}

<Image,Hyperlink>{
  [^\]]+"]("[^ \)]+")" {
  		        char *hl, *orig, *name, *path;
		        hl = orig = strndup(&yytext[0], yyleng);
		        name = strtok(hl, "](");
		        path = strtok(NULL, "](");
			path[strlen(path) - 1] = '\0';
			turn_on_bold_underline();
			addstr(name);
			turn_off_bold_underline();
			add_naviable_hyperlink(&navs, name, path);
		        free(orig);
		        BEGIN FmtLine;
  		      }
}


<CodeListing>{
   (^.+$)+^[`]{3}$  { addstr(&yytext[0]); 
   			add_naviable_listing(&navs, last_tag, &yytext[0]); 
			free(last_tag);
			BEGIN INITIAL;  }
   [\n]		 { print_newline(); }
}

<Italic>{
    "*"         { BEGIN FmtLine; turn_off_italic(); }
    "_"         { BEGIN FmtLine; turn_off_italic(); }
    .           { PRINT_BUFF(); }
}

<Bold>{
    "**"        { BEGIN FmtLine; turn_off_bold(); }
    "__"        { BEGIN FmtLine; turn_off_bold(); }
    .           { PRINT_BUFF(); }
}

<InlineCode>{
    "`"         { BEGIN FmtLine; turn_off_bold_underline(); }
    .           { PRINT_BUFF(); }
}

<Strikethrough>{
    "~~"        { BEGIN FmtLine; turn_off_underline(); }
    .           { PRINT_BUFF(); }
}

<BoldItalic>{
    "___"	{ BEGIN FmtLine; turn_off_bold(); turn_off_italic(); }
    "***"	{ BEGIN FmtLine; turn_off_bold(); turn_off_italic(); }
    .		{ PRINT_BUFF(); }
}

<TableCell>{
    "|\n"	{ BEGIN INITIAL;  }
    "|"         { print_tab(); }
    .           { PRINT_BUFF();  }
}

<TableSep>[-|] { }


%%
