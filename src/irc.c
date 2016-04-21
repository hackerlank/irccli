#include "irc.h"

int irc_parse(char src[512], char ***output) {
	// Lovingly used from
	// https://mybuddymichael.com/writings/a-regular-expression-for-irc-messages.html
	// (slightly modified)
	char *regex = "^(?:[:](\\S+) )?(\\S+)(?: (?!:)(.+?))?(?: (?!:)(.+?))?(?: [:](.+))?$";
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

void irc_receive(char *buffer) {
	char **output;
	char *prefix, *type, *dest, *middle, *msg;
	int print = 1;

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

		if (print) {
			time_t rawtime;
			struct tm * timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);
			char out_time[32];
			snprintf(out_time, sizeof(out_time), "[%d:%d]", timeinfo->tm_hour, timeinfo->tm_min);

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
		exit(0);
	}
}

void irc_send(char *buffer) {
	char send[512];
	snprintf(send, sizeof(send), "%s\r\n", buffer);
	write_socket(send);
}
