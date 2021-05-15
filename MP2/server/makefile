#Project Makefile
CC = gcc
LIBS = -lm -lpthread -lrt
CFLAGS = -Wall 

s: main.o parser.o utils.o linkedlist.o buffer.o threads.o lib.o fifos.o
	$(CC) $(CFLAGS) -o s main.o parser.o utils.o linkedlist.o buffer.o threads.o lib.o $(LIBS)

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c $(LIBS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c $(LIBS)

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c $(LIBS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(LIBS)

threads.o: threads.c threads.h
	$(CC) $(CFLAGS) -c threads.c $(LIBS)

fifos.o: fifos.c fifos.h
	$(CC) $(CFLAGS) -c fifos.c $(LIBS)

lib.o: ./library/lib.c ./library/lib.h ./library/delay.h
	gcc -Wall -c -o lib.o ./library/lib.c

clean:
	rm *.o s