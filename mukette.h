#ifndef MUKETTE_H
#define MUKETTE_H

#define ATTR_ADDSTR(ATTR, TEXT) \
  do {                          \
    attron(ATTR);               \
    addstr(TEXT);               \
    attroff(ATTR);              \
  } while (0)


#define ATTR_ADDCOLOR(PAIRNUM, FORE, BACK)       \
  do {                                           \
    init_pair(PAIRNUM, FORE, BACK);              \
  } while (0)


#define START_COLOR() 			start_color()
#define PRINT_COLOR(pnum, text) 	ATTR_ADDSTR(COLOR_PAIR(pnum), text)
#define PRINT_ATTR(attr, text) 		ATTR_ADDSTR(attr, text)
#define NEWLINE() 			addch('\n')
#define WHITESPACE() 			addch(' ')
#define TAB() 				addch('\t')
#define POLL() 				getch()
#define REFRESH() 			refresh()
#define GETPOS(y, x) 			getyx(stdscr, y, x)

#define KEY_IS_NOT_EXIT(ch) 		(ch != KEY_EXIT && ch != KEY_F(1))
#define KEY_IS_VERT_ARROW(ch) 		(ch == KEY_UP || ch == KEY_DOWN)
#define KEY_IS_HORIZ_ARROW(ch)		(ch == KEY_LEFT || ch == KEY_RIGHT)
#define KEY_IS_ACTION(ch) 		(ch == KEY_ENTER || ch == KEY_COMMAND)
#define KEY_IS_NEXT(ch) 		(ch == KEY_STAB || ch == KEY_NPAGE)
#define KEY_IS_PREV(ch)			(ch == KEY_PPAGE || ch == KEY_BTAB)

#define INIT_SCREEN() 			initscr()
#define END_SCREEN() 			endwin()


#endif
