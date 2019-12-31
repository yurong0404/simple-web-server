#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "header.h"

void static_object_handler() {

}
void directory_handler(char *path, struct result *rst) {
    DIR *dir;
    struct dirent *file;
    if (path[strlen(path)-1] != '/') {
        rst->status_code = 301
        rst->content = ""
    }
    if ((dir = opendir(path))) {
        while ((file = readdir(dir)) != NULL) {
            printf("%s\n", file->d_name);
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