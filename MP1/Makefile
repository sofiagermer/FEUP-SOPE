#Project Makefile

CC = gcc
LIBS = -lm

main.exe: main.o modes.o options.o
	$(CC) -Wall main.o modes.o options.o -o main.exe $(LIBS)

main.o: main.c
	gcc -Wall -c main.c

modes.o: modes.h modes.c
	gcc -Wall -c modes.c

options.o: options.c options.h
	gcc -Wall -c options.c

clean:
	rm *.o *.exe *.log