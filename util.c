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
    if ((dir = opendir(path))) {
        closedir(dir);
        return 1;
    }
    else if (errno == ENOENT) {
        return 0;
    }
    return -1;
}

// check the path is a file and it exists, not a directory
int check_object(char *path) {
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

void read_directory(char *buf, char *dirname) {
    int old_stdout = 0;
    char command[128];
    int fd;

    fd = open("temp", W_OK);
    old_stdout = dup(1);
    dup2(fd,1);
    strcpy(command, "ls -al ");
    strcat(command, dirname);
    system(command);
    close(fd);
    fd = open("temp", R_OK);
    read(fd, buf, 400);
    // recover stdout fd
    dup2(old_stdout, 1);
}

void create_html(char *html, char *text) {
    strcpy(html, "<html>\n<head>\
\n<meta http-equiv='content-type' content='text/html; charset=unicode'/>\
\n</head>\n<body>\n<hr/>\n");
    strcat(html, text);
    strcat(html, "<hr/>\n</body>\n</html>");
}