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
    /* verify that this memory region is large enough to hold an MBR. */
    if (size < FAT32_GPT_PROTECTIVE_MBR_MINIMUM_SIZE)
    {
        return FAT32_ERROR_GPT_BAD_SIZE;
    }

    (void)ptr;
    (void)mbr;
    return -1;
}
