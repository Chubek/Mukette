#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <curses.h>

#ifndef MAX_HYPERLINK
#define MAX_HYPERLINK	4096
#endif

typedef struct Pair Pair;
static struct Pair { 
	int y, x; 
	char *link; } hyperlinks[MAX_HYPERLINK] = {{0,0}};

#define ATTR_ADDSTR(ATTR, TEXT)								\
	do { attron(ATTR); addstr(TEXT); attroff(ATTR); } while (0)

#define ATTR_COLORPAIR_INIT(PAIRNUM, FORE, BACK)					\
	do { init_pair(PAIRNUM, FORE, BACK); } while (0)


#define PRINT_BOLD(text)   			ATTR_ADDSTR(A_BOLD, text)
#define PRINT_ITALIC(text) 			ATTR_ADDSTR(A_ITALIC, text)
#define PRINT_BOLD_ITALIC(text)			ATTR_ADDSTR(A_BOLD | A_ITALIC, text)
#define PRINT_UNDERLINE(text)			ATTR_ADDSTR(A_UNDERLINE, text)
#define PRINT_BOLD_UNDERLINE(text)		ATTR_ADDSTR(A_BOLD | A_UNDERLINE, text)
#define PRINT_BOLD_ITALIC_UNDERLINE(text)	ATTR_ADDSTR(A_BOLD | A_UNDERLINE | A_ITALIC, text)
#define PRINT_COLOR_PAIR(pnum, text)		ATTR_ADDSTR(COLOR_PAIR(pnum), text)

#define 

#define NEWLINE()				addch('\n')
#define WAIT()					getch();

static inline void init_screen(void) { initscr(); }
static inline void end_screen(void) { endwin(); }
static inline Pair new_pair(int y, int x, char *link) { return (Pair){ 
							.y = y, 
							.x = x,
							.link = link }; }
static inline char *get_link(int y, int x) {
	for (size_t i = 0; i < MAX_HYYPERLINK; i++)
		if (hyperlinks[i].x == x && hyperlinks[i].y == y)
			return hyperlinks[i].link;
	return NULL;
}

int main(int argc, char **argv) {
   init_screen();

   while(--argc) {
	PRINT_BOLD(argv[argc]);
	NEWLINE();
	PRINT_ITALIC(argv[argc]);
	NEWLINE();
   }

   WAIT();


   end_screen();
}
