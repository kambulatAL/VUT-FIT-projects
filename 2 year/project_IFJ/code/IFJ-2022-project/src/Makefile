# ***************************************************************
# *                         IFJ22                               *
# ***************************************************************
# @file Makefile
# Author: Kambulat Alakaev <xalaka00@stud.fit.vutbr.cz>


CC = gcc
CFLAGS = -Werror -Wall -pedantic -std=c99
ZIPNAME = xalaka00
INZIP = *.c *.h Makefile rozdeleni dokumentace.pdf



all: testparser

testparser:  main.o parser.o scanner.o symstack.o expressions.o  symtable.o
	$(CC) $(CFLAGS)  main.o parser.o expressions.o scanner.o symstack.o symtable.o  -o ifj22 


scanner.o:
	$(CC) $(CFLAGS) -c scanner.c
expressions.o:
	$(CC) $(CFLAGS) -c expressions.c
symstack.o:
	$(CC) $(CFLAGS) -c symstack.c
parser.o:
	$(CC) $(CFLAGS) -c parser.c
symtable.o:
	$(CC) $(CFLAGS) -c symtable.c
main.o:
	$(CC) $(CFLAGS) -c main.c 


zip:
	zip $(ZIPNAME).zip $(INZIP)
	
clean:
	rm -f main.o parser.o scanner.o symstack.o expressions.o symtable.o ifj22