/**
 * \file models/shadow/gpt/gpt_protective_mbr_write.c
 *
 * \brief Shadow impl for \ref gpt_protective_mbr_write.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>

FAT32_IMPORT_gpt;

static int nondet_retval();

/**
 * \brief Write a protective mbr to a given location in RAM.
 *
 * \param ptr               The pointer to which this record is written.
 * \param size              The expected size of the written record.
 * \param mbr               The record to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_write)(
    void* ptr, size_t size, const FAT32_SYM(gpt_protective_mbr)* mbr)
{
    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_write), ptr, size, mbr);

    int retval = nondet_retval();

    switch (retval)
    {
        case STATUS_SUCCESS:
            __CPROVER_havoc_object(ptr);
            break;

        default:
        case FAT32_ERROR_GPT_BAD_SIZE:
            retval = FAT32_ERROR_GPT_BAD_SIZE;
            break;
    }

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_write), retval, ptr, size, mbr);

    return retval;
}
