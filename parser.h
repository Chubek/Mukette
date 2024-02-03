#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Stat State;
typedef enum TokenKind TokenKind;
typedef enum Action Action;
typedef struct Token Token;

enum State {
  ANY_STATE,
  LINE_START,
  LINE,
  HEADER1,
  HEADER2,
  HEADER3,
  HEADER4,
  HEADER5,
  HEADER6,
  LIST_NUMBER,
  LIST_BULLET,
  LIST_ITEM,
  IMAGE_ALT,
  IMAGE_PATH,
  LINK_NAME,
  LINK_URL,
  CODE_LISTING,
  FORMAT_START,
  ITALIC,
  BOLD,
  STRIKETHROUGH,
  TABLE_CELL,
  TABLE_SEP,
};

enum Action {
  NO_ACTION,
  ANY_ACTION,
  LOAD_TEXT,
  ACCUMULATE_TEXT,
  LINE_END_ACTION,
  HEADER1_ACTION,
  HEADER2_ACTION,
  HEADER3_ACTION,
  HEADER4_ACTION,
  HEADER5_ACTION,
  HEADER6_ACTION,
  LIST_NUBMER_ACTION,
  LIST_BULLET_ACTION,
  LIST_ITEM_ACTION,
  IMAGE_ALT_ACTION,
  IMAGE_PATH_ACTION,
  LINK_NAME_ACTION,
  LINK_URL_ACTION,
  CODE_LISTING_ACTION,
  ITALIC_ACTION,
  BOLD_ACTION,
  BOLD_ITALIC_ACTION,
  STRIKE_THROUGH_ACTION,
  TABLE_CELL_ACTION,
  TABLE_ROW_ACTION,
};

enum TokenKind {
  SENTINEL,
  CLASS,
  LITERAL,
  EPSILON,
  NEWLINE,
  WHITESPACE,
};

struct Token {
  TokenKind kind;
  const char *value;
};

struct Transition {
  Token token;
  State next_state;
  Action action;
};

#include "transtbl.h"

#endif
