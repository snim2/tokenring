JAVAC=javac

BASEDIR = $(shell pwd)
JCSP = $(BASEDIR)/../../dependencies/jcsp/jcsp.jar

# FIXME: Find this automatically.
# At the moment this information is not contained in the manifest of the JAR.
VERSION=1.1-rc4

.PHONY: clean version version-short

all: TokenRing.java
	$(JAVAC) -Xlint:unchecked -cp $(JCSP) $<

version:
	@echo jcsp-$(VERSION)

version-short:
	@echo $(VERSION)

clean:
	-@rm *.class
