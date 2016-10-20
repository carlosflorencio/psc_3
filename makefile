# Variables
CC = gcc
CFLAGS=-Wall -pedantic -std=c99 -g

# Entry Point
all: libtest

# Test HttpLib
libtest: libtest.o
	$(CC) -g -o libtest libtest.o ./lib/libhttp.so -ljansson

libtest.o: libtest.c
	$(CC) $(CFLAGS) -c libtest.c
	

# Clean
clean:
	rm libtest -f *.o
