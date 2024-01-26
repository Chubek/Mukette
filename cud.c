#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <ncurses.h>

typedef enum MarkdownElement MarkdownElement;
typedef struct MarkdownConstruct MarkdownConstruct;






enum MarkdownElement
{
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

struct MarkdownConstruct
{
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
};

const MarkdownConstruct const construct_table[MARKDOWN_ELEMENT] =
{
   [EMPHASIS] = { .delim_begin = "*", .alt_delim_begin = "_",
	   	  .delim_end = "*", .alt_delim_end = "_", },
   [BOLD] = { .delim_begin = "**", .alt_delim_begin = "__",
	   	  .delim_end = "**", .alt_delim_end = "__", },
   [STRIKETHROUGH] = { .delim_begin = "~",  .delim_end = "~", },
   [UNDERLINE] = { .delim_begin = "~~",  .delim_end = "~~", },
   [BLOCKQUOTE] = { .prefix_literal = ">", },
   [CODE_INLINE] = { .delim_begin = "`", .delim_end = "`", },
   [CODE_LISTING] = { .delim_begin = "```\n", .delim_end = "\n```", },
   [HYPERLINK] = { .delim_begin = "[", .delim_end = "]", 
    		.trail_delim_begin = "(", .trail_delim_end = ")", },
   [LIST_ORDERED] = { .prefix_pattern = "[0-9]+[-]", },
   [LIST_UNORDERED] = { .prefix = "-", },
   [TABLE_CELL] = { .delim_begin = "|", .delim_end = "|", },
   [TABLE_SEP] = { .delim_begin = "|", .delim_end = "|", 
	   	 .conent_pattern = "[-]+", },
   [ESCAPED_CODE] = { .prefix_literal = "\\", },
   [HEADER1] = { .prefix_literal = "# ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [HEADER2] = { .prefix_literal = "## ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [HEADER3] = { .prefix_literal = "### ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [HEADER4] = { .prefix_literal = "#### ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [HEADER5] = { .prefix_literal = "##### ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [HEADER6] = { .prefix_literal = "###### ", suffix_pattern = "[\\n][-]+",
   		.alt_suffix_pattern = "[\\n][=]+", },
   [SIMPLE_TEXT] = { .content_pattern = ".+", },
};




