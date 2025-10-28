/**
 * \file gpt/gpt_protective_mbr_read.c
 *
 * \brief Read an MBR record from memory.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <string.h>

FAT32_IMPORT_gpt;

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
    int retval;

    /* function contract preconditions. */
    MODEL_CONTRACT_CHECK_PRECONDITIONS(
        FAT32_SYM(gpt_protective_mbr_read), mbr, ptr, size);

    /* verify that this memory region is large enough to hold an MBR. */
    if (size < FAT32_GPT_PROTECTIVE_MBR_MINIMUM_SIZE)
    {
        retval = FAT32_ERROR_GPT_BAD_SIZE;
        goto done;
    }

    /* clear mbr. */
    memset(mbr, 0, sizeof(*mbr));

    /* make working with the memory region more convenient. */
    const uint8_t* bptr = (const uint8_t*)ptr;

    /* read the boot code. */
    memcpy(mbr->boot_code, bptr, sizeof(mbr->boot_code));
    bptr += sizeof(mbr->boot_code);

    /* read the unique disk signature. */
    memcpy(
        mbr->unique_disk_signature, bptr, sizeof(mbr->unique_disk_signature));
    bptr += sizeof(mbr->unique_disk_signature);

    /* verify this signature matches UEFI specification. */
    if (
        (0 != mbr->unique_disk_signature[0])
     || (0 != mbr->unique_disk_signature[1])
     || (0 != mbr->unique_disk_signature[2])
     || (0 != mbr->unique_disk_signature[3]))
    {
        retval = FAT32_ERROR_GPT_BAD_RECORD;
        goto done;
    }

    /* skip the unknown bytes. */
    bptr += 2;

    /* read each of the partition records. */
    for (int i = 0; i < 4; ++i)
    {
        retval =
            gpt_protective_mbr_partition_record_read(
                &mbr->partition_record[i], bptr,
                FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE);
        if (STATUS_SUCCESS != retval)
        {
            goto done;
        }

        bptr += FAT32_GPT_PROTECTIVE_MBR_PARTITION_RECORD_SIZE;
    }

    /* read the signature. */
    mbr->signature = 0;
    mbr->signature |= ((uint16_t)bptr[0]);
    mbr->signature |= ((uint16_t)bptr[1]) << 8;

    /* verify signature. */
    if (0xAA55 != mbr->signature)
    {
        retval = FAT32_ERROR_GPT_MBR_BAD_SIGNATURE;
        goto done;
    }

    retval = STATUS_SUCCESS;
    goto done;

done:
    /* function contract postconditions. */
    MODEL_CONTRACT_CHECK_POSTCONDITIONS(
        FAT32_SYM(gpt_protective_mbr_read), retval, mbr, ptr, size);

    return retval;
}
