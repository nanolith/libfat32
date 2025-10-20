/**
 * \file libfat32/gpt.h
 *
 * \brief An interface for GPT.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <libfat32/function_decl.h>
#include <stdint.h>

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

/**
 * \brief A partition record in the protective MBR.
 *
 * \note As defined in the UEFI 2.11 Specification (Section 5.2.3 Table 5.4);
 */
typedef struct FAT32_SYM(gpt_protective_mbr_partition_record)
FAT32_SYM(gpt_protective_mbr_partition_record);

struct FAT32_SYM(gpt_protective_mbr_partition_record)
{
    uint8_t boot_indicator;
    uint32_t starting_chs; //Note: serialized as 24-bit.
    uint8_t os_type;
    uint32_t ending_chh; //Note: serialized as 24-bit.
    uint32_t starting_lba;
    uint32_t size_in_lba;
};

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
