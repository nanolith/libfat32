/**
 * \file gpt/gpt_protective_mbr_partition_record_init_span.c
 *
 * \brief Initialize a protective MBR partition record covering a given span.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <string.h>

FAT32_IMPORT_gpt;

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

    retval = gpt_protective_mbr_partition_record_init_clear(rec);
    if (STATUS_SUCCESS != retval)
    {
        return retval;
    }

    /* compute the disk size in sectors. */
    size_t lba_size = (size / 512UL) - 1UL;
    if (lba_size > 0xFFFFFFFF)
    {
        lba_size = 0xFFFFFFFF;
    }

    /* Set as per UEFI Specification 2.11, section 5.2.3. */
    rec->boot_indicator = 0;
    rec->starting_chs = 0x00000200;
    rec->os_type = 0xEE;
    rec->ending_chs = 0x00FFFFFF;
    rec->starting_lba = 0x00000001;
    rec->size_in_lba = lba_size;

    return STATUS_SUCCESS;
}
