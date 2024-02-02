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

Transition transition_table[] = {
    [ANY_STATE] =
        (Transition){
            .token = (Token){.kind = SENTINEL},
            .next_state = LINE_START,
            .action = LOAD_TEXT,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "#"},
            .next_state = HEADER1,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "# "},
            .next_state = HEADER1,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "## "},
            .next_state = HEADER2,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "### "},
            .next_state = HEADER3,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "#### "},
            .next_state = HEADER4,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "###### "},
            .next_state = HEADER5,
            .action = NO_ACTION,
        },
    [LINE_START] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "###### "},
            .next_state = HEADER6,
            .action = NO_ACTION,
        },

    [HEADER1] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER1,
            .action = ACCUMULATE_TEXT,
        },

    [HEADER2] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER2,
            .action = ACCUMULATE_TEXT,
        },

    [HEADER3] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER3,
            .action = ACCUMULATE_TEXT,
        },

    [HEADER4] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER4,
            .action = ACCUMULATE_TEXT,
        },

    [HEADER5] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER5,
            .action = ACCUMULATE_TEXT,
        },

    [HEADER6] =
        (Transition){
            .token = (Token){.kind = EPSILON, .value = NULL},
            .next_state = HEADER6,
            .action = ACCUMULATE_TEXT,
        },

    [LIST_NUMBER] =
        (Transition){
            .token = (Token){.kind = CLASS, .value = "[1-9]+"},
            .next_state = LIST_NUMBER,
            .action = ACCUMULATE_TEXT,
        },

    [LIST_BULLET] =
        (Transition){
            .token = (Token){.kind = CLASS, .value = "[*-]"},
            .next_state = LIST_BULLET,
            .action = ACCUMULATE_TEXT,
        },

    [LIST_ITEM] =
        (Transition){
            .token = (Token){.kind = NEWLINE},
            .next_state = LINE_START,
            .action = LIST_ITEM_ACTION,
        },

    [IMAGE_ALT] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = "!["},
            .next_state = IMAGE_ALT,
            .action = ACCUMULATE_TEXT,
        },

    [IMAGE_PATH] =
        (Transition){
            .token = (Token){.kind = LITERAL, .value = ")"},
            .next_state = LINE_START,
            .action = IMAGE_PATH_ACTION,
        },

};

void parseInput(const char *input) {
  enum State currentState = AnyState;
  enum State nextState;
  int i = 0;

  while (input[i] != '\0') {
    char token[20];
    int j = 0;

    while (input[i] != '\0' && input[i] != ' ' && input[i] != '\n' &&
           input[i] != '\r') {
      token[j++] = input[i++];
    }
    token[j] = '\0';

    for (int k = 0; k < sizeof(transition_table) / sizeof(transition_table[0]);
         k++) {
      if (currentState == transition_table[k].nextState &&
          (strcmp(token, transition_table[k].input) == 0 ||
           strstr(transition_table[k].input, token) != NULL)) {
        nextState = transition_table[k].output;
        break;
      }
    }

    switch (nextState) {
    case GetNewLine:

      break;
    case AccumulateText:

      break;
    }

    currentState = nextState;
    i++;
  }
}
