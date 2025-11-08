/**
 * \file libfat32/gpt.h
 *
 * \brief An interface for GPT.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <libfat32/function_decl.h>
#include <libfat32/guid.h>
#include <libfat32/model_check/assert.h>
#include <libfat32/model_check/function_contracts.h>
#include <libfat32/model_check/memory.h>
#include <libfat32/status.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

#define FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE                      16
#define FAT32_GPT_PROTECTIVE_MBR_MINIMUM_SIZE                              512

/**
 * \brief A partition record in the protective MBR.
 *
 * \note As defined in the UEFI 2.11 Specification (Section 5.2.3 Table 5.4);
 */
typedef struct FAT32_SYM(gpt_protective_mbr_partition_record)
FAT32_SYM(gpt_protective_mbr_partition_record);

struct FAT32_SYM(gpt_protective_mbr_partition_record)
{
    uint8_t boot_indicator;
    uint32_t starting_chs; //Note: serialized as 24-bit.
    uint8_t os_type;
    uint32_t ending_chs; //Note: serialized as 24-bit.
    uint32_t starting_lba;
    uint32_t size_in_lba;
};

/**
 * \brief The LBA 0 Protective MBR.
 *
 * \note As defined in the UEFI 2.11 Specification (Section 5.2.3 Table 5.3).
 */
typedef struct FAT32_SYM(gpt_protective_mbr) FAT32_SYM(gpt_protective_mbr);

struct FAT32_SYM(gpt_protective_mbr)
{
    uint8_t boot_code[440];
    uint8_t unique_disk_signature[4];
    FAT32_SYM(gpt_protective_mbr_partition_record) partition_record[4];
    uint16_t signature;
};

/**
 * \brief The GPT Header.
 *
 * \note As defined in the UEFI 2.11 Specification (Section 5.3.2 Table 5.5).
 */
typedef struct FAT32_SYM(gpt_header) FAT32_SYM(gpt_header);

struct FAT32_SYM(gpt_header)
{
    uint8_t signature[8];
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint8_t reserved[4];
    uint64_t my_lba;
    uint64_t alternative_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    FAT32_SYM(guid) disk_guid;
    uint64_t partition_entry_lba;
    uint32_t number_of_partition_entries;
    uint32_t size_of_partition_entry;
    uint32_t partition_entry_array_crc32;
};

/**
 * \brief A GPT Partiton Entry.
 *
 * \note As defined in UEFI 2.11 Specification (Section 5.3.3 table 5.6)
 */
typedef struct FAT32_SYM(gpt_partition_entry) FAT32_SYM(gpt_partition_entry);

struct FAT32_SYM(gpt_partition_entry)
{
    FAT32_SYM(guid) partition_type_guid;
    FAT32_SYM(guid) unique_partition_guid;
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint16_t partition_name[36];
};

/******************************************************************************/
/* Start of properties.                                                       */
/******************************************************************************/

/**
 * \brief Returns true if the given protective MBR partition record is valid.
 *
 * \note Valid records here are either empty records or the protective span
 * record.
 *
 * \param rec           The record to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec);

/**
 * \brief Returns true if the given protective MBR is valid.
 *
 * \note A valid protective MBR has all records set as expected, one
 * protective span partition record, and three empty partition records.
 *
 * \param mbr           The MBR to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_protective_mbr_valid)(
    const FAT32_SYM(gpt_protective_mbr)* mbr);

/**
 * \brief Returns true if the given gpt header is valid.
 *
 * \note A valid gpt header has a valid signature and sane defaults. There isn't
 * much more we can do to verify it.
 *
 * \param hdr           The header to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_header_valid)(
    const FAT32_SYM(gpt_header)* hdr);

/**
 * \brief Returns true if the given gpt partition entry is valid.
 *
 * \note A valid gpt partition entry has sane values. There isn't much more we
 * can do to verify it.
 *
 * \param entry         The entry to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_partition_entry_valid)(
    const FAT32_SYM(gpt_partition_entry)* hdr);

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Initialize an empty protective mbr partition record.
 *
 * \param rec               The record to initialize.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_init_clear)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_clear),
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
        /* rec must be accessible. */
        MODEL_CHECK_OBJECT_RW(rec, sizeof(*rec));
MODEL_CONTRACT_PRECONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_clear))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_clear),
    int retval, FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
        /* this method always succeeds. */
        MODEL_ASSERT(STATUS_SUCCESS == retval);
        /* on success, the partition record is clear, which is valid. */
        MODEL_ASSERT(
            FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(rec));
MODEL_CONTRACT_POSTCONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_clear))

/**
 * \brief Initialize a protective mbr partition record spanning the entire disk.
 *
 * \param rec               The record to initialize.
 * \param size              Size of the entire disk in bytes.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_init_span)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_span),
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, size_t size)
        /* rec must be accessible. */
        MODEL_CHECK_OBJECT_RW(rec, sizeof(*rec));
MODEL_CONTRACT_PRECONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_span))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_span),
    int retval, FAT32_SYM(gpt_protective_mbr_partition_record)* rec,
    size_t size)
        /* this method either succeeds or fails with a bad size error. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE == retval));
        /* on success, the partition record is a span record, which is valid. */
        if (STATUS_SUCCESS == retval)
        {
            MODEL_ASSERT(
                FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(
                    rec));
        }
MODEL_CONTRACT_POSTCONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_init_span))

/**
 * \brief Initialize a protective mbr spanning the entire disk.
 *
 * \param mbr               The record to initialize.
 * \param size              Size of the entire disk in bytes.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_init_span)(
    FAT32_SYM(gpt_protective_mbr)* mbr, size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_init_span), FAT32_SYM(gpt_protective_mbr)* mbr,
    size_t size)
        /* mbr must be accessible. */
        MODEL_CHECK_OBJECT_RW(mbr, sizeof(*mbr));
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(gpt_protective_mbr_init_span))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_init_span), int retval,
    FAT32_SYM(gpt_protective_mbr)* mbr, size_t size)
        /* this method either succeeds or fails with the following failure
         * codes. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE == retval));
        /* on success, the mbr is valid. */
        if (STATUS_SUCCESS == retval)
        {
            MODEL_ASSERT(FAT32_SYM(property_gpt_protective_mbr_valid)(mbr));
        }
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(gpt_protective_mbr_init_span))

/**
 * \brief Initialize a GPT header with the given disk GUID, first usable lba,
 * last usable lba, and alternative lba.
 *
 * \param header            The record to initialize.
 * \param disk_guid         The disk GUID.
 * \param first_lba         The first usable lba.
 * \param last_lba          The last usable lba.
 * \param alt_lba           The alternative lba.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_header_init)(
    FAT32_SYM(gpt_header)* header, const FAT32_SYM(guid)* disk_guid,
    uint64_t first_lba, uint64_t last_lba, uint64_t alt_lba);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_header_init), FAT32_SYM(gpt_header)* header,
    const FAT32_SYM(guid)* disk_guid, uint64_t first_lba, uint64_t last_lba,
    uint64_t alt_lba)
        /* header must be accessible. */
        MODEL_CHECK_OBJECT_RW(header, sizeof(*header));
        /* disk_guid must be valid. */
        MODEL_ASSERT(FAT32_SYM(property_guid_valid)(disk_guid));
        /* last_lba must be greater than first_lba. */
        MODEL_ASSERT(last_lba > first_lba);
        /* alt_lba must be less than or equal to last lba. */
        MODEL_ASSERT(alt_lba <= last_lba);
        /* alt_lba must be greater than first lba. */
        MODEL_ASSERT(alt_lba > first_lba);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(gpt_protective_mbr_init_span))

/******************************************************************************/
/* Start of public methods.                                                   */
/******************************************************************************/

/**
 * \brief Read a protective mbr partition record from a given location in RAM.
 *
 * \param rec               The record to populate with RAM data.
 * \param ptr               The pointer from which this data is read.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_read)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, const void* ptr,
    size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_read),
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, const void* ptr,
    size_t size)
        /* rec must be accessible. */
        MODEL_CHECK_OBJECT_RW(rec, sizeof(*rec));
        /* ptr must be accessible. */
        MODEL_CHECK_OBJECT_READ(ptr, size);
MODEL_CONTRACT_PRECONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_read))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_read),
    int retval, FAT32_SYM(gpt_protective_mbr_partition_record)* rec,
    const void* ptr, size_t size)
        /* this method either succeeds or fails with one of the following
         * failure codes. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE == retval)
         || (FAT32_ERROR_GPT_BAD_RECORD == retval));
        /* if this method succeeds, then the record is valid. */
        if (STATUS_SUCCESS == retval)
        {
            MODEL_ASSERT(
                FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(
                    rec));
        }
MODEL_CONTRACT_POSTCONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_read))

/**
 * \brief Write a protective mbr partition record to a given location in RAM.
 *
 * \param ptr               The pointer to which this record is written.
 * \param size              The expected size of the written record.
 * \param rec               The record to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_write)(
    void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_write),
    void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
        /* rec must be valid. */
        MODEL_ASSERT(
            FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(rec));
        /* ptr must be accessible. */
        MODEL_CHECK_OBJECT_WRITE(ptr, size);
MODEL_CONTRACT_PRECONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_write))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_partition_record_write),
    int retval, void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
        /* this method either succeeds or fails with one of the following. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE));
MODEL_CONTRACT_POSTCONDITIONS_END(
    FAT32_SYM(gpt_protective_mbr_partition_record_write))

/**
 * \brief Read a protective mbr from a given location in RAM.
 *
 * \param mbr               The record to populate with RAM data.
 * \param ptr               The pointer from which this data is read.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_read)(
    FAT32_SYM(gpt_protective_mbr)* mbr, const void* ptr, size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_read),
    FAT32_SYM(gpt_protective_mbr)* mbr, const void* ptr, size_t size)
        /* mbr must be accessible. */
        MODEL_CHECK_OBJECT_RW(mbr, sizeof(*mbr));
        /* ptr must be accessible. */
        MODEL_CHECK_OBJECT_READ(ptr, size);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(gpt_protective_mbr_read))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_read),
    int retval, FAT32_SYM(gpt_protective_mbr)* mbr, const void* ptr,
    size_t size)
        /* this method either succeeds or fails with one of the following
         * failure codes. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE == retval)
         || (FAT32_ERROR_GPT_BAD_RECORD == retval)
         || (FAT32_ERROR_GPT_MBR_BAD_SIGNATURE == retval));
        /* if this method succeeds, then the record is valid. */
        if (STATUS_SUCCESS == retval)
        {
            MODEL_ASSERT(FAT32_SYM(property_gpt_protective_mbr_valid)(mbr));
        }
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(gpt_protective_mbr_read))

/**
 * \brief Write a protective mbr to a given location in RAM.
 *
 * \param ptr               The pointer to which this record is written.
 * \param size              The expected size of the written record.
 * \param mbr               The record to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_write)(
    void* ptr, size_t size, const FAT32_SYM(gpt_protective_mbr)* mbr);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_write), void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr)* mbr)
        /* mbr must be valid. */
        MODEL_ASSERT(FAT32_SYM(property_gpt_protective_mbr_valid)(mbr));
        /* ptr must be accessible. */
        MODEL_CHECK_OBJECT_WRITE(ptr, size);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(gpt_protective_mbr_write))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(gpt_protective_mbr_write), int retval, void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr)* mbr)
        /* this method either succeeds or fails with one of the following. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GPT_BAD_SIZE));
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(gpt_protective_mbr_write))

/******************************************************************************/
/* Start of public exports.                                                   */
/******************************************************************************/

#define __INTERNAL_FAT32_IMPORT_gpt_sym(sym) \
    FAT32_BEGIN_EXPORT \
    typedef FAT32_SYM(gpt_protective_mbr_partition_record) \
    sym ## gpt_protective_mbr_partition_record; \
    typedef FAT32_SYM(gpt_protective_mbr) sym ## gpt_protective_mbr; \
    typedef FAT32_SYM(gpt_header) sym ## gpt_header; \
    typedef FAT32_SYM(gpt_partition_entry) sym ## gpt_partition_entry; \
    static inline bool \
    sym ## property_gpt_protective_mbr_partition_record_valid( \
        const FAT32_SYM(gpt_protective_mbr_partition_record)* x) { \
            return \
                FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)( \
                    x); \
        } \
    static inline bool \
    sym ## property_gpt_protective_mbr_valid( \
        const FAT32_SYM(gpt_protective_mbr)* x) { \
            return \
                FAT32_SYM(property_gpt_protective_mbr_valid)(x); \
        } \
    static inline bool \
    sym ## property_gpt_header_valid( \
        const FAT32_SYM(gpt_header)* x) { \
            return FAT32_SYM(property_gpt_header_valid)(x); \
        } \
    static inline bool \
    sym ## property_gpt_partition_entry_valid( \
        const FAT32_SYM(gpt_partition_entry)* x) { \
            return FAT32_SYM(property_gpt_partition_entry_valid)(x); \
        } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_partition_record_init_clear( \
        FAT32_SYM(gpt_protective_mbr_partition_record)* x) { \
            return \
                FAT32_SYM(gpt_protective_mbr_partition_record_init_clear)(x); \
        } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_partition_record_init_span( \
        FAT32_SYM(gpt_protective_mbr_partition_record)* x, size_t y) { \
            return \
                FAT32_SYM(gpt_protective_mbr_partition_record_init_span)(x,y); \
        } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_init_span( \
        FAT32_SYM(gpt_protective_mbr)* x, size_t y) { \
            return FAT32_SYM(gpt_protective_mbr_init_span)(x,y); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_partition_record_read( \
        FAT32_SYM(gpt_protective_mbr_partition_record)* x, const void* y, \
        size_t z) { \
            return \
                FAT32_SYM(gpt_protective_mbr_partition_record_read)(x,y,z); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_partition_record_write( \
        void* x, size_t y, \
        const FAT32_SYM(gpt_protective_mbr_partition_record)* z) { \
            return \
                FAT32_SYM(gpt_protective_mbr_partition_record_write)(x,y,z); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_read( \
        FAT32_SYM(gpt_protective_mbr)* x, const void* y, size_t z) { \
            return FAT32_SYM(gpt_protective_mbr_read)(x,y,z); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## gpt_protective_mbr_write( \
        void* x, size_t y, const FAT32_SYM(gpt_protective_mbr)* z) { \
            return FAT32_SYM(gpt_protective_mbr_write)(x,y,z); } \
    FAT32_END_EXPORT \
    REQUIRE_SEMICOLON_HERE
#define FAT32_IMPORT_gpt_as(sym) \
    __INTERNAL_FAT32_IMPORT_gpt_sym(sym ## _)
#define FAT32_IMPORT_gpt \
    __INTERNAL_FAT32_IMPORT_gpt_sym()

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
