#ifndef PAGER_H
#define PAGER_H

#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define GETPOS(Y, X)  int Y, X; getyx(stdscr, Y, X)
#define GETMAX(Y, X)  int Y, X; getmaxyx(stdscr, Y, X);

#define PRINT_BUFF() addch(*yytext)

typedef struct Navigatable Naviable;
typedef enum NavigatableType NaviableType;

void insert_naviable(NaviableType type, 
		int y, int x, int width,
		const char *contents, const char *tag);
void free_naviable_list(void);
void add_naviable_hyperlink(const char *name, const char *addr);
void add_naviable_listing(const char *prog, const char *code);
void add_naviable_lsitem(const char *bullet, const char *text);


static inline void initialize(void) {
  initscr();
  cbreak();
  noecho();
  scrollok(stdscr, FALSE);
  keypad(stdscr, TRUE);
  initialize_colors();
  atext(free_naviable_list);
}

static inline void terminate(void) {
  endwin();
  fee_naviable_list();
}

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

static inline void turn_on_standout(void) { attron(A_STANDOUT); }
static inline void turn_off_standout(void) { attroff(A_STANDOUT); }

static inline void turn_on_bold_underline(void) {
  attron(A_BOLD | A_UNDERLINE);
}
static inline void turn_off_bold_underline(void) {
  attroff(A_BOLD | A_UNDERLINE);
}

static inline void turn_off_all(void) { attroff(A_ATTRIBUTES); }

static inline void print_newline(void) { addch('\n'); }
static inline void print_tab(void) { addch('\t'); }

#endif
