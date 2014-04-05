.PHONY: version version-short clean



all: tokenring.c
	$(CC) $(CFLAGS) $(CC_INC) -O3 -Wall -I /usr/local/include/kroc/ $< -o $@ $(CC_LIB) -lccsp -lpthread

version:
	echo "not implmented."

version-short:
	echo "not implemented."	

clean:
	-@ rm *.o tokenring
