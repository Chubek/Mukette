#include "pager.h"

typedef enum NavigatableType {
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
  Naviable *nav = (Naviable *)calloc(1, sizeof(Naviable));
  nav->type = type;
  nav->y = y;
  nav->x = x;
  nav->width = width;
  nav->contents = contents;
  nav->tag = tag;
  nav->win = win;

  nav->next = *navs;
  nav->prev = NULL;
  if (*navs != NULL) {
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

static inline void highlight_naviable(Naviable *nav) {
  mvchgat(nav->y, nav->x, nav->width, A_STANDOUT, COLOR_PAIR(0), NULL);
  refresh();
}

static inline void unhighlight_naviable(Naviable *nav) {
  mvchgat(nav->y, nav->x, nav->width, A_BOLD | A_UNDERLINE, COLOR_PAIR(0), NULL);
  refresh();
}

static inline bool next_naviable(Naviable *nav) {
  if (nav->next == NULL)
    return false;
  nav = nav->next;
  return true;
}

static inline bool previous_naviable(Naviable *nav) {
  if (nav->prev == NULL)
    return false;
  nav = nav->prev;
  return true;
}

void add_naviable_hyperlink(Naviable **navs, const char *name,
                            const char *addr) {
  GETPOS(y, x);

  size_t name_len = strlen(name);
  size_t addr_len = strlen(addr);
  
  char *name_copy = strndup(name, name_len);
  char *addr_copy = strndup(addr, addr_len);

  char *final_addr;
  if (strncmp(addr_copy, "http://", 7) || strncmp(addr_copy, "https://", 8))
	  final_addr = addr_copy;
  else {
	asprintf(&final_addr, "https://%s", addr_copy);
	free(addr_copy);
  }

  insert_naviable(navs, HYPERLINK, y, x, name_len, name_copy, final_addr, NULL);
}

void add_naviable_listing(Naviable **navs, const char *prog, const char *code) {
  GETPOS(y, x);

  size_t prog_len = strlen(prog);
  size_t code_len = strlen(code);
  
  const char *prog_copy = strndup(prog, prog_len);
  const char *code_copy = strndup(code, code_len);

  insert_naviable(navs, CODE_LISTING, y, x, code_len, code_copy, prog_copy, NULL);
}


void naviable_execute(Naviable *nav, bool use_env, bool exec_allowed) {
    if (!exec_allowed) {
        beep();
        return;
    }

    if (nav->type == HYPERLINK) {
        char *open_command;
        #ifdef __linux__
            open_command = "xdg-open";
        #elif __APPLE__
            open_command = "open";
        #else
            open_command = "chrome";
        #endif

        char *command;
        asprintf(&command, "%s %s", open_command, nav->tag);
        system(command);
        free(command);
    } else {
        char tmpfile_name[] = "/tmp/XXXXXX.c";
        int fdesc = mkstemp(tmpfile_name);
        if (fdesc == -1) {
            fprintf(stderr, "Error creating temporary file\n");
            return;
        }
        FILE *fstream = fdopen(fdesc, "w");
        if (fstream == NULL) {
            fprintf(stderr, "Error opening temporary file\n");
            return;
        }
        fputs(nav->contents, fstream);
        fclose(fstream);

        char *command;
        asprintf(&command, "cat %s | %s %s", tmpfile_name,
                 use_env ? "env -S" : "", nav->tag);
        system(command);
        free(command);

        unlink(tmpfile_name);
    }
}


void poll_and_navigate(Naviable **navs, bool use_env, bool exec_allowed) {
  if (*navs == NULL) {
	int c = getch();
	if (c == KEY_EXIT || c == KEY_F(1) || c == 'q' || c == 'Q')
		return;
  }

  Naviable *current_nav, *head;
  current_nav = head = *navs;

  for (;;) {
	int c = getch();

	if (c == KEY_LEFT) {
		unhighlight_naviable(current_nav);
		current_nav = current_nav->next == NULL ? current_nav : current_nav->next;
	} else if (c == KEY_RIGHT) {
		unhighlight_naviable(current_nav);
		current_nav = current_nav->prev == NULL ? head : current_nav->prev;
	} else if (c == 'r' || c == 'R') {
		unhighlight_naviable(current_nav);
		current_nav = head;
	} else if (c == KEY_EXIT || c == KEY_F(1) || c == 'q' || c == 'Q')
		return;
	else if (c == KEY_COMMAND || c == KEY_ENTER || c == '\r' || c == ' ' ||  c == '\t') 
		naviable_execute(current_nav, use_env, exec_allowed);

	highlight_naviable(current_nav);
  }


}
