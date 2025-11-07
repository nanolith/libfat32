/**
 * \file shadow/crc/crc32.c
 *
 * \brief Shadow impl of crc32.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/crc.h>

uint32_t nondet_uint32();

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
    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(FAT32_SYM(crc32), data, size);

    uint32_t crc = nondet_uint32();

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(FAT32_SYM(crc32), crc, data, size);

    return crc;
}
