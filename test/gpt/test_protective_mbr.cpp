/**
 * \file test/gpt/test_protective_mbr.cpp
 *
 * \brief Unit tests for protective MBR interface.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/gpt.h>
#include <libfat32/status.h>
#include <minunit/minunit.h>
#include <string.h>

FAT32_IMPORT_gpt;

TEST_SUITE(protective_mbr);

/**
 * We can initialize an empty MBR partition record.
 */
TEST(gpt_protective_mbr_partition_record_init_clear)
{
    gpt_protective_mbr_partition_record rec;

    /* precondition: fill with junk. */
    memset(&rec, 0xa5, sizeof(rec));

    /* initialize this data structure. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_clear(&rec));

    /* verify the partition record data. */
    TEST_EXPECT(0 == rec.boot_indicator);
    TEST_EXPECT(0 == rec.starting_chs);
    TEST_EXPECT(0 == rec.os_type);
    TEST_EXPECT(0 == rec.ending_chs);
    TEST_EXPECT(0 == rec.starting_lba);
    TEST_EXPECT(0 == rec.size_in_lba);
}

/**
 * We can initialize a "span" mbr partition record covering 128GB.
 */
TEST(gpt_protective_mbr_partition_record_init_span_128GB)
{
    gpt_protective_mbr_partition_record rec;
    const size_t disk_size = 128UL * 1024UL * 1024UL * 1024UL;
    const size_t disk_sectors = disk_size / 512UL;
    const size_t lba_size = disk_sectors - 1UL;

    /* precondition: fill with junk. */
    memset(&rec, 0xa5, sizeof(rec));

    /* initialize this data structure. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(&rec, disk_size));

    /* verify the partition record data. */
    TEST_EXPECT(0 == rec.boot_indicator);
    TEST_EXPECT(0x00000200 == rec.starting_chs);
    TEST_EXPECT(0xEE == rec.os_type);
    TEST_EXPECT(0x00FFFFFF == rec.ending_chs);
    TEST_EXPECT(0x00000001 == rec.starting_lba);
    TEST_EXPECT(lba_size == rec.size_in_lba);
}

/**
 * We can initialize an "oversize" "span" mbr partition record covering 4TB.
 */
TEST(gpt_protective_mbr_partition_record_init_span_oversize_4TB)
{
    gpt_protective_mbr_partition_record rec;
    const size_t disk_size = 4UL * 1024UL * 1024UL * 1024UL * 1024UL;

    /* precondition: fill with junk. */
    memset(&rec, 0xa5, sizeof(rec));

    /* initialize this data structure. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(&rec, disk_size));

    /* verify the partition record data. */
    TEST_EXPECT(0 == rec.boot_indicator);
    TEST_EXPECT(0x00000200 == rec.starting_chs);
    TEST_EXPECT(0xEE == rec.os_type);
    TEST_EXPECT(0x00FFFFFF == rec.ending_chs);
    TEST_EXPECT(0x00000001 == rec.starting_lba);
    TEST_EXPECT(0xFFFFFFFF == rec.size_in_lba);
}

/**
 * It is an error to call gpt_protective_mbr_partition_record_write with an
 * invalid size.
 */
TEST(gpt_protective_mbr_partition_record_write_bad_size)
{
    gpt_protective_mbr_partition_record rec;
    uint8_t buffer[32];

    TEST_ASSERT(
        STATUS_SUCCESS == gpt_protective_mbr_partition_record_init_clear(&rec));

    /* a bad size fails. */
    TEST_ASSERT(
        FAT32_ERROR_GPT_BAD_SIZE
            == gpt_protective_mbr_partition_record_write(buffer, 8, &rec));
    TEST_ASSERT(
        FAT32_ERROR_GPT_BAD_SIZE
            == gpt_protective_mbr_partition_record_write(buffer, 24, &rec));
}

/**
 * Verify that we can write an empty protective MBR partition record.
 */
TEST(gpt_protective_mbr_partition_record_write_empty)
{
    gpt_protective_mbr_partition_record rec;
    uint8_t buffer[16];

    /* precondition: fill buffer with junk. */
    memset(buffer, 0x5a, sizeof(buffer));

    /* create an empty partition record. */
    TEST_ASSERT(
        STATUS_SUCCESS == gpt_protective_mbr_partition_record_init_clear(&rec));

    /* write succeeds. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_write(buffer, 16, &rec));

    /* an empty record is all zeroes. */
    for (int i = 0; i < 16; ++i)
    {
        TEST_EXPECT(0 == buffer[i]);
    }
}

/**
 * Verify that we can write a protective MBR record covering a small span.
 */
TEST(gpt_protective_mbr_partition_record_write_small_span)
{
    gpt_protective_mbr_partition_record rec;
    uint8_t buffer[16];
    const size_t disk_size = 128UL * 1024UL * 1024UL * 1024UL;

    /* initialize this data structure. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(&rec, disk_size));

    /* write succeeds. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_write(buffer, 16, &rec));

    /* verify the boot indicator. */
    TEST_EXPECT(0 == buffer[0]);

    /* verify the starting chs. */
    TEST_EXPECT(0x00 == buffer[ 1]);
    TEST_EXPECT(0x02 == buffer[ 2]);
    TEST_EXPECT(0x00 == buffer[ 3]);

    /* verify the OS type record. */
    TEST_EXPECT(0xEE == buffer[ 4]);

    /* verify the ending chs. */
    TEST_EXPECT(0xFF == buffer[ 5]);
    TEST_EXPECT(0xFF == buffer[ 6]);
    TEST_EXPECT(0xFF == buffer[ 7]);

    /* verify the starting lba. */
    TEST_EXPECT(0x01 == buffer[ 8]);
    TEST_EXPECT(0x00 == buffer[ 9]);
    TEST_EXPECT(0x00 == buffer[10]);
    TEST_EXPECT(0x00 == buffer[11]);

    /* verify the size in lba. */
    TEST_EXPECT(0xFF == buffer[12]);
    TEST_EXPECT(0xFF == buffer[13]);
    TEST_EXPECT(0xFF == buffer[14]);
    TEST_EXPECT(0x0F == buffer[15]);
}
