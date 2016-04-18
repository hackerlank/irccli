#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "sock_util.h"

void usage(char *prog) {
	fprintf(stderr, "Usage: %s server:port nickname [username] [realname]\n", prog);
	exit(0);
}

int main(int argc, char **argv) {
	if (argc < 3)
		usage(argv[0]);

	// Split server:port
	char **serv_port = calloc(2, sizeof(char *));
	char *token;
	int i = 0;
	while ( (token = strsep(&argv[1], ":")) ) {
		serv_port[i++] = token;
	}
	if (i != 2) {
		fprintf(stderr, "Incorrect server and port argument.\n");
		usage(argv[0]);
	}

	char *endptr;
	int portno = strtol(serv_port[1], &endptr, 10);
	if (*endptr) {
		fprintf(stderr, "Incorrect port.\n");
		usage(argv[0]);
	}

	int sockfd;
	connect_socket(&sockfd, serv_port[0], portno);
	free(serv_port);

	// Assign messages to send to server
	char nick_msg[512], user_msg[512];
	char *nick = argv[2];
	char *user = (argc < 4) ? nick : argv[3];
	char *real = (argc < 5) ? nick : argv[4];
	sprintf(nick_msg, "NICK %s\r\n", nick);
	sprintf(user_msg, "USER %s 0 * :%s\r\n", user, real);

	// Communicate with the server
	sockwrite(sockfd, nick_msg);
	sockwrite(sockfd, user_msg);

	int n;
	char buffer[512];
	while ( (n = read(sockfd, buffer, 511)) > 0 ) {
		printf("%s", buffer);
		memset(buffer, 0, 512);
	}
	if (n < 0) {
		error("Error reading from socket");
	}

	// Close the socket
	close(sockfd);

	return 0;
}
