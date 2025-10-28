/**
 * \file models/shadow/gpt/gpt_protective_mbr_read.c
 *
 * \brief \ref gpt_protective_mbr_read shadow impl.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>

FAT32_IMPORT_gpt;

int nondet_retval();

/**
 * \brief Read a protective mbr from a given location in RAM.
 *
 * \param mbr               The record to populate with RAM data.
 * \param ptr               The pointer from which this data is read.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_read)(
    FAT32_SYM(gpt_protective_mbr)* mbr, const void* ptr, size_t size)
{
    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_read), mbr, ptr, size);

    int retval = nondet_retval();

    switch (retval)
    {
        case STATUS_SUCCESS:
            __CPROVER_havoc_object(mbr);
            MODEL_ASSUME(property_gpt_protective_mbr_valid(mbr));
            break;

        default:
            retval = FAT32_ERROR_GPT_BAD_RECORD;
        case FAT32_ERROR_GPT_BAD_SIZE:
        case FAT32_ERROR_GPT_BAD_RECORD:
        case FAT32_ERROR_GPT_MBR_BAD_SIGNATURE:
            __CPROVER_havoc_object(mbr);
            MODEL_ASSUME(!property_gpt_protective_mbr_valid(mbr));
            break;
    }

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_read), retval, mbr, ptr, size);

    return retval;
}
