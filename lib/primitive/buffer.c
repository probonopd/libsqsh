/******************************************************************************
 *                                                                            *
 * Copyright (c) 2023, Enno Boland <g@s01.de>                                 *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions are     *
 * met:                                                                       *
 *                                                                            *
 * * Redistributions of source code must retain the above copyright notice,   *
 *   this list of conditions and the following disclaimer.                    *
 * * Redistributions in binary form must reproduce the above copyright        *
 *   notice, this list of conditions and the following disclaimer in the      *
 *   documentation and/or other materials provided with the distribution.     *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS    *
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *                                                                            *
 ******************************************************************************/

/**
 * @author       Enno Boland (mail@eboland.de)
 * @file         buffer.c
 */

#include "../../include/sqsh_primitive_private.h"

#include "../../include/sqsh_error.h"
#include "../utils/utils.h"

int
sqsh__buffer_init(struct SqshBuffer *buffer) {
	int rv = 0;

	buffer->data = NULL;
	buffer->capacity = buffer->size = 0;

	return rv;
}

int
sqsh__buffer_add_capacity(
		struct SqshBuffer *buffer, uint8_t **additional_buffer,
		size_t additional_size) {
	const size_t buffer_size = buffer->size;
	uint8_t *new_data;
	size_t new_capacity;

	if (SQSH_ADD_OVERFLOW(buffer_size, additional_size, &new_capacity)) {
		return -SQSH_ERROR_INTEGER_OVERFLOW;
	}

	if (new_capacity > buffer->capacity) {
		new_data = realloc(buffer->data, new_capacity);
		if (new_data == NULL) {
			return -SQSH_ERROR_MALLOC_FAILED;
		}
		buffer->data = new_data;
		buffer->capacity = new_capacity;
	}
	if (additional_buffer != NULL) {
		*additional_buffer = &buffer->data[buffer_size];
	}
	return new_capacity;
}

int
sqsh__buffer_add_size(struct SqshBuffer *buffer, size_t additional_size) {
	const size_t buffer_size = buffer->size;
	size_t new_size;
	if (SQSH_ADD_OVERFLOW(buffer_size, additional_size, &new_size)) {
		return -SQSH_ERROR_INTEGER_OVERFLOW;
	}

	buffer->size = new_size;
	return 0;
}

int
sqsh__buffer_append(
		struct SqshBuffer *buffer, const uint8_t *source, const size_t size) {
	int rv = 0;
	uint8_t *additional_buffer;

	if (size <= 0) {
		return 0;
	}

	rv = sqsh__buffer_add_capacity(buffer, &additional_buffer, size);
	if (rv < 0) {
		return rv;
	}

	memcpy(additional_buffer, source, size);
	rv = sqsh__buffer_add_size(buffer, size);
	return rv;
}

int
sqsh__buffer_move(struct SqshBuffer *buffer, struct SqshBuffer *source) {
	sqsh__buffer_cleanup(buffer);

	memcpy(buffer, source, sizeof(struct SqshBuffer));
	memset(source, 0, sizeof(struct SqshBuffer));

	return 0;
}

const uint8_t *
sqsh__buffer_data(const struct SqshBuffer *buffer) {
	return buffer->data;
}
size_t
sqsh__buffer_size(const struct SqshBuffer *buffer) {
	return buffer->size;
}

int
sqsh__buffer_cleanup(struct SqshBuffer *buffer) {
	free(buffer->data);
	buffer->data = NULL;
	buffer->size = buffer->capacity = 0;
	return 0;
}
