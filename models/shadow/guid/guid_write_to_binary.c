/**
 * \file models/shadow/guid/guid_write_to_binary.c
 *
 * \brief The shadow impl of \ref guid_write_to_binary.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>

int nondet_retval();

/**
 * \brief Write the guid representation to the given binary buffer.
 *
 * \param buffer            The binary buffer.
 * \param size              The size of the binary buffer.
 * \param id                The guid to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(guid_write_to_binary)(
    void* buffer, size_t size, const FAT32_SYM(guid)* id)
{
    int retval;

    /* check preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(guid_write_to_binary), buffer, size, id);

    retval = nondet_retval();

    if (STATUS_SUCCESS == retval && size == FAT32_GUID_BINARY_SIZE)
    {
        __CPROVER_havoc_object(buffer);
    }
    else
    {
        retval = FAT32_ERROR_GUID_DATA_INVALID_SIZE;
    }

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_write_to_binary), retval, buffer, size, id);

    return retval;
}
