/**
 * \file models/shadow/gpt/gpt_protective_mbr_partition_record_init_span.c
 *
 * \brief Shadow impl of \ref gpt_protective_mbr_partition_record_init_span.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <string.h>

FAT32_IMPORT_gpt;

int nondet_retval();
size_t nondet_lba_size();

/**
 * \brief Initialize a protective mbr partition record spanning the entire disk.
 *
 * \param rec               The record to initialize.
 * \param size              Size of the entire disk in bytes.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_init_span)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, size_t size)
{
    int retval;

    /* verify preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_init_span), rec, size);

    retval = nondet_retval();

    if (STATUS_SUCCESS == retval)
    {
        rec->os_type = 0xEE;
        MODEL_ASSUME(property_gpt_protective_mbr_partition_record_valid(rec));
    }
    else
    {
        retval = FAT32_ERROR_GPT_BAD_SIZE;
    }

    /* verify postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_init_span), retval, rec,
        size);

    return retval;
}
