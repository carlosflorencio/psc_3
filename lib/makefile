# Variables
CC = gcc
CFLAGS=-Wall -pedantic -std=c99 -g

# Entry Point
libhttp.so: parser.o http.o io.o utils.o
	$(CC) -shared -o libhttp.so parser.o http.o io.o utils.o -lcurl -ljansson

parser.o: http.o utils.o parser.c
	$(CC) $(CFLAGS) -fpic -c parser.c

http.o: io.o http.c
	$(CC) $(CFLAGS) -fpic -c http.c
    
io.o: io.c
	$(CC) $(CFLAGS) -fpic -c io.c
    
utils.o: utils.c
	$(CC) $(CFLAGS) -fpic -c utils.c


# Clean
clean:
	rm -f libhttp.so *.o

