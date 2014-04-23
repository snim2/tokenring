GHC=ghc

.PHONY: version version-short clean

all: tokenring.hs
	$(GHC) --make -O2 -threaded $< -o $@

version:
	echo "not implmented."

version-short:
	echo "not implemented."	

clean:
	-@ rm *.o tokenring
