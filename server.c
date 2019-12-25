#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct request {
	/*
	-1, Bad Request
	*/
	char *method;
	char *path;
};

struct result {
	int status_code;
	char *content;
};

void serv_client(int fd, struct sockaddr_in *sin);
void split(char **arr, char *str, const char *del);
void header_decode(char *header, struct request *rin);
void header_encode(char *header, struct result *rst);
void method_handler(struct request *rin, struct result *rst);
void encode_status_code(char *header, int status_code);

int main(int argc, char *argv[]) {
	pid_t pid;
	int fd, pfd;
	unsigned val;
	struct sockaddr_in sin, psin;
	char *root;
	if(argc < 2) {
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return(-1);
	}
	
	root = argv[2];
	
	signal(SIGCHLD, SIG_IGN);
	if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return(-1);
	}
	val = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
		perror("setsockopt");
		return(-1);
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[1]));
	if(bind(fd, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		perror("bind");
		return(-1);
	}
	if(listen(fd, SOMAXCONN) < 0) {	/* SOMAXCONN -> the max number of request in the queue */
		perror("listen");
		return(-1);
	}
	while(1) {
		val = sizeof(psin);
		bzero(&psin, sizeof(psin));
		if((pfd = accept(fd, (struct sockaddr*) &psin, &val))<0) {
			perror("accept");
			return(-1);
		}
		if((pid = fork()) < 0) {
			perror("fork");
			return(-1);
		} else if(pid == 0) {	/* child */
			close(fd);
			serv_client(pfd, &psin);
			exit(0);
		}
		/* parent */
		close(pfd);
	}
}

void serv_client(int fd, struct sockaddr_in *sin) {
	int len;
	char buf[2048];
	struct request rin;
	struct result rst;

	printf("connected from %s:%d\n", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
	while((len = recv(fd, buf, sizeof(buf), 0)) > 0) {
		printf("%s\n", buf);
		header_decode(buf, &rin);
		method_handler(&rin, &rst);
		memset(buf, 0, sizeof(buf));
		header_encode(buf, &rst);

		if(send(fd, buf, strlen(buf), 0) < 0) {
			perror("send");
			exit(-1);
		}
	}
	printf("disconnected from %s:%d\n",
		inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
	return;
}

void split(char **arr, char *str, const char *del) {
	char *s = strtok(str, del);

	while(s != NULL) {
		*arr++ = s;
		s = strtok(NULL, del);
	}
}

void header_decode(char *header, struct request *rin) {
	char *arr[20];
	split(arr, header, " ");
	if (strstr(arr[2], "HTTP") == NULL){
		rin->method = "Bad Requests";
		return;
	}
	if (strcmp(arr[0], "GET") == 0 ){
		rin->method = "GET";
		return;
	}
}

void header_encode(char *header, struct result *rst) {
	encode_status_code(header, rst->status_code);
}

void method_handler(struct request *rin, struct result *rst) {
	if (strcmp(rin->method, "Bad Requests") == 0) {
		rst->status_code = 400;
	}
}

void encode_status_code(char *header, int status_code) {
	if (status_code == 400)
		strcat(header, "HTTP/1.1 400 Bad Requests\x0d\x0a");
}
