/**
 * \file libfat32/model_check/function_contracts.h
 *
 * \brief Macros for handling function contract declarations and assertions.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#include <assert.h>
#include <libfat32/function_decl.h>
#include <libfat32/macro_tricks.h>

#define MODEL_CONTRACT_PRECONDITIONS_BEGIN1(function, ...) \
    FAT32_BEGIN_CONTRACT_HELPER \
    inline void function ## _preconditions(__VA_ARGS__) {
#define MODEL_CONTRACT_PRECONDITIONS_BEGIN(function, ...) \
    MODEL_CONTRACT_PRECONDITIONS_BEGIN1(function, __VA_ARGS__)
