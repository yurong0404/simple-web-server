all:hw4

run:hw4
	./server 12345 ~/unix_programming/web_server

hw4:server.c
	gcc -o server -Wall -g server.c

clean:
	rm server