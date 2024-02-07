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
#include <pthread.h>

#define MAX_LINK 4096
#define MAX_ADDR 256
#define MAX_CONTENTS 512

#define LINES_OFFS 2
#define COLS_OFFS 6

#define COLOR_PAIR_HEADER 1
#define COLOR_PAIR_CODE 2


#ifndef DEFAULT_BROWSER
#define DEFAULT_BROWSER "firefox"
#endif

static pthread_t input_thread = 0;
jmp_buf jbuf = {0};

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

static inline void print_list_item(const char *point, const char *item) {
  int y, x;
  getyx(stdscr, y, x);
  mvprintw(y, 1, "%s - %s\n", point, item);
}

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

static inline bool is_approaching(int link_x, int curr_x, int link_len) {
    return (curr_x >= link_x - link_len) && (curr_x <= link_x + link_len);
}


void navigate_to_addr(const char*);

void *input_thread_func(void *arg) {
    struct Hyperlink *link = (struct Hyperlink *)arg;
    while (true) {
        int c = getch();
        if (c == KEY_ENTER || c == '\n') {
            navigate_to_addr(link->addr);
            mvchgat(link->y, link->x, link->width * link->height, A_BOLD | A_REVERSE, COLOR_PAIR(0), NULL);
	}
        usleep(10000);
    }
    return NULL;
}

void navigate_hyperlinks() {
    int y, x;
    struct Hyperlink *link;

    for (size_t i = 0; i < curr_link; i++) {
        link = &links[i];
        getyx(stdscr, y, x);
        if (link->y == y && is_approaching(link->x, x, link->width)) {
            mvchgat(link->y, link->x, link->width * link->height, A_BOLD | A_REVERSE, COLOR_PAIR(0), NULL);
            refresh();
	    pthread_create(&input_thread, NULL, input_thread_func, (void *)link);
            pthread_detach(input_thread);
            pthread_exit(NULL);
        }
    }

    longjmp(jbuf, 1);

}
void navigate_to_addr(const char *link) {
    #ifdef __linux__
    char *const argv[] = { "xdg-open", (char*)link, NULL };
    #elif __APPLE__
    char *const argv[] = { "open", (char*)link, NULL };
    #else
    char *const argv[] = { DEFAULT_BROWSER, (char*)link, NULL };
    #endif

    posix_spawn_file_actions_t file_actions;
    posix_spawn_file_actions_init(&file_actions);

    pid_t pid;
    if (posix_spawnp(&pid, argv[0], &file_actions, NULL, argv, NULL) == 0) {
        waitpid(pid, NULL, 0);
    } else {
        perror("posix_spawnp");
    }

    posix_spawn_file_actions_destroy(&file_actions);
}

#endif
