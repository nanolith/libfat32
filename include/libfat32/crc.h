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
#include <stddef.h>
#include <stdint.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief Calculates the CRC-32 of a given section of memory.
 *
 * \param data          Data array to CRC.
 * \param size          Size of this array.
 *
 * \returns the CRC-32 of this section of memory.
 */
uint32_t FAT32_SYM(crc32)(const void* data, size_t size);

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
