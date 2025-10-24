/**
 * \file models/guid/guid_init_from_string/main.c
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

static void init_success(const char* str)
{
    guid id;

    MODEL_ASSERT(STATUS_SUCCESS == guid_init_from_string(&id, str));
}

static void init_fail(const char* str)
{
    guid id;

    MODEL_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD == guid_init_from_string(&id, str));
}

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

    /* these should succeed. */
    init_success("e64f4d23-81a8-4cce-9ba4-58a6d470a6f9");
    init_success("2fa4cf54e22d4a5480f74a448a8454b7");

    /* these should fail. */
    init_fail("");
    init_fail("aa0ee1f6-c8e4-413f-aeee-8d8aa3476eca0");

    return 0;
}
