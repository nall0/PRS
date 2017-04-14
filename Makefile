all: server 

utilPRS.o : utilPRS.c utilPRS.h
	gcc -Wall -c utilPRS.c 

server.o : server.c utilPRS.c utilPRS.h
	gcc -Wall -c server.c utilPRS.c

server : server.o utilPRS.o
	gcc -Wall -o server server.o utilPRS.o

clean : 
	rm *.o
