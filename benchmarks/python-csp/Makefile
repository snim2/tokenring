.PHONY: clean version-short version

all: tokenring.py
	-@ echo 'Nothing to do.'

version-short:
	-@ pip show python-csp | awk 'NR==3' | awk '{ print $$2 }'

version:
	-@ pip show python-csp | awk 'NR==3'

clean:
	-@ rm *.pyc
