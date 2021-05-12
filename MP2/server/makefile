#Project Makefile
CC = gcc
LIBS = -lm -lpthread
CFLAGS = -Wall 

s: main.o parser.o utils.o linkedlist.o
	$(CC) $(CFLAGS) -o s main.o parser.o utils.o linkedlist.o $(LIBS)

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c $(LIBS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c $(LIBS)

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c $(LIBS)

clean:
	rm *.o c