all: tokenring.ml
	ocamlopt -noassert -unsafe -nodynlink -inline 100 -thread unix.cmxa threads.cmxa tokenring.ml -o tokenring

clean:
	-@ rm tokenring *.cmi *.cmx *.o

