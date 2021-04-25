#Project Makefile
CC = gcc
LIBS = -lm -lpthread
CFLAGS = -Wall 

c: main.o parser.o utils.o
	$(CC) $(CFLAGS) -o c main.o parser.o utils.o $(LIBS)

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c $(LIBS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c $(LIBS)

clean:
	rm *.o c