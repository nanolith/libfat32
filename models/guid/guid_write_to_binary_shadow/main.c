/**
 * \file models/guid/guid_write_to_binary_shadow/main.c
 *
 * \brief Model checks for \ref guid_write_to_binary shadow impl.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/guid.h>

FAT32_IMPORT_guid;

static size_t nondet_size();

static size_t binary_size()
{
    size_t retval = nondet_size();

    if (retval > 40)
    {
        retval = 40;
    }

    return retval;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    guid id;
    char input_string[40];
    uint8_t output_data[40];

    /* initialize the guid. */
    retval = guid_init_from_string(&id, input_string);
    if (STATUS_SUCCESS != retval)
    {
        return 1;
    }

    /* write the guid to the output buffer. */
    retval = guid_write_to_binary(output_data, binary_size(), &id);
    if (STATUS_SUCCESS != retval)
    {
        return 1;
    }

    return 0;
}
