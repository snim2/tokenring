#
# To compile everything just type make:
#
#    $ make
#
# To run a program, make the 'run' target and override EXEC and MPIFLAGS, e.g.:
#
#    $ make run PROG=sum MPIFLAGS="-n 4"
#    $ make run PROG=sum MPIFLAGS="--host 127.0.0.1,127.0.0.1"
#
# - Sarah Mount, November 2011
#

CC=mpicc

RM=rm

CFLAGS=-c -Wall -O3

LDFLAGS=-lm

SOURCES=$(wildcard *.c)

OBJECTS=$(SOURCES:.c=.o)

EXECS=$(SOURCES:%.c=%)


all: $(OBJECTS) $(EXECS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.o.:
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	-@ $(RM) *.o
	-@ $(RM) $(EXECS)
