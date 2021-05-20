#Project Makefile
CC = gcc
LIBS = -lm -lpthread -lrt
CFLAGS = -Wall 

s: main.o parser.o utils.o linkedlist.o threads.o lib.o delay.o
	$(CC) $(CFLAGS) -o s main.o parser.o utils.o linkedlist.o threads.o lib.o delay.o $(LIBS)

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c $(LIBS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c $(LIBS)

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c $(LIBS)

threads.o: threads.c threads.h
	$(CC) $(CFLAGS) -c threads.c $(LIBS)

lib.o: ./library/lib.c ./library/lib.h ./library/delay.h
	$(CC) $(CFLAGS) -c ./library/lib.c $(LIBS)

delay.o: ./library/delay.c ./library/delay.h
	$(CC) $(CFLAGS) -c ./library/delay.c $(LIBS)

clean:
	rm *.o s