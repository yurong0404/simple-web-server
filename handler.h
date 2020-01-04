void static_object_handler(char *path, struct result *rst);
void directory_handler(char *path, struct result *rst);
void CGI_handler();
void handler_301(char *path, struct result *rst);
void handler_403(struct result *rst);
void handler_404(struct result *rst);