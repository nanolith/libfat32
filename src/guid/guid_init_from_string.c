/**
 * \file guid/guid_init_from_string.c
 *
 * \brief Initialize a guid from a string.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>
#include <string.h>

#ifdef CBMC
int isxdigit(int c);
#else
#include <ctype.h>
#endif

/* forward decls. */
static uint32_t convert_digits(const uint8_t* digits, size_t count);

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
    uint8_t digits[32];
    size_t index = 0;

    /* iterate through all digits of the sequence. */
    for (; 0 != *str; ++str)
    {
        if (isxdigit(*str))
        {
            if (index < 32)
            {
                digits[index++] = *str;
            }
            else
            {
                retval = FAT32_ERROR_GUID_STRING_BAD;
                goto done;
            }
        }
    }

    /* The index must be exactly 32. */
    if (32 != index)
    {
        retval = FAT32_ERROR_GUID_STRING_BAD;
        goto done;
    }

    /* convert the digits. */
    id->data1 = convert_digits(digits, 8);
    id->data2 = (uint16_t)convert_digits(digits + 8, 4);
    id->data3 = (uint16_t)convert_digits(digits + 12, 4);
    for (int i = 0; i < 8; ++i)
    {
        id->data4[i] = (uint8_t)convert_digits(digits + 16 + i*2, 2);
    }

    retval = STATUS_SUCCESS;
    goto done;

done:
    memset(digits, 0, sizeof(digits));

    return retval;
}

/**
 * \brief Convert the hex digits into a 32-bit unsigned integer.
 *
 * \param digits            An array of digits to convert.
 * \param count             The number of digits in this array.
 *
 * \returns the uint32_t representation of these digits.
 */
static uint32_t convert_digits(const uint8_t* digits, size_t count)
{
    uint32_t value = 0;

    for (size_t i = 0; i < count; ++i)
    {
        uint32_t val;

        if (digits[i] >= '0' && digits[i] <= '9')
        {
            val = digits[i] - '0';
        }
        else if (digits[i] >= 'A' && digits[i] <= 'F')
        {
            val = digits[i] - 'A' + 10;
        }
        else
        {
            val = digits[i] - 'a' + 10;
        }

        value <<= 4;
        value += val;
    }

    return value;
}
