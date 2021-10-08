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
 * @author      : Enno Boland (mail@eboland.de)
 * @file        : error
 * @created     : Saturday May 08, 2021 20:03:38 CEST
 */

#include "utils.h"

#ifndef SQUASH_ERROR_H

#define SQUASH_ERROR_H

#define SQUASH_ERROR_SECTION (1 << 8)
enum SquashError {
	// Avoid collisions with errno
	SQUASH_ERROR_SUPERBLOCK_TOO_SMALL = SQUASH_ERROR_SECTION + 1,
	SQUASH_ERROR_WRONG_MAGIG,
	SQUASH_ERROR_BLOCKSIZE_MISSMATCH,
	SQUASH_ERROR_SIZE_MISSMATCH,
	SQUASH_ERROR_CHECKFLAG_SET,
	SQUASH_ERROR_METABLOCK_UNSUPPORTED_COMPRESSION,
	SQUASH_ERROR_METABLOCK_INFO_IS_COMPRESSED,
	SQUASH_ERROR_METABLOCK_ZERO_SIZE,
	SQUASH_ERROR_METABLOCK_INIT,
	SQUASH_ERROR_COMPRESSION_INIT,
	SQUASH_ERROR_COMPRESSION_DECOMPRESS,
	SQUASH_ERROR_UNKOWN_INODE_TYPE,
	SQUASH_ERROR_COMPRESSION_STREAM_INIT,
	SQUASH_ERROR_COMPRESSION_STREAM_CLEANUP,
	SQUASH_ERROR_STREAM_NOT_ENOUGH_BYTES,
	SQUASH_ERROR_GZIP_HEADER_TOO_SMALL,
	SQUASH_ERROR_GZIP_HEADER_READ,
	SQUASH_ERROR_GZIP_READ_AFTER_END,
	SQUASH_ERROR_NOT_A_DIRECTORY,
	SQUASH_ERROR_NOT_A_FILE,
	SQUASH_ERROR_MALLOC_FAILED,
	SQUASH_ERROR_DIRECTORY_INIT,
	SQUASH_ERROR_INODE_INIT,
	SQUASH_ERROR_INTEGER_OVERFLOW,
	SQUASH_ERROR_NO_SUCH_FILE,
	SQUASH_ERROR_NO_FRAGMENT,
	SQUASH_ERROR_SEEK_OUT_OF_RANGE,
	SQUASH_ERROR_SEEK_IN_FRAGMENT,
	SQUASH_ERROR_TODO,
};

void squash_perror(int error_code, const char *msg);

SQUASH_NO_UNUSED const char *squash_error_str(int errorcode);

#endif /* end of include guard SQUASH_ERROR_H */
