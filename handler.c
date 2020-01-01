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

void static_object_handler() {

}
void directory_handler(char *path, struct result *rst) {
    DIR *dir;
    if (path[strlen(path)-1] != '/') {
        rst->status_code = 301;
        return;
    }
    if ((dir = opendir(path))) {
        char html[4096];
        memset(html, 0, 4096);
        if (index_html_exist(dir)) {    // find index.html
            int fd;
            fd = open("index.html" , R_OK);
            read(fd, html, sizeof(html));
            close(fd);
            rst->status_code = 200;
            rst->content = malloc(sizeof(html));
            strcpy(rst->content, html);
        }
        else {      // create index.html
            char buf[2048];
            memset(buf, 0, 2048);
            read_directory(buf, path, sizeof(buf));
            create_html(html, buf);
            rst->content = malloc(sizeof(html));
            strcpy(rst->content, html);
            rst->status_code = 200;
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