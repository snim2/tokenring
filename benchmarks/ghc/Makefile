all: tokenring.hs
	ghc --make -fllvm -O2 -XBangPatterns -rtsopts -threaded tokenring.hs -o tokenring

clean:
	-@ rm tokenring *.o *.hi

