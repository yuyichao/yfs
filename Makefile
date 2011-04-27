CC = gcc -g
CXX = g++ -g
LINK = g++ -g

all: yfs

yfs: yfs.o
	$(LINK) -o yfs yfs.o -lfuse
yfs.o: main.c
	$(CC) -c -D_FILE_OFFSET_BITS=64 -o yfs.o main.c

clean:
	find -name "*~" -type f -exec rm -v {} \;
	rm -v yfs yfs.o 2> /dev/null || true
