#ifndef TRANS_TBL
#define TRANS_TBL

Transition transition_table[] = {
    [ANY_STATE] = (Transition) {
        .token = (Token) {.kind = SENTINEL},
        .next_state = LINE_START,
        .action = LOAD_TEXT,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "#"},
        .next_state = HEADER1,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "# "},
        .next_state = HEADER1,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "## "},
        .next_state = HEADER2,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "### "},
        .next_state = HEADER3,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "#### "},
        .next_state = HEADER4,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "##### "},
        .next_state = HEADER5,
        .action = NO_ACTION,
    },

    [LINE_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "###### "},
        .next_state = HEADER6,
        .action = NO_ACTION,
    },

    [HEADER1] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER1,
        .action = ACCUMULATE_TEXT,
    },

    [HEADER2] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER2,
        .action = ACCUMULATE_TEXT,
    },

    [HEADER3] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER3,
        .action = ACCUMULATE_TEXT,
    },

    [HEADER4] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER4,
        .action = ACCUMULATE_TEXT,
    },

    [HEADER5] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER5,
        .action = ACCUMULATE_TEXT,
    },

    [HEADER6] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = HEADER6,
        .action = ACCUMULATE_TEXT,
    },

    [LIST_NUMBER] = (Transition) {
        .token = (Token) {.kind = CLASS, .value = "[1-9]+"},
        .next_state = LIST_NUMBER,
        .action = ACCUMULATE_TEXT,
    },

    [LIST_NUMBER] = (Transition) {
        .token = (Token) {.kind = WHITESPACE, .value = NULL},
        .next_state = LIST_ITEM,
        .action = LIST_NUMBER_ACTION,
    },

    [LIST_BULLET] = (Transition) {
        .token = (Token) {.kind = CLASS, .value = "*-"},
        .next_state = LIST_BULLET,
        .action = ACCUMULATE_TEXT,
    },

    [LIST_BULLET] = (Transition) {
        .token = (Token) {.kind = WHITESPACE, .value = NULL},
        .next_state = LIST_ITEM,
        .action = LIST_BULLET_ACTION,
    },

    [LIST_ITEM] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = LIST_ITEM,
        .action = ACCUMULATE_TEXT,
    },

    [LIST_ITEM] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = LINE_START,
        .action = LIST_ITEM_ACTION,
    },

    [LIST_ITEM] = (Transition) {
        .token = (Token) {.kind = CLASS, .value = "[*-]"},
        .next_state = LIST_BULLET,
        .action = ACCUMULATE_TEXT,
    },

    [LIST_ITEM] = (Transition) {
        .token = (Token) {.kind = NEWLINE},
        .next_state = LINE_START,
        .action = LIST_ITEM_ACTION,
    },

    [IMAGE_ALT] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "!["},
        .next_state = IMAGE_ALT,
        .action = ACCUMULATE_TEXT,
    },

    [IMAGE_PATH] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = ")"},
        .next_state = LINE_START,
        .action = IMAGE_PATH_ACTION,
    },

    [IMAGE_ALT] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "!["},
        .next_state = IMAGE_ALT,
        .action = ACCUMULATE_TEXT,
    },

    [IMAGE_PATH] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = ")"},
        .next_state = LINE_START,
        .action = IMAGE_PATH_ACTION,
    },

    [LINK_NAME] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = LINK_NAME,
        .action = ACCUMULATE_TEXT,
    },

    [LINK_NAME] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "]("},
        .next_state = LINK_URL,
        .action = LINK_NAME_ACTION,
    },

    [LINK_URL] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = LINK_URL,
        .action = ACCUMULATE_TEXT,
    },

    [LINK_URL] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = ")"},
        .next_state = LINE,
        .action = LINK_URL_ACTION,
    },

    [LINK_URL] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [CODE_LISTING] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "```"},
        .next_state = LINE_START,
        .action = CODE_LISTING_ACTION,
    },

    [CODE_LISTING] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = CODE_LISTING,
        .action = ACCUMULATE_TEXT,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "*"},
        .next_state = ITALIC,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "_"},
        .next_state = ITALIC,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "__"},
        .next_state = BOLD,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "**"},
        .next_state = BOLD,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "`"},
        .next_state = INLINE_CODE,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "~~"},
        .next_state = STRIKETHROUGH,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = LINE,
        .action = NO_ACTION,
    },

    [FORMAT_START] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [ITALIC] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "*"},
        .next_state = LINE,
        .action = ITALIC_ACTION,
    },

    [ITALIC] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "_"},
        .next_state = LINE,
        .action = ITALIC_ACTION,
    },

    [ITALIC] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = ITALIC,
        .action = ACCUMULATE_TEXT,
    },

    [ITALIC] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [BOLD] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "**"},
        .next_state = LINE,
        .action = BOLD_ACTION,
    },

    [BOLD] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "__"},
        .next_state = LINE,
        .action = BOLD_ACTION,
    },

    [BOLD] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = BOLD,
        .action = ACCUMULATE_TEXT,
    },

    [BOLD] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [INLINE_CODE] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "`"},
        .next_state = LINE,
        .action = CODE_ACTION,
    },

    [INLINE_CODE] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = INLINE_CODE,
        .action = ACCUMULATE_TEXT,
    },

    [INLINE_CODE] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [STRIKETHROUGH] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "~~"},
        .next_state = LINE,
        .action = STRIKETHROUGH_ACTION,
    },

    [STRIKETHROUGH] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = STRIKETHROUGH,
        .action = ACCUMULATE_TEXT,
    },

    [STRIKETHROUGH] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = ERROR,
        .action = NO_ACTION,
    },

    [TABLE_CELL] = (Transition) {
        .token = (Token) {.kind = EPSILON, .value = NULL},
        .next_state = TABLE_CELL,
        .action = ACCUMULATE_TEXT,
    },

    [TABLE_CELL] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "|"},
        .next_state = TABLE_CELL,
        .action = TABLE_CELL_ACTION,
    },

    [TABLE_CELL] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = LINE_START,
        .action = TABLE_ROW_ACTION,
    },

    [TABLE_CELL] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "-"},
        .next_state = TABLE_SEP,
        .action = NO_ACTION,
    },

    [TABLE_SEP] = (Transition) {
        .token = (Token) {.kind = NEWLINE, .value = NULL},
        .next_state = LINE_START,
        .action = NO_ACTION,
    },

    [TABLE_SEP] = (Transition) {
        .token = (Token) {.kind = LITERAL, .value = "-"},
        .next_state = TABLE_SEP,
        .action = NO_ACTION,
    },
};

#endif
