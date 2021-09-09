#
# Makefile
# tox, 2018-04-02 16:39
#
include config.mk

HDR = \
	bin/printb.h \
	src/compression/compression.h \
	src/context/directory_context.h \
	src/context/inode_context.h \
	src/context/metablock_context.h \
	src/error.h \
	src/extract.h \
	src/format/compression_options.h \
	src/format/directory.h \
	src/format/fragment.h \
	src/format/inode.h \
	src/format/inode_internal.h \
	src/format/metablock.h \
	src/format/metablock_internal.h \
	src/format/superblock.h \
	src/format/xattr.h \
	src/squash.h \
	src/utils.h \

SRC = \
	src/compression/compression.c \
	src/compression/gzip.c \
	src/compression/lz4.c \
	src/compression/lzma.c \
	src/compression/lzo.c \
	src/compression/null.c \
	src/compression/xz.c \
	src/compression/zstd.c \
	src/context/directory_context.c \
	src/context/inode_context.c \
	src/context/metablock_context.c \
	src/extract.c \
	src/format/compression_options.c \
	src/format/directory.c \
	src/format/inode.c \
	src/format/metablock.c \
	src/format/superblock.c \
	src/squash.c \
	src/utils.c \


OBJ = $(SRC:.c=.o)

BIN = \
	bin/lssquash \
	bin/squashinfo \
	bin/squashfs-fuse \

TST = \

TST_BIN = $(TST:.c=-test)

FZZ = \
	fuzzer/simple.c

FZZ_BIN = $(FZZ:.c=-fuzz)

BCH = \

BCH_BIN = $(BCH:.c=-bench)
BCH_CFLAGS = \

MAJOR=$(shell echo $(VERSION) | cut -d . -f 1)

CFLAGS += \
	$(shell pkg-config --cflags zlib liblzma lzo2 liblz4 libzstd fuse3) \

LDFLAGS += \
	$(shell pkg-config --libs zlib liblzma lzo2 liblz4 libzstd fuse3) \

all: $(BIN) libsquashfs.a libsquashfs.so

libsquashfs.a: $(OBJ)
	@echo AR $@
	@ar rc $@ $(OBJ)
	@ranlib $@

libsquashfs.so: $(OBJ)
	@echo SH $@
	@$(CC) -shared $(LDFLAGS) $(OBJ) -o $@.$(VERSION) -Wl,-soname=$@.$(MAJOR)
	@ln -sf $@.$(VERSION) $@.$(MAJOR)
	@ln -sf $@.$(VERSION) $@

bin/%: bin/%.o $(OBJ)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJ) $< -o $@

bench/%-bench: bench/%.c test/test.h $(OBJ)
	@echo CCBENCH $@
	@$(CC) $(CFLAGS) $(LDFLAGS) $(BCH_CFLAGS) $(OBJ) $< -o $@

test/%-test: test/%.c test/test.h $(SRC)
	@echo CCTEST $@
	@$(CC) $(CFLAGS) $(LDFLAGS) $(TST_CFLAGS) $(SRC) $< -o $@

fuzzer/%-fuzz: fuzzer/%.c $(OBJ)
	@echo CCFUZZ $@
	@$(CC) $(CFLAGS) $(LDFLAGS) $(FZZ_CFLAGS) $(SRC) $< -o $@

%.o: %.c $(HDR)
	@echo CC $@
	@$(CC) $(CFLAGS) -c -o $@ $<

check: $(TST_BIN)
	@for i in $(TST_BIN); do ./$$i || exit 1; done

speed: $(BCH_BIN) $(BENCH_JSON) $(BENCH_PLIST)
	@for i in $(BCH_BIN); do ./$$i; done

fuzz: $(FZZ_BIN)
	@for i in $(FZZ_BIN); do ./$$i $${i%-fuzz}-corpus -max_total_time=120 -dict=$${i%-fuzz}-dict || exit 1; done

doc: doxygen.conf $(TST) $(SRC) $(HDR) README.md
	@sed -i "/^PROJECT_NUMBER\s/ s/=.*/= $(VERSION)/" $<
	@doxygen $<

bench/bench-file.json:
	@wget http://eu.battle.net/auction-data/258993a3c6b974ef3e6f22ea6f822720/auctions.json -O $@

bench/bench-file.plist: bench/bench-file.json
	@npm install plist-cli
	@node_modules/.bin/plist-cli < $< > $@
	@rm -rf package-lock.json node_modules

coverage: check
	@mkdir cov
	@gcovr -r . --html --html-details -o cov/index.html

clean:
	@echo cleaning...
	@rm -rf doc cov
	@rm -f *.gcnp *.gcda
	@rm -f $(TST_BIN) $(BCH_BIN) $(FZZ_BIN) $(OBJ) $(BIN) libsquashfs.so* libsquashfs.a

.PHONY: check all clean speed coverage fuzz


# vim:ft=make
#
