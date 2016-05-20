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

// Lovingly used from https://en.wikipedia.org/wiki/MurmurHash
#define ROT32(x, y) ((x << y) | (x >> (32 - y))) // avoid effort
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	uint32_t k;
	for (i = 0; i < nblocks; i++) {
		k = blocks[i];
		k *= c1;
		k = ROT32(k, r1);
		k *= c2;

		hash ^= k;
		hash = ROT32(hash, r2) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = ROT32(k1, r1);
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}
