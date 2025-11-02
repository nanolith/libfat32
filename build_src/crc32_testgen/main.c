/**
 * \file crc32_testgen/main.c
 *
 * \brief Generate test vectors for CRC-32 using a reference implementation in
 * Z3.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <stdio.h>

/**
 * \brief This is the SMT-LIB implementation of the bit-wise CRC-32 which we use
 * to derive our test vectors.
 */
static const char smt_crc32_script[] = {
#embed "crc32.smt2"
    ,0 /* ASCII zero this data. */
};

/**
 * \brief Entry point for CRC-32 test vector generator.
 *
 * \param argc      Argument count (unused).
 * \param argv      Argument vector (unused).
 *
 * \returns 0 on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    (void)smt_crc32_script;

    return 1;
}
