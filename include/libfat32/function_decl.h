/**
 * \file libfat32/function_decl.h
 *
 * \brief Function declaration macros for libfat32.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <libfat32/config.h>

/*
 * With GCC 3.4 and clang, we can force a warning / error if the return value
 * of a function isn't checked.
 */
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
# if (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || (__GNUC__ > 3)
#  define FN_DECL_MUST_CHECK __attribute__((warn_unused_result))
# endif
#endif

/*
 * For other compilers, skip this check.
 */
#ifndef FN_DECL_MUST_CHECK
# define FN_DECL_MUST_CHECK
#endif

/*
 * The FAT32_UNIQUE_NAME is a UUID-based symbol.
 */
#define FAT32_UNIQUE_NAME 708394ec_192d_4989_9512_fbfb7845b0c6

/*
 * Simple concat macro.
 */
#define FAT32_CONCAT(x,y) x ## y

/*
 * Symbol expansion and combination macro.
 */
#define FAT32_SYM_COMBINE(x, y, z) fat32 ## _ ## x ## _ ## y ## _ ## z
#define FAT32_SYM_COMBINE1(x, y, z) FAT32_SYM_COMBINE(x, y, z)

/*
 * The FAT32_SYM macro elevates a given symbol to the FAT32 namespace.
 */
#define FAT32_SYM(sym) \
    FAT32_SYM_COMBINE1(FAT32_UNIQUE_NAME, FAT32_VERSION_SYM, sym)

/**
 * Begin an export section.
 */
#define FAT32_BEGIN_EXPORT \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-function\"")

/**
 * End an export section.
 */
#define FAT32_END_EXPORT \
    _Pragma("GCC diagnostic pop")
