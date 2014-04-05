.PHONEY: all clean

CC=gcc

CFLAGS=-Wall -O3 -g
LDFLAGS=-lrt -lm

all: clock_res timer

# FIXME: Should not need to state this explicitly. What is up with -lm?
timer: timer.c
	$(CC) timer.c -o timer $(CFLAGS) $(LDFLAGS)

clock_res: clock_res.c

valgrind:
	valgrind --leak-check=full  ./timer -v -i 1 -c "sleep 2"
	valgrind --leak-check=full  ./clock_res

clean:
	-@ rm clock_res timer core
