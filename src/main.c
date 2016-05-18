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
#include "xterm.h"
#include "sock_util.h"
#include "irc.h"

static int loop = 1;

void usage(char *prog) {
	fprintf(stderr, "Usage: %s <server>[:<port>] <nickname> [<username>] [<realname>]\n", prog);
}

void handle_line(char *line) {
	// Quit the program
	if (!line) {
		rl_callback_handler_remove();
		loop = 0;
	}
	else if (strcmp(line, "") != 0) { ///////////////////////// *line
		add_history(line);
		if (!irc_send(line))
			loop = 0;
	}
}

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argv[0]);
		return 0;
	}

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
			free(serv_port);
			return 0;
		}
	}

	if (!portno) {
		char *endptr;
		portno = strtol(serv_port[1], &endptr, 10);
		if (*endptr) {
			fprintf(stderr, "Incorrect port.\n");
			usage(argv[0]);
			free(serv_port);
			return 0;
		}
	}

	// Connect to the server
	connect_socket(serv_port[0], portno);

	// Free serv_port
	free(serv_port);

	// Communicate with the irc server
	char *nick = argv[2];
	char *user = (argc < 4) ? nick : argv[3];
	char *real = (argc < 5) ? nick : argv[4];
	irc_nick(nick);
	irc_user(user, real);


	// Output variables
	int n;
	char buffer[512];
	char *buffsave;
	size_t buffsavesize = sizeof(buffsave);

	// Select() variables
	fd_set fds;
	int retval;

	// Readline setup
	using_history(); // Enable history
	const char *prompt = scolor("> ", "blue");
	rl_callback_handler_install(prompt, &handle_line);
	rl_bind_key ('\t', rl_insert); // Disable autocomplete

	// Main program loop
	while (loop) {
		FD_ZERO(&fds);
		FD_SET(0, &fds); // 0 is stdin
		FD_SET(getsockfd(), &fds);

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
		if (FD_ISSET(getsockfd(), &fds)) {
			n = read_socket(buffer);
			buffer[n] = 0; // Safe because read_socket returns at most 511

			// Received in the middle of a line, so store in another buffer
			if (buffer[n - 1] != '\n') {
				if (buffsavesize > sizeof(char *)) {
					buffsavesize += (strlen(buffer)+1) * sizeof(char);
					buffsave = realloc(buffsave, buffsavesize);

					// Add buffer's contents to the saved buffer
					memmove(buffsave+strlen(buffsave), buffer, strlen(buffer)+1);
				}
				else {
					buffsavesize += (strlen(buffer)+1) * sizeof(char);
					buffsave = malloc(buffsavesize);

					// Add buffer's contents to the saved buffer
					memmove(buffsave, buffer, strlen(buffer)+1);
				}
			}
			// Received complete line(s) now, so print saved buffer (first
			// add the last part just received)
			else if (buffsavesize > sizeof(char *)) {
				buffsavesize += (strlen(buffer)+1) * sizeof(char);
				char *tofree;
				tofree = buffsave = realloc(buffsave, buffsavesize);
				memmove(buffsave+strlen(buffsave), buffer, strlen(buffer)+1);

				// Handle each line
				while ( (token = strsep(&buffsave, "\n")) ) {
					if (strlen(token)) {
						if (!irc_receive(token, 1)) {
							loop = 0;
							break;
						}
					}
				}

				free(tofree);
				buffsavesize = sizeof(buffsave);
			}
			// Received complete line(s)
			else {
				// Handle each line
				char *buffcpy, *tofree;
				tofree = buffcpy = malloc(512);
				strncpy(buffcpy, buffer, 512);
				while ( (token = strsep(&buffcpy, "\n")) ) {
					if (strlen(token)) {
						if (!irc_receive(token, 1)) {
							loop = 0;
							break;
						}
					}
				}
				free(tofree);
			}

			memset(buffer, 0, sizeof(buffer));
		}
	}

	// Cleanup
	if (buffsavesize > sizeof(char *)) {
		free(buffsave);
	}

	// Always switch back to normal screen if entered alternate screen buffer
	if (galt())
		printf("%s", xget("rmcup"));

	irc_clean();      // Clean irc variables
	destroy_prompt(); // Readline cleanup
	close_socket();   // Close the socket

	return 0;
}
