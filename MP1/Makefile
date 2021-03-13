#Project Makefile

CC = gcc
LIBS = -lm

main.exe: main.o modes.o
	$(CC) -Wall main.o modes.o -o main.exe $(LIBS)

main.o: main.c
	gcc -Wall -c main.c

modes.o: modes.h modes.c
	gcc -Wall -c modes.c

clean:
	rm *.o *.exe *.log