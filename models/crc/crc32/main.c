/**
 * \file models/crc/crc32/main.c
 *
 * \brief Model checks for \ref crc32.
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

    if (retval > 10)
    {
        retval = 10;
    }

    return retval;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    uint8_t data[10];

    __CPROVER_havoc_object(data);

    /* perform a CRC of this data. */
    uint32_t value = crc32(data, input_size());

    return 0;
}
