#include "sock_util.h"

static int sockfd;
static const char *encoding = "";

int getsockfd() {
	return sockfd;
}

int read_socket(char buffer[512+1]) {
	int n = read(sockfd, buffer, 512);

	if (n < 0)
		error("Error reading from socket");

	return n;
}

void write_socket(char msg[512]) {
	if (*encoding) {
		////////  Convert from unicode to encoding  ////////
		iconv_t cd = iconv_open(encoding, "UTF-8");
		char buffer[512];
		char *outptr = (char *) &buffer[0];
		size_t insize  = 512;
		size_t outsize = 512;
		iconv(cd, &msg, &insize, &outptr, &outsize);
		msg = buffer;
	}

	int n = write(sockfd, msg, strlen(msg));
	if (n < 0)
		error("Error writing to socket");
}

void connect_socket(char *server_name, int portno) {
	struct hostent *server;

	// Try using IPv6 first
	int ipv6_success = 1;
	// Create socket
	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0)
		ipv6_success = 0;

	server = gethostbyname2(server_name, AF_INET6);
	if (server == NULL)
		ipv6_success = 0;

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

		// Connect the socket
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			ipv6_success = 0;
		}
	}

	// IPv6 failed, try using IPv4
	if (!ipv6_success) {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			error("Error opening socket");
		}

		server = gethostbyname(server_name);
		if (server == NULL) {
			fprintf(stderr, "Error: no such host\n");
			exit(0);
		}

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
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("Error connecting");
	}
}

void encode_socket(const char *enc) {
	encoding = enc;
}

void close_socket() {
	close(sockfd);
}
