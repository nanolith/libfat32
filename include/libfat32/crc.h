/**
 * \file libfat32/crc.h
 *
 * \brief CRC related functions.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <libfat32/function_decl.h>
#include <libfat32/model_check/assert.h>
#include <libfat32/model_check/function_contracts.h>
#include <libfat32/model_check/memory.h>
#include <stddef.h>
#include <stdint.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief Constants for the CRC-32 function.
 */
extern const uint32_t FAT32_SYM(crc32_constants)[256];

/**
 * \brief Calculates the CRC-32 of a given section of memory.
 *
 * \param data          Data array to CRC.
 * \param size          Size of this array.
 *
 * \returns the CRC-32 of this section of memory.
 */
uint32_t FAT32_SYM(crc32)(const void* data, size_t size);

/* preconditions. */
MODEL_CONTRACT_PRECONDITIONS_BEGIN(
    FAT32_SYM(crc32), const void* data, size_t size)
        /* data must be accessible. */
        MODEL_CHECK_OBJECT_RW(data, size);
MODEL_CONTRACT_PRECONDITIONS_END(FAT32_SYM(crc32))

/* postconditions. */
MODEL_CONTRACT_POSTCONDITIONS_BEGIN(
    FAT32_SYM(crc32), int retval, const void* data, size_t size)
        /* this function performs a computation over data. There are no
         * postconditions to check beyond unit testing. */
MODEL_CONTRACT_POSTCONDITIONS_END(FAT32_SYM(crc32))

/******************************************************************************/
/* Start of public exports.                                                   */
/******************************************************************************/

#define __INTERNAL_FAT32_IMPORT_crc_sym(sym) \
    FAT32_BEGIN_EXPORT \
    static inline uint32_t sym ## crc32( \
        const void* x, size_t y) { \
            return FAT32_SYM(crc32)(x,y); } \
    FAT32_END_EXPORT \
    REQUIRE_SEMICOLON_HERE
#define FAT32_IMPORT_crc_as(sym) \
    __INTERNAL_FAT32_IMPORT_crc_sym(sym ## _)
#define FAT32_IMPORT_crc \
    __INTERNAL_FAT32_IMPORT_crc_sym()

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
