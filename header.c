#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "util.h"
#include "header.h"
#include "handler.h"


void header_decode(char *header, struct result *rst) {
	char *arr[128];
	char filename[64];
	memset(arr, 0, sizeof(arr)/sizeof(arr[0]));
	memset(filename, 0 ,sizeof(filename));
	split(arr, header, " ");
	strcpy(filename, arr[1]+1);
	if (strstr(arr[2], "HTTP") == NULL){
		rst->status_code = 400;
		return;
	}
	if (strcmp(arr[0], "GET") == 0 ){
		if (check_directory(filename)) {
			directory_handler(filename, rst);
		}
		else if (check_object(filename)){
			static_object_handler(filename, rst);
		}
		else 
			rst->status_code = 403;
		return;
	}
}

void header_encode(char *header, struct result *rst) {
	encode_status_code(header, rst);
	encode_content(header, rst->content);
	//encode_content_length(header, rst->content);
}

void encode_status_code(char *header, struct result *rst) {
	if (rst->status_code == 400)
		strcat(header, "HTTP/1.1 400 Bad Requests\x0d\x0a");
	else if (rst->status_code == 301) {
		strcat(header, "HTTP/1.1 301 MOVE PERMANENTLY\x0d\x0a");
		strcat(header, "Location: ");
		strcat(header, rst->location);
		strcat(header, "\x0d\x0a");
	}
	else if (rst->status_code == 403)
		strcat(header, "HTTP/1.1 403 FORBIDDEN\x0d\x0a");
	else if (rst->status_code == 200)
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
