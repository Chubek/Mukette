#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include "pager.h"

extern FILE *yyin;
extern Naviable *navs;
extern void free_naviable_list(Naviable *navs);
extern void poll_and_navigate(Naviable **navs, bool use_env, bool exec_allowed);
int argc;
char **argv;
extern int yylex(void);

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
  if (yyin == NULL && !isatty(STDIN_FILENO))
    yyin = stdin;
  else if (yyin == NULL && optind < argc)
    yyin = fopen(argv[optind], "r");
  else if (yyin != NULL && optind < argc) {
    fclose(yyin);
    yyin = fopen(argv[optind], "r");
  } else if (yyin == NULL && isatty(STDIN_FILENO) && optind == argc)
    return 1;
  else if (yyin != NULL && optind == argc)
    return 1;
  return 0;
}

int main(int largc, char **largv) {
  argc = largc, argv = largv;

  bool x_flag = false, e_flag = false;
  get_options(&x_flag, &e_flag);

  initialize();

  if (!yywrap()) {
    yylex();
    poll_and_navigate(&navs, x_flag, e_flag);
  }

  terminate();
  free_naviable_list(navs);

  return 0;
}
