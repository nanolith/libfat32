/**
 * \file models/shadow/gpt/property_gpt_protective_mbr_partition_record_valid.c
 *
 * \brief Verify that a given GPT protective MBR partition record is valid.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>

/**
 * \brief Returns true if the given protective MBR partition record is valid.
 *
 * \note Valid records here are either empty records or the protective span
 * record.
 *
 * \param rec           The record to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_protective_mbr_partition_record_valid)(
    const FAT32_SYM(gpt_protective_mbr_partition_record)* rec)
{
    MODEL_CHECK_OBJECT_READ(rec, sizeof(*rec));

    /* is this a zero record? */
    if (
        (0 == rec->boot_indicator)
     && (0 == rec->starting_chs)
     && (0 == rec->os_type)
     && (0 == rec->ending_chs)
     && (0 == rec->starting_lba)
     && (0 == rec->size_in_lba))
    {
        return true;
    }

    /* protective GPT partitions can't be bootable. */
    if (0 != rec->boot_indicator)
    {
        return false;
    }

    /* starting CHS should be 0x200. */
    if (0x00000200 != rec->starting_chs)
    {
        return false;
    }

    /* OS type should be 0xEE. */
    if (0xEE != rec->os_type)
    {
        return false;
    }

    /* ending CHS should be set to dummy sentry value. */
    if (0x00FFFFFF != rec->ending_chs)
    {
        return false;
    }

    /* starting LBA should be 1. */
    if (0x00000001 != rec->starting_lba)
    {
        return false;
    }

    /* size in LBA should be at least large enough to cover the partition
     * records. */
    if (rec->size_in_lba < 33)
    {
        return false;
    }

    /* if all of these tests pass, the record must be valid. */
    return true;
}
