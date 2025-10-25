/**
 * \file libfat32/guid.h
 *
 * \brief Representation of the Microsoft GUID.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <libfat32/function_decl.h>
#include <libfat32/model_check/assert.h>
#include <libfat32/model_check/function_contracts.h>
#include <libfat32/model_check/memory.h>
#include <libfat32/status.h>
#include <stdint.h>
#include <stddef.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

#define FAT32_GUID_BINARY_SIZE 16
#define FAT32_GUID_STRING_SIZE 37

/**
 * \brief A GUID is similar to a UUID, but it is serialized differently, both ta
 * and from binary fields and to and from strings.
 */
typedef struct FAT32_SYM(guid) FAT32_SYM(guid);

struct FAT32_SYM(guid)
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
};

/******************************************************************************/
/* Start of constructors.                                                     */
/******************************************************************************/

/**
 * \brief Initialize a guid from a string.
 *
 * \param id                The guid to initialize.
 * \param str               The input string from which this guid is
 *                          initialized.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_init_from_string)(FAT32_SYM(guid)* id, const char* str);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(guid_init_from_string),
    FAT32_SYM(guid)* id, const char* str)
        /* id must be accessible. */
        MODEL_CHECK_OBJECT_RW(id, sizeof(*id));
        /* str must not be NULL. */
        MODEL_ASSERT(NULL != str);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(guid_init_from_string))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(guid_init_from_string),
    int retval, FAT32_SYM(guid)* id, const char* str)
        /* this call either succeeds or fails with a
         * FAT32_ERROR_GUID_STRING_BAD. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GUID_STRING_BAD == retval));
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(guid_init_from_string))

/**
 * \brief Initialize a guid from binary data.
 *
 * \param id                The guid to initialize.
 * \param ptr               Pointer to the binary data from which this guid is
 *                          initialized.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_init_from_data)(
    FAT32_SYM(guid)* id, const void* ptr, size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(guid_init_from_data),
    FAT32_SYM(guid)* id, const void* ptr, size_t size)
        /* id must be accessible. */
        MODEL_CHECK_OBJECT_RW(id, sizeof(*id));
        /* data must be readable. */
        MODEL_CHECK_OBJECT_READ(ptr, size);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(guid_init_from_data))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(guid_init_from_data),
    int retval, FAT32_SYM(guid)* id, const void* ptr, size_t size)
        /* this call either succeeds or fails with a
         * FAT32_ERROR_GUID_DATA_INVALID_SIZE. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GUID_DATA_INVALID_SIZE == retval));
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(guid_init_from_data))

/******************************************************************************/
/* Start of public methods.                                                   */
/******************************************************************************/

/**
 * \brief Write the guid representation in the given string buffer.
 *
 * \param str               The string buffer.
 * \param size              The maximum size of the string buffer.
 * \param id                The guid to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_write_to_string)(
    char* str, size_t size, const FAT32_SYM(guid)* id);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(guid_write_to_string),
    char* str, size_t size, const FAT32_SYM(guid)* id)
        /* str must be accessible. */
        MODEL_CHECK_OBJECT_WRITE(str, size);
        /* id must be accessible. */
        MODEL_CHECK_OBJECT_READ(id, sizeof(*id));
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(guid_write_to_string))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(guid_write_to_string),
    int retval, char* str, size_t size, const FAT32_SYM(guid)* id)
        /* this call either succeeds or fails with a
         * FAT32_ERROR_GUID_STRING_BAD. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GUID_STRING_BAD == retval));
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(guid_write_to_string))

/**
 * \brief Write the guid representation to the given binary buffer.
 *
 * \param buffer            The binary buffer.
 * \param size              The size of the binary buffer.
 * \param id                The guid to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_write_to_binary)(
    void* buffer, size_t size, const FAT32_SYM(guid)* id);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(guid_write_to_binary),
    void* buffer, size_t size, const FAT32_SYM(guid)* id)
        /* buffer must be accessible. */
        MODEL_CHECK_OBJECT_WRITE(buffer, size);
        /* id must be accessible. */
        MODEL_CHECK_OBJECT_READ(id, sizeof(*id));
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(guid_write_to_binary))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(guid_write_to_binary),
    int retval, void* buffer, size_t size, const FAT32_SYM(guid)* id)
        /* this call either succeeds or fails with a
         * FAT32_ERROR_GUID_DATA_INVALID_SIZE. */
        MODEL_ASSERT(
            (STATUS_SUCCESS == retval)
         || (FAT32_ERROR_GUID_DATA_INVALID_SIZE == retval));
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(guid_write_to_binary))

/******************************************************************************/
/* Start of public exports.                                                   */
/******************************************************************************/

#define __INTERNAL_FAT32_IMPORT_guid_sym(sym) \
    FAT32_BEGIN_EXPORT \
    typedef FAT32_SYM(guid) sym ## guid; \
    static inline int FN_DECL_MUST_CHECK \
    sym ## guid_init_from_string( \
        FAT32_SYM(guid)* x, const char* y) { \
            return FAT32_SYM(guid_init_from_string)(x,y); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## guid_init_from_data( \
        FAT32_SYM(guid)* x, const void* y, size_t z) { \
            return FAT32_SYM(guid_init_from_data)(x,y,z); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## guid_write_to_string( \
        char* x, size_t y, const FAT32_SYM(guid)* z) { \
            return FAT32_SYM(guid_write_to_string)(x,y,z); } \
    static inline int FN_DECL_MUST_CHECK \
    sym ## guid_write_to_binary( \
        void* x, size_t y, const FAT32_SYM(guid)* z) { \
            return FAT32_SYM(guid_write_to_binary)(x,y,z); } \
    FAT32_END_EXPORT \
    REQUIRE_SEMICOLON_HERE
#define FAT32_IMPORT_guid_as(sym) \
    __INTERNAL_FAT32_IMPORT_guid_sym(sym ## _)
#define FAT32_IMPORT_guid \
    __INTERNAL_FAT32_IMPORT_guid_sym()

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
