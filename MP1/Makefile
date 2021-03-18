#Project Makefile
CC = gcc
LIBS = -lm
CFLAGS = -Wall

main.exe: main.o modes.o options.o utils.o
	$(CC) $(CFLAGS) main.o modes.o utils.o options.o -o main.exe $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

modes.o: modes.h modes.c
	$(CC) $(CFLAGS) -c modes.c

options.o: options.h options.c
	$(CC) $(CFLAGS) -c options.c

utils.o: utils.h utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o *.exe *.log