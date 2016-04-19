#include "irc.h"

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

	free(*output);
	*output = calloc(pcre_result, sizeof(char *));
	for (int i = 0; i < pcre_result; i++) {
		pcre_get_substring(src, groups, pcre_result, i, &(group));

		(*output)[i] = malloc( (strlen(group)+1) * sizeof(char) );
		strncpy((*output)[i], group, strlen(group)+1);
	}

	pcre_free_substring(group);
	pcre_free(compiled);

	if(extra != NULL)
		pcre_free(extra);

	return 1;
}
