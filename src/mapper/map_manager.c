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
 * @file         map_manager.c
 */

#include "../../include/sqsh_mapper_private.h"

#include "../../include/sqsh.h"
#include "../../include/sqsh_error.h"
#include "../utils.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void
map_cleanup_cb(void *data) {
	struct SqshMapping *mapping = data;
	sqsh__mapping_unmap(mapping);
}

int
sqsh__map_manager_init(
		struct SqshMapManager *manager, const void *input,
		const struct SqshConfig *config) {
	int rv;
	size_t map_size;

	rv = sqsh__mapper_init(&manager->mapper, input, config);
	if (rv < 0) {
		goto out;
	}

	map_size = SQSH_DIVIDE_CEIL(
			sqsh__mapper_size(&manager->mapper),
			sqsh__mapper_block_size(&manager->mapper));

	rv = sqsh__ref_count_array_init(
			&manager->maps, map_size, sizeof(struct SqshMapping),
			map_cleanup_cb);

out:
	if (rv < 0) {
		sqsh__map_manager_cleanup(manager);
	}
	return rv;
}

uint64_t
sqsh__map_manager_size(const struct SqshMapManager *manager) {
	return sqsh__mapper_size(&manager->mapper);
}

size_t
sqsh__map_manager_block_size(const struct SqshMapManager *manager) {
	return sqsh__mapper_block_size(&manager->mapper);
}

size_t
sqsh__map_manager_block_count(const struct SqshMapManager *manager) {
	return sqsh__ref_count_array_size(&manager->maps);
}

static int
load_mapping(
		struct SqshMapManager *manager, const struct SqshMapping **target,
		sqsh_index_t index, int span) {
	int rv = 0;
	struct SqshMapping mapping = {0};
	assert(span == 1);

	sqsh_index_t offset;
	size_t size = sqsh__mapper_block_size(&manager->mapper);
	if (SQSH_MULT_OVERFLOW(index, size, &offset)) {
		return -SQSH_ERROR_INTEGER_OVERFLOW;
	}

	if (index == sqsh__map_manager_block_count(manager) - 1) {
		size = sqsh__map_manager_size(manager) % size;
	}

	rv = sqsh__mapper_map(&mapping, &manager->mapper, offset, size);
	if (rv < 0) {
		goto out;
	}

	*target = sqsh__ref_count_array_set(&manager->maps, index, &mapping, span);

out:
	return rv;
}

int
sqsh__map_manager_get(
		struct SqshMapManager *manager, sqsh_index_t index, int span,
		const struct SqshMapping **target) {
	int rv = 0;
	assert(span == 1);
	int real_index = index;
	*target = sqsh__ref_count_array_retain(&manager->maps, &real_index);

	if (*target == NULL) {
		rv = load_mapping(manager, target, index, span);
	}
	return rv;
}

int
sqsh__map_manager_release(
		struct SqshMapManager *manager, const struct SqshMapping *mapping) {
	return sqsh__ref_count_array_release(&manager->maps, mapping);
}

int
sqsh__map_manager_cleanup(struct SqshMapManager *manager) {
	sqsh__ref_count_array_cleanup(&manager->maps);
	sqsh__mapper_cleanup(&manager->mapper);

	return 0;
}
