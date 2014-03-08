GHC=ghc

.PHONY: version version-short clean

all: tokenring.hs
	$(GHC) --make -O2 -fglasgow-exts -threaded $< -o $@

version:
	echo "not implmented."

version-short:
	echo "not implemented."	

clean:
	-@ rm *.o tokenring
