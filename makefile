CC = gcc
CFLAGS = -Wall -Wextra -g

LIBS = -lncurses -lm


all: pingpong


pingpong: pingpong.c $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@


.PHONY: clean

clean:
	rm -f pingpong
