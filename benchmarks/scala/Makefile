.PHONY: clean version-short version

all: tokenring.scala
	scalac -optimise tokenring.scala

version-short:
	-@ scala -version | awk '{ print $$5 }'

version:
	-@ echo `scala -version`

clean:
	-@ rm *.class

