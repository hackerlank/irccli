#include "util.h"

void error(char *msg) {
	destroy_prompt();
	perror(msg);
	exit(1);
}

void rl_printf(char *fmt, ...) {
	int need_hack = rl_readline_state > 0;
	char *saved_line;
	int saved_point;
	if (need_hack) {
		saved_point = rl_point;
		saved_line = rl_copy_text(0, rl_end);
		rl_save_prompt();
		rl_replace_line("", 0);
		rl_redisplay();
	}

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	if (need_hack) {
		rl_restore_prompt();
		rl_replace_line(saved_line, 0);
		rl_point = saved_point;
		rl_redisplay();
		free(saved_line);
	}
}

void destroy_prompt() {
	rl_save_prompt();
	rl_replace_line("", 0);
	rl_redisplay();
	rl_callback_handler_remove();
}

char *scolor(char *str, const char *color) {
	color = xget(color);
	const char *reset = xget("reset");
	int size = strlen(color) + strlen(str) + strlen(reset) + 1;
	char *result = malloc(size);
	snprintf(result, size, "%s%s%s", color, str, reset);
	return result;
}

uint32_t adler32(const void *buf, size_t buflength) {
	const uint8_t *buffer = (const uint8_t*)buf;

	uint32_t s1 = 1;
	uint32_t s2 = 0;

	for (size_t n = 0; n < buflength; n++) {
		s1 = (s1 + buffer[n]) % 65521;
		s2 = (s2 + s1) % 65521;
	}
	return (s2 << 16) | s1;
}
