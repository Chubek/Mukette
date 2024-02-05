LEX = flex
CC = gcc

LEX_SRC = parse.l

LEX_GEN = lex.yy.c
LEX_EXE = parser

all: $(LEX_EXE)

$(LEX_EXE): $(LEX_GEN)
	$(CC) -o $@ $^ -lfl

$(LEX_GEN): $(LEX_SRC)
	$(LEX) -o$@ $^

clean:
	rm -f $(LEX_GEN) $(LEX_EXE)

.PHONY: all clean

