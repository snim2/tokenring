EXEC=tokenring

CC=gcc

CFLAGS=-pipe -Wall -O3 -fomit-frame-pointer -march=native -pthread

all: tokenring.c
	$(CC) $(CFLAGS) $(EXEC).c -o $(EXEC)

clean:
	-@ rm $(EXEC)
