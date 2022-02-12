CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LIBS=-lncurses
DEPS=polytris.h
OBJ=main.o polytris.o polyminoes.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

polytris: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ)

