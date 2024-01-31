#ifndef MUKETTE_H
#define MUKETTE_H

#include <curses.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef MAX_HYPERLINK
#define MAX_HYPERLINK 4096
#endif

extern char **environ;

typedef struct Pair {
  int y, x;
  const char *link;
} Pair;

static Pair hyperlinks[MAX_HYPERLINK] = {{0, 0}};
static size_t hyperlinks_idx = 0;

struct MuketteConfig {
  const char *muk_browser;
} configuration;

#define ATTR_ADDSTR(ATTR, TEXT) \
  do {                          \
    attron(ATTR);               \
    addstr(TEXT);               \
    attroff(ATTR);              \
  } while (0)

#define ADDR_ADDCOLOR(PAIRNUM, FORE, BACK) 	 \
  do {                                           \
    init_pair(PAIRNUM, FORE, BACK);              \
  } while (0)

#define PRINT_NORMAL(text) ATTR_ADDSTR(A_NORMAL, text)
#define PRINT_BOLD(text) ATTR_ADDSTR(A_BOLD, text)
#define PRINT_ITALIC(text) ATTR_ADDSTR(A_ITALIC, text)
#define PRINT_BOLD_ITALIC(text) ATTR_ADDSTR(A_BOLD | A_ITALIC, text)
#define PRINT_UNDERLINE(text) ATTR_ADDSTR(A_UNDERLINE, text)
#define PRINT_BOLD_UNDERLINE(text) ATTR_ADDSTR(A_BOLD | A_UNDERLINE, text)
#define PRINT_ITALIC_UNDERLINE(text) ATTR_ADDSTR(A_ITALIC | A_UNDERLINE, text)
#define PRINT_BOLD_ITALIC_UNDERLINE(text) \
  ATTR_ADDSTR(A_BOLD | A_UNDERLINE | A_ITALIC, text)
#define PRINT_COLOR(pnum, text) ATTR_ADDSTR(COLOR_PAIR(pnum), text)

#define NEWLINE() addch('\n')
#define WHITESPACE() addch(' ')
#define TAB() addch('\t')
#define POLL() getch()
#define REFRESH() refresh()
#define GETPOS(y, x) getyx(stdscr, y, x)

#define KEY_IS_NOT_EXIT(ch) (ch != KEY_EXIT && ch != KEY_F(1))
#define KEY_IS_VERT_ARROW(ch) (ch == KEY_UP || ch == KEY_DOWN)
#define KEY_IS_HORIZ_ARROW(ch) (ch == KEY_LEFT || ch == KEY_RIGHT)
#define KEY_IS_ACTION(ch) (ch == KEY_ENTER || ch == KEY_COMMAND)
#define KEY_IS_NEXT(ch) (ch == KEY_STAB || ch == KEY_NPAGE)
#define KEY_IS_PREV(ch) (ch == KEY_PPAGE || ch == KEY_BTAB)

#define INIT_SCREEN() initscr()
#define END_SCREEN() endwin()

static inline Pair new_pair(int y, int x, const char *link) {
  return (Pair){.y = y, .x = x, .link = link};
}

static inline const char *get_link(int y, int x) {
  for (size_t i = 0; i < hyperlinks_idx; i++)
    if ((hyperlinks[i].x > x &&
         hyperlinks[i].x < x + strlen(hyperlinks[i].link)) &&
        hyperlinks[i].y == y)
      return hyperlinks[i].link;
  return NULL;
}

static inline void navigate_to_link(const char *link) {
  const char *browser = configuration.muk_browser;
  const char *args[] = {link, NULL};

  pid_t child_pid;
  int status;

  if (posix_spawnp(&child_pid, browser, NULL, NULL, (char *const *)args,
                   environ) == 0) {
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      printf("Browser exited with status: %d\n", exit_status);
    } else {
      printf("Browser process did not exit successfully\n");
    }
  } else {
    perror("Failed to spawn browser process");
  }
}

static inline void hyperlink_action(void) {
  int y, x;
  GETPOS(y, x);
  const char *link = get_link(y, x);
  if (link == NULL)
    return;
  navigate_to_link(link);
}

static inline void add_hyperlink(const char *link) {
  Pair p = new_pair(0, 0, link);
  GETPOS(p.y, p.x);
  hyperlinks[hyperlinks_idx++] = p;
}

static inline void free_hyperlinks(void) {
  while (hyperlinks_idx > 0)
    free((char *)hyperlinks[--hyperlinks_idx].link);
}

static inline void move_up(void) {
  int y, x;
  GETPOS(y, x);
  move(y - 1, x);
}

static inline void move_down(void) {
  int y, x;
  GETPOS(y, x);
  move(y + 1, x);
}

static inline void move_left(void) {
  int y, x;
  GETPOS(y, x);
  move(y, x - 1);
}

static inline void move_right(void) {
  int y, x;
  GETPOS(y, x);
  move(y, x + 1);
}

static inline void jump_next_link(void) {
  int y, x;
  GETPOS(y, x);

  for (size_t i = 0; i < hyperlinks_idx; i++) {
    if (hyperlinks[i].y > y || (hyperlinks[i].y == y && hyperlinks[i].x > x)) {
      move(hyperlinks[i].y, hyperlinks[i].x);
      return;
    }
  }

  if (hyperlinks_idx > 0) {
    move(hyperlinks[0].y, hyperlinks[0].x);
  }
}

static inline void jump_prev_link(void) {
  int y, x;
  GETPOS(y, x);

  for (size_t i = hyperlinks_idx; i > 0; i--) {
    if (hyperlinks[i - 1].y < y ||
        (hyperlinks[i - 1].y == y && hyperlinks[i - 1].x < x)) {
      move(hyperlinks[i - 1].y, hyperlinks[i - 1].x);
      return;
    }
  }

  if (hyperlinks_idx > 0) {
    move(hyperlinks[hyperlinks_idx - 1].y, hyperlinks[hyperlinks_idx - 1].x);
  }
}

static inline void poll_and_navigate(void) {
  int ch;
  while ((ch = POLL()), KEY_IS_NOT_EXIT(ch)) {
    if (KEY_IS_VERT_ARROW(ch) || KEY_IS_HORIZ_ARROW(ch)) {
      switch (ch) {
      case KEY_UP:
        move_up();
        break;
      case KEY_DOWN:
        move_down();
        break;
      case KEY_LEFT:
        move_left();
        break;
      case KEY_RIGHT:
        move_right();
        break;

      default:
        break;
      }
    } else if (KEY_IS_ACTION(ch))
      hyperlink_action();
    else if (KEY_IS_NEXT(ch))
      jump_next_link();
    else if (KEY_IS_PREV(ch))
      jump_prev_link();
    else
       return;
  }
}

static inline void initialize_muk_config(void) {
  const char *env_var = NULL;

  if ((env_var = getenv("MUK_BROWSER")) == NULL)
    configuration.muk_browser = "w3m";
  else
    configuration.muk_browser = env_var;
}

#endif

