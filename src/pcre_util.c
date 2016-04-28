#include "pcre_util.h"

int re_match(char *src, const char *regex, char ***output) {
	pcre *compiled;
	pcre_extra *extra;
	const char *error;
	int error_offset;
	int pcre_result;
	int size = 30; // Max number of match groups (must be multiple of 3)
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
		size             // Length of groups
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

	*output = calloc(pcre_result, strlen(src)+1);
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
