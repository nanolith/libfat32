/**
 * \file models/shadow/guid/guid_init_from_data.c
 *
 * \brief Shadow function for \ref guid_init_from_data.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>
#include <string.h>

static int nondet_retval();

/**
 * \brief Initialize a guid from binary data.
 *
 * \param id                The guid to initialize.
 * \param ptr               Pointer to the binary data from which this guid is
 *                          initialized.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_init_from_data)(
    FAT32_SYM(guid)* id, const void* ptr, size_t size)
{
    int retval;

    /* check preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(guid_init_from_data), id, ptr, size);

    /* get return value. */
    retval = nondet_retval();

    switch (retval)
    {
        case STATUS_SUCCESS:
            __CPROVER_havoc_object(id);
            break;

        default:
            retval = FAT32_ERROR_GUID_DATA_INVALID_SIZE;
            break;
    }

    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_init_from_data), retval, id, ptr, size);

    return retval;
}
