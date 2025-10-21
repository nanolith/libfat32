/**
 * \file libfat32/status.h
 *
 * \brief Status codes for libfat32.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

/* C++ compatibility. */
# ifdef   __cplusplus
extern "C" {
# endif /*__cplusplus*/

enum fat32_status_codes
{
    STATUS_SUCCESS =                                                        0,
    FAT32_ERROR_GUID_DATA_TOO_SMALL =                                       1,
};

/* C++ compatibility. */
# ifdef   __cplusplus
}
# endif /*__cplusplus*/
