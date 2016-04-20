#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"
#include "sock_util.h"
#include "irc.h"

static int CONTINUE_LOOP = 1;
static int sockfd;

void usage(char *prog) {
	fprintf(stderr, "Usage: %s server[:port] nickname [username] [realname]\n", prog);
	exit(0);
}

void parse_msg(char *line) {
	// Quit the program
	if (!line) {
		rl_callback_handler_remove();
		CONTINUE_LOOP = 0;
	}
	else {
		add_history(line);
		char buffer[512];
		snprintf(buffer, sizeof(buffer), "%s\r\n", line);
		sockwrite(sockfd, buffer);
	}
}

int main(int argc, char **argv) {
	if (argc < 3)
		usage(argv[0]);

	// Split server:port
	char **serv_port = calloc(2, sizeof(char *));
	char *token;
	int i = 0;
	int portno = 0;
	while ( (token = strsep(&argv[1], ":")) ) {
		serv_port[i++] = token;
	}
	if (i != 2) {
		if (i == 1) {
			portno = 6667;
		}
		else {
			fprintf(stderr, "Incorrect server and port argument.\n");
			usage(argv[0]);
		}
	}

	if (!portno) {
		char *endptr;
		portno = strtol(serv_port[1], &endptr, 10);
		if (*endptr) {
			fprintf(stderr, "Incorrect port.\n");
			usage(argv[0]);
		}
	}

	connect_socket(&sockfd, serv_port[0], portno);
	free(serv_port);

	// Assign messages to send to server
	char nick_msg[512], user_msg[512];
	char *nick = argv[2];
	char *user = (argc < 4) ? nick : argv[3];
	char *real = (argc < 5) ? nick : argv[4];
	snprintf(nick_msg, sizeof(nick_msg), "NICK %s\r\n", nick);
	snprintf(user_msg, sizeof(user_msg), "USER %s 0 * :%s\r\n", user, real);

	// Communicate with the server
	sockwrite(sockfd, nick_msg);
	sockwrite(sockfd, user_msg);


	// Parsing variables
	char **output;
	char *prefix, *type, *dest, *msg;
	char pong[512];

	// Output variables
	int n;
	char buffer[512];
	char *buffsave;
	int buffsavesize = sizeof(buffsave);
	int print;

	// Select() variables
	fd_set fds;
	struct timeval tv;
	int retval;

	// Readline setup
	using_history();
	const char *prompt = "> ";
	rl_callback_handler_install(prompt, &parse_msg);

	// Main program loop
	while (CONTINUE_LOOP) {
		FD_ZERO(&fds);
		FD_SET(0, &fds); // 0 is stdin
		FD_SET(sockfd, &fds);

		retval = select(FD_SETSIZE, &fds, NULL, NULL, NULL);

		if (retval == -1) {
			error("Error with select()");
		}
		else if (!retval) {
			continue;
		}

		// Stdin received input
		if (FD_ISSET(0, &fds)) {
			rl_callback_read_char();
		}
		// Socket received input
		if (FD_ISSET(sockfd, &fds)) {
			n = read(sockfd, buffer, 512);
			if (n > 0) {
				buffer[n] = 0;
				print = 1;
				if (irc_parse(buffer, &output)) {
					prefix = output[1];
					type   = output[2];
					dest   = output[3];
					msg    = output[4];

					if (strcmp(type, "PING") == 0) {
						print = 0;
						memset(pong, 0, 512);
						sprintf(pong, "PONG :%s\r\n", msg);
						sockwrite(sockfd, pong);
					}
					// else {
					// 	rl_printf("%s\n", msg);
					// 	// This doesn't always work because sometimes the
					// 	// input is mulitple lines; need to fix this later
					// }
				}

				if (print) {
					// Not yet received full line so store in another buffer
					if (buffer[n - 1] != '\n') {
						if (buffsavesize > sizeof(char *)) {
							buffsavesize += (strlen(buffer)+1) * sizeof(char);
							buffsave = realloc(buffsave, buffsavesize);
						}
						else {
							buffsavesize += (strlen(buffer)+1) * sizeof(char);
							buffsave = malloc(buffsavesize);
						}
						// Add buffer's contents to the saved buffer
						memmove(buffsave+strlen(buffsave), buffer, strlen(buffer)+1);
					}
					// Received full line now, so print saved buffer and
					// the last part just received
					else if (buffsavesize > sizeof(char *)) {
						buffsavesize += (strlen(buffer)+1) * sizeof(char);
						buffsave = realloc(buffsave, buffsavesize);
						memmove(buffsave+strlen(buffsave), buffer, strlen(buffer)+1);

						rl_printf("%s", buffsave);
						free(buffsave);
						buffsavesize = sizeof(buffsave);
					}
					// Received entire line at once
					else {
						rl_printf("%s", buffer);
					}
				}
				memset(buffer, 0, 512);
			}
			else {
				error("Error reading from socket");
			}
		}
	}

	// Cleanup
	free(output);
	if (buffsavesize > sizeof(char *)) {
		free(buffsave);
	}

	// Close the socket
	close(sockfd);

	return 0;
}
