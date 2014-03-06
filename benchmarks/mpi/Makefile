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

.PHONY: clean version-short version

CC=mpicc

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

version-short:
	-@ mpicc --version | awk 'NR==1' | awk '{ print $$4 }'

version:
	-@ mpicc --version | awk 'NR==1'

clean:
	-@ $(RM) *.o
	-@ $(RM) $(EXECS)
