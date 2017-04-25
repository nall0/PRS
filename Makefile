all: server test

utilPRS.o : utilPRS.c utilPRS.h
	gcc -Wall -c utilPRS.c 

server.o : server.c utilPRS.c utilPRS.h
	gcc -Wall -c server.c utilPRS.c

server : server.o utilPRS.o
	gcc -Wall -o server server.o utilPRS.o

test.o : test.c utilPRS.c utilPRS.h
	gcc -Wall -c test.c utilPRS.c

test : test.o utilPRS.o
	gcc -Wall -o test test.o utilPRS.o

clean : 
	rm *.o	
