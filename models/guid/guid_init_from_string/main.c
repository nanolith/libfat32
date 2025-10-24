/**
 * \file models/guid/guid_init_from_string.c
 *
 * \brief Model checks for \ref guid_init_from_string.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/guid.h>

FAT32_IMPORT_guid;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    guid id;
    char guid_str[40];

    /* randomize the input string. */
    __CPROVER_havoc_object(guid_str);
    guid_str[39] = 0;

    /* initialize the guid. */
    retval = guid_init_from_string(&id, guid_str);
    if (STATUS_SUCCESS != retval)
    {
        MODEL_ASSERT(FAT32_ERROR_GUID_STRING_BAD == retval);
        return 1;
    }

    return 0;
}
