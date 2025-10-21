/**
 * \file guid/guid_init_from_data.c
 *
 * \brief Initialize a guid from binary data.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>

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
    (void)id;
    (void)ptr;

    /* verify that the binary data is the correct size. */
    if (size != FAT32_GUID_BINARY_SIZE)
    {
        return FAT32_ERROR_GUID_DATA_INVALID_SIZE;
    }

    return -1;
}
