#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mukette.h"

#ifdef __unix__
#define SYSTEM_NEWLINE "\n"
#else
#define SYSTEM_NEWLINE "\r\n"
#endif


/* The following code is a basic markdown parser. It uses C's standard string functions
 * to parse Standard Markdown. 
 * The PEG grammar for Markdown can be summarized to:
===========================================================================
markdown <- markdown-element+ EOF
markdown-element <- formatted-text / table / list / any-text / newline
table <- table-item+ newlinetable-sep newlinetable-body newline
table-body <- table-item+
table-sep <- "|" ( !"|") + "|"
table-item <- "|" formated-text "|"
list <- (ordered-list-item / unordered-list-item)+
ordered-list-item <- [0-9]+ "-" any-text "\n"
unordered-list-item <- [*-.] "-" any-text "\n"
formatted-text <- hyperlink / strikethrough / bold-italic / bold / italic
hyperlink <- "[" hyperlink-name "](" hyperlink-url ")"
hyperlink-url <- [(http|https)://]?(www.)? (!".") "." [a-zA-Z]+
hyperlink-name <- any-text
strikethrough <- "~~" any-text "~~"
bold-italic <- [*_]{1} any-text
bold <- [*_]{2} .* [*_]{2}
italtic-text <- [*_]{1} any-text [*_]{1}
any-text <- (!newline)+
newline <- [\r\n]{1}
EOF <- !.
============================================================================
*/


/* Checkes whether the prefix of `string` is `prefix` */
static inline bool string_prefix_is(const char *string, const char *prefix) {
  return strspn(string, prefix) == strlen(prefix);
}


/* Checks whether the suffix of `string` is `prefix` */
static inline bool string_suffix_is(const char *string, const char *suffix) {
  return *(strstr(string, suffix) + 1) == '\0';
}


/* Checks whether `string` has in it `contains */
static inline bool string_contains(const char *string, const char *contains) {
  return strstr(string, contains) != NULL;
}

/* Returns a pointer to the first whitespace (space or tab) character */
static inline char *pointer_to_first_whitespace(const char *string) {
  return strpbrk(string, " \t");
}

/* Returns a pointer to the first newline (Windows or Unix) character */
static inline char *pointer_to_first_newline(const char *string) {
  return strpbrk(string, "\r\n");
}

/* Returns a pointer to the first occurance of format characters (Bold, Italic, etc) */
static inline char *pointer_to_first_format_delim(const char *string) {
  return strpbrk(string, "*_~`|[");
}

/* Returns a pointer to the first non-format character (see above) */
static inline char *pointer_to_first_non_format_delim(const char *string) {
  return strstr(string, "*_~`|[");
}

/* Returns a pointer to the end of hyperlink name */
static inline char *pointer_to_hyperlink_name_end(const char *string) {
  return strstr(string, "](");
}

/* Returns a pointer to the end of a hyperlink URL */
static inline char *pointer_to_hyperlink_url_end(const char *string) {
  return strchr(string, ')');
}

/* Returns a pointer to the end of a code listing block */
static inline char *pointer_to_codeblock_end(const char *string) {
  return strstr(string, "```");
}

/* Returns a pointer to the first bullet point for an unordered list */
static inline char *pointer_to_first_bullet_point(const char *string) {
  return strpbrk(string, "*-.");
}

/* Returns a pointer to the first bullet point for an ordered list (a number or a digit) */
static inline char *pointer_to_first_digit(const char *string) {
  return strpbrk(string, "0123456789");
}


/* The parsing table for Markdown
 * Notations:
 * Single characters are enclosed between ' and '
 * Strings are enclosed between " and "
 * Multiple possible characters are enclosed between [ and ]
 * 'space' means whitespace (tab or space)
 * 'epsilon' means any text that does not contain newline
 * 'newline' means \r\n in Windows and \n in Linux
===============================================================================
| State           | Input            | Next State       | Output              |
|-----------------|------------------|------------------|---------------------|
| LineStart       | '#'              | Header1          | - 	  	      |
| LineStart       | "##"             | Header2          | -         	      |
| LineStart       | "###"            | Header3          | -         	      |
| LineStart       | "####"           | Header4          | -         	      |
| LineStart       | "#####"          | Header5          | -         	      |
| LineStart       | "######"         | Header6          | -         	      |
| LineStart       | "```"            | CodeListing      | -                   |
| LineStart       | "!["	     | ImageAlt	 	| -                   |
| LineStart       | [1-9]+ space -   | ListNumber       | -                   |
| LineStart       | [*-] space       | ListBullet       | -                   |
| LineStart       | '|'              | TableCell        | -                   |
| LineStart       | epsilon  	     | Line   		| -  		      |
| Line            | '['              | LinkName         | -                   |
| Line            | [-~_*`]	     | FormatStart      | -                   |
| Line            | epsilon          | Line             | AccumulateText      |
| Line            | newline          | LineStart        | LineEndAction       |
| Header1         | epsilon    	     | Header1          | AccumulateText      |
| Header2         | epsilon    	     | Header2          | AccumulateText      |
| Header3         | epsilon    	     | Header3          | AccumulateText      |
| Header4         | epsilon    	     | Header4          | AccumulateText      |
| Header5         | epsilon    	     | Header5          | AccumulateText      |
| Header6         | epsilon    	     | Header6          | AccumulateText      |
| Header1         | newline          | LineStart        | Header1Action       |
| Header2         | newline          | LineStart        | Header2Action       |
| Header3         | newline          | LineStart        | Header3Action       |
| Header4         | newline          | LineStart        | Header4Action       |
| Header5         | newline          | LineStart        | Header5Action       |
| Header6         | newline          | LineStart        | Header6Action       |
| ListNumber      | [0-9]            | ListNumber       | AccumulateText      |
| ListNumber      | space            | ListItem         | ListNumberAction    |
| ListBullet      | [*-]             | ListBullet       | AccumulateText      |
| ListBullet      | space            | ListItem         | ListBulletAction    |
| ListItem        | epsilon          | ListItem         | AccumulateText      |
| ListItem        | newline          | LineStart        | ListItemAction      |
| ImageAlt        | epsilon          | ImageAlt         | AccumulateText      |
| ImageAlt        | "]("	     | ImagePath        | ImageAltAction      |
| ImagePath       | epsilon 	     | ImagePath        | AccumulateText      |
| ImagePath       | ')'              | LineStart        | ImagePathActio      |
| ImagePath       | newline          | Error		| -                   |
| LinkName        | epsilon    	     | LinkName         | AccumulateText      |
| LinkName        | "]("             | LinkURL          | LinkNameAction      |
| LinkURL         | epsilon    	     | LinkURL          | AccumulateText      |
| LinkURL         | ')'              | Line             | LinkURLAction       |
| LinkURL         | newline          | Error            | -                   |
| CodeListing     | epsilon/newline  | CodeListing      | AccumulateText      |
| CodeListing     | "```"	     | LineStart        | CodeListingAction   |
| FormatStart     | '*'		     | Italic           | -                   | 
| FormatStart     | '_'              | Italic           | -                   |
| FormatStart     | "__"             | Bold             | -                   |
| FormatStart     | "**"             | Bold             | -                   |
| FormatStart     | '`'              | Code             | -                   |
| FormatStart     | "~~"             | Strikethrough    | -                   |
| FormatStart     | epsilon          | Line             | -                   |
| FormatStart     | newline          | Error            | -                   |
| Italic          | '*'              | Line             | ItalicAction        |
| Italic          | '_'              | Line             | ItalicAction        |
| Italic          | epsilon          | Italic           | AccumulateText      |
| Italic          | newline          | Error            | -                   |
| Bold            | "**"             | Line             | BoldAction          |
| Bold            | "__"             | Line             | BoldAction          |
| Bold            | epsilon          | Line             | AccumulateText      |
| Bold            | newline          | Error            | -                   |
| Code            | '`'              | Line             | CodeAction          |
| Code            | epsilon          | Line             | AccumulateText      |
| Code            | newline          | Error            | -                   |
| Strikethrough   | "~~"             | Line             | StrikethroughAction |
| Strikethrough   | epsilon          | StrikeThrough    | AccumulateText      |
| Strikethrough   | newline          | Error            | -                   |
| TableCell       | epsilon          | TableCell        | AccumulateText      |
| TableCell       | '|'              | TableCell        | TableCellAction     |
| TableCell       | newline          | LineStart        | TableRowAction      |
| TableCell       | '-'              | TableSep         | -                   |
| TableSep        | newline          | LineStart        | -                   |
===============================================================================
*/
