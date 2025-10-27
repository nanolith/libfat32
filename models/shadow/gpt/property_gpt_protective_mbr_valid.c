/**
 * \file models/shadow/gpt/property_gpt_protective_mbr_valid.c
 *
 * \brief Verify that a given GPT protective MBR is valid.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>

FAT32_IMPORT_gpt;

/**
 * \brief Returns true if the given protective MBR is valid.
 *
 * \note A valid protective MBR has all records set as expected, one
 * protective span partition record, and three empty partition records.
 *
 * \param mbr           The MBR to check.
 *
 * \returns true if this record is valid and false otherwise.
 */
bool FAT32_SYM(property_gpt_protective_mbr_valid)(
    const FAT32_SYM(gpt_protective_mbr)* mbr)
{
    MODEL_CHECK_OBJECT_READ(mbr, sizeof(*mbr));

    /* verify that the unique mbr disk signature is 0. */
    if (0 != mbr->unique_disk_signature)
    {
        return false;
    }

    /* verify that the signature is 0xAA55. */
    if (0xAA55 != mbr->signature)
    {
        return false;
    }

    /* verify that all four partition records are valid. */
    for (int i = 0; i < 4; ++i)
    {
        if (
            !property_gpt_protective_mbr_partition_record_valid(
                &mbr->partition_record[i]))
        {
            return false;
        }
    }

    /* For our opinionated library, the first partition record is the span
     * record. */
    if (0xEE != mbr->partition_record[0].os_type)
    {
        return false;
    }

    /* if all of these tests pass, the record must be valid. */
    return true;
}
