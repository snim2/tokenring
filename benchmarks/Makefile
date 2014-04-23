#
# Recursive makefile executes 'make all' on all subdirectories.
#

# SUBDIRS = $(shell ls -d */)

# chp
# ccsp
# stackless
SUBDIRS = clojure erlang ghc golang haskell jcsp mpi ocaml occam pthread python-csp scala

all: 
	@for dir in $(SUBDIRS); \
	do \
		$(MAKE) -C $${dir} $@ || exit $$?; \
	done

clean:
	@for dir in $(SUBDIRS); \
	do \
		$(MAKE) -C $${dir} clean || exit $$?; \
	done

