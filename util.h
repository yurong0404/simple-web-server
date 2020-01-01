#include <dirent.h>
void split(char **arr, char *str, const char *del);
int check_directory(char *path);
int check_object(char *path);
void read_directory(char *buf, char *dirname, int bufsz);
void create_html(char *html, char *text);
int index_html_exist(DIR *dir);