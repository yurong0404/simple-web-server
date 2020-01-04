#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "util.h"

void split(char **arr, char *str, const char *del) {
	char *s = strtok(str, del);

	while(s != NULL) {
		*arr++ = s;
		s = strtok(NULL, del);
	}
}

int check_directory(char *path) {
    DIR *dir;
    if (strcmp(path, "") == 0) {  // url is the root of web server
        strcpy(path, "./");
        return 1;
    }
    else if ((dir = opendir(path))) {
        closedir(dir);
        return 1;
    }
    else if (errno == ENOENT) {
        return 0;
    }
    return 0;
}

// check the path is a file and it exists, not a directory
int is_object(char *path) {
    if( access( path, F_OK ) != -1 ) {
        DIR *dir;
        if ((dir = opendir(path))) {
            closedir(dir);
            return 0;
        }
        return 1;
    }
    else {
        return 0;
    }
}

void read_directory(char *buf, char *dirname, int bufsz) {
    int old_stdout = 0;
    char command[128];
    int pipe_out[2];
    pipe(pipe_out);

    old_stdout = dup(1);
    dup2(pipe_out[1],1);
    close(pipe_out[1]);
    strcpy(command, "ls -al ");
    strcat(command, dirname);
    system(command);
    read(pipe_out[0], buf, bufsz);

    // recover stdout fd
    dup2(old_stdout, 1);
}

void create_html(char *html, char *text) {
    char *arr[128] = { };
    strcpy(html, "<html>\n<head>\
\n<meta http-equiv='content-type' content='text/html; charset=utf-8'/>\
\n<style>\nbody{font-family: monospace;\nwhite-space: pre;}</style>\
\n</head>\n<body>\n<hr/>");
    split(arr, text, "\n");
    for (int i=1 ; arr[i]!=0; i++) {
        char *arr2[16] = { };
        split(arr2, arr[i], " ");
        int x=0;
        for (; arr2[x+1]!=0;x++) {
            strcat(html, arr2[x]);
            strcat(html, "\t");
        }
        strcat(html, " <a href='");
        strcat(html, arr2[x]);
        strcat(html, "'>");
        strcat(html, arr2[x]);
        strcat(html, "</a>\n");
    }
    strcat(html, "<hr/>\n</body>\n</html>");
}

int index_html_exist(DIR *dir) {
    struct dirent *file;
    while((file=readdir(dir))) {
        if (strcmp(file->d_name, "index.html") == 0)
            return 1;
    }
    return 0;
}

void create_index_html(char *path, struct result *rst) {
    char buf[2048];
    char html[4096];
    memset(buf, 0, 2048);
    memset(html, 0, 4096);
    read_directory(buf, path, sizeof(buf));
    create_html(html, buf);
    rst->content_len = (int) strlen(html);
    rst->content = malloc(strlen(html));
    memset(rst->content, 0, strlen(html));
    strcpy(rst->content, html);
    rst->status_code = 200;
}

int is_plain_text(char *extname) {
    if (extname == 0)
        return 1;
    else if (strcmp(extname, ".txt") == 0)
        return 1;
    else
        return 0;
}

int is_html(char *extname) {
    if (strcmp(extname, ".html") == 0)
        return 1;
    else
        return 0;
}

int is_image(char *extname) {
    if (strcmp(extname, ".png") == 0)
        return 1;
    else if (strcmp(extname, ".jpg") == 0)
        return 1;
    else if (strcmp(extname, ".jpeg") == 0)
        return 1;
    else if (strcmp(extname, ".bmp") == 0)
        return 1;
    else if (strcmp(extname, ".gif") == 0)
        return 1;
    else if (strcmp(extname, ".tif") == 0)
        return 1;
    else if (strcmp(extname, ".raw") == 0)
        return 1;
    else
        return 0;
}