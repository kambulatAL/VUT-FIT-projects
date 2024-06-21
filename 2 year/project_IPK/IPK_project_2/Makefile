CC = gcc
CFLAGS = -Werror -Wall -pedantic


all: sniffer

sniffer: ipk-sniffer.c
	$(CC) $(CFLAGS) -o ipk-sniffer ipk-sniffer.c -lpcap

clean:
	rm -f ipk-sniffer