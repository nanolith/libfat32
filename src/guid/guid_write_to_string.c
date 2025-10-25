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

/* forward decls. */
static void write_hex_chars(char* str, uint64_t value, size_t count);
static char hex_nibble(unsigned int value);

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

    /* verify that this buffer can hold a guid representation. */
    if (size < FAT32_GUID_STRING_SIZE)
    {
        retval = FAT32_ERROR_GUID_STRING_BAD;
        goto done;
    }

    /* convert to the string representation. */
    write_hex_chars(str,      id->data1, 8);
    write_hex_chars(str +  9, id->data2, 4);
    write_hex_chars(str + 14, id->data3, 4);
    for (int i = 0; i < 2; ++i)
    {
        write_hex_chars(str + 19 + 2*i, id->data4[i], 2);
    }
    for (int i = 2; i < 8; ++i)
    {
        write_hex_chars(str + 20 + 2*i, id->data4[i], 2);
    }
    str[8] = '-';
    str[13] = '-';
    str[18] = '-';
    str[23] = '-';
    str[36] = 0;

    retval = STATUS_SUCCESS;
    goto done;

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(guid_write_to_string), retval, str, size, id);

    return retval;
}

/**
 * \brief Write hex characters to a string.
 *
 * \note This method does not zero terminate the string.
 *
 * \param str           The output string to which the characters are written.
 * \param value         The value to write as a hex string.
 * \param count         The number of characters to write.
 */
static void write_hex_chars(char* str, uint64_t value, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        str[count - i - 1] = hex_nibble(value & 0x0F);
        value >>= 4;
    }
}

/**
 * \brief Return the hex nibble of a given value.
 *
 * \param value         The value to convert.
 *
 * \returns a hex character representation of this nibble.
 */
static char hex_nibble(unsigned int value)
{
    MODEL_ASSERT(value <= 0xF);

    if (value <= 9)
    {
        return value + '0';
    }
    else
    {
        return value - 10 + 'a';
    }
}
