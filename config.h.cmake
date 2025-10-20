/**
 * \file libfat32/config.h
 *
 * \brief Generated configuration file for libfat32.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#define MAKE_C_VERSION(X, Y) V ## X ## _ ## Y
#define FAT32_VERSION_SYM \
    MAKE_C_VERSION(@LIBFAT32_VERSION_MAJOR@, @LIBFAT32_VERSION_MINOR@)

#define FAT32_VERSION_STRING \
     "@LIBFAT32_VERSION_MAJOR@.@LIBFAT32_VERSION_MINOR@.@LIBFAT32_VERSION_REL@"
