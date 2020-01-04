#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "header.h"
#include "util.h"

void static_object_handler(char *path, struct result *rst) {
    char *dot;
    dot = strrchr(path, '.');
    if (is_plain_text(dot)) {
        rst->content_type = "text/plain";
        rst->status_code = 200;
        set_result_content_from_file(path, rst);
    }
    else if (is_html(dot)) {
        rst->content_type = "text/html";
        rst->status_code = 200;
        set_result_content_from_file(path, rst);
    }
    else if (is_image(dot)) {
        set_image_content_type(dot, rst);
        rst->status_code = 200;
        set_result_content_from_file(path, rst);
    }
    else {
        printf("str: %s\n", path);
        rst->status_code = 403;
    }
}

void handler_301(char *path, struct result *rst) {
    rst->status_code = 301;
    rst->location = malloc(sizeof(char)*64);
    rst->content_type = 0;
    rst->content = 0;
    rst->content_len = 0;
    memset(rst->location, 0, 64);
    strcat(rst->location, "/");
    strcat(rst->location, path);
    strcat(rst->location, "/");
}

void directory_handler(char *path, struct result *rst) {
    DIR *dir;
    if (path[strlen(path)-1] != '/') {
        handler_301(path, rst);
        return;
    }
    if ((dir = opendir(path))) {
        if (index_html_exist(dir)) {    // find index.html
            strcat(path, "index.html");
            rst->content_type = "text/html";
            rst->status_code = 200;
            set_result_content_from_file(path, rst);
        }
        else {      // create index.html
            create_index_html(path, rst);
        }
        rst->content_type = "text/html";
        closedir(dir);
    }
    else if (errno == ENOENT) {
        perror("directory handler");
        exit(-1);
    }
}
void CGI_handler() {

}

