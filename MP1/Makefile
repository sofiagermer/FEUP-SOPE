#Project Makefile
CC = gcc
LIBS = -lm
CFLAGS = -Wall

xmod: main.o modes.o utils.o logfile.o signal_handling.o
	$(CC) -o xmod main.o modes.o utils.o logfile.o signal_handling.o $(CFLAGS) $(LIBS)

modes.o: modes.c modes.h
	$(CC) -c modes.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c $(CFLAGS)

logfile.o: logfile.c logfile.h utils.h
	$(CC) -c logfile.c $(CFLAGS) 

signal_handling.o: signal_handling.c signal_handling.h utils.h
	$(CC) -c signal_handling.c $(CFLAGS)

clean:
	rm *.o *.exe *.log