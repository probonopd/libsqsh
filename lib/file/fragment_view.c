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
 * @file         fragment_view.c
 */

#include "../../include/sqsh_file_private.h"

#include "../../include/sqsh_data_private.h"

#include "../../include/sqsh_archive_private.h"
#include "../../include/sqsh_error.h"
#include "../../include/sqsh_inode_private.h"
#include "../utils/utils.h"
#ifdef __linux__
#	include <alloca.h>
#endif

static int
apply_fragment(
		struct SqshFragmentView *view, const struct SqshArchive *archive,
		const struct SqshInode *inode, const uint8_t *data, size_t data_size) {
	const struct SqshSuperblock *superblock = sqsh_archive_superblock(archive);
	uint32_t block_size = sqsh_superblock_block_size(superblock);
	uint32_t offset = sqsh_inode_file_fragment_block_offset(inode);
	uint32_t size = sqsh_inode_file_size(inode) % block_size;
	uint32_t end_offset;

	if (SQSH_ADD_OVERFLOW(offset, size, &end_offset)) {
		return -SQSH_ERROR_INTEGER_OVERFLOW;
	}
	if (end_offset > data_size) {
		return -SQSH_ERROR_SIZE_MISMATCH;
	}

	view->data = &data[offset];
	view->size = size;

	return 0;
}

static int
read_fragment_compressed(
		struct SqshFragmentView *view, struct SqshArchive *archive,
		const struct SqshInode *inode) {
	int rv = 0;
	struct SqshExtractView *extract_view = &view->extract_view;
	struct SqshMapReader *reader = &view->map_reader;
	struct SqshExtractManager *extract_manager = NULL;

	rv = sqsh__archive_data_extract_manager(archive, &extract_manager);
	if (rv < 0) {
		goto out;
	}

	rv = sqsh__extract_view_init(extract_view, extract_manager, reader);
	if (rv < 0) {
		goto out;
	}
	const uint8_t *data = sqsh__extract_view_data(extract_view);
	const size_t size = sqsh__extract_view_size(extract_view);
	rv = apply_fragment(view, archive, inode, data, size);
	if (rv < 0) {
		goto out;
	}
out:
	return rv;
}

static int
read_fragment_uncompressed(
		struct SqshFragmentView *view, struct SqshArchive *archive,
		const struct SqshInode *inode) {
	int rv = 0;
	struct SqshMapReader *reader = &view->map_reader;
	const uint8_t *data = sqsh__map_reader_data(reader);
	const size_t size = sqsh__map_reader_size(reader);

	rv = apply_fragment(view, archive, inode, data, size);
	if (rv < 0) {
		goto out;
	}
out:
	return rv;
}

int
sqsh__fragment_view_init(
		struct SqshFragmentView *view, const struct SqshInode *inode) {
	struct SqshArchive *archive = inode->archive;
	struct SqshMapReader *reader = &view->map_reader;
	struct SqshFragmentTable *table = NULL;

	int rv = 0;
	struct SqshDataFragment *fragment_info = alloca(SQSH_SIZEOF_FRAGMENT);

	rv = sqsh_archive_fragment_table(archive, &table);
	if (rv < 0) {
		goto out;
	}
	rv = sqsh__fragment_table_get(table, inode, fragment_info);
	if (rv < 0) {
		goto out;
	}

	const struct SqshSuperblock *superblock = sqsh_archive_superblock(archive);
	const uint64_t start_address = sqsh__data_fragment_start(fragment_info);
	const uint32_t size_info = sqsh__data_fragment_size_info(fragment_info);
	const uint32_t size = sqsh_datablock_size(size_info);
	const bool is_compressed = sqsh_datablock_is_compressed(size_info);
	const uint64_t upper_limit = sqsh_superblock_inode_table_start(superblock);
	struct SqshMapManager *map_manager = sqsh_archive_map_manager(archive);

	rv = sqsh__map_reader_init(reader, map_manager, start_address, upper_limit);
	if (rv < 0) {
		goto out;
	}
	rv = sqsh__map_reader_advance(reader, 0, size);
	if (rv < 0) {
		goto out;
	}

	if (is_compressed) {
		rv = read_fragment_compressed(view, archive, inode);
	} else {
		rv = read_fragment_uncompressed(view, archive, inode);
	}
	if (rv < 0) {
		goto out;
	}

out:
	return rv;
}

const uint8_t *
sqsh__fragment_view_data(const struct SqshFragmentView *view) {
	return view->data;
}

size_t
sqsh__fragment_view_size(const struct SqshFragmentView *view) {
	return view->size;
}

int
sqsh__fragment_view_cleanup(struct SqshFragmentView *view) {
	view->data = NULL;
	view->size = 0;
	sqsh__extract_view_cleanup(&view->extract_view);
	sqsh__map_reader_cleanup(&view->map_reader);
	return 0;
}
