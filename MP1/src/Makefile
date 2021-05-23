#Project Makefile
CC = gcc
LIBS = -lm
CFLAGS = -Wall

xmod: main.o modes.o utils.o logfile.o signal_handling.o
	$(CC) $(CFLAGS) -o xmod main.o modes.o utils.o logfile.o signal_handling.o $(LIBS)

modes.o: modes.c modes.h
	$(CC) $(CFLAGS) -c modes.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c 

logfile.o: logfile.c logfile.h utils.h
	$(CC) $(CFLAGS) -c logfile.c 

signal_handling.o: signal_handling.c signal_handling.h utils.h
	$(CC) $(CFLAGS) -c signal_handling.c 

clean:
	rm *.o xmod