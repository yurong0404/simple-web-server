#ifndef HEADER_H
#define HEADER_H
struct result {
	int status_code;
	char *type;
	char *location;
	char *content;
};

void header_decode(char *header, struct result *rst);
void header_encode(char *header, struct result *rst);
void encode_status_code(char *header, struct result *rst);
void encode_content_length(char *header, char *content);
void encode_content(char *header, char *content);
#endif