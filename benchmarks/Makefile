#
# Recursive makefile executes 'make all' on all subdirectories.
#

# SUBDIRS = $(shell ls -d */)

SUBDIRS = clojure erlang ghc golang haskell jcsp mpi ocaml occam pthread python-csp scala stackless

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

