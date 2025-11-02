/**
 * \file crc32_constants/main.c
 *
 * \brief Build the constants for the crc32 function.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <stdint.h>
#include <stdio.h>

#define CRC32_POLYNOMIAL 0xedb88320L

/**
 * \brief Entry point for CRC-32 constants generator.
 *
 * \param argc      Argument count (unused).
 * \param argv      Argument vector (unused).
 *
 * \returns 0 on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    /* verify that we have an output file. */
    if (argc != 2)
    {
        fprintf(stderr, "Expecting one argument, the output filename.\n");
        return 1;
    }

    /* open the output file for writing. */
    FILE* out = fopen(argv[1], "w");
    if (NULL == out)
    {
        fprintf(stderr, "Could not open %s for writing.\n", argv[1]);
        return 2;
    }

    /* front matter. */
    fprintf(out, "#include <libfat32/crc.h>\n\n");

    /* begin the constant array. */
    fprintf(out, "const uint32_t FAT32_SYM(crc32_constants)[256] = {");

    for (uint32_t i = 0; i < 256; ++i)
    {
        /* the CRC constant starts with the byte. */
        uint32_t c = i;

        /* ensure that the constants respect the 80 column rule. */
        if (0 == (i % 6))
        {
            fprintf(out, "\n    ");
        }

        /* iterate over each bit. */
        for (uint32_t k = 0; k < 8; ++k)
        {
            /* if the bit is set, xor in the CRC polynomial. */
            if (c & 1)
            {
                c = CRC32_POLYNOMIAL ^ (c >> 1);
            }
            /* otherwise, shift this constant down by one. */
            else
            {
                c >>= 1;
            }
        }

        /* emit the completed constant to the C source file. */
        fprintf(out, "0x%08x, ", c);
    }

    /* terminate the array. */
    fprintf(out, "\n};\n");

    /* close the output file. */
    fclose(out);

    return 0;
}
