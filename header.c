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
		else {
			perror("header_decode");
			exit(-1);
		}
		return;
	}
}

void header_encode(char *header, struct result *rst) {
	encode_status_code(header, rst->status_code);
}

void encode_status_code(char *header, int status_code) {
	if (status_code == 400)
		strcat(header, "HTTP/1.1 400 Bad Requests\x0d\x0a");
}
