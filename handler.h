void static_object_handler(char *path, struct result *rst);
void directory_handler(char *path, struct result *rst);
void cgi_handler(char *header, int len, char *path, char *request_method, char *querystr, struct result *rst);
void handler_301(char *path, struct result *rst);
void handler_403(struct result *rst);
void handler_404(struct result *rst);
void header_parser(char *header, int len, char *request_method, char *filename, char *querystr);