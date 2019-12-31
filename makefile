all:hw4

run:hw4
	./main 12345 ~/unix_programming/web_server

hw4:main.c header.c util.c handler.c
	gcc -o main -Wall -g main.c header.c util.c handler.c

clean:
	rm main