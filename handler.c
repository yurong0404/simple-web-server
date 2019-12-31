#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "header.h"
#include "util.h"

void static_object_handler() {

}
void directory_handler(char *path, struct result *rst) {
    DIR *dir;
    if (path[strlen(path)-1] != '/') {
        rst->status_code = 301;
        return;
    }
    if ((dir = opendir(path))) {
        // TODO : FIND index.html or create index.html
        char buf[1024];
        char html[2048];
        memset(buf, 0, 1024);
        read_directory(buf, path);
        create_html(html, buf);
        rst->content = malloc(sizeof(buf));
        strcpy(rst->content, buf);
        rst->status_code = 200;
        closedir(dir);
    }
    else if (errno == ENOENT) {
        perror("directory handler");
        exit(-1);
    }
}
void CGI_handler() {

}