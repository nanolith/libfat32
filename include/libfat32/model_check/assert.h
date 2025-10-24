/**
 * \file libfat32/model_check/assert.h
 *
 * \brief Macros for handling model check assertions, assumptions, and
 * exceptions in source code.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <assert.h>
#include <libfat32/macro_tricks.h>

#ifdef CBMC
#define MODEL_ASSERT(x) __CPROVER_assert((x), #x); REQUIRE_SEMICOLON_HERE
#define MODEL_ASSUME(x) __CPROVER_assume((x)); REQUIRE_SEMICOLON_HERE
#define MODEL_EXEMPT(x)
#else
#define MODEL_ASSERT(x) REQUIRE_SEMICOLON_HERE
#define MODEL_ASSUME(x) REQUIRE_SEMICOLON_HERE
#define MODEL_EXEMPT(x) x
#endif
