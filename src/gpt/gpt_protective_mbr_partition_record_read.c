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
#include <string.h>

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
    int retval;

    /* check function preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_read), rec, ptr, size);

    /* verify the memory region size. */
    if (FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE != size)
    {
        retval = FAT32_ERROR_GPT_BAD_SIZE;
        goto done;
    }

    /* clear record. */
    memset(rec, 0, sizeof(*rec));

    /* make working with this memory more convenient. */
    const uint8_t* bptr = (const uint8_t*)ptr;

    /* read the boot indicator. */
    rec->boot_indicator = bptr[0];

    /* For UEFI GPT protection records, the boot indicator must be false. */
    if (0 != rec->boot_indicator)
    {
        retval = FAT32_ERROR_GPT_BAD_RECORD;
        goto done;
    }

    /* read the starting chs as little endian. */
    rec->starting_chs = 0;
    rec->starting_chs |= ((uint32_t)bptr[1]);
    rec->starting_chs |= ((uint32_t)bptr[2]) <<  8;
    rec->starting_chs |= ((uint32_t)bptr[3]) << 16;

    /* read the OS type record. */
    rec->os_type = bptr[4];

    /* read the ending chs as little endian. */
    rec->ending_chs = 0;
    rec->ending_chs |= ((uint32_t)bptr[5]);
    rec->ending_chs |= ((uint32_t)bptr[6]) <<  8;
    rec->ending_chs |= ((uint32_t)bptr[7]) << 16;

    /* read the starting lba as little endian. */
    rec->starting_lba = 0;
    rec->starting_lba |= ((uint32_t)bptr[ 8]);
    rec->starting_lba |= ((uint32_t)bptr[ 9]) <<  8;
    rec->starting_lba |= ((uint32_t)bptr[10]) << 16;
    rec->starting_lba |= ((uint32_t)bptr[11]) << 24;

    /* read the size in lba as little endian. */
    rec->size_in_lba = 0;
    rec->size_in_lba |= ((uint32_t)bptr[12]);
    rec->size_in_lba |= ((uint32_t)bptr[13]) <<  8;
    rec->size_in_lba |= ((uint32_t)bptr[14]) << 16;
    rec->size_in_lba |= ((uint32_t)bptr[15]) << 24;

    /* For UEFI GPT protection records, we are expecting a blank record, or a
     * span protection record. First, check for the blank record. */
    if (0 == rec->starting_chs)
    {
        retval = STATUS_SUCCESS;
    }
    else if (0x200 == rec->starting_chs)
    {
        retval = STATUS_SUCCESS;
    }
    else
    {
        retval = FAT32_ERROR_GPT_BAD_RECORD;
    }

    goto done;

done:

    /* check function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_partition_record_read),
        retval, rec, ptr, size);

    return retval;
}
