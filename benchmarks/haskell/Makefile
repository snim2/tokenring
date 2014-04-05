GHC=ghc

.PHONY: clean version version-short

all: tokenring.hs
	 $(GHC) --make -O2 -fglasgow-exts -threaded $< -o tokenring

version:
	echo "not implmented."

version-short:
	echo "not implmemented."

clean:
	-@ rm tokenring *.o *.hi
