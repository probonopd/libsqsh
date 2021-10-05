VERSION = 0.1.0

CONFIG_COMPRESSION_GZIP = 1
CONFIG_COMPRESSION_LZ4 = 1
CONFIG_COMPRESSION_LZMA = 1
CONFIG_COMPRESSION_LZO = 0
CONFIG_COMPRESSION_XZ = 1
CONFIG_COMPRESSION_ZSTD = 1

# release flags
#CFLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wall -fPIC -Werror -Wpedantic -O2 -pthread

# debug flags
CFLAGS = -Wall -Werror -Wpedantic -O0 -g -fPIC -pthread -D_FILE_OFFSET_BITS=64 -fsanitize=address
LDFLAGS = -pthread -fsanitize=address

TST_CFLAGS = -fsanitize=address
#TST_CFLAGS += -fprofile-arcs
#TST_CFLAGS += -ftest-coverage
FZZ_CFLAGS = -fsanitize=fuzzer,address -fcoverage-mapping -fprofile-instr-generate -g

# if you're feeling lucky:
#CFLAGS += -DNDEBUG

## Comment this in for profiling
#LDFLAGS += -lprofiler
