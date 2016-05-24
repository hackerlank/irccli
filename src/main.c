#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>

#include "util.h"
#include "xterm.h"
#include "sock_util.h"
#include "irc.h"
#include "encode.h"

static int loop = 1;

void handle_line(char *line) {
	// Quit the program
	if (!line) {
		rl_callback_handler_remove();
		loop = 0;
	}
	else if (*line) {
		add_history(line);
		if (!irc_send(line))
			loop = 0;
	}
}

int main(int argc, char **argv) {
	char *prog = argv[0];
	char usage[1024];
	snprintf(usage, sizeof(usage), "\
Usage:  %s [options] server[:port] nickname [username] [realname]\n\
\n\
Options:\n\
  -h,  --help                   Print this help\n\
  -e,  --set-encoding ENCODING  Select encoding (\"--set-encoding help\" for a list)\n\
  -l,  --log                    Keep logs of all channel output\
", prog);

	int keep_logs = 0;

	int c;
	while (1) {
		static struct option long_options[] = {
			// Options that set a flag
			// E.g., {"verbose", no_argument, &verbose_flag, 1},
			// These options don't set a flag
			{"help",          no_argument,       0, 'h'},
			{"log",           no_argument,       0, 'l'},
			{"set-encoding",  required_argument, 0, 'e'},
			{0, 0, 0, 0}
		};
		int option_index = 0;

		c = getopt_long (argc, argv, "hle:", long_options, &option_index);
		if (c == -1) // End of options
			break;
		switch (c) {
			case 0:
				break;
			case 'h':
				printf("%s\n", usage);
				return 0;
			case 'l':
				keep_logs = 1;
				break;
			case 'e': {
				int osize = sizeof(optarg);
				char optargcpy[osize+1];
				strncpy(optargcpy, optarg, osize);
				optargcpy[osize] = 0;
				for (int i = 0; optargcpy[i]; i++)
					optargcpy[i] = tolower(optargcpy[i]);
				// Check if user typed "help"
				if (strcmp(optargcpy, "help") == 0) {
					printf("%s\n", genc());
					return 0;
				}

				enc(optarg);
				break;
			}
			default:
				error("Error parsing options");
		}
	}

	char *serv_port_arg = "",
	     *nick = "",
	     *user = "",
	     *real = "";

	if (optind < argc) {
		int i = 0;
		while (optind < argc) {
			switch (++i) {
				case 1:
					serv_port_arg = argv[optind];
					break;
				case 2:
					nick = argv[optind];
					break;
				case 3:
					user = argv[optind];
					break;
				case 4:
					real = argv[optind];
					break;
				default:
					printf("%s\n", usage);
					return 0;
			}
			optind++;
		}
		if (i < 2) {
			printf("%s\n", usage);
			return 0;
		}
	}
	else {
		printf("%s\n", usage);
		return 0;
	}

	// Split server:port
	char **serv_port = calloc(2, sizeof(char *));
	char *token;
	int i = 0;
	int portno = 0;
	while ( (token = strsep(&serv_port_arg, ":")) ) {
		serv_port[i++] = token;
	}
	if (i != 2) {
		if (i == 1) {
			portno = 6667;
		}
		else {
			fprintf(stderr, "Incorrect server and port argument.\n");
			printf("%s\n", usage);
			free(serv_port);
			return 0;
		}
	}

	if (!portno) {
		char *endptr;
		portno = strtol(serv_port[1], &endptr, 10);
		if (*endptr) {
			fprintf(stderr, "Incorrect port.\n");
			printf("%s\n", usage);
			free(serv_port);
			return 0;
		}
	}

	// Connect to the server
	connect_socket(serv_port[0], portno);

	if (!*user)
		user = nick;
	if (!*real)
		real = nick;
	// Initialize irc (communicate with the irc server)
	if (!irc_init(serv_port[0], keep_logs, nick, user, real)) {
		free(serv_port); // Free serv_port
		return 0;
	}

	// Free serv_port
	free(serv_port);


	// Output variables
	int n;
	char buffer[512+1];
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
			buffer[n] = 0; // Safe because read_socket returns at most 512

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
					if (*token) {
						unencode(&token, 512);
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
				tofree = buffcpy = malloc(512+1);
				strncpy(buffcpy, buffer, 512+1);
				while ( (token = strsep(&buffcpy, "\n")) ) {
					if (*token) {
						unencode(&token, 512);
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
	if (galt()) {
		printf("%s", xget("rmcup"));
		fflush(stdout);
	}

	irc_clean();      // Clean irc variables
	destroy_prompt(); // Readline cleanup
	close_socket();   // Close the socket

	return 0;
}
