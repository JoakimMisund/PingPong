CC = gcc
CFLAGS = -Wall -Wextra -g

all: pingpong


pingpong: pingpong.c
	$(CC) $(CFLAGS) -o pingpong pingpong.c -lncurses -lm


.PHONY: clean

clean:
	rm -f pingpong
