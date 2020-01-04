#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include "util.h"
#include "header.h"
#include "handler.h"


void header_decode(char *header, struct result *rst) {
	char *arr[128];
	char filename[64];
	int fd=0;
	memset(arr, 0, sizeof(arr)/sizeof(arr[0]));
	memset(filename, 0 ,sizeof(filename));
	split(arr, header, " ");
	strcpy(filename, arr[1]+1);
	if (strstr(arr[2], "HTTP") == NULL){
		rst->status_code = 400;
		return;
	}
	if (strcmp(arr[0], "GET") == 0 ){
		rst->download = "no";
		if (strcmp(filename, "") == 0)  // url is the root of web server
        	strcpy(filename, "./");
		if( access( filename, F_OK ) == -1 )	// check file exist
			handler_403(rst);
		else if ((fd = open(filename, R_OK))== -1)	//check file accessible
        	handler_404(rst);
		else if (is_directory(filename))
			directory_handler(filename, rst);
		else if (is_object(filename))
			static_object_handler(filename, rst);
		else {
			rst->status_code = 200;
		}
		close(fd);
		return;
	}
}

int header_encode(char *header, struct result *rst) {
	encode_status_code(header, rst);
	encode_content_type(header, rst->content_type);
	encode_content_length(header, rst->content_len);
	encode_content_disposition(header, rst->download);
	int header_len = strlen(header);
	encode_content(header, rst->content, rst->content_len);
	return header_len;
}

void encode_status_code(char *header, struct result *rst) {
	if (rst->status_code == 400)
		strcat(header, "HTTP/1.1 400 Bad Requests\x0d\x0a");
	else if (rst->status_code == 301) {
		strcat(header, "HTTP/1.1 301 MOVE PERMANENTLY\x0d\x0a");
		strcat(header, "Location: ");
		strcat(header, rst->location);
		strcat(header, "\x0d\x0a\x0d\x0a");
		free(rst->location);
	}
	else if (rst->status_code == 403)
		strcat(header, "HTTP/1.1 403 FORBIDDEN\x0d\x0a");
	else if (rst->status_code == 200)
		strcat(header, "HTTP/1.1 200 OK\x0d\x0a");
	else if (rst->status_code == 404)
		strcat(header, "HTTP/1.1 404 NOT FOUND\x0d\x0a");
}

void encode_content_length(char *header, int len) {
	if (len == 0)
		return;
	char str[10];
	strcat(header, "Content-Length: ");
	sprintf(str, "%d", len);
	strcat(header, str);
	strcat(header, "\x0d\x0a");
}

void encode_content_type(char *header, char *type) {
	if (type == 0)
		return;
	strcat(header, "Content-Type: ");
	strcat(header, type);
	strcat(header, "\x0d\x0a");
}

void encode_content_disposition(char *header, char *download) {
	if (strcmp(download, "yes") == 0) {
		strcat(header, "Content-Disposition: ");
		strcat(header, "attachment;");
		strcat(header, "\x0d\x0a");
	}
}

void encode_content(char *header, char *content, int len) {
	strcat(header, "\x0d\x0a");
	if (content != 0) {
		memcpy(header+strlen(header), content, len);
	}
	free(content);
}

void set_result_content_from_file(char *path, struct result *rst) {
	int fd = open(path, O_RDONLY);
	int length = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char *buf = malloc(length);
	memset(buf, 0, length);
	read(fd, buf, length);
	close(fd);
	rst->content = malloc(length);
	memcpy(rst->content, buf, length);
	rst->content_len = length;
	free(buf);
}

void set_image_content_type(char *extname, struct result *rst) {
	if (strcmp(extname, ".png") == 0)
        rst->content_type = "image/png";
    else if (strcmp(extname, ".jpg") == 0)
        rst->content_type = "image/jpeg";
	else if (strcmp(extname, ".jpe") == 0)
        rst->content_type = "image/jpeg";
    else if (strcmp(extname, ".jpeg") == 0)
        rst->content_type = "image/jpeg";
    else if (strcmp(extname, ".bmp") == 0)
        rst->content_type = "image/x-ms-bmp";
    else if (strcmp(extname, ".gif") == 0)
        rst->content_type = "image/gif";
    else if (strcmp(extname, ".tif") == 0)
        rst->content_type = "image/tiff";
	else if (strcmp(extname, ".tiff") == 0)
        rst->content_type = "image/tiff";
	else if (strcmp(extname, ".ico") == 0)
        rst->content_type = "image/vnd.microsoft.icon";
    else
        rst->content_type = 0;
}

void set_audio_content_type(char *extname, struct result *rst) {
	if (strcmp(extname, ".mp3") == 0)
        rst->content_type = "audio/mpeg";
    else if (strcmp(extname, ".m4a") == 0)
        rst->content_type = "audio/mpeg";
	else if (strcmp(extname, ".wma") == 0)
        rst->content_type = "audio/x-ms-wma";
	else if (strcmp(extname, ".wax") == 0)
        rst->content_type = "audio/x-ms-wax";
	else if (strcmp(extname, ".wav") == 0)
        rst->content_type = "audio/x-wav";
	else if (strcmp(extname, ".ogg") == 0)
        rst->content_type = "audio/ogg";
}

void set_video_content_type(char *extname, struct result *rst) {
	if (strcmp(extname, ".mp4") == 0)
        rst->content_type = "video/mp4";
    else if (strcmp(extname, ".mpeg") == 0)
        rst->content_type = "video/mpeg";
	else if (strcmp(extname, ".mpg") == 0)
        rst->content_type = "video/mpeg";
	else if (strcmp(extname, ".mpe") == 0)
        rst->content_type = "video/mpeg";
	else if (strcmp(extname, ".avi") == 0)
        rst->content_type = "video/x-msvideo";
	else if (strcmp(extname, ".flv") == 0)
        rst->content_type = "video/x-flv";
	else if (strcmp(extname, ".ogv") == 0)
        rst->content_type = "video/ogg";
	else if (strcmp(extname, ".wmv") == 0)
        rst->content_type = "video/x-ms-wmv";
	else if (strcmp(extname, ".wmx") == 0)
        rst->content_type = "video/x-ms-wmx";
	else if (strcmp(extname, ".wvx") == 0)
        rst->content_type = "video/x-ms-wvx";
}