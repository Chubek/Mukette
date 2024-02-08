#include "pager.h"

typedef enum NavigatbleType {
  HYPERLINK,
  CODE_LISTING,
} NaviableType;

typedef struct Navigatable {
  NaviableType type;
  int y, x, width;
  const char *tag;
  const char *contents;
  WINDOW *win;
  Naviable *prev;
  Naviable *next;
} Naviable;

void insert_naviable(Naviable **navs, NaviableType type, int y, int x,
                     int width, const char *contents, const char *tag,
                     WINDOW *win) {
  size_t contents_len = strlen(contents);
  Naviable *nav = (Naviable *)calloc(1, sizeof(Naviable));
  nav->type = type;
  nav->y = y;
  nav->x = x;
  nav->width = width;
  nav->contents = contents;
  nav->tag = tag;
  nav->win = win;

  nav->next = navs;
  nav->prev = NULL;
  if (navs != NULL) {
    (*navs)->prev = nav;
  }
  *navs = nav;
}

void free_naviable_list(Naviable *navs) {
  Naviable *current = navs;
  while (current != NULL) {
    Naviable *temp = current;
    current = current->next;
    if (temp->contents != NULL)
      free((void *)temp->contents);
    if (temp->tag != NULL)
      free((void *)temp->tag);
    if (temp->win != NULL)
      delwin(temp->win);
    free(temp);
  }
  navs = NULL;
}

void add_naviable_hyperlink(Naviable **navs, const char *name,
                            const char *addr) {
  GETPOS(y, x);

  size_t name_len = strlen(name);
  insert_naviable(navs, HYPERLINK, y, x, name_len, addr, tag, NULL);

  refresh();
}

void add_naviable_listing(Naviable **navs, const char *prog, const char *code) {
  GETPOS(y, x);

  size_t code_len = strlen(code);
  insert_naviable(navs, CODE_LISTING, y, x, code_len, code, prog, NULL);

  refresh();
}

void naviable_execute(Naviable *nav, bool use_env, bool exec_allowed) {
  if (!exec_allowed) {
    fprintf("Execution of external programs is prohibited unless you pass -x "
            "to the program\n");
    return;
  }

  if (nav->type == HYPERLINK) {
    char *buffer, *open;
    if (__linux__)
      open = "xdg-open";
    else if (__APPLE__)
      open = "open";
    else
      open = "chrome";

    asprintf(buffer, "%s %s", open, nav->contents);
    system(buffer);
    free(buffer);
  } else {
    char *buffer;
    char tmpfile_name[] = {'/', 't', 'm', 'p', '/', 'X',
                           'X', 'X', '.', 'c', '\0'};
    int fdesc = mkstemp(&tmpfile_name[0]);
    FILE *fstream = fdopen(fdesc, "w");
    fputs(fstream, nav->contents);
    asprintf(buffer, "cat %s | %s %s", &tmpfile_name[0],
             use_env ? "env -S" : "", nav->tag);
    system(buffer);
    free(buffer);
    fclose(fstream);
    unlink(&tmpfile_name[0]);
  }
}
