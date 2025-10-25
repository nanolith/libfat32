/**
 * \file models/shadow/guid/guid_write_to_string.c
 *
 * \brief Shadow method for guid_write_to_string.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>

static int nondet_retval();

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
    int retval;

    /* check preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(guid_write_to_string), str, size, id);

    retval = nondet_retval();
    if (STATUS_SUCCESS == retval && size >= FAT32_GUID_STRING_SIZE)
    {
        __CPROVER_havoc_object(str);
        str[36] = 0;
    }
    else
    {
        retval = FAT32_ERROR_GUID_STRING_BAD;
    }

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_write_to_string), retval, str, size, id);

    return retval;
}
