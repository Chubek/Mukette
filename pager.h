#ifndef PAGER_H
#define PAGER_H

#include <curses.h>

#define COLOR_PAIR_H1 1
#define COLOR_PAIR_H2 2
#define COLOR_PAIR_H3 3
#define COLOR_PAIR_H4 4
#define COLOR_PAIR_H5 5
#define COLOR_PAIR_h6 6


static inline void init_color_h1(void) {
  init_pair(COLOR_PAIR_H1, COLOR_BLUE, COLOR_BLACK);
}

static inline void init_color_h2(void) {
  init_pair(COLOR_PAIR_H2, COLOR_WHITE, COLOR_BLACK);
}

static inline void init_color_h3(void) {
  init_pair(COLOR_PAIR_H3, COLOR_BLACK, COLOR_WHITE);
}

static inline void init_color_h4(void) {
  init_pair(COLOR_PAIR_H4, COLOR_CYAN, COLOR_WHITE);
}

static inline void init_color_h5(void) {
  init_pair(COLOR_PAIR_H5, COLOR_RED, COLOR_BLACK);
}

static inline void init_color_h6(void) {
  init_pair(COLOR_PAIR_H6, COLOR_MAGENTA, COLOR_BLACK);
}

static inline void print_horiz_line(void) {
  mvhline(4, 1, ASC_HLINE, 20);
}

static inline void print_header(const char *text) {
  mvprintw(1, 1, text);
}

static inline void turn_on_color(int icolor) {
   attron(COLOR_PAIR(icolor));
}

static inline void turn_off_color(int icolor) {
   attroff(COLOR_PAIR(icolor));
}

static inline void turn_on_bold(void) {
  attron(A_BOLD);
}

static inline void turn_off_bold(void) {
  attroff(A_BOLD);
}

static inline void turn_on_italic(void) {
  attron(A_ITALIC);
}

static inline void turn_off_italic(void) {
  attroff(A_ITALIC);
}

static inline void turn_on_underline(void) {
  attron(A_UNDERLINE);
}

static inline void turn_off_underline(void) {
  attroff(A_UNDERLINE);
}

static inline void turn_on_reverse(void) {
  attron(A_REVERSE);
}

static inline void turn_off_reverse(void) {
  attroff(A_REVERSE);
}

static inline void print_text(const char *text) {
  printw("%s", text);
}

static inline void print_newline(void) {
  addchr('\n');
}














#endif
