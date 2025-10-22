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
