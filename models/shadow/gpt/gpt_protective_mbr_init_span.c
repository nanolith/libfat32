/**
 * \file models/shadow/gpt/gpt_protective_mbr_init_span.c
 *
 * \brief Shadow impl of \ref gpt_protective_mbr_init_span.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <string.h>

FAT32_IMPORT_gpt;

static int nondet_retval();

/**
 * \brief Initialize a protective mbr spanning the entire disk.
 *
 * \param mbr               The record to initialize.
 * \param size              Size of the entire disk in bytes.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_init_span)(
    FAT32_SYM(gpt_protective_mbr)* mbr, size_t size)
{
    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_init_span), mbr, size);

    int retval = nondet_retval();

    if (STATUS_SUCCESS == retval)
    {
        __CPROVER_havoc_object(mbr);
        MODEL_ASSUME(property_gpt_protective_mbr_valid(mbr));
    }
    else
    {
        retval = FAT32_ERROR_GPT_BAD_SIZE;
    }

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_init_span), retval, mbr, size);

    return retval;
}
