/**
 * \file gpt/gpt_protective_mbr_partition_record_read.c
 *
 * \brief Read a protective MBR partition record from memory.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>

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
    /* verify the memory region size. */
    if (FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE != size)
    {
        return FAT32_ERROR_GPT_BAD_SIZE;
    }

    (void)rec;
    (void)ptr;
    return -1;
}
