/**
 * \file models/shadow/guid/guid_init_from_string.c
 *
 * \brief Shadow method for guid_init_from_string.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>

static int nondet_retval();

/**
 * \brief Initialize a guid from a string.
 *
 * \param id                The guid to initialize.
 * \param str               The input string from which this guid is
 *                          initialized.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_init_from_string)(FAT32_SYM(guid)* id, const char* str)
{
    int retval;

    /* check preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(guid_init_from_string), id, str);

    /* determine return value. */
    retval = nondet_retval();

    switch (retval)
    {
        case STATUS_SUCCESS:
            __CPROVER_havoc_object(id);
            break;

        default:
            retval = FAT32_ERROR_GUID_STRING_BAD;
            break;
    }

    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_init_from_string), retval, id, str);

    return retval;
}
