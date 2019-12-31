#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
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
}