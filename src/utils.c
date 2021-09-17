/**
 * @author      : Enno Boland (mail@eboland.de)
 * @file        : utils
 * @created     : Saturday Jun 05, 2021 20:20:07 CEST
 */

#include "utils.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

uint32_t
squash_log2_u32(uint32_t x) {
	return sizeof(uint32_t) * 8 - 1 - __builtin_clz(x);
}

uint32_t
squash_divide_ceil_u32(uint32_t x, uint32_t y) {
	return x / y + (x % y != 0);
}

int
squash_memdup(char **target, const char *source, size_t size) {
	*target = calloc(size + 2, sizeof(char));
	if (*target == NULL) {
		return -SQUASH_ERROR_MALLOC_FAILED;
	}
	memcpy(*target, source, size);

	return size;
}
