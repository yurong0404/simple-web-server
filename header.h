#ifndef HEADER_H
#define HEADER_H
struct result {
	int status_code;
	char *content_type;
	char *location;
	char *content;
	int content_len;
	char *download;
};

void header_decode(char *header, struct result *rst);
int header_encode(char *header, struct result *rst);
void encode_status_code(char *header, struct result *rst);
void encode_content_length(char *header, int len);
void encode_content_type(char *header, char *type);
void encode_content_disposition(char *header, char *download);
void encode_content(char *header, char *content, int len);
void set_result_content_from_file(char *path, struct result *rst);
void set_image_content_type(char *extname, struct result *rst);
void set_audio_content_type(char *extname, struct result *rst);
void set_video_content_type(char *extname, struct result *rst);
#endif