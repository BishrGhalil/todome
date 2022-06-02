CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
LIBS=-lpcre
SRC=main.c recdir.c dna_string.c
OBJ=${SRC:.c=.o}
OUTPUT=todome

all: todome

asprintf.o: asprintf.h
dna_string.o: dna_string.h
recdir.o: dna_string.o recdir.h
main.o: search.h errors.h recdir.o dna_string.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

todome: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

install: all
	cp $(OUTPUT) /usr/bin/$(OUTPUT)

uninstall:
	rm -rf /usr/bin/$(OUTPUT)

clean:
	rm -rf $(OUTPUT) *.o

run: todome
	./todome

.PHONY: clean install uninstall all run
