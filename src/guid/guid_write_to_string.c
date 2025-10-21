/**
 * \file guid/guid_write_to_string.c
 *
 * \brief Write a guid to a string.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>
#include <stdio.h>

/**
 * \brief Write the guid representation in the given string buffer.
 *
 * \param str               The string buffer.
 * \param size              The maximum size of the string buffer.
 * \param id                The guid to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_write_to_string)(
    char* str, size_t size, const FAT32_SYM(guid)* id)
{
    /* verify that this buffer can hold a guid representation. */
    if (size < FAT32_GUID_STRING_SIZE)
    {
        return FAT32_ERROR_GUID_STRING_BAD;
    }

    /* convert to the string representation. */
    sprintf(str, "%08x", id->data1);
    sprintf(str +  9, "%04x", id->data2);
    sprintf(str + 14, "%04x", id->data3);
    for (int i = 0; i < 2; ++i)
    {
        sprintf(str + 19 + 2*i, "%02x", id->data4[i]);
    }
    for (int i = 2; i < 8; ++i)
    {
        sprintf(str + 20 + 2*i, "%02x", id->data4[i]);
    }
    str[8] = '-';
    str[13] = '-';
    str[18] = '-';
    str[23] = '-';
    str[36] = 0;

    return STATUS_SUCCESS;
}
