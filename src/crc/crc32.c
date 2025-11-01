/**
 * \file crc/crc32.c
 *
 * \brief CRC-32 algorithm, as described in RFC 1952, but with some constant
 * expansion.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/crc.h>

/**
 * \brief Calculates the CRC-32 of a given section of memory.
 *
 * \param data          Data array to CRC.
 * \param size          Size of this array.
 *
 * \returns the CRC-32 of this section of memory.
 */
uint32_t FAT32_SYM(crc32)(const void* data, size_t size)
{
    (void)data;
    (void)size;

    return -1;
}
