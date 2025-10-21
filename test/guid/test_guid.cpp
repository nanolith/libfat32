/**
 * \file test/guid/test_guid.cpp
 *
 * \brief Unit tests for guid interface.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/guid.h>
#include <libfat32/status.h>
#include <minunit/minunit.h>

FAT32_IMPORT_guid;

TEST_SUITE(guid);

/**
 * A guid requires 16 bytes of binary data to be initialized.
 */
TEST(guid_init_from_data_small_size)
{
    uint8_t data[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    size_t DATA_SIZE = 4;
    guid id;

    TEST_ASSERT(
        FAT32_ERROR_GUID_DATA_INVALID_SIZE
            == guid_init_from_data(&id, data, DATA_SIZE));
}
