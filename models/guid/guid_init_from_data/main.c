/**
 * \file models/guid/guid_init_from_data/main.c
 *
 * \brief Model checks for \ref guid_init_from_data.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/guid.h>

FAT32_IMPORT_guid;

static size_t nondet_size();

static size_t data_size()
{
    size_t retval = nondet_size();

    if (retval > 20)
    {
        retval = 20;
    }

    return retval;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    guid id;
    uint8_t data[20];

    /* randomize input data. */
    __CPROVER_havoc_object(data);

    /* initialize the guid. */
    retval = guid_init_from_data(&id, data, data_size());
    if (STATUS_SUCCESS != retval)
    {
        MODEL_ASSERT(FAT32_ERROR_GUID_DATA_INVALID_SIZE == retval);
        return 1;
    }

    return 0;
}
