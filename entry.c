#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include "pager.h"

extern FILE *yyin = NULL;
extern Naviable *navs = NULL;
int argc, char *argv[];

void get_options(bool *x_flag, bool *e_flag) {
  int opt;
  while ((opt = getopt(argc, argv, "xe")) != -1) {
    switch (opt) {
    case 'x':
      *x_flag = true;
      break;
    case 'e':
      *e_flag = true;
      break;
    case 'h':
      display_help_and_exit(argv[0]);
    case 'v':
      display_version_and_exit();
    default:
      fprintf(stderr, "Usage: %s [-x] [-e] FILES...\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }
}

int yywrap(void) {
  if (optind == argc && isatty(STDIN_FILENO))
    return 1;
  else if (!isatty(STDIN_FILENO)) {
    yyin = stdin;
    return 1;
  } else if (yyin != NULL && yyin != stdin)
    fclose(yyin);
  yyin = fopen(argv[optind++], "r");
  return 0;
}

void free_navs(void) { free_naviable_list(navs); }

int main(int largc, char *largv[]) {
  argc = largc, argv = largv;

  bool x_flag = false, e_eflag = false;

  get_options(&x_flag, &e_flag);

  initialize();

  if (!yywrap())
    yylext();

  poll_and_navigate(&navs, x_flag, e_flag);

  terminate();

  return 0;
}
