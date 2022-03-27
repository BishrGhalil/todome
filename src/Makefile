CC=gcc
CFLAGS=-Wall -std=gnu99
LIBS=-lpcre -lncurses -lm -lsqlite3
SRC=main.c
OBJ=${SRC:.c=.o}
OUTPUT=todome

all: todome

main.o:

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

todome: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

install: all
	cp $(OUTPUT) /usr/bin/$(OUTPUT)

uninstall:
	rm -rf /usr/bin/$(OUTPUT)

clean:
	rm -rf $(OUTPUT) src/*.o

.PHONY: clean install uninstall all
