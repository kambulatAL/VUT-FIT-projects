# Author: Kambulat Alakaev <xalaka00@stud.fit.vutbr.cz>


CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -Werror

all: testscanner

testscanner:  scanner.o main.o
	$(CC) $(CFLAGS) -o testmain main.o scanner.o 


scanner.o:
	$(CC) $(CFLAGS) -c scanner/scanner.c

main.o:
	$(CC) $(CFLAGS) -c main.c 

clean:
	rm -f main.o scanner.o testmain