CC=gcc
CFLAGS=-c -Wall

all: Simple_Triangle

Simple_Triangle: main.o util.o
	$(CC) main.o util.o -o Simple_Triangle -lGLEW -lglut -lGL

main.o: main.c
	$(CC) $(CFLAGS) main.c

util.o: ../Common/util.c
	$(CC) $(CFLAGS) ../Common/util.c

clean:
	rm -rf *.o Simple_Triangle
