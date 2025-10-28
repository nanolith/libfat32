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
#include <string.h>

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
    int retval;

    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_write), ptr, size, rec);

    /* verify that the record size is correct. */
    if (size != FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE)
    {
        retval = FAT32_ERROR_GPT_BAD_SIZE;
        goto done;
    }

    uint8_t* bptr = (uint8_t*)ptr;

    /* clear memory. */
    memset(ptr, 0, size);

    /* write the boot indicator. */
    bptr[0] = rec->boot_indicator;

    /* write the starting chs as little endian. */
    bptr[1] = ((rec->starting_chs      ) & 0xFF);
    bptr[2] = ((rec->starting_chs >>  8) & 0xFF);
    bptr[3] = ((rec->starting_chs >> 16) & 0xFF);

    /* write the OS type record. */
    bptr[4] = rec->os_type;

    /* write the ending chs as little endian. */
    bptr[5] = ((rec->ending_chs      ) & 0xFF);
    bptr[6] = ((rec->ending_chs >>  8) & 0xFF);
    bptr[7] = ((rec->ending_chs >> 16) & 0xFF);

    /* write the starting lba as little endian. */
    bptr[ 8] = ((rec->starting_lba      ) & 0xFF);
    bptr[ 9] = ((rec->starting_lba >>  8) & 0xFF);
    bptr[10] = ((rec->starting_lba >> 16) & 0xFF);
    bptr[11] = ((rec->starting_lba >> 24) & 0xFF);

    /* write the size in lba as little endian. */
    bptr[12] = ((rec->size_in_lba      ) & 0xFF);
    bptr[13] = ((rec->size_in_lba >>  8) & 0xFF);
    bptr[14] = ((rec->size_in_lba >> 16) & 0xFF);
    bptr[15] = ((rec->size_in_lba >> 24) & 0xFF);

    retval = STATUS_SUCCESS;
    goto done;

done:

    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_write),
        retval, ptr, size, rec);

    return retval;
}
