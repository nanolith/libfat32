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

/* forward decls. */
static void write_little_endian(uint8_t* buffer, uint64_t value, size_t count);

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

    /* make sure the buffer is the right size. */
    if (FAT32_GUID_BINARY_SIZE != size)
    {
        retval = FAT32_ERROR_GUID_DATA_INVALID_SIZE;
        goto done;
    }

    uint8_t* bbuf = (uint8_t*)buffer;

    write_little_endian(bbuf + 0, id->data1, sizeof(id->data1));
    write_little_endian(bbuf + 4, id->data2, sizeof(id->data2));
    write_little_endian(bbuf + 6, id->data3, sizeof(id->data3));
    memcpy(bbuf +  8, id->data4, sizeof(id->data4));

    retval = STATUS_SUCCESS;
    goto done;

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_write_to_binary), retval, buffer, size, id);

    return retval;
}

/**
 * \brief Write a little-endian value to the given buffer.
 *
 * \param buffer            The output buffer.
 * \param value             The value to write.
 * \param count             The number of bytes to write.
 */
static void write_little_endian(uint8_t* buffer, uint64_t value, size_t count)
{
    while (count--)
    {
        *(buffer++) = value & 0xFF;
        value >>= 8;
    }
}
