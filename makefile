CFLAGS = -I. -pedantic -Wall

main.o: main.c
	gcc -o shell main.c $(CFLAGS)

clean:
	\rm -f *.o shell
