/**
 * \file models/shadow/gpt/gpt_protective_mbr_partition_record_read.c
 *
 * \brief Shadow impl of \ref gpt_protective_mbr_partition_record_read.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>

FAT32_IMPORT_gpt;

int nondet_retval();

/**
 * \brief Read a protective mbr partition record from a given location in RAM.
 *
 * \param rec               The record to populate with RAM data.
 * \param ptr               The pointer from which this data is read.
 * \param size              The size of this data.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_read)(
    FAT32_SYM(gpt_protective_mbr_partition_record)* rec, const void* ptr,
    size_t size)
{
    /* check function preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_read), rec, ptr, size);

    int retval = nondet_retval();

    switch (retval)
    {
        /* on success, the record is valid. */
        case STATUS_SUCCESS:
            __CPROVER_havoc_object(rec);
            MODEL_ASSUME(
                property_gpt_protective_mbr_partition_record_valid(rec));
            break;

        /* With this error code, the record is NOT valid. */
        default:
        case FAT32_ERROR_GPT_BAD_RECORD:
            retval = FAT32_ERROR_GPT_BAD_RECORD;
            __CPROVER_havoc_object(rec);
            MODEL_ASSUME(
                !property_gpt_protective_mbr_partition_record_valid(rec));
            break;

        /* With this error code, the size is bad. */
        case FAT32_ERROR_GPT_BAD_SIZE:
            break;
    }

    /* check function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_read),
        retval, rec, ptr, size);

    return retval;
}
