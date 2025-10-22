/**
 * \file gpt/gpt_protective_mbr_write.c
 *
 * \brief Write an MBR record to memory.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <string.h>

FAT32_IMPORT_gpt;

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
    int retval;

    /* make sure this memory region is at least large enough for this MBR. */
    if (size < FAT32_GPT_PROTECTIVE_MBR_MINIMUM_SIZE)
    {
        return FAT32_ERROR_GPT_BAD_SIZE;
    }

    /* clear memory. */
    memset(ptr, 0, size);

    /* make working with this pointer more convenient. */
    uint8_t* bptr = (uint8_t*)ptr;

    /* write the boot code. */
    memcpy(bptr, mbr->boot_code, sizeof(mbr->boot_code));
    bptr += sizeof(mbr->boot_code);

    /* skip the unique disk signature. */
    bptr += sizeof(mbr->unique_disk_signature);

    /* skip the unknown bytes. */
    bptr += 2;

    /* write each partition record. */
    for (int i = 0; i < 4; ++i)
    {
        retval =
            gpt_protective_mbr_partition_record_write(
                bptr, FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE,
                &mbr->partition_record[i]);
        if (STATUS_SUCCESS != retval)
        {
            return retval;
        }

        bptr += FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE;
    }

    /* write the signature. */
    bptr[0] = (mbr->signature      ) & 0xFF;
    bptr[1] = (mbr->signature >>  8) & 0xFF;

    return STATUS_SUCCESS;
}
