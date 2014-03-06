.PHONY: clean version-short version

all: tokenring.hs
	ghc --make -fllvm -O2 -XBangPatterns -rtsopts -threaded tokenring.hs -o tokenring

version-short:
	-@ ghc --version | awk '{ print $$8 }'

version:
	-@ ghc --version

clean:
	-@ rm tokenring *.o *.hi

