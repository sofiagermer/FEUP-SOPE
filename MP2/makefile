#Project Makefile
CC = gcc
LIBS = -lm -lpthread
CFLAGS = -Wall 

c: main.o parser.o
	$(CC) $(CFLAGS) -o c main.o parser.o $(LIBS)

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm *.o c