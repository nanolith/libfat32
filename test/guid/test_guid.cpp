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
#include <string.h>

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

/**
 * A guid requires 16 bytes of binary data to be initialized.
 */
TEST(guid_init_from_data_large_size)
{
    uint8_t data[24] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    size_t DATA_SIZE = sizeof(data);
    guid id;

    TEST_ASSERT(
        FAT32_ERROR_GUID_DATA_INVALID_SIZE
            == guid_init_from_data(&id, data, DATA_SIZE));
}

/**
 * All zero guid data results in a zero guid.
 */
TEST(guid_init_from_data_zeroes)
{
    uint8_t data[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    size_t DATA_SIZE = sizeof(data);
    guid id;

    /* precondition: set id to a non-zero value. */
    memset(&id, 0xa5, sizeof(id));

    /* initialization should succeed. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == guid_init_from_data(&id, data, DATA_SIZE));

    /* postcondition: id should be zero. */
    TEST_EXPECT(0 == id.data1);
    TEST_EXPECT(0 == id.data2);
    TEST_EXPECT(0 == id.data3);
    TEST_EXPECT(0 == memcmp(id.data4, data + 8, sizeof(id.data4)));
}

/**
 * guids are mixed-endian. LE, LE, LE, BE.
 */
TEST(guid_init_from_data_mixed_endian)
{
    uint8_t data[16] = {
        0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x01, 0x02,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    size_t DATA_SIZE = sizeof(data);
    guid id;

    /* precondition: set id to a non-zero value. */
    memset(&id, 0xa5, sizeof(id));

    /* initialization should succeed. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == guid_init_from_data(&id, data, DATA_SIZE));

    /* postcondition: id should be in the correct mixed-endian format. */
    TEST_EXPECT(0x04030201 == id.data1);
    TEST_EXPECT(0x0201 == id.data2);
    TEST_EXPECT(0x0201 == id.data3);
    TEST_EXPECT(0 == memcmp(id.data4, data + 8, sizeof(id.data4)));
}

/**
 * A guid cannot be initialized from a string with less than 32 hex digits.
 */
TEST(guid_init_from_string_small_strings)
{
    guid id;

    /* each of these strings should fail. */
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, ""));
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, "a"));
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, "ab"));
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, "ab,c123"));
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, "1234567890abcdef1234567890abcde"));
}

/**
 * A guid cannot be initialized from a string with more than 32 hex digits.
 */
TEST(guid_init_from_string_large_string)
{
    guid id;

    /* this string should fail. */
    TEST_ASSERT(
        FAT32_ERROR_GUID_STRING_BAD
            == guid_init_from_string(&id, "1234567890abcdef1234567890abcdef9"));
}

/**
 * Verify a few GUID strings.
 */
TEST(guid_init_from_string_basics)
{
    guid id;

    /* 00000000-0000-0000-0000-000000000000 */
    memset(&id, 0xa5, sizeof(id));
    TEST_ASSERT(
        STATUS_SUCCESS
            == guid_init_from_string(
                    &id, "00000000-0000-0000-0000-000000000000"));
    TEST_EXPECT(0x00000000 == id.data1);
    TEST_EXPECT(0x0000 == id.data2);
    TEST_EXPECT(0x0000 == id.data3);
    const uint8_t first_last8[8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    TEST_EXPECT(0 == memcmp(id.data4, first_last8, sizeof(id.data4)));

    /* dd59d73b-ed16-4a2d-8131-48d1fe45b008 */
    memset(&id, 0xa5, sizeof(id));
    TEST_ASSERT(
        STATUS_SUCCESS
            == guid_init_from_string(
                    &id, "dd59d73b-ed16-4a2d-8131-48d1fe45b008"));
    TEST_EXPECT(0xdd59d73b == id.data1);
    TEST_EXPECT(0xed16 == id.data2);
    TEST_EXPECT(0x4a2d == id.data3);
    const uint8_t second_last8[8] = {
        0x81, 0x31, 0x48, 0xd1, 0xfe, 0x45, 0xb0, 0x08 };
    TEST_EXPECT(0 == memcmp(id.data4, second_last8, sizeof(id.data4)));
}
