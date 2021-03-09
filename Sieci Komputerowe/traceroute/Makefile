CC = gcc

all:	main.o functions.o
	$(CC) functions.o main.o -o traceroute

main.o:	main.c
	$(CC) main.c -c -Wall -Wextra -o main.o

functions.o:
	$(CC) functions.c -c -Wall -Wextra -o functions.o

clean:
	rm -f *.o main
