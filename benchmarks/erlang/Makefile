.PHONY: clean version-short version

all: tokenring.erl
	erlc +native +"{hipe, [o3]}"  tokenring.erl

version-short:
	-@ erl -version 2>&1 >/dev/null | awk '{ print $$6 }'

version:
	-@ erl -version 2>&1 >/dev/null

clean:
	-@ rm *.beam *.dump

