CC=gcc

.PHONY:compile clean run
compile:main.x

main.x:main.o
	$(CC) -o main.x main.o

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f *.o *.x

run:compile
	./main.x
