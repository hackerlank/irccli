#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg) {
	perror(msg);
	exit(1);
}

void usage(char *prog) {
	fprintf(stderr, "Usage: %s server:port nickname [username] [realname]\n", prog);
	exit(0);
}

void sockwrite(int sockfd, char msg[512]) {
	int n = write(sockfd, msg, strlen(msg));
	if (n < 0)
		error("Error writing to socket");
}

int main(int argc, char **argv) {
	int sockfd, portno, n;
	struct sockaddr_in6 serv_addr;
	struct hostent *server;
	char nick_msg[512], user_msg[512];

	if (argc < 3)
		usage(argv[0]);

	// Split server:port
	char **serv_port = (char **) calloc(32, sizeof(char *));
	char *token;
	int i = 0;
	while ( (token = strsep(&argv[1], ":")) ) {
		serv_port[i++] = token;
	}

	if (i != 2) {
		fprintf(stderr, "Incorrect server and port.\n");
		usage(argv[0]);
	}
	char *endptr;
	portno = strtol(serv_port[1], &endptr, 10);
	if (*endptr) {
		fprintf(stderr, "Incorrect port.\n");
		usage(argv[0]);
	}

	// Assign messages to send to server
	char *nick = argv[2];
	char *user = (argc < 4) ? nick : argv[3];
	char *real = (argc < 5) ? nick : argv[4];
	sprintf(nick_msg, "NICK %s\r\n", nick);
	sprintf(user_msg, "USER %s 0 * :%s\r\n", user, real);

	// Create the socket
	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("Error opening socket");
	}

	// Set the fields in serv_addr
	server = gethostbyname2(serv_port[0], AF_INET6);
	if (server == NULL) {
		fprintf(stderr, "Error: no such host\n");
		return 0;
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin6_flowinfo = 0;
	serv_addr.sin6_family = AF_INET6;
	memmove(
		(char *) &serv_addr.sin6_addr.s6_addr,
		(char *) server->h_addr,
		server->h_length
	);
	serv_addr.sin6_port = htons(portno);

	// Connect the socket
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Error connecting");

	// Communicate with the server
	sockwrite(sockfd, nick_msg);
	sockwrite(sockfd, user_msg);

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
