/**
 * @author      : Enno Boland (mail@eboland.de)
 * @file        : stream
 * @created     : Saturday May 08, 2021 21:43:42 CEST
 */

#include <stdint.h>
#include <stdlib.h>

#include "compression/compression.h"

#ifndef STREAM_H

#define STREAM_H

struct Squash;

struct SquashStream {
	union SquashDecompressorStreamInfo wrap;
	struct SquashDecompressor *decompressor;
	struct SquashMetablock *metablock;
	off_t offset;
};

int squash_stream_init(struct SquashStream *stream, struct Squash *squash,
		struct SquashMetablock *metablock, off_t block, off_t offset);

size_t squash_stream_size(struct SquashStream *stream);

int squash_stream_more(struct SquashStream *stream, size_t min_read_bytes);

int squash_stream_to_end(struct SquashStream *stream);

uint8_t *squash_stream_data(struct SquashStream *stream);

int squash_stream_cleanup(struct SquashStream *stream);

#endif /* end of include guard STREAM_H */
