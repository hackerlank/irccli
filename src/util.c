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
