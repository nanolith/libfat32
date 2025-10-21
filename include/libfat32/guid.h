/**
 * \file libfat32/guid.h
 *
 * \brief Representation of the Microsoft GUID.
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
 * \brief A GUID is similar to a UUID, but it is serialized differently, both ta
 * and from binary fields and to and from strings.
 */
typedef struct FAT32_SYM(guid) FAT32_SYM(guid);

struct FAT32_SYM(guid)
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
};

/******************************************************************************/
/* Start of public exports.                                                   */
/******************************************************************************/

#define __INTERNAL_FAT32_IMPORT_guid_sym(sym) \
    FAT32_BEGIN_EXPORT \
    typedef FAT32_SYM(guid) sym ## guid; \
    FAT32_END_EXPORT \
    REQUIRE_SEMICOLON_HERE
#define FAT32_IMPORT_guid_as(sym) \
    __INTERNAL_FAT32_IMPORT_guid_sym(sym ## _)
#define FAT32_IMPORT_guid \
    __INTERNAL_FAT32_IMPORT_guid_sym()

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
