/******************************************************************************
 *                                                                            *
 * Copyright (c) 2021, Enno Boland <g@s01.de>                                 *
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
 * @file         sqsh-cat.c
 */

#include "common.h"
#include <sqsh_context.h>

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int
usage(char *arg0) {
	printf("usage: %s FILESYSTEM PATH [PATH ...]\n", arg0);
	printf("       %s -v\n", arg0);
	return EXIT_FAILURE;
}

static int
cat_path(struct SqshPathResolverContext *sqsh, char *path) {
	struct SqshInodeContext inode = {0};
	struct SqshFileContext *file = NULL;

	int rv = 0;
	rv = sqsh_path_resolver_resolve(sqsh, &inode, path);
	if (rv < 0) {
		sqsh_perror(rv, path);
		rv = EXIT_FAILURE;
		goto out;
	}

	file = sqsh_file_new(&inode, &rv);
	if (rv < 0) {
		sqsh_perror(rv, path);
		rv = EXIT_FAILURE;
		goto out;
	}

	rv = sqsh_file_read(file, sqsh_inode_file_size(&inode));
	if (rv < 0) {
		sqsh_perror(rv, path);
		rv = EXIT_FAILURE;
		goto out;
	}

	fwrite(sqsh_file_data(file), sizeof(uint8_t), sqsh_file_size(file), stdout);
out:
	sqsh_file_free(file);
	sqsh_inode_cleanup(&inode);
	return rv;
}

int
main(int argc, char *argv[]) {
	int rv = 0;
	int opt = 0;
	const char *image_path;
	struct Sqsh *sqsh = NULL;
	struct SqshPathResolverContext *resolver = NULL;

	while ((opt = getopt(argc, argv, "vh")) != -1) {
		switch (opt) {
		case 'v':
			puts("sqsh-cat-" VERSION);
			return 0;
		default:
			return usage(argv[0]);
		}
	}

	if (optind + 1 >= argc) {
		return usage(argv[0]);
	}

	image_path = argv[optind];
	optind++;

	sqsh = open_archive(image_path, &rv);
	if (rv < 0) {
		sqsh_perror(rv, image_path);
		rv = EXIT_FAILURE;
		goto out;
	}
	resolver = sqsh_path_resolver_new(sqsh, &rv);
	if (rv < 0) {
		sqsh_perror(rv, image_path);
		rv = EXIT_FAILURE;
		goto out;
	}

	for (; optind < argc; optind++) {
		rv = cat_path(resolver, argv[optind]);
		if (rv < 0) {
			goto out;
		}
	}

out:
	sqsh_path_resolver_free(resolver);
	sqsh_free(sqsh);
	return rv;
}
