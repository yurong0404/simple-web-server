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

void read_directory(char *buf, char *dirname, int bufsz) {
    int old_stdout = 0;
    char command[128];
    int fd;
    
    fd = open("temp", O_RDWR|O_CREAT, S_IRWXU);
    old_stdout = dup(1);
    dup2(fd,1);
    strcpy(command, "ls -al ");
    strcat(command, dirname);
    system(command);
    close(fd);
    fd = open("temp", R_OK);
    read(fd, buf, bufsz);
    close(fd);
    system("rm temp");
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