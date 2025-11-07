/**
 * \file models/crc/crc32_shadow/main.c
 *
 * \brief Model checks for the shadow implementation of \ref crc32.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/crc.h>
#include <libfat32/model_check/assert.h>

FAT32_IMPORT_crc;

size_t nondet_size();

size_t input_size()
{
    size_t retval = nondet_size();

    if (retval > 512)
    {
        retval = 512;
    }

    return retval;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    uint8_t data[512];

    __CPROVER_havoc_object(data);

    /* perform a CRC of this data. */
    uint32_t value = crc32(data, input_size());

    return 0;
}
