/**
 * \file guid/guid_write_to_binary.c
 *
 * \brief Write a guid to a binary buffer.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>
#include <string.h>

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
    /* make sure the buffer is the right size. */
    if (FAT32_GUID_BINARY_SIZE != size)
    {
        return FAT32_ERROR_GUID_DATA_INVALID_SIZE;
    }

    uint8_t* bbuf = (uint8_t*)buffer;

    /* TODO - this assumes little-endian. */
    memcpy(bbuf +  0, &id->data1, sizeof(id->data1));
    memcpy(bbuf +  4, &id->data2, sizeof(id->data2));
    memcpy(bbuf +  6, &id->data3, sizeof(id->data3));
    memcpy(bbuf +  8, id->data4, sizeof(id->data4));

    return STATUS_SUCCESS;
}
