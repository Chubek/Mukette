#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mukette.h"

/* The following code is a basic markdown parser. It uses C's standard string functions
 * to parse Standard Markdown. 
 * The PEG grammar for Markdown can be summarized to:
 * ================
 * markdown <- markdown-element+ EOF
 * markdown-element <- formatted-text / table / list / any-text / newline
 * table <- table-item+ newlinetable-sep newlinetable-body newline
 * table-body <- table-item+
 * table-sep <- "|" ( !"|") + "|"
 * table-item <- "|" formated-text "|"
 * list <- (ordered-list-item / unordered-list-item)+
 * ordered-list-item <- [0-9]+ "-" any-text "\n"
 * unordered-list-item <- [*-.] "-" any-text "\n"
 * formatted-text <- hyperlink / strikethrough / bold-italic / bold / italic
 * hyperlink <- "[" hyperlink-name "](" hyperlink-url ")"
 * hyperlink-url <- [(http|https)://]?(www.)? (!".") "." [a-zA-Z]+
 * hyperlink-name <- any-text
 * strikethrough <- "~~" any-text "~~"
 * bold-italic <- [*_]{1} any-text
 * bold <- [*_]{2} .* [*_]{2}
 * italtic-text <- [*_]{1} any-text [*_]{1}
 * any-text <- (!newline)+
 * newline <- [\r\n]{1}
 * EOF <- !.
 * ================
*/


/* Checkes whether the prefix of `string` is `prefix` */
static inline bool string_prefix_is(char *string, const char *prefix) {
  return strspn(string, prefix) == strlen(prefix);
}


/* Checks whether the suffix of `string` is `prefix` */
static inline bool string_suffix_is(char *string, const char *suffix) {
  return (strstr(string, suffix) + 1) == '\0';
}


/* Checks whether `string` has in it `contains */
static inline bool string_contains(char *string, const char *contains) {
  return strstr(string, suffix) != NULL;
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
  return strcspn(string, "*_~`|[");
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

