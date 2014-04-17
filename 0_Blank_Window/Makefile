CC=gcc
CFLAGS=-c -Wall

all: Blank_Window

Blank_Window: main.o util.o
	$(CC) main.o util.o -o Blank_Window -lGLEW -lglut -lGL

main.o: main.c
	$(CC) $(CFLAGS) main.c

util.o: ../Common/util.c
	$(CC) $(CFLAGS) ../Common/util.c

clean:
	rm -rf *.o Blank_Window
