.PHONY: clean version-short version

all: tokenring.ml
	ocamlopt -noassert -unsafe -nodynlink -inline 100 -thread unix.cmxa threads.cmxa tokenring.ml -o tokenring

version-short:
	-@ ocamlopt -version

version:
	-@ ocamlopt -version

clean:
	-@ rm tokenring *.cmi *.cmx *.o

