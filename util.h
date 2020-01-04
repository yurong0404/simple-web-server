#include <dirent.h>
#include "header.h"
void split(char **arr, char *str, const char *del);
int is_directory(char *path);
int is_object(char *path);
void read_directory(char *buf, char *dirname, int bufsz);
void create_html(char *html, char *text);
int index_html_exist(DIR *dir);
void create_index_html(char *path, struct result *rst);
int is_plain_text(char *path);
int is_html(char *extname);
int is_image(char *extname);
int is_audio(char *extname);
int is_video(char *extname);