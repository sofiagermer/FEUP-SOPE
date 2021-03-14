#Project Makefile

CC = gcc
LIBS = -lm

main.exe: main.o modes.o options.o
	$(CC) -Wall main.o modes.o options.o -o main.exe $(LIBS)

main.o: main.c
	$(CC) -Wall -c main.c

modes.o: modes.h modes.c
	$(CC) -Wall -c modes.c

options.o: options.c options.h
	$(CC) -Wall -c options.c

clean:
	rm *.o *.exe *.log