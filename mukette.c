#include <curses.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mukette.h"

extern char **environ;

Pair new_pair(int y, int x, const char *link) {
  return (Pair){.y = y, .x = x, .link = link};
}

void print_attribute_list(const char *text, int attrs[MAX_ATTR]) {
   int attr_num = 0, attr = 0, fin_attr = 0;

   while ((attr = attrs[attr_num++]) != ATTR_SENTINEL)
	   fin_attr |= attr;

   PRINT_ATTR(fin_attr, text);
}

const char *get_link(int y, int x) {
  for (size_t i = 0; i < hyperlinks_idx; i++)
    if ((hyperlinks[i].x > x &&
         hyperlinks[i].x < x + strlen(hyperlinks[i].link)) &&
        hyperlinks[i].y == y)
      return hyperlinks[i].link;
  return NULL;
}

void horiz_rule(void) {
  int my, mx;
  getmaxyx(stdscr, my, mx);
  hline('-', mx);
}

void print_header(int color, const char *text, const int header_offset) {
  int y, x;
  GETPOS(y, x);
  attron(COLOR_PAIR(color) | A_BOLD);
  mvprintw(y, header_offset, ">> %s", text);
  attroff(COLOR_PAIR(color) | A_BOLD);
}

void print_ordered_list(int inum, const char *text, const int list_offset) {
  int y, x;
  GETPOS(y, x);
  mvprintw(y, list_offset, "[%d] - %s\n", inum, text);
}

void print_unordered_list(const char *text) {
  int y, x;
  GETPOS(y, x);
  mvprintw(y, LIST_OFFSET, "[*] - %s\n", text);
}

void navigate_to_link(const char *link) {
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

void hyperlink_action(void) {
  int y, x;
  GETPOS(y, x);
  const char *link = get_link(y, x);
  if (link == NULL)
    return;
  navigate_to_link(link);
}

void add_hyperlink(const char *link) {
  Pair p = new_pair(0, 0, link);
  GETPOS(p.y, p.x);
  hyperlinks[hyperlinks_idx++] = p;
}

void free_hyperlinks(void) {
  while (hyperlinks_idx > 0)
    free((char *)hyperlinks[--hyperlinks_idx].link);
}

void move_up(void) {
  int y, x;
  GETPOS(y, x);
  move(y - 1, x);
}

void move_down(void) {
  int y, x;
  GETPOS(y, x);
  move(y + 1, x);
}

void move_left(void) {
  int y, x;
  GETPOS(y, x);
  move(y, x - 1);
}

void move_right(void) {
  int y, x;
  GETPOS(y, x);
  move(y, x + 1);
}

void jump_next_link(void) {
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

void jump_prev_link(void) {
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

void poll_and_navigate(void) {
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

void initialize_muk_config(void) {
  const char *env_var = NULL;

  if ((env_var = getenv("MUK_BROWSER")) == NULL)
    configuration.muk_browser = "w3m";
  else
    configuration.muk_browser = env_var;
}


