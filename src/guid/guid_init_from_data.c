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
#include <string.h>

/* forward decls. */
static uint64_t read_little_endian(const uint8_t* buffer, size_t count);

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

    /* verify that the binary data is the correct size. */
    if (size != FAT32_GUID_BINARY_SIZE)
    {
        retval = FAT32_ERROR_GUID_DATA_INVALID_SIZE;
        goto done;
    }

    const uint8_t* bptr = (const uint8_t*)ptr;

    id->data1 = read_little_endian(bptr, sizeof(id->data1));
    bptr += sizeof(id->data1);
    id->data2 = read_little_endian(bptr, sizeof(id->data2));
    bptr += sizeof(id->data2);
    id->data3 = read_little_endian(bptr, sizeof(id->data3));
    bptr += sizeof(id->data3);
    memcpy(id->data4, bptr, sizeof(id->data4));
    bptr += sizeof(id->data4);

    retval = STATUS_SUCCESS;
    goto done;

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_init_from_data), retval, id, ptr, size);

    return retval;
}

/**
 * \brief Read a little-endian value from the given buffer.
 *
 * \param buffer            The buffer from which this value is read.
 * \param count             The nmubre of bytes to read.
 *
 * \returns the uint64_t representation of this value.
 */
static uint64_t read_little_endian(const uint8_t* buffer, size_t count)
{
    uint64_t value = 0;

    for (size_t i = 0; i < count; ++i)
    {
        value |= (((uint64_t)buffer[i]) << (i * 8));
    }

    return value;
}
