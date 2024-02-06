#ifndef PAGER_H
#define PAGER_H

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <spawn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <setjmp.h>

#define MAX_SUBWIN 512
#define MAX_LINK 4096
#define MAX_ADDR 256
#define MAX_CONTENTS 512
#define MAX_CELL 1024

#define LINES_OFFS 2
#define COLS_OFFS 6

#define COLOR_PAIR_HEADER 1
#define COLOR_PAIR_CODE 2

#define KEY_IS_NOT_EXIT(ch) (ch != KEY_EXIT && ch != KEY_F(1))
#define KEY_IS_VERT_ARROW(ch) (ch == KEY_UP || ch == KEY_DOWN)
#define KEY_IS_HORIZ_ARROW(ch) (ch == KEY_LEFT || ch == KEY_RIGHT)
#define KEY_IS_ACTION(ch) (ch == KEY_ENTER || ch == KEY_COMMAND)

static struct Hyperlink {
  int y, x, len;
  char addr[MAX_ADDR];
} links[MAX_LINK];
size_t curr_link = 0;

static struct TableCell {
  int row, col;
  bool breaks;
  char contents[MAX_CELL];
} cells[MAX_CELL];
size_t curr_cell = 0;

WINDOW *subwins[MAX_SUBWIN];
size_t curr_subwin = 0;

jmp_buf jbuf = {0};


void navigate_to_addr(const char *link) {
    char *const argv[] = { "xdg-open", (char*)link, NULL };
    posix_spawn_file_actions_t file_actions;

    posix_spawn_file_actions_init(&file_actions);

    pid_t pid;
    if (posix_spawnp(&pid, "xdg-open", &file_actions, NULL, argv, NULL) == 0) {
        waitpid(pid, NULL, 0);
    } else {
        perror("posix_spawnp");
    }

    posix_spawn_file_actions_destroy(&file_actions);
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

static inline void print_list_item(const char *point, const char *item) {
  int y, x;
  getyx(stdscr, y, x);
  mvprintw(y, 1, "%s - %s\n", point, item);
}

static inline void print_hyperlink(const char *name, const char *addr) {
  int y, x;
  getyx(stdscr, y, x);
  links[curr_link++] =
      (struct Hyperlink){.y = y, .x = x, .len = strlen(name), .addr = {0}};
  strncat(&links[curr_link - 1].addr[0], &addr[0], MAX_ADDR);
  addstr(name);
}

bool is_approaching(int target, int value, int tolerance) {
  return (value >= target - tolerance) && (value <= target + tolerance);
}

static inline void navigate_hyperlinks(void) {
  struct Hyperlink *link;
  size_t i = 0;
  int y, x;
  getyx(stdscr, y, x);
  while (i < curr_link) {
    link = &links[i];
    if (link->y == y && is_approaching(link->x, x, link->len)) {
      turn_on_reverse();
      mvprintw(link->y, link->x, "%s", &link->addr[0]);
      turn_off_reverse();
      return;
    }
    i++;
  }
}

static inline void display_code_listing(char **code, int lines) {
  int y, x;
  int my, mx;
  getyx(stdscr, y, x);
  getmaxyx(stdscr, my, mx);

  WINDOW *code_win = subwins[curr_subwin++] =
      newwin(lines + LINES_OFFS, mx + COLS_OFFS, y, x);
  scrollok(code_win, false);

  wattron(code_win, COLOR_PAIR(COLOR_PAIR_CODE));

  for (int yy = 0; yy < lines; yy++) {
    mvwprintw(code_win, yy, 1, "%s", code[yy]);
  }
}

static inline void display_table(void) {
  int y, x;
  getyx(stdscr, y, x);

  int rows = cells[curr_cell].row;
  int cols = cells[curr_cell].col;

  WINDOW *table_win = subwins[curr_subwin++] =
      newwin(rows + LINES_OFFS, cols + COLS_OFFS, y, x);
  scrollok(table_win, false);

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      mvwprintw(table_win, r, c * MAX_CONTENTS + 1, "| %s |",
                &cells[r * cols + c].contents[0]);
      if (cells[r * cols + c].breaks)
        print_newline();
    }
  }
}

static inline void add_table_cell(const char *contents, bool breaks) {
  cells[curr_cell] = (struct TableCell){.row = cells[curr_cell].row,
                                        .col = cells[curr_cell].col,
                                        .breaks = breaks,
                                        .contents = {0}};
  strncat(cells[curr_cell].contents, contents, MAX_CONTENTS);
  curr_cell++;
}

static inline void blank_out_table_cells(void) {
  while (--curr_cell)
    memset(&cells[curr_cell], 0, sizeof(struct TableCell));
}

static void free_subwins(void) {
  while (curr_subwin--) {
    delwin(subwins[curr_subwin]);
  }
}

#endif
