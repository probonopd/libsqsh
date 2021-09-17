/**
 * @author      : Enno Boland (mail@eboland.de)
 * @file        : error
 * @created     : Saturday Sep 11, 2021 12:26:48 CEST
 */

#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
squash_perror(int error_code, const char *msg) {
	if (msg) {
		fputs(msg, stderr);
		fputs(": ", stderr);
	}
	fputs(squash_error_str(error_code), stderr);
	fputc('\n', stderr);
}

const char *
squash_error_str(int errorcode) {
	errorcode = abs(errorcode);

	if (errorcode < SQUASH_ERROR_SECTION) {
		return strerror(errorcode);
	}
	switch (errorcode) {
	case SQUASH_ERROR_SUPERBLOCK_TOO_SMALL:
		return "Superblock too small";
	case SQUASH_ERROR_WRONG_MAGIG:
		return "Wrong magig";
	case SQUASH_ERROR_BLOCKSIZE_MISSMATCH:
		return "Blocksize missmatch";
	case SQUASH_ERROR_SIZE_MISSMATCH:
		return "Size missmatch";
	case SQUASH_ERROR_CHECKFLAG_SET:
		return "Checkflag set";
	case SQUASH_ERROR_METABLOCK_UNSUPPORTED_COMPRESSION:
		return "Metablock unsupported compression";
	case SQUASH_ERROR_METABLOCK_INFO_IS_COMPRESSED:
		return "Metablock info is compressed";
	case SQUASH_ERROR_METABLOCK_INIT:
		return "Metablock init";
	case SQUASH_ERROR_COMPRESSION_INIT:
		return "Compression init";
	case SQUASH_ERROR_COMPRESSION_DECOMPRESS:
		return "Compression decompress";
	case SQUASH_ERROR_UNKOWN_INODE_TYPE:
		return "Unkown inode type";
	case SQUASH_ERROR_COMPRESSION_STREAM_INIT:
		return "Compression stream init";
	case SQUASH_ERROR_COMPRESSION_STREAM_CLEANUP:
		return "Compression stream cleanup";
	case SQUASH_ERROR_STREAM_NOT_ENOUGH_BYTES:
		return "Stream not enough bytes";
	case SQUASH_ERROR_GZIP_HEADER_TOO_SMALL:
		return "Gzip header too small";
	case SQUASH_ERROR_GZIP_HEADER_READ:
		return "Gzip header read";
	case SQUASH_ERROR_GZIP_READ_AFTER_END:
		return "Gzip read after end";
	case SQUASH_ERROR_DIRECTORY_WRONG_INODE_TYPE:
		return "Directory wrong inode type";
	case SQUASH_ERROR_MALLOC_FAILED:
		return "Malloc Failed";
	case SQUASH_ERROR_DIRECTORY_INIT:
		return "Directory Init";
	case SQUASH_ERROR_INODE_INIT:
		return "Inode init";
	case SQUASH_ERROR_INTEGER_OVERFLOW:
		return "Integer overflow";
	case SQUASH_ERROR_NO_SUCH_FILE:
		return "No such file or directory";
	case SQUASH_ERROR_TODO:
		return "Todo";
	default:
		return "Unknown Error";
	}
}
