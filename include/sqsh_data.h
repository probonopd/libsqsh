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

#ifndef SQSH_DATA_H
#define SQSH_DATA_H

#define SQSH_UNALIGNED __attribute__((packed, aligned(1)))

#include "sqsh_common.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////
// data/compression_options_data.c

#define SQSH_SIZEOF_COMPRESSION_OPTIONS_GZIP 8
#define SQSH_SIZEOF_COMPRESSION_OPTIONS_XZ 8
#define SQSH_SIZEOF_COMPRESSION_OPTIONS_LZ4 8
#define SQSH_SIZEOF_COMPRESSION_OPTIONS_ZSTD 4
#define SQSH_SIZEOF_COMPRESSION_OPTIONS_LZO 8
#define SQSH_SIZEOF_COMPRESSION_OPTIONS 8

struct SQSH_UNALIGNED SqshDataCompressionOptionsGzip;

struct SQSH_UNALIGNED SqshDataCompressionOptionsXz;

struct SQSH_UNALIGNED SqshDataCompressionOptionsLz4;

struct SQSH_UNALIGNED SqshDataCompressionOptionsZstd;

struct SQSH_UNALIGNED SqshDataCompressionOptionsLzo;

union SqshDataCompressionOptions;

uint32_t sqsh_compression_data_options_gzip_compression_level(
		const union SqshDataCompressionOptions *options);
uint16_t sqsh_compression_data_options_gzip_window_size(
		const union SqshDataCompressionOptions *options);
uint16_t sqsh_compression_data_options_gzip_strategies(
		const union SqshDataCompressionOptions *options);

uint32_t sqsh_compression_data_options_xz_dictionary_size(
		const union SqshDataCompressionOptions *options);
uint32_t sqsh_compression_data_options_xz_filters(
		const union SqshDataCompressionOptions *options);

uint32_t sqsh_compression_data_options_lz4_version(
		const union SqshDataCompressionOptions *options);
uint32_t sqsh_compression_data_options_lz4_flags(
		const union SqshDataCompressionOptions *options);

uint32_t sqsh_compression_data_options_zstd_compression_level(
		const union SqshDataCompressionOptions *options);

uint32_t sqsh_compression_data_options_lzo_algorithm(
		const union SqshDataCompressionOptions *options);
uint32_t sqsh_compression_data_options_lzo_compression_level(
		const union SqshDataCompressionOptions *options);

////////////////////////////////////////
// data/datablock_data.c

#define SQSH_SIZEOF_DATABLOCK_SIZE 4

struct SQSH_UNALIGNED SqshDataDatablockSize;

uint32_t
sqsh_data_datablock_size(const struct SqshDataDatablockSize *datablock_size);
bool sqsh_data_datablock_is_compressed(
		const struct SqshDataDatablockSize *datablock_size);

////////////////////////////////////////
// data/directory_data.c

#define SQSH_SIZEOF_DIRECTORY_FRAGMENT 12
#define SQSH_SIZEOF_DIRECTORY_ENTRY 8

struct SQSH_UNALIGNED SqshDataDirectoryEntry;

struct SQSH_UNALIGNED SqshDataDirectoryFragment;

uint16_t
sqsh_data_directory_entry_offset(const struct SqshDataDirectoryEntry *entry);
int16_t sqsh_data_directory_entry_inode_offset(
		const struct SqshDataDirectoryEntry *entry);
uint16_t
sqsh_data_directory_entry_type(const struct SqshDataDirectoryEntry *entry);
uint16_t
sqsh_data_directory_entry_name_size(const struct SqshDataDirectoryEntry *entry);
const uint8_t *
sqsh_data_directory_entry_name(const struct SqshDataDirectoryEntry *entry);

uint32_t sqsh_data_directory_fragment_count(
		const struct SqshDataDirectoryFragment *fragment);
uint32_t sqsh_data_directory_fragment_start(
		const struct SqshDataDirectoryFragment *fragment);
uint32_t sqsh_data_directory_fragment_inode_number(
		const struct SqshDataDirectoryFragment *fragment);
const struct SqshDataDirectoryEntry *sqsh_data_directory_fragment_entries(
		const struct SqshDataDirectoryFragment *fragment);

////////////////////////////////////////
// data/fragment_data.c

#define SQSH_SIZEOF_FRAGMENT 16

struct SQSH_UNALIGNED SqshDataFragment;

uint64_t sqsh_data_fragment_start(const struct SqshDataFragment *fragment);
const struct SqshDataDatablockSize *
sqsh_data_fragment_size_info(const struct SqshDataFragment *fragment);
uint32_t
sqsh_data_fragment_is_compressed(const struct SqshDataFragment *fragment);

////////////////////////////////////////
// data/inode_data.c

#define SQSH_SIZEOF_INODE_DIRECTORY_INDEX 12
#define SQSH_SIZEOF_INODE_DIRECTORY 16
#define SQSH_SIZEOF_INODE_DIRECTORY_EXT 24
#define SQSH_SIZEOF_INODE_FILE 16
#define SQSH_SIZEOF_INODE_FILE_EXT 40
#define SQSH_SIZEOF_INODE_SYMLINK 8
#define SQSH_SIZEOF_INODE_SYMLINK_EXT 8
#define SQSH_SIZEOF_INODE_SYMLINK_EXT_TAIL 4
#define SQSH_SIZEOF_INODE_DEVICE 8
#define SQSH_SIZEOF_INODE_DEVICE_EXT 12
#define SQSH_SIZEOF_INODE_IPC 4
#define SQSH_SIZEOF_INODE_IPC_EXT 8
#define SQSH_SIZEOF_INODE_HEADER 16

enum SqshDataInodeType {
	SQSH_INODE_TYPE_BASIC_DIRECTORY = 1,
	SQSH_INODE_TYPE_BASIC_FILE = 2,
	SQSH_INODE_TYPE_BASIC_SYMLINK = 3,
	SQSH_INODE_TYPE_BASIC_BLOCK = 4,
	SQSH_INODE_TYPE_BASIC_CHAR = 5,
	SQSH_INODE_TYPE_BASIC_FIFO = 6,
	SQSH_INODE_TYPE_BASIC_SOCKET = 7,
	SQSH_INODE_TYPE_EXTENDED_DIRECTORY = 8,
	SQSH_INODE_TYPE_EXTENDED_FILE = 9,
	SQSH_INODE_TYPE_EXTENDED_SYMLINK = 10,
	SQSH_INODE_TYPE_EXTENDED_BLOCK = 11,
	SQSH_INODE_TYPE_EXTENDED_CHAR = 12,
	SQSH_INODE_TYPE_EXTENDED_FIFO = 13,
	SQSH_INODE_TYPE_EXTENDED_SOCKET = 14,
};

struct SQSH_UNALIGNED SqshDataInodeDirectory;
struct SQSH_UNALIGNED SqshDataInodeDirectoryExt;
struct SQSH_UNALIGNED SqshDataInodeFile;
struct SQSH_UNALIGNED SqshDataInodeFileExt;
struct SQSH_UNALIGNED SqshDataInodeSymlink;
struct SQSH_UNALIGNED SqshDataInodeSymlinkExt;
struct SQSH_UNALIGNED SqshDataInodeSymlinkExtTail;
struct SQSH_UNALIGNED SqshDataInodeDevice;
struct SQSH_UNALIGNED SqshDataInodeDeviceExt;
struct SQSH_UNALIGNED SqshDataInodeIpc;
struct SQSH_UNALIGNED SqshDataInodeIpcExt;
struct SQSH_UNALIGNED SqshDataInode;

struct SQSH_UNALIGNED SqshDataInodeDirectoryIndex;

uint16_t sqsh_data_inode_type(const struct SqshDataInode *inode);
uint16_t sqsh_data_inode_permissions(const struct SqshDataInode *inode);
uint16_t sqsh_data_inode_uid_idx(const struct SqshDataInode *inode);
uint16_t sqsh_data_inode_gid_idx(const struct SqshDataInode *inode);
uint32_t sqsh_data_inode_modified_time(const struct SqshDataInode *inode);
uint32_t sqsh_data_inode_number(const struct SqshDataInode *inode);

const struct SqshDataInodeDirectory *
sqsh_data_inode_directory(const struct SqshDataInode *inode);
const struct SqshDataInodeDirectoryExt *
sqsh_data_inode_directory_ext(const struct SqshDataInode *inode);
const struct SqshDataInodeFile *
sqsh_data_inode_file(const struct SqshDataInode *inode);
const struct SqshDataInodeFileExt *
sqsh_data_inode_file_ext(const struct SqshDataInode *inode);
const struct SqshDataInodeSymlink *
sqsh_data_inode_symlink(const struct SqshDataInode *inode);
const struct SqshDataInodeSymlinkExt *
sqsh_data_inode_symlink_ext(const struct SqshDataInode *inode);
const struct SqshDataInodeDevice *
sqsh_data_inode_device(const struct SqshDataInode *inode);
const struct SqshDataInodeDeviceExt *
sqsh_data_inode_device_ext(const struct SqshDataInode *inode);
const struct SqshDataInodeIpc *
sqsh_data_inode_ipc(const struct SqshDataInode *inode);
const struct SqshDataInodeIpcExt *
sqsh_data_inode_ipc_ext(const struct SqshDataInode *inode);

uint32_t sqsh_data_inode_directory_block_start(
		const struct SqshDataInodeDirectory *directory);
uint32_t sqsh_data_inode_directory_hard_link_count(
		const struct SqshDataInodeDirectory *directory);
uint16_t sqsh_data_inode_directory_file_size(
		const struct SqshDataInodeDirectory *directory);
uint16_t sqsh_data_inode_directory_block_offset(
		const struct SqshDataInodeDirectory *directory);
uint32_t sqsh_data_inode_directory_parent_inode_number(
		const struct SqshDataInodeDirectory *directory);

uint32_t sqsh_data_inode_directory_ext_hard_link_count(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint32_t sqsh_data_inode_directory_ext_file_size(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint32_t sqsh_data_inode_directory_ext_block_start(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint32_t sqsh_data_inode_directory_ext_parent_inode_number(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint16_t sqsh_data_inode_directory_ext_index_count(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint16_t sqsh_data_inode_directory_ext_block_offset(
		const struct SqshDataInodeDirectoryExt *directory_ext);
uint32_t sqsh_data_inode_directory_ext_xattr_idx(
		const struct SqshDataInodeDirectoryExt *directory_ext);
const uint8_t *sqsh_data_inode_directory_ext_index(
		const struct SqshDataInodeDirectoryExt *directory_ext);

uint32_t sqsh_data_inode_directory_index_index(
		const struct SqshDataInodeDirectoryIndex *directory_index);
uint32_t sqsh_data_inode_directory_index_start(
		const struct SqshDataInodeDirectoryIndex *directory_index);
uint32_t sqsh_data_inode_directory_index_name_size(
		const struct SqshDataInodeDirectoryIndex *directory_index);
const uint8_t *sqsh_data_inode_directory_index_name(
		const struct SqshDataInodeDirectoryIndex *directory_index);

uint32_t
sqsh_data_inode_file_blocks_start(const struct SqshDataInodeFile *file);
uint32_t
sqsh_data_inode_file_fragment_block_index(const struct SqshDataInodeFile *file);
uint32_t
sqsh_data_inode_file_block_offset(const struct SqshDataInodeFile *file);
uint32_t sqsh_data_inode_file_size(const struct SqshDataInodeFile *file);
const struct SqshDataDatablockSize *
sqsh_data_inode_file_block_sizes(const struct SqshDataInodeFile *file);

uint64_t sqsh_data_inode_file_ext_blocks_start(
		const struct SqshDataInodeFileExt *file_ext);
uint64_t
sqsh_data_inode_file_ext_size(const struct SqshDataInodeFileExt *file_ext);
uint64_t
sqsh_data_inode_file_ext_sparse(const struct SqshDataInodeFileExt *file_ext);
uint32_t sqsh_data_inode_file_ext_hard_link_count(
		const struct SqshDataInodeFileExt *file_ext);
uint32_t sqsh_data_inode_file_ext_fragment_block_index(
		const struct SqshDataInodeFileExt *file_ext);
uint32_t sqsh_data_inode_file_ext_block_offset(
		const struct SqshDataInodeFileExt *file_ext);
uint32_t
sqsh_data_inode_file_ext_xattr_idx(const struct SqshDataInodeFileExt *file_ext);
const struct SqshDataDatablockSize *sqsh_data_inode_file_ext_block_sizes(
		const struct SqshDataInodeFileExt *file_ext);

uint32_t sqsh_data_inode_symlink_hard_link_count(
		const struct SqshDataInodeSymlink *directory);
uint32_t sqsh_data_inode_symlink_target_size(
		const struct SqshDataInodeSymlink *directory);
const uint8_t *sqsh_data_inode_symlink_target_path(
		const struct SqshDataInodeSymlink *directory);

uint32_t sqsh_data_inode_symlink_ext_hard_link_count(
		const struct SqshDataInodeSymlinkExt *directory);
uint32_t sqsh_data_inode_symlink_ext_target_size(
		const struct SqshDataInodeSymlinkExt *directory);
const uint8_t *sqsh_data_inode_symlink_ext_target_path(
		const struct SqshDataInodeSymlinkExt *directory);
uint32_t sqsh_data_inode_symlink_ext_xattr_idx(
		const struct SqshDataInodeSymlinkExt *directory);

uint32_t sqsh_data_inode_device_hard_link_count(
		const struct SqshDataInodeDevice *device);
uint32_t
sqsh_data_inode_device_device(const struct SqshDataInodeDevice *device);

uint32_t sqsh_data_inode_device_ext_hard_link_count(
		const struct SqshDataInodeDeviceExt *device);
uint32_t
sqsh_data_inode_device_ext_device(const struct SqshDataInodeDeviceExt *device);
uint32_t sqsh_data_inode_device_ext_xattr_idx(
		const struct SqshDataInodeDeviceExt *device);

uint32_t
sqsh_data_inode_ipc_hard_link_count(const struct SqshDataInodeIpc *ipc);

uint32_t
sqsh_data_inode_ipc_ext_hard_link_count(const struct SqshDataInodeIpcExt *ipc);
uint32_t
sqsh_data_inode_ipc_ext_xattr_idx(const struct SqshDataInodeIpcExt *ipc);

////////////////////////////////////////
// data/metablock_data.c

#define SQSH_SIZEOF_METABLOCK 2

#define SQSH_METABLOCK_BLOCK_SIZE 8192

struct SQSH_UNALIGNED SqshDataMetablock;

int
sqsh_data_metablock_is_compressed(const struct SqshDataMetablock *metablock);

const uint8_t *
sqsh_data_metablock_data(const struct SqshDataMetablock *metablock);

uint16_t sqsh_data_metablock_size(const struct SqshDataMetablock *metablock);

////////////////////////////////////////
// data/superblock_data.c

#define SQSH_SIZEOF_SUPERBLOCK 96

#define SQSH_SUPERBLOCK_MAGIC 0x73717368

struct SQSH_UNALIGNED SqshDataSuperblock;

uint32_t
sqsh_data_superblock_magic(const struct SqshDataSuperblock *superblock);
uint32_t
sqsh_data_superblock_inode_count(const struct SqshDataSuperblock *superblock);
uint32_t sqsh_data_superblock_modification_time(
		const struct SqshDataSuperblock *superblock);
uint32_t
sqsh_data_superblock_block_size(const struct SqshDataSuperblock *superblock);
uint32_t sqsh_data_superblock_fragment_entry_count(
		const struct SqshDataSuperblock *superblock);
uint16_t sqsh_data_superblock_compression_id(
		const struct SqshDataSuperblock *superblock);
uint16_t
sqsh_data_superblock_block_log(const struct SqshDataSuperblock *superblock);
uint16_t
sqsh_data_superblock_flags(const struct SqshDataSuperblock *superblock);
uint16_t
sqsh_data_superblock_id_count(const struct SqshDataSuperblock *superblock);
uint16_t
sqsh_data_superblock_version_major(const struct SqshDataSuperblock *superblock);
uint16_t
sqsh_data_superblock_version_minor(const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_root_inode_ref(
		const struct SqshDataSuperblock *superblock);
uint64_t
sqsh_data_superblock_bytes_used(const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_id_table_start(
		const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_xattr_id_table_start(
		const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_inode_table_start(
		const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_directory_table_start(
		const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_fragment_table_start(
		const struct SqshDataSuperblock *superblock);
uint64_t sqsh_data_superblock_export_table_start(
		const struct SqshDataSuperblock *superblock);

////////////////////////////////////////
// data/xattr_data.c

#define SQSH_SIZEOF_XATTR_KEY 4
#define SQSH_SIZEOF_XATTR_VALUE 4
#define SQSH_SIZEOF_XATTR_LOOKUP_TABLE 16
#define SQSH_SIZEOF_XATTR_ID_TABLE 16

struct SQSH_UNALIGNED SqshDataXattrKey;

struct SQSH_UNALIGNED SqshDataXattrValue;

struct SQSH_UNALIGNED SqshDataXattrLookupTable;

struct SQSH_UNALIGNED SqshDataXattrIdTable;

uint16_t sqsh_data_xattr_key_type(const struct SqshDataXattrKey *xattr_key);
uint16_t
sqsh_data_xattr_key_name_size(const struct SqshDataXattrKey *xattr_key);
const uint8_t *
sqsh_data_xattr_key_name(const struct SqshDataXattrKey *xattr_key);

uint32_t
sqsh_data_xattr_value_size(const struct SqshDataXattrValue *xattr_value);
uint64_t
sqsh_data_xattr_value_ref(const struct SqshDataXattrValue *xattr_value);
const uint8_t *
sqsh_data_xattr_value(const struct SqshDataXattrValue *xattr_value);

uint64_t sqsh_data_xattr_lookup_table_xattr_ref(
		const struct SqshDataXattrLookupTable *lookup_table);
uint32_t sqsh_data_xattr_lookup_table_count(
		const struct SqshDataXattrLookupTable *lookup_table);
uint32_t sqsh_data_xattr_lookup_table_size(
		const struct SqshDataXattrLookupTable *lookup_table);

uint64_t sqsh_data_xattr_id_table_xattr_table_start(
		const struct SqshDataXattrIdTable *xattr_id_table);
uint32_t sqsh_data_xattr_id_table_xattr_ids(
		const struct SqshDataXattrIdTable *xattr_id_table);
const uint64_t *
sqsh_data_xattr_id_table(const struct SqshDataXattrIdTable *xattr_id_table);

#ifdef __cplusplus
}
#endif
#endif // SQSH_DATA_H
