#include "sock_util.h"

void sockwrite(int sockfd, char msg[512]) {
	int n = write(sockfd, msg, strlen(msg));
	if (n < 0)
		error("Error writing to socket");
}

void connect_socket(int *sockfd, char *server_name, int portno) {
	struct hostent *server;

	// Try using IPv6 first
	int ipv6_success = 1;
	// Create socket
	*sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (*sockfd < 0)
		ipv6_success = 0;

	server = gethostbyname2(server_name, AF_INET6);
	if (server == NULL)
		ipv6_success = 0;

	printf("Using IPv%s\n", (ipv6_success) ? "6" : "4");///////

	// IPv6
	if (ipv6_success) {
		struct sockaddr_in6 serv_addr;
		// Set the fields in serv_addr
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin6_flowinfo = 0;
		serv_addr.sin6_family = AF_INET6;
		memmove(
			(char *) &serv_addr.sin6_addr.s6_addr,
			(char *) server->h_addr,
			server->h_length
		);
		serv_addr.sin6_port = htons(portno);

		printf("Connecting...");///////

		// Connect the socket
		if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			// error("Error connecting");
			ipv6_success = 0;
		}

		///////
		if (ipv6_success == 1)
			printf("connected!\n");
		else
			printf("failed.\n");
		///////
	}

	// IPv6 failed, try using IPv4
	if (!ipv6_success) {
		printf("Using IPv4\n");///////

		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (*sockfd < 0) {
			error("Error opening socket");
		}

		server = gethostbyname(server_name);
		if (server == NULL) {
			fprintf(stderr, "Error: no such host\n");
			exit(0);
		}

		printf("%s\n", server->h_name); ///////////

		struct sockaddr_in serv_addr;
		// Set the fields in serv_addr
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		memmove(
			(char *) &serv_addr.sin_addr.s_addr,
			(char *) server->h_addr,
			server->h_length
		);
		serv_addr.sin_port = htons(portno);

		// Connect the socket
		if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("Error connecting");
	}
}
