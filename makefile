CC=gcc
CFLAGS=-ansi -pedantic -Wall

.PHONY:compile clean run
compile:main.x

main.x:main.o
	$(CC) $(CFLAGS) -o main.x main.o

main.o:main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o *.x

run:compile
	./main.x
