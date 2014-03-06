.PHONY clean version-short version

all: tokenring.go
	go build -o tokenring

version-short:
	-@ go version | awk '{ print $$3 }'

version:
	-@ go version

clean:
	-@ rm tokenring

