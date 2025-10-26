/**
 * \file gpt/gpt_protective_mbr_partition_record_init_clear.c
 *
 * \brief Initialize an empty protective mbr partition record.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <string.h>

/**
 * \brief Initialize an empty protective mbr partition record.
 *
 * \param rec               The record to initialize.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_init_clear)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
{
    int retval;

    /* check preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_init_clear), rec);

    memset(rec, 0, sizeof(*rec));

    retval = STATUS_SUCCESS;
    goto done;

done:
    /* check postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_init_clear), retval, rec);

    return retval;
}
