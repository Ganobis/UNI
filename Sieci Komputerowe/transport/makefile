CC = gcc
CFLAGS = -c -Wall -Wextra
VPATH = SRC INCLUDE

all:	main.o functions.o
	$(CC) functions.o main.o -lm -o transport

main.o:	main.c
	$(CC) main.c $(CFLAGS) -o main.o

functions.o: functions.c
	$(CC) functions.c $(CFLAGS) -o functions.o

clean:
	rm -f *.o transport
