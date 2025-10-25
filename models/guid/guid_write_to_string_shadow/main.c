/**
 * \file models/guid/guid_write_to_string_shadow/main.c
 *
 * \brief Model checks for \ref guid_write_to_string shadow method.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/guid.h>

FAT32_IMPORT_guid;

static size_t nondet_size();

static size_t string_size()
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
    char output_string[40];

    /* initialize the guid. */
    retval = guid_init_from_string(&id, input_string);
    if (STATUS_SUCCESS != retval)
    {
        return 1;
    }

    /* write the guid to the output string. */
    retval = guid_write_to_string(output_string, string_size(), &id);
    if (STATUS_SUCCESS != retval)
    {
        return 1;
    }

    return 0;
}
