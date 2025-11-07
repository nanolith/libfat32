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
    uint32_t crc = 0xffffffff;

    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(FAT32_SYM(crc32), data, size);

    const uint8_t* bdata = (const uint8_t*)data;
    for (size_t i = 0; i < size; ++i)
    {
        int offset = (crc ^ bdata[i]) & 0xFF;
        crc = FAT32_SYM(crc32_constants)[offset] ^ (crc >> 8);
    }

    crc ^= 0xffffffff;

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(FAT32_SYM(crc32), crc, data, size);

    return crc;
}
