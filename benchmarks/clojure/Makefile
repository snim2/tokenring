.PHONY: clean version-short version

all: tokenring.clj
	java -Dclojure.compile.path=. -cp .:/usr/share/java/clojure.jar: clojure.lang.Compile tokenring

version-short:
	-@ clojure -e '(clojure-version)' | sed s/\"//g

version:
	-@ clojure -e '(clojure-version)' | sed s/\"//g

clean:
	-@ rm *.class

