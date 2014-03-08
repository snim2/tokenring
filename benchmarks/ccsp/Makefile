.PHONY: version version-short clean

all: tokenring.c
	$(CC) $(CFLAGS) $(CC_INC) -O2 -Wall $< -o $@ $(CC_LIB) -lccsp -lpthread

version:
	echo "not implmented."

version-short:
	echo "not implemented."	

clean:
	-@ rm *.o tokenring
