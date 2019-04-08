# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g

all: mirror_client

mirror_client: mirror_client.o functions.o
	$(CC) $(CFLAGS) mirror_client.o functions.o -lm -o exe/mirror_client

mirror_client.o: src/mirror_client.c
	$(CC) $(CFLAGS) -c src/mirror_client.c

functions.o: src/functions.c
	$(CC) $(CFLAGS) -c src/functions.c

clean:
	rm -fv *.o exe/*
	rm -f tmp/*

cunit:
	sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev