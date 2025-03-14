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
 * @file         integration.c
 */

#include "../include/sqsh_archive_private.h"
#include "../include/sqsh_chrome.h"
#include "../include/sqsh_directory_private.h"
#include "../include/sqsh_file_private.h"
#include "../include/sqsh_inode_private.h"
#include "../include/sqsh_tree.h"
#include "../include/sqsh_tree_private.h"
#include "common.h"
#include <pthread.h>
#include <testlib.h>

extern uint8_t squashfs_image[];
extern size_t squashfs_image_size;

#define TEST_SQUASHFS_IMAGE_LEN squashfs_image_size
#define TEST_SQUASHFS_IMAGE squashfs_image

static void
sqsh_empty(void) {
	int rv;
	struct SqshArchive sqsh = {0};
	const struct SqshConfig config = DEFAULT_CONFIG(0);
	rv = sqsh__archive_init(&sqsh, NULL, &config);
	assert(rv == -SQSH_ERROR_SUPERBLOCK_TOO_SMALL);
}

static void
sqsh_get_nonexistant(void) {
	int rv;
	struct SqshArchive sqsh = {0};
	struct SqshTreeWalker walker = {0};

	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "/nonexistant", false);
	assert(rv < 0);

	rv = sqsh__tree_walker_cleanup(&walker);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
tree_walker(void) {
	int rv;
	struct SqshTreeWalker walker = {0};
	struct SqshArchive sqsh = {0};
	struct SqshInode *inode;
	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "/large_dir", false);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "999", false);
	assert(rv == 0);

	inode = sqsh_tree_walker_inode_load(&walker, &rv);
	assert(inode != NULL);
	assert(rv == 0);

	rv = sqsh_inode_free(inode);
	assert(rv == 0);

	rv = sqsh__tree_walker_cleanup(&walker);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_ls(void) {
	int rv;
	char *name;
	struct SqshInode inode = {0};
	struct SqshDirectoryIterator *iter = NULL;
	struct SqshArchive sqsh = {0};
	const struct SqshSuperblock *superblock;
	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	superblock = sqsh_archive_superblock(&sqsh);
	rv = sqsh__inode_init(
			&inode, &sqsh, sqsh_superblock_inode_root_ref(superblock));
	assert(rv == 0);

	iter = sqsh_directory_iterator_new(&inode, &rv);
	assert(iter != NULL);
	assert(rv == 0);

	rv = sqsh_directory_iterator_next(iter);
	assert(rv > 0);
	name = sqsh_directory_iterator_name_dup(iter);
	assert(name != NULL);
	assert(strcmp("a", name) == 0);
	free(name);

	rv = sqsh_directory_iterator_next(iter);
	assert(rv >= 0);
	name = sqsh_directory_iterator_name_dup(iter);
	assert(name != NULL);
	assert(strcmp("b", name) == 0);
	free(name);

	rv = sqsh_directory_iterator_next(iter);
	assert(rv >= 0);
	name = sqsh_directory_iterator_name_dup(iter);
	assert(name != NULL);
	assert(strcmp("large_dir", name) == 0);
	free(name);

	rv = sqsh_directory_iterator_next(iter);
	// End of file list
	assert(rv == 0);

	rv = sqsh_directory_iterator_free(iter);
	assert(rv == 0);

	rv = sqsh__inode_cleanup(&inode);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_read_content(void) {
	int rv;
	char *data;
	struct SqshArchive archive = {0};
	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&archive, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	data = sqsh_file_content(&archive, "/a");
	assert(strcmp(data, "a\n") == 0);
	free(data);

	rv = sqsh__archive_cleanup(&archive);
	assert(rv == 0);
}

static void
sqsh_cat_fragment(void) {
	int rv;
	const uint8_t *data;
	size_t size;
	struct SqshInode *inode = NULL;
	struct SqshFileReader reader = {0};
	struct SqshArchive sqsh = {0};
	struct SqshTreeWalker walker = {0};
	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "a", false);
	assert(rv == 0);

	inode = sqsh_tree_walker_inode_load(&walker, &rv);
	assert(inode != NULL);
	assert(rv == 0);

	rv = sqsh__file_reader_init(&reader, inode);
	assert(rv == 0);

	size = sqsh_inode_file_size(inode);
	assert(size == 2);

	rv = sqsh_file_reader_advance(&reader, 0, size);
	assert(rv == 0);

	data = sqsh_file_reader_data(&reader);
	assert(memcmp(data, "a\n", size) == 0);

	rv = sqsh__file_reader_cleanup(&reader);
	assert(rv == 0);

	rv = sqsh_inode_free(inode);
	assert(rv == 0);

	rv = sqsh__tree_walker_cleanup(&walker);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_cat_datablock_and_fragment(void) {
	int rv;
	const uint8_t *data;
	size_t size;
	struct SqshInode *inode = NULL;
	struct SqshFileReader reader = {0};
	struct SqshArchive sqsh = {0};
	struct SqshTreeWalker walker = {0};
	const struct SqshConfig config = {
			.source_mapper = sqsh_mapper_impl_static,
			.source_size = TEST_SQUASHFS_IMAGE_LEN,
	};
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "b", false);
	assert(rv == 0);

	inode = sqsh_tree_walker_inode_load(&walker, &rv);
	assert(inode != NULL);
	assert(rv == 0);

	rv = sqsh__file_reader_init(&reader, inode);
	assert(rv == 0);

	size = sqsh_inode_file_size(inode);
	assert(size == 1050000);

	rv = sqsh_file_reader_advance(&reader, 0, size);
	assert(rv == 0);
	assert(size == sqsh_file_reader_size(&reader));

	data = sqsh_file_reader_data(&reader);
	for (sqsh_index_t i = 0; i < size; i++) {
		assert(data[i] == 'b');
	}

	rv = sqsh__file_reader_cleanup(&reader);
	assert(rv == 0);

	rv = sqsh_inode_free(inode);
	assert(rv == 0);

	rv = sqsh__tree_walker_cleanup(&walker);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_cat_size_overflow(void) {
	int rv;
	size_t size;
	struct SqshInode *inode = NULL;
	struct SqshFileReader reader = {0};
	struct SqshArchive sqsh = {0};
	struct SqshTreeWalker walker = {0};
	const struct SqshConfig config = {
			.source_mapper = sqsh_mapper_impl_static,
			.source_size = TEST_SQUASHFS_IMAGE_LEN,
	};
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "b", false);
	assert(rv == 0);

	inode = sqsh_tree_walker_inode_load(&walker, &rv);
	assert(inode != NULL);
	assert(rv == 0);

	rv = sqsh__file_reader_init(&reader, inode);
	assert(rv == 0);
	size = sqsh_inode_file_size(inode);
	assert(size == 1050000);

	rv = sqsh_file_reader_advance(&reader, 0, size + 4096);
	assert(rv != 0); // TODO: check for correct error code

	rv = sqsh__file_reader_cleanup(&reader);
	assert(rv == 0);

	rv = sqsh_inode_free(inode);
	assert(rv == 0);

	rv = sqsh__tree_walker_cleanup(&walker);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_test_uid_and_gid(void) {
	int rv;
	uint32_t uid, gid;
	struct SqshInode inode = {0};
	struct SqshArchive sqsh = {0};
	const struct SqshSuperblock *superblock;
	const struct SqshConfig config = {
			.source_mapper = sqsh_mapper_impl_static,
			.source_size = TEST_SQUASHFS_IMAGE_LEN,
	};
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	superblock = sqsh_archive_superblock(&sqsh);
	rv = sqsh__inode_init(
			&inode, &sqsh, sqsh_superblock_inode_root_ref(superblock));
	assert(rv == 0);

	uid = sqsh_inode_uid(&inode);
	assert(uid == 2020);
	gid = sqsh_inode_gid(&inode);
	assert(gid == 202020);

	rv = sqsh__inode_cleanup(&inode);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

static void
sqsh_test_extended_dir(void) {
	int rv;
	struct SqshInode *inode = NULL;
	struct SqshArchive sqsh = {0};
	struct SqshTreeWalker walker = {0};
	const struct SqshConfig config = {
			.source_mapper = sqsh_mapper_impl_static,
			.source_size = TEST_SQUASHFS_IMAGE_LEN,
	};
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	rv = sqsh__tree_walker_init(&walker, &sqsh);
	assert(rv == 0);

	rv = sqsh_tree_walker_resolve(&walker, "/large_dir/999", false);
	assert(rv == 0);

	inode = sqsh_tree_walker_inode_load(&walker, &rv);
	assert(inode != NULL);

	rv = sqsh_inode_free(inode);
	assert(rv == 0);

	rv = sqsh__tree_walker_cleanup(&walker);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

#if 0
static void
sqsh_test_xattr(void) {
	const char *expected_value = "1234567891234567891234567890001234567890";
	int rv;
	char *name, *value;
	struct SqshInode inode = {0};
	struct SqshInode *entry_inode = NULL;
	struct SqshDirectoryIterator *dir_iter = NULL;
	struct SqshXattrIterator *xattr_iter = NULL;
	struct SqshArchive sqsh = {0};
	const struct SqshSuperblock *superblock;
	const struct SqshConfig config = {
			.source_mapper = sqsh_mapper_impl_static,
			.source_size = TEST_SQUASHFS_IMAGE_LEN,
	};
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	superblock = sqsh_archive_superblock(&sqsh);
	rv = sqsh__inode_init(
			&inode, &sqsh, sqsh_superblock_inode_root_ref(superblock));
	assert(rv == 0);

	xattr_iter = sqsh_xattr_iterator_new(&inode, &rv);
	assert(xattr_iter != NULL);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_next(xattr_iter);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_free(xattr_iter);
	assert(rv == 0);

	dir_iter = sqsh_directory_iterator_new(&inode, &rv);
	assert(dir_iter != NULL);
	assert(rv == 0);

	rv = sqsh_directory_iterator_next(dir_iter);
	assert(rv > 0);
	name = sqsh_directory_iterator_name_dup(dir_iter);
	assert(name != NULL);
	assert(strcmp("a", name) == 0);
	free(name);
	entry_inode = sqsh_directory_iterator_inode_load(dir_iter, &rv);
	assert(rv == 0);
	xattr_iter = sqsh_xattr_iterator_new(entry_inode, &rv);
	assert(xattr_iter != NULL);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_next(xattr_iter);
	assert(rv > 0);
	assert(sqsh_xattr_iterator_is_indirect(xattr_iter) == false);
	name = sqsh_xattr_iterator_fullname_dup(xattr_iter);
	assert(name != NULL);
	assert(strcmp("user.foo", name) == 0);
	free(name);
	value = sqsh_xattr_iterator_value_dup(xattr_iter);
	assert(value != NULL);
	assert(strcmp(expected_value, value) == 0);
	free(value);
	rv = sqsh_xattr_iterator_next(xattr_iter);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_free(xattr_iter);
	assert(rv == 0);
	rv = sqsh_inode_free(entry_inode);
	assert(rv == 0);

	rv = sqsh_directory_iterator_next(dir_iter);
	assert(rv >= 0);
	name = sqsh_directory_iterator_name_dup(dir_iter);
	assert(name != NULL);
	assert(strcmp("b", name) == 0);
	free(name);
	entry_inode = sqsh_directory_iterator_inode_load(dir_iter, &rv);
	assert(rv == 0);
	xattr_iter = sqsh_xattr_iterator_new(entry_inode, &rv);
	assert(xattr_iter != NULL);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_next(xattr_iter);
	assert(rv > 0);
	assert(sqsh_xattr_iterator_is_indirect(xattr_iter) == true);
	name = sqsh_xattr_iterator_fullname_dup(xattr_iter);
	assert(name != NULL);
	assert(strcmp("user.bar", name) == 0);
	free(name);
	value = sqsh_xattr_iterator_value_dup(xattr_iter);
	assert(value != NULL);
	assert(strcmp(expected_value, value) == 0);
	free(value);
	rv = sqsh_xattr_iterator_next(xattr_iter);
	assert(rv == 0);
	rv = sqsh_xattr_iterator_free(xattr_iter);
	assert(rv == 0);
	rv = sqsh_inode_free(entry_inode);
	assert(rv == 0);

	rv = sqsh_directory_iterator_free(dir_iter);
	assert(rv == 0);

	rv = sqsh__inode_cleanup(&inode);
	assert(rv == 0);

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}
#endif

struct Walker {
	struct SqshArchive *sqsh;
	uint64_t inode_number;
};

static void *
multithreaded_walker(void *arg) {
	int rv;
	struct Walker *walker = arg;
	struct Walker my_walker = {
			.sqsh = walker->sqsh,
	};

	struct SqshInode *inode =
			sqsh_inode_new(walker->sqsh, walker->inode_number, &rv);

	if (sqsh_inode_type(inode) == SQSH_INODE_TYPE_DIRECTORY) {
		struct SqshDirectoryIterator *iter =
				sqsh_directory_iterator_new(inode, &rv);
		while (sqsh_directory_iterator_next(iter) > 0) {
			multithreaded_walker(&my_walker);
		}
		sqsh_directory_iterator_free(iter);
	} else {
		struct SqshFileReader *reader = sqsh_file_reader_new(inode, &rv);
		size_t size = sqsh_inode_file_size(inode);
		rv = sqsh_file_reader_advance(reader, 0, size);
		assert(rv == 0);
		sqsh_file_reader_free(reader);
	}

	sqsh_inode_free(inode);

	return 0;
}
static void
multithreaded(void) {
	int rv;
	pthread_t threads[16] = {0};
	struct SqshArchive sqsh = {0};

	const struct SqshConfig config = DEFAULT_CONFIG(TEST_SQUASHFS_IMAGE_LEN);
	rv = sqsh__archive_init(&sqsh, (char *)TEST_SQUASHFS_IMAGE, &config);
	assert(rv == 0);

	const struct SqshSuperblock *superblock = sqsh_archive_superblock(&sqsh);
	struct Walker walker = {
			.sqsh = &sqsh,
			.inode_number = sqsh_superblock_inode_root_ref(superblock),
	};
	for (unsigned long i = 0; i < LENGTH(threads); i++) {
		rv = pthread_create(&threads[i], NULL, multithreaded_walker, &walker);
		assert(rv == 0);
	}

	for (unsigned long i = 0; i < LENGTH(threads); i++) {
		rv = pthread_join(threads[i], NULL);
		assert(rv == 0);
	}

	rv = sqsh__archive_cleanup(&sqsh);
	assert(rv == 0);
}

DECLARE_TESTS
TEST(sqsh_empty)
TEST(sqsh_ls)
TEST(tree_walker)
TEST(sqsh_get_nonexistant)
TEST(sqsh_read_content)
TEST(sqsh_cat_fragment)
TEST(sqsh_cat_datablock_and_fragment)
TEST(sqsh_cat_size_overflow)
TEST(sqsh_test_uid_and_gid)
TEST(sqsh_test_extended_dir)
// TEST(sqsh_test_xattr);
TEST(multithreaded)
END_TESTS
