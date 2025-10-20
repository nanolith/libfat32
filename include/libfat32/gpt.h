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

/**
 * \brief The LBA 0 Protective MBR.
 *
 * \note As defined in the UEFI 2.11 Specification (Section 5.2.3 Table 5.3).
 */
typedef struct FAT32_SYM(gpt_protective_mbr) FAT32_SYM(gpt_protective_mbr);

struct FAT32_SYM(gpt_protective_mbr)
{
    uint8_t boot_code[440];
    uint8_t unique_disk_signature[4];
    uint8_t unknown[2];
    FAT32_SYM(gpt_protective_mbr_partition_record) partition_record[4];
    uint16_t signature;
    uint8_t reserved[2];
};

/******************************************************************************/
/* Start of public exports.                                                   */
/******************************************************************************/

#define __INTERNAL_FAT32_IMPORT_gpt_sym(sym) \
    FAT32_BEGIN_EXPORT \
    typedef FAT32_SYM(gpt_protective_mbr_partition_record) \
    sym ## gpt_protective_mbr_partition_record; \
    typedef FAT32_SYM(gpt_protective_mbr) sym ## gpt_protective_mbr; \
    FAT32_END_EXPORT \
    REQUIRE_SEMICOLON_HERE
#define FAT32_IMPORT_gpt_as(sym) \
    __INTERNAL_FAT32_IMPORT_gpt_sym(sym ## _)
#define FAT32_IMPORT_gpt \
    __INTERNAL_FAT32_IMPORT_gpt_sym()

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
