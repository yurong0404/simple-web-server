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
    
}

void handler_301(char *path, struct result *rst) {
    rst->status_code = 301;
    rst->location = malloc(sizeof(char)*64);
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
            find_index_html(path, rst);
        }
        else {      // create index.html
            create_index_html(path, rst);
        }
        closedir(dir);
    }
    else if (errno == ENOENT) {
        perror("directory handler");
        exit(-1);
    }
}
void CGI_handler() {

}

