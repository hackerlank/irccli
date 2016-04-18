#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcre.h>

#include "util.h"
#include "sock_util.h"

void usage(char *prog) {
	fprintf(stderr, "Usage: %s server:port nickname [username] [realname]\n", prog);
	exit(0);
}

int irc_parse(char src[512], char ***output) {
	// Lovingly used from
	// https://mybuddymichael.com/writings/a-regular-expression-for-irc-messages.html
	char *regex = "^(?:[:](\\S+) )?(\\S+)(?: (?!:)(.+?))?(?: [:](.+))?$";
	pcre *compiled;
	pcre_extra *extra;
	const char *error;
	int error_offset;
	int pcre_result;
	int size = 30;
	int groups[size];
	const char *group;

	compiled = pcre_compile(regex, 0, &error, &error_offset, NULL);
	if (compiled == NULL) {
		// printf("ERROR: Could not compile '%s': %s\n", regex, error);
		return 0;
	}

	extra = pcre_study(compiled, 0, &error);
	if(error != NULL) {
		// printf("ERROR: Could not study '%s': %s\n", regex, error);
		return 0;
	}

    pcre_result = pcre_exec(
    	compiled,
        extra,
        src,
        strlen(src),     // Length of string
        0,               // Start looking at this point
        0,               // OPTIONS
        groups,
        size             // Length of subStr
	);

	if (pcre_result < 0) {
		// printf("ERROR: PCRE failed.\n");
		return 0;
	}
	else if(pcre_result == 0) {
		// printf("ERROR: Too many substrings.\n");
		return 0;
	}

	*output = calloc(pcre_result, sizeof(char *));
	for (int i = 0; i < pcre_result; i++) {
        pcre_get_substring(src, groups, pcre_result, i, &(group));

		char *buff = malloc(strlen(group)+1);
		strncpy(buff, group, strlen(group)+1);
		(*output)[i] = buff;
	}

	pcre_free_substring(group);
	pcre_free(compiled);

	if(extra != NULL)
		pcre_free(extra);

	return 1;
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
	char **output;
	char *prefix, *type, *dest, *msg;
	char *ping = "PING";
	char pong[512];
	int print;
	while ( (n = read(sockfd, buffer, 511)) > 0 ) {
		print = 1;
		if (irc_parse(buffer, &output)) {
			prefix = output[1];
			type   = output[2];
			dest   = output[3];
			msg    = output[4];

			if (strcmp(type, ping) == 0) {
				print = 0;
				memset(pong, 0, 512);
				sprintf(pong, "PONG :%s", msg);
				sockwrite(sockfd, pong);
			}
		}

		if (print)
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
