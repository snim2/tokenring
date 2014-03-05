#
# Recursive makefile executes 'make all' on all subdirectories.
#

SUBDIRS = $(shell ls -d */)

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

