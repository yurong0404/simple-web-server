#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "util.h"
#include "header.h"
#include "handler.h"


void header_decode(char *header, struct result *rst) {
	char *arr[128];
	split(arr, header, " ");
	
	if (strstr(arr[2], "HTTP") == NULL){
		rst->status_code = 400;
		return;
	}
	if (strcmp(arr[0], "GET") == 0 ){
		if (check_directory(arr[1]+1)) {
			directory_handler(arr[1]+1, rst);
		}
		else if (check_object(arr[1]+1)){
			printf("true");
			return;
		}
		else 
			rst->status_code = 403;
		return;
	}
}

void header_encode(char *header, struct result *rst) {
	encode_status_code(header, rst->status_code);
	encode_content(header, rst->content);
	//encode_content_length(header, rst->content);
}

void encode_status_code(char *header, int status_code) {
	if (status_code == 400)
		strcat(header, "HTTP/1.1 400 Bad Requests\x0d\x0a");
	else if (status_code == 301)
		strcat(header, "HTTP/1.1 301 MOVE PERMANENTLY\x0d\x0a");
	else if (status_code == 403)
		strcat(header, "HTTP/1.1 403 FORBIDDEN\x0d\x0a");
	else if (status_code == 200)
		strcat(header, "HTTP/1.1 200 OK\x0d\x0a");
}

void encode_content_length(char *header, char *content) {
	strcat(header, "Content-Length: 0\x0d\x0a");
}

void encode_content(char *header, char *content) {
	strcat(header, "\x0d\x0a");
	if (content != 0) {
		strcat(header, content);
	}
	free(content);
}
