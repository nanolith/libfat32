/**
 * \file gpt/gpt_protective_mbr_partition_record_write.c
 *
 * \brief Write a protective MBR partition record to memory.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>

/**
 * \brief Write a protective mbr partition record to a given location in RAM.
 *
 * \param ptr               The pointer to which this record is written.
 * \param size              The expected size of the written record.
 * \param rec               The record to write.
 *
 * \returns a status code indicating success or failure.
 *      - STATUS_SUCCESS on success.
 *      - a non-zero error code on failure.
 */
int FN_DECL_MUST_CHECK
FAT32_SYM(gpt_protective_mbr_partition_record_write)(
    void* ptr, size_t size,
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
{
    /* verify that the record size is correct. */
    if (size != FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE)
    {
        return FAT32_ERROR_GPT_BAD_SIZE;
    }

    (void)ptr;
    (void)rec;
    return -1;
}
