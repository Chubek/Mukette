#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

typedef enum MarkdownElement MarkdownElement;
typedef struct MarkdownConstruct MarkdownConstruct;
typedef void(FormatFunction)(const char *, void *);

enum MarkdownElement {
  EMPHASIS,
  BOLD,
  STRIKETHROUGH,
  UNDERLINE,
  BLOCKQUOTE,
  CODE_INLINE,
  CODE_LISTING,
  HYPERLINK,
  LIST_ORDERED,
  LIST_UNORDERED,
  TABLE_CELL,
  TABLE_SEP,
  ESCAPED_CODE,
  HEADER1,
  HEADER2,
  HEADER3,
  HEADER4,
  HEADER5,
  HEADER6,
  SIMPLE_TEXT,
  MARKDOWN_ELEMENT,
};

void print_emphasis_text(const char *, void *);
void print_bold_text(const char *, void *);
void print_strikethrough_text(const char *, void *);
void print_underline_text(const char *, void *);
void print_blockquote_text(const char *, void *);
void print_inline_code(const char *, void *);
void print_listing_code(const char *, void *);
void print_hyperlink_formatted(const char *, void *);
void print_list_item_ordered(const char *, void *);
void print_list_item_unordered(const char *, void *);
void print_table_cell(const char *, void *);
void print_table_sep(const char *, void *);
void print_escaped_code(const char *, void *);
void print_header1_text(const char *, void *);
void print_header2_text(const char *, void *);
void print_header3_text(const char *, void *);
void print_header4_text(const char *, void *);
void print_header5_text(const char *, void *);
void print_header6_text(const char *, void *);
void print_simple_text(const char *, void *);

struct MarkdownConstruct {
  const char *delim_begin;
  const char *delim_end;
  const char *alt_delim_begin;
  const char *alt_delim_end;
  const char *trail_begin;
  const char *trail_end;
  const char *prefix_literal;
  const char *prefix_pattern;
  const char suffix_pattern;
  const char *alt_suffix_pattern;
  const char *content_literal;
  const char *content_pattern;
  FormatFunction format_function;
};

const MarkdownConstruct const construct_table[MARKDOWN_ELEMENT] = {
    [EMPHASIS] =
        {
            .delim_begin = "*",
            .alt_delim_begin = "_",
            .delim_end = "*",
            .alt_delim_end = "_",
            .format_function = print_emphasis_text,
        },
    [BOLD] =
        {
            .delim_begin = "**",
            .alt_delim_begin = "__",
            .delim_end = "**",
            .alt_delim_end = "__",
            .format_function = print_bold_text,
        },
    [STRIKETHROUGH] =
        {
            .delim_begin = "~",
            .delim_end = "~",
            .format_function = print_strikethrough_text,
        },
    [UNDERLINE] =
        {
            .delim_begin = "~~",
            .delim_end = "~~",
            .format_function = print_underline_text,
        },
    [BLOCKQUOTE] =
        {
            .prefix_literal = ">",
            .format_function = print_blockquote_text,
        },
    [CODE_INLINE] =
        {
            .delim_begin = "`",
            .delim_end = "`",
            .format_function = print_inline_code,
        },
    [CODE_LISTING] =
        {
            .delim_begin = "```\n",
            .delim_end = "\n```",
            .format_function = print_listing_code,
        },
    [HYPERLINK] =
        {
            .delim_begin = "[",
            .delim_end = "]",
            .trail_delim_begin = "(",
            .trail_delim_end = ")",
            .format_function = print_hyperlink_formatted,
        },
    [LIST_ORDERED] =
        {
            .prefix_pattern = "[0-9]+[-]",
            .format_function = print_list_item_ordered,
        },
    [LIST_UNORDERED] =
        {
            .prefix = "-",
            .format_function = print_list_item_unordered,
        },
    [TABLE_CELL] =
        {
            .delim_begin = "|",
            .delim_end = "|",
            .format_function = print_table_cell,
        },
    [TABLE_SEP] =
        {
            .delim_begin = "|",
            .delim_end = "|",
            .conent_pattern = "[-]+",
            .format_function = print_table_sep,
        },
    [ESCAPED_CODE] =
        {
            .prefix_literal = "\\",
            .format_function = print_escaped_code,
        },
    [HEADER1] =
        {
            .prefix_literal = "# ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header1_text,
        },
    [HEADER2] =
        {
            .prefix_literal = "## ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header2_text,
        },
    [HEADER3] =
        {
            .prefix_literal = "### ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header3_text,
        },
    [HEADER4] =
        {
            .prefix_literal = "#### ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header4_text,
        },
    [HEADER5] =
        {
            .prefix_literal = "##### ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header5_text,
        },
    [HEADER6] =
        {
            .prefix_literal = "###### ",
            suffix_pattern = "[\\n][-]+",
            .alt_suffix_pattern = "[\\n][=]+",
            .format_function = print_header6_text,
        },
    [SIMPLE_TEXT] =
        {
            .content_pattern = ".+",
            .format_function = print_simple_text,
        },
};
