all: server 

utilPRS.o : utilPRS.c utilPRS.h
	gcc -Wall -c utilPRS.c -lm

server.o : server.c utilPRS.c utilPRS.h
	gcc -Wall -c server.c utilPRS.c -lm

server : server.o utilPRS.o
	gcc -Wall -o server server.o utilPRS.o -lm

test.o : test.c utilPRS.c utilPRS.h
	gcc -Wall -c test.c utilPRS.c -lm

test : test.o utilPRS.o
	gcc -Wall -o test test.o utilPRS.o -lm

clean :
	rm *.o

atom :
	atom *.c *.h Makefile
