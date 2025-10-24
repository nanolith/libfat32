/**
 * \file libfat32/model_check/memory.h
 *
 * \brief Macros for handling model memory checks in source code.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <assert.h>
#include <libfat32/macro_tricks.h>

#ifdef CBMC
#define MODEL_CHECK_OBJECT_READ(x, size) \
    __CPROVER_assert(__CPROVER_r_ok((x), (size)), #x " read " #size); \
    REQUIRE_SEMICOLON_HERE
#define MODEL_CHECK_OBJECT_WRITE(x, size) \
    __CPROVER_assert(__CPROVER_w_ok((x), (size)), #x " write " #size); \
    REQUIRE_SEMICOLON_HERE
#else
#define MODEL_CHECK_OBJECT_READ(x, size) REQUIRE_SEMICOLON_HERE
#define MODEL_CHECK_OBJECT_WRITE(x, size) REQUIRE_SEMICOLON_HERE
#endif
