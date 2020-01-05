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

int is_directory(char *path) {
    DIR *dir;
    if ((dir = opendir(path))) {
        closedir(dir);
        return 1;
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
    return 0;
}

int is_cgi(char *path) {
    char *dot = 0;
    char *arr[16];
    char filename[256];
    for (int i=0; i<16; i++)
        arr[i] = 0;
    strcpy(filename, path);
    dot = strrchr(filename, '.');
    if (dot != 0)
        if (strcmp(dot, ".cgi") == 0)
            return 1;
    split(arr, filename, "/");
    for (int i=0; arr[i+1]!=0; i++) {
        if (strstr(arr[i], "cgi") != NULL) {
            return 1;
        }
    }
    return 0;
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
\n<style>\nbody{font-family: monospace;\nfont-size: 16px;\nwhite-space: pre;}</style>\
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
    else if (strcmp(extname, ".jpe") == 0)
        return 1;
    else if (strcmp(extname, ".jpeg") == 0)
        return 1;
    else if (strcmp(extname, ".bmp") == 0)
        return 1;
    else if (strcmp(extname, ".gif") == 0)
        return 1;
    else if (strcmp(extname, ".tif") == 0)
        return 1;
    else if (strcmp(extname, ".tiff") == 0)
        return 1;
    else if (strcmp(extname, ".ico") == 0)
        return 1;
    else
        return 0;
}

int is_audio(char *extname) {
    if (strcmp(extname, ".mp3") == 0)
        return 1;
    else if (strcmp(extname, ".m4a") == 0)
        return 1;
    else if (strcmp(extname, ".wma") == 0)
        return 1;
    else if (strcmp(extname, ".wax") == 0)
        return 1;
    else if (strcmp(extname, ".wav") == 0)
        return 1;
    else if (strcmp(extname, ".ogg") == 0)
        return 1;
    else
        return 0;
}

int is_video(char *extname) {
    if (strcmp(extname, ".mp4") == 0)
        return 1;
    else if (strcmp(extname, ".mpeg") == 0)
        return 1;
    else if (strcmp(extname, ".mpg") == 0)
        return 1;
    else if (strcmp(extname, ".mpe") == 0)
        return 1;
    else if (strcmp(extname, ".avi") == 0)
        return 1;
    else if (strcmp(extname, ".flv") == 0)
        return 1;
    else if (strcmp(extname, ".ogv") == 0)
        return 1;
    else if (strcmp(extname, ".wmv") == 0)
        return 1;
    else if (strcmp(extname, ".wmx") == 0)
        return 1;
    else if (strcmp(extname, ".wvx") == 0)
        return 1;
    else
        return 0;
}

void cgi_env_parse(char *header, int len, struct cgi_env *env) {
    char *buf = malloc(len);
    char *arr[128];
    for (int i=0; i<128; i++)
        arr[i] = 0;
    char *substr;
    memcpy(buf, header, len);
    split(arr, buf, "\x0d\x0a");
    int i;
    for(i=0; arr[i]!=0;i++) {
        if (strstr(arr[i], "GET") != NULL || strstr(arr[i], "POST") != NULL) {
            char *arr2[8];
            split(arr2, arr[i], " ");
            env->request_method = malloc(strlen(arr2[0]));
            strcpy(env->request_method, arr2[0]);
            env->url = malloc(strlen(arr2[1]));
            strcpy(env->url, arr2[1]);
            if ((substr = strstr(arr2[1], "?")) !=NULL){
                env->querystr = malloc(strlen(substr+1));
                strcpy(env->querystr, substr+1);
            }
            else {
                env->url = malloc(strlen(arr2[1]));
                strcpy(env->url, arr2[1]);
            }
        }
        if (strstr(arr[i], "Content-Type:") != NULL) {
            char *arr4[2];
            split(arr4, arr[i], " ");
            env->content_type = malloc(strlen(arr4[1]));
            strcpy(env->content_type, arr4[1]);
        }
        if (strstr(arr[i], "Content-Length:") != NULL) {
            char *arr5[2];
            split(arr5, arr[i], " ");
            env->content_len = atoi(arr5[1]);
        }
    }
    if (env->content_len != 0) {
        env->content = malloc(strlen(arr[i-1]));
        strcpy(env->content, arr[i-1]);
    }
    free(buf);
}

void set_cgi_env(struct cgi_env env) {
    if (env.request_method != 0) {
        char *REQUEST_METHOD = malloc(strlen(env.request_method)+strlen("REQUEST_METHOD="));
        strcpy(REQUEST_METHOD, "REQUEST_METHOD=");
        strcat(REQUEST_METHOD, env.request_method);
        putenv(REQUEST_METHOD);
        free(REQUEST_METHOD);
    }

    if (env.querystr != 0) {
        char *QUERY_STRING = malloc(strlen(env.querystr)+strlen("QUERY_STRING="));
        strcpy(QUERY_STRING, "QUERY_STRING=");
        strcat(QUERY_STRING, env.querystr);
        putenv(QUERY_STRING);
        free(QUERY_STRING);
    }
    
    if (env.url != 0) {
        char *URL = malloc(strlen(env.url)+strlen("REQUEST_URI="));
        strcpy(URL, "REQUEST_URI=");
        strcat(URL, env.url);
        putenv(URL);
        free(URL);
    }

    if (env.content_type != 0) {
        char *CONTENT_TYPE = malloc(strlen(env.content_type)+strlen("CONTENT_TYPE="));
        strcpy(CONTENT_TYPE, "CONTENT_TYPE=");
        strcat(CONTENT_TYPE, env.content_type);
        putenv(CONTENT_TYPE);
        free(CONTENT_TYPE);
    }
}
