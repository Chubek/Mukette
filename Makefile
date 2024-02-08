LEX = flex
CC = gcc
INSTALL = install
MANDIR = /usr/local/share/man/man1
BINDIR = /usr/local/bin

LEX_SRC = parse.l
ENTRY_SRC = entry.c

DOT_SRC = parse.dot
DOT_PNG = parse.png

LEX_GEN = lex.yy.c
LEX_EXE = mukette

all: $(LEX_EXE)

graph: $(DOT_SRC)
	dot -Tpng -o $(DOT_PNG) $(DOT_SRC)

$(LEX_EXE): $(LEX_GEN) $(ENTRY_SRC)
	$(CC) $(DEBUG) -o $@ $(ENTRY_SRC) $(LEX_GEN) -lfl -lcurses

$(LEX_GEN): $(LEX_SRC)
	$(LEX) -o$@ $^

install: $(LEX_EXE) mukette.1
	$(INSTALL) -d $(DESTDIR)$(MANDIR)
	$(INSTALL) -m 644 mukette.1 $(DESTDIR)$(MANDIR)
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 $(LEX_EXE) $(DESTDIR)$(BINDIR)
	mandb

clean:
	rm -f $(LEX_GEN) $(LEX_EXE) $(DOT_PNG)

.PHONY: all clean install

