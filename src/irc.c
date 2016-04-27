#include "irc.h"

void irc_nick(char *_nick) {
	strncpy(nick, _nick, sizeof(nick));
	char nick_msg[512];
	snprintf(nick_msg, sizeof(nick_msg), "NICK %s\r\n", nick);
	write_socket(nick_msg);
}

void irc_user(char *_user, char *_real) {
	strncpy(user, _user, sizeof(user));
	strncpy(real, _real, sizeof(real));
	char user_msg[512];
	snprintf(user_msg, sizeof(user_msg), "USER %s 0 * :%s\r\n", user, real);
	write_socket(user_msg);
}

int irc_parse(char src[512], char ***output) {
	// Lovingly used from
	// https://mybuddymichael.com/writings/a-regular-expression-for-irc-messages.html
	// (slightly modified)
	const char *regex = "^(?:[:](\\S+) )?(\\S+)(?: (?!:)(.+?))?(?: (?!:)(.+?))?(?: [:](.+))?$";
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
		// rl_printf("ERROR: Could not compile '%s': %s\n", regex, error);
		return 0;
	}

	extra = pcre_study(compiled, 0, &error);
	if (error != NULL) {
		// rl_printf("ERROR: Could not study '%s': %s\n", regex, error);
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
		// switch(pcre_result) {
		// 	case PCRE_ERROR_NOMATCH      : rl_printf("String did not match the pattern\n");        break;
		// 	case PCRE_ERROR_NULL         : rl_printf("Something was null\n");                      break;
		// 	case PCRE_ERROR_BADOPTION    : rl_printf("A bad option was passed\n");                 break;
		// 	case PCRE_ERROR_BADMAGIC     : rl_printf("Magic number bad (compiled re corrupt?)\n"); break;
		// 	case PCRE_ERROR_UNKNOWN_NODE : rl_printf("Something kooky in the compiled re\n");      break;
		// 	case PCRE_ERROR_NOMEMORY     : rl_printf("Ran out of memory\n");                       break;
		// 	default                      : rl_printf("Unknown error\n");                           break;
		// }
		return 0;
	}
	else if (pcre_result == 0) {
		// rl_printf("ERROR: Too many substrings.\n");
		return 0;
	}

	*output = calloc(pcre_result, 512 * sizeof(char *));
	for (int i = 0; i < pcre_result; i++) {
		pcre_get_substring(src, groups, pcre_result, i, &(group));

		(*output)[i] = malloc( (strlen(group)+1) * sizeof(char) );
		strncpy((*output)[i], group, strlen(group)+1);
	}

	pcre_free_substring(group);
	pcre_free(compiled);

	if (extra != NULL)
		pcre_free(extra);

	return pcre_result;
}

int irc_receive(char *buffer) {
	char **output;
	char *prefix, *type, *dest, *middle, *msg;
	int print = 1;
	int retval = 1;

	if (buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = 0;

	int r = irc_parse(buffer, &output);
	if (r > 3) { // Must at least have dest, which is third match
		prefix = output[1];
		type   = output[2];
		dest   = output[3];
		middle = (r > 4) ? output[4] : "";
		msg    = (r > 5) ? output[5] : "";

		if (strcmp(type, "PING") == 0) {
			print = 0;
			char pong[512];
			snprintf(pong, sizeof(pong), "PONG :%s\r\n", msg);
			write_socket(pong);
		}

		if (strcmp(dest, "*") == 0) {
			// Color
		}
		else if (strcmp(dest, nick) == 0) {
			// Another color
		}
		// Channels names are strings (beginning with a '&' or '#' character)
		// (https://tools.ietf.org/html/rfc1459#section-1.3)
		else if (dest[0] == '#' || dest[0] == '&') {
			if (strcmp(dest, current_channel) == 0) {
				print = 0;
				// Color for current channel
			}
			else {
				// Log the chat (in a file?)
				// Read from and display that log when user switches /channel <channel>
			}
		}

		if (print) {
			time_t rawtime;
			struct tm * timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);
			char out_time[16];
			// Color the time?
			snprintf(out_time, sizeof(out_time), "[%02d:%02d]", timeinfo->tm_hour, timeinfo->tm_min);

			if (strlen(middle) > 0) {
				if (strlen(msg) > 0)
					rl_printf("%s %s :%s\n", out_time, middle, msg);
				else
					rl_printf("%s %s\n", out_time, middle);
			}
			else if (strlen(msg) > 0) {
				rl_printf("%s %s\n", out_time, msg);
			}
		}
	}
	else {
		destroy_prompt();
		rl_printf("Error parsing message from irc server\n");
		retval = 0;
	}

	// Free output
	char **p;
	for (p = output; *p; p++)
		free(*p);
	free(output);

	return retval;
}

int irc_send(char *buffer) {
	char send[512];
	char **output;
	char *dest;
	int r = irc_parse(buffer, &output);
	char *channel;
	int retval = 1;

	if (buffer[0] == '/') {
		buffer++;

		char *buffcpy, *tofree;
		tofree = buffcpy = malloc(512);
		strncpy(buffcpy, buffer, 512);
		char *command;

		command = strsep(&buffcpy, " ");
		for (int i = 0; command[i]; i++)
			command[i] = tolower(command[i]);

		if (strcmp(command, "help") == 0) {
			printf("\
Commands available:\n\
\n\
/help                  Shows this help message\n\
/join <channel>        Joins a channel\n\
/part [<channel>]      Leaves a specified channel\n\
/quit                  Closes the connection with the server and quits\n\
/msg <user> <message>  Sends a private message to a user\n\
/names [<channel>]     Lists the users in a specified channel\n\
/channel <channel>     Changes the current output channel\n\
");
		}
		else if (strcmp(command, "join") == 0) {
			if (r > 3) {
				dest = output[3];
				channel = strsep(&dest, ",");

				memset(current_channel, 0, sizeof(current_channel));
				strncpy(current_channel, channel, sizeof(current_channel));
			}
			else {
				printf("Usage: /join <channel>, Joins a channel\n");
			}
			snprintf(send, sizeof(send), "%s\r\n", buffer);
			write_socket(send);
		}
		else if (strcmp(command, "part") == 0) {
			if (r > 3) {
				dest = output[3];
				snprintf(send, sizeof(send), "%s\r\n", buffer);
				if (strcmp(dest, current_channel) == 0) {
					memset(current_channel, 0, sizeof(current_channel));
				}
			}
			else {
				snprintf(send, sizeof(send), "PART %s\r\n", current_channel);
				memset(current_channel, 0, sizeof(current_channel));
			}
			write_socket(send);
		}
		else if (strcmp(command, "quit") == 0) {
			snprintf(send, sizeof(send), "QUIT\r\n");
			write_socket(send);
			retval = 0;
		}
		else if (strcmp(command, "names") == 0) {
			if (r > 3) {
				dest = output[3];
				channel = strsep(&dest, ",");
			}
			else {
				channel = current_channel;
			}

			if (channel[0]) {
				snprintf(send, sizeof(send), "NAMES %s\r\n", channel);
				write_socket(send);
			}
			else {
				printf("No channel joined. Try /join #<channel>\n");
			}
		}
		else {
			printf("%s :Unknown command\n", command);
		}

		free(tofree);
	}
	else {
		if (current_channel[0]) {
			snprintf(send, sizeof(send), "PRIVMSG %s :%s\r\n", current_channel, buffer);
			write_socket(send);
		}
		else {
			printf("No channel joined. Try /join #<channel>\n");
		}
	}

	// Free output
	char **p;
	for (p = output; *p; p++)
		free(*p);
	free(output);

	return retval;
}
