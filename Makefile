CC = gcc
CFLAGS = -o3 -lncurses

LEX_SOURCE = markdown.l
LEX_TARGET = lex.yy.c
C_TARGET = mukette

all: $(C_TARGET)

$(C_TARGET): $(LEX_TARGET)
	$(CC) $(LEX_TARGET) $(CFLAGS) -o $(C_TARGET)

$(LEX_TARGET): $(LEX_SOURCE)
	lex $(LEX_SOURCE)

.PHONY: clean

clean:
	rm -f $(LEX_TARGET) $(C_TARGET)

