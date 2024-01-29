#ifndef MUKETTE_H
#define MUKETTE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curses.h>

#ifndef MAX_HYPERLINK
#define MAX_HYPERLINK 4096
#endif

typedef struct Pair Pair;
static struct Pair {
  int y, x;
  char *link;
} hyperlinks[MAX_HYPERLINK] = {{0, 0}};
static size_t hyperlinks_idx = 0;

#define ATTR_ADDSTR(ATTR, TEXT)                                                \
  do {                                                                         \
    attron(ATTR);                                                              \
    addstr(TEXT);                                                              \
    attroff(ATTR);                                                             \
  } while (0)

#define ATTR_COLORPAIR_INIT(PAIRNUM, FORE, BACK)                               \
  do {                                                                         \
    init_pair(PAIRNUM, FORE, BACK);                                            \
  } while (0)

#define PRINT_BOLD(text) ATTR_ADDSTR(A_BOLD, text)
#define PRINT_ITALIC(text) ATTR_ADDSTR(A_ITALIC, text)
#define PRINT_BOLD_ITALIC(text) ATTR_ADDSTR(A_BOLD | A_ITALIC, text)
#define PRINT_UNDERLINE(text) ATTR_ADDSTR(A_UNDERLINE, text)
#define PRINT_BOLD_UNDERLINE(text) ATTR_ADDSTR(A_BOLD | A_UNDERLINE, text)
#define PRINT_ITALIC_UNDERLINE(text) ATTR_ADDSTR(A_ITALIC | A_UNDERLINE, text)
#define PRINT_BOLD_ITALIC_UNDERLINE(text)                                      \
  ATTR_ADDSTR(A_BOLD | A_UNDERLINE | A_ITALIC, text)
#define PRINT_COLOR_PAIR(pnum, text) ATTR_ADDSTR(COLOR_PAIR(pnum), text)

#define

#define NEWLINE() addch('\n')
#define POLL() getch()
#define REFRESH() refresh()
#define GETPOS(y, x) getyx(y, x)

#define KEY_IS_NOT_EXIT(ch) (ch != KEY_EXIT && ch != KEY_F(1))
#define KEY_IS_VERT_ARROW(ch) (ch == KEY_UP || ch == KEY_DOWN)
#define KEY_IS_HORIZ_ARROW(ch) (ch == KEY_LEFT || ch == KEY_RIGHT)
#define KEY_IS_ACTION(ch) (ch == KEY_ENTER || ch == KEY_COMMAND)

#define INIT_SCREEN() initscr()
#define END_SCREEN() endwin()

static inline Pair new_pair(int y, int x, char *link) {
  return (Pair){.y = y, .x = x, .link = link};
}
static inline char *get_link(int y, int x) {
  for (size_t i = 0; i < hyperlinks_idx; i++)
    if ((hyperlinks[i].x > x &&
         hyperinks[i].x < x + strlen(hyperlinks[i].link)) &&
        hyperlinks[i].y == y)
      return hyperlinks[i].link;
  return NULL;
}

static inline void hyperlink_action(void) {
  int y, x;
  getyx(y, x);
  char *link = get_link(y, x);
  if (link == NULL)
    return;
  navigate_to_link(link);
}

static inline void add_hyperlink(char *link) {
  Pair p = new_pair(0, 0, link);
  GETPOS(p.y, p.x);
  hyperlinks[hyperlinks_idx++] = p;
}

static inline void free_hyperlinks(void) {
  while (--hyperlinks_idx)
    free(hyperlinks[hyperlinks_idx].link);
}

static inline void move_up(void) {
  int y, x;
  int ybeg, xbeg;
  getyx(y, x);
  getbegyx(initscr, ybeg, xbeg);
  y = y - ybeg;
  move(y, x);
}

static inline void move_down(void) {
  int y, x;
  int ybeg, xbeg;
  getyx(y, x);
  getbegyx(initscr, ybeg, xbeg);
  y = y + (y - ybeg);
  move(y, x);
}

static inline void move_left(void) {
  int y, x;
  int ybeg, xbeg;
  getyx(y, x);
  getbegyx(initscr, ybeg, xbeg);
  x = xbeg - x;
  move(y, x);
}

static inline void move_right(void) {
  int y, x;
  int ybeg, xbeg;
  getyx(y, x);
  getbegyx(initscr, ybeg, xbeg);
  x = x + (xbeg - x);
  move(y, x);
}

static inline void navigate(void) {
  int ch;
  while ((ch = POLL()), KEY_IS_NOT_EXIT(ch)) {
    if (KEY_IS_VERT_ARROW(ch) || KEY_IS_HORIZ_ARROW(ch)) {
      switch (ch) {
      case KEY_UP:
        inverted ? move_down() : move_up();
        break;
      case KEY_DOWN:
        inverted ? move_up() : move_down();
        break;
      case KEY_LEFT:
        inverted ? move_right() : move_left();
        break;
      case KEY_RIGHT:
        inverted ? move_left() : move_right();
        break;

      default:
        break;
      }
    } else if (KEY_IS_ACTION(ch))
  }
}

#endif
