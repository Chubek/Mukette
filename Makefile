LEX = flex
CC = gcc

LEX_SRC = parse.l

DOT_SRC = parse.dot
DOT_PNG = parse.png

LEX_GEN = lex.yy.c
LEX_EXE = mukette

all: $(LEX_EXE)

graph:  $(DOT_SRC)
	dot -Tpng -o $(DOT_PNG) $(DOT_SRC)

$(LEX_EXE): $(LEX_GEN)
	$(CC) $(DEBUG) -o $@ $^ -lfl -lcurses

$(LEX_GEN): $(LEX_SRC)
	$(LEX) -o$@ $^

clean:
	rm -f $(LEX_GEN) $(LEX_EXE) $(DOT_PNG)

.PHONY: all clean

