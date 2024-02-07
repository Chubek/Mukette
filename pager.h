#ifndef PAGER_H
#define PAGER_H

#include <curses.h>
#include <unistd.h>

#define MAX_LINK 4096
#define MAX_ADDR 256
#define MAX_CONTENTS 512

#define LINES_OFFS 2
#define COLS_OFFS 6

#define COLOR_PAIR_HEADER 1
#define COLOR_PAIR_CODE 2

#define MAX_CMD MAX_LINK + 512

#ifndef DEFAULT_BROWSER
#define DEFAULT_BROWSER "firefox"
#endif

static struct Hyperlink {
  int y, x, width, height;
  char addr[MAX_ADDR];
} links[MAX_LINK];
size_t curr_link = 0;

static inline void move_up(void) {
  int y, x;
  getyx(stdscr, y, x);
  move(y - 1, x);
}

static inline void move_down(void) {
  int y, x;
  getyx(stdscr, y, x);
  move(y + 1, x);
}

static inline void move_left(void) {
  int y, x;
  getyx(stdscr, y, x);
  move(y, x - 1);
}

static inline void move_right(void) {
  int y, x;
  getyx(stdscr, y, x);
  move(y, x + 1);
}

static inline void initialize_colors(void) {
  init_pair(COLOR_PAIR_HEADER, COLOR_BLACK, COLOR_WHITE);
  init_pair(COLOR_PAIR_CODE, COLOR_WHITE, COLOR_BLACK);
}

static inline void print_newline(void);

static inline void print_header(const char *text) {
  int y, x;
  getyx(stdscr, y, x);
  mvprintw(y, 1, "%s\n", text);
  hline('-', 20);
  print_newline();
}

static inline void turn_on_color(int icolor) { attron(COLOR_PAIR(icolor)); }
static inline void turn_off_color(int icolor) { attroff(COLOR_PAIR(icolor)); }

static inline void turn_on_bold(void) { attron(A_BOLD); }
static inline void turn_off_bold(void) { attroff(A_BOLD); }

static inline void turn_on_italic(void) { attron(A_ITALIC); }
static inline void turn_off_italic(void) { attroff(A_ITALIC); }

static inline void turn_on_underline(void) { attron(A_UNDERLINE); }
static inline void turn_off_underline(void) { attroff(A_UNDERLINE); }

static inline void turn_on_reverse(void) { attron(A_REVERSE); }
static inline void turn_off_reverse(void) { attroff(A_REVERSE); }

static inline void turn_on_bold_underline(void) {
  attron(A_BOLD | A_UNDERLINE);
}
static inline void turn_off_bold_underline(void) {
  attroff(A_BOLD | A_UNDERLINE);
}

static inline void print_text(const char *text) { addstr(text); }
static inline void print_newline(void) { addch('\n'); }

static inline void print_hyperlink(const char *name, const char *addr) {
  int y, ny, x;
  getyx(stdscr, y, x);
  turn_on_underline();
  turn_on_italic();
  links[curr_link++] =
      (struct Hyperlink){.y = y, .x = x, .width = strlen(name), .addr = {0}};
  strncat(&links[curr_link - 1].addr[0], &addr[0], MAX_ADDR);
  addstr(name);
  turn_off_underline();
  turn_off_italic();
  getyx(stdscr, ny, x);
  links[curr_link - 1].height = 1 + ny - y;
}

static inline void navigate_to_addr(const char *link) {
#ifdef __linux__
  char const *app = "xdg-open";
#elif __APPLE__
  char const *app = "open";
#else
  char const *app = DEFAULT_BROWSER;
#endif

  char cmd_full[MAX_CMD] = {0};

  sprintf(&cmd_full[0], "%s %s", &app[0], (char *)link);
  system(&cmd_full[0]);
}

static inline void jump_to_next_link(int *link_curs_at) {
  int curs_deref = *link_curs_at;
  if (curs_deref == curr_link) {
    *link_curs_at = curs_deref = 0;
  } else {
    struct Hyperlink *link = &links[curs_deref];
    *link_curs_at = ++curs_deref;

    mvchgat(link->y, link->x, link->width * link->height, A_BOLD | A_REVERSE,
            COLOR_PAIR(0), NULL);
    refresh();

    int c = getch();
    if (c == KEY_ENTER || c == KEY_COMMAND || c == '\n')
      navigate_to_addr(&link->addr[0]);
    else if (c == KEY_STAB || c == '\t' || c == KEY_NEXT) {   
        mvchgat(link->y, link->x, link->width * link->height, A_NORMAL,
            COLOR_PAIR(0), NULL);
    	refresh();
        jump_to_next_link(link_curs_at);
    }

    mvchgat(link->y, link->x, link->width * link->height, A_NORMAL,
            COLOR_PAIR(0), NULL);
    refresh();
  }
}

#endif
