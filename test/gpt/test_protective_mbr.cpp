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

/**
 * Verify that we can write a protective MBR record covering an "oversize" span.
 */
TEST(gpt_protective_mbr_partition_record_write_oversize_span)
{
    gpt_protective_mbr_partition_record rec;
    uint8_t buffer[16];
    const size_t disk_size = 5UL * 1024UL * 1024UL * 1024UL * 1024UL;

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
    TEST_EXPECT(0xFF == buffer[15]);
}

/**
 * Verify that attempting to read a protective MBR record from an incorrectly
 * sized memory location fails.
 */
TEST(gpt_protective_mbr_partition_record_read_bad_size)
{
    gpt_protective_mbr_partition_record rec;
    uint8_t buffer[32];

    /* precondition: zero memory. */
    memset(buffer, 0, sizeof(buffer));

    /* reads from the wrong memory size fail. */
    TEST_ASSERT(
        FAT32_ERROR_GPT_BAD_SIZE ==
            gpt_protective_mbr_partition_record_read(&rec, buffer, 15));
    TEST_ASSERT(
        FAT32_ERROR_GPT_BAD_SIZE ==
            gpt_protective_mbr_partition_record_read(&rec, buffer, 24));
}

/**
 * Verify that we can write an empty protective MBR partition record and read it
 * back.
 */
TEST(gpt_protective_mbr_partition_record_write_and_read_empty)
{
    gpt_protective_mbr_partition_record rec;
    gpt_protective_mbr_partition_record read_rec;
    uint8_t buffer[16];

    /* precondition: fill buffer with junk. */
    memset(buffer, 0x5a, sizeof(buffer));
    memset(&read_rec, 0x5a, sizeof(read_rec));

    /* create an empty partition record. */
    TEST_ASSERT(
        STATUS_SUCCESS == gpt_protective_mbr_partition_record_init_clear(&rec));

    /* write succeeds. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_write(buffer, 16, &rec));

    /* read the record back. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_read(
                    &read_rec, buffer, sizeof(buffer)));

    /* verify fields. */
    TEST_EXPECT(rec.boot_indicator == read_rec.boot_indicator);
    TEST_EXPECT(rec.starting_chs == read_rec.starting_chs);
    TEST_EXPECT(rec.os_type == read_rec.os_type);
    TEST_EXPECT(rec.ending_chs == read_rec.ending_chs);
    TEST_EXPECT(rec.starting_lba == read_rec.starting_lba);
    TEST_EXPECT(rec.size_in_lba == read_rec.size_in_lba);
}

/**
 * Verify that we can write a small protective MBR partition record and read it
 * back.
 */
TEST(gpt_protective_mbr_partition_record_write_and_read_small)
{
    gpt_protective_mbr_partition_record rec;
    gpt_protective_mbr_partition_record read_rec;
    uint8_t buffer[16];
    const size_t disk_size = 128UL * 1024UL * 1024UL * 1024UL;

    /* precondition: fill buffer with junk. */
    memset(buffer, 0x5a, sizeof(buffer));
    memset(&read_rec, 0x5a, sizeof(read_rec));

    /* create a span partition record. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(&rec, disk_size));

    /* write succeeds. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_write(buffer, 16, &rec));

    /* read the record back. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_read(
                    &read_rec, buffer, sizeof(buffer)));

    /* verify fields. */
    TEST_EXPECT(rec.boot_indicator == read_rec.boot_indicator);
    TEST_EXPECT(rec.starting_chs == read_rec.starting_chs);
    TEST_EXPECT(rec.os_type == read_rec.os_type);
    TEST_EXPECT(rec.ending_chs == read_rec.ending_chs);
    TEST_EXPECT(rec.starting_lba == read_rec.starting_lba);
    TEST_EXPECT(rec.size_in_lba == read_rec.size_in_lba);
}

/**
 * Verify that we can write an oversize protective MBR partition record and read
 * it back.
 */
TEST(gpt_protective_mbr_partition_record_write_and_read_oversize)
{
    gpt_protective_mbr_partition_record rec;
    gpt_protective_mbr_partition_record read_rec;
    uint8_t buffer[16];
    const size_t disk_size = 4UL * 1024UL * 1024UL * 1024UL * 1024UL;

    /* precondition: fill buffer with junk. */
    memset(buffer, 0x5a, sizeof(buffer));
    memset(&read_rec, 0x5a, sizeof(read_rec));

    /* create a span partition record. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(&rec, disk_size));

    /* write succeeds. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_write(buffer, 16, &rec));

    /* read the record back. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_read(
                    &read_rec, buffer, sizeof(buffer)));

    /* verify fields. */
    TEST_EXPECT(rec.boot_indicator == read_rec.boot_indicator);
    TEST_EXPECT(rec.starting_chs == read_rec.starting_chs);
    TEST_EXPECT(rec.os_type == read_rec.os_type);
    TEST_EXPECT(rec.ending_chs == read_rec.ending_chs);
    TEST_EXPECT(rec.starting_lba == read_rec.starting_lba);
    TEST_EXPECT(rec.size_in_lba == read_rec.size_in_lba);
}

/**
 * We can initialize a "span" protective MBR covering 128GB.
 */
TEST(gpt_protective_mbr_init_span_128GB)
{
    gpt_protective_mbr mbr;
    gpt_protective_mbr_partition_record span_record;
    gpt_protective_mbr_partition_record unused_record;
    size_t disk_size = 128UL * 1024UL * 1024UL * 1024UL;

    /* precondition: fill with junk. */
    memset(&mbr, 0x5a, sizeof(mbr));

    /* initialize this data structure. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_init_span(&mbr, disk_size));

    /* initialize our span record. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_span(
                    &span_record, disk_size));

    /* initialize our unused record. */
    TEST_ASSERT(
        STATUS_SUCCESS
            == gpt_protective_mbr_partition_record_init_clear(&unused_record));

    /* the boot code should be a nop sled followed by a halt and a jump. */
    for (int i = 0; i < 437; ++i)
    {
        /* NOP. */
        TEST_EXPECT(0x90 == mbr.boot_code[i]);
    }
    /* HALT. */
    TEST_EXPECT(0xF4 == mbr.boot_code[437]);
    /* JMP back to the HALT. */
    TEST_EXPECT(0xEB == mbr.boot_code[438]);
    TEST_EXPECT(0xFD == mbr.boot_code[439]);

    /* the signature is all zeroes. */
    for (int i = 0; i < 4; ++i)
    {
        TEST_EXPECT(0x00 == mbr.unique_disk_signature[i]);
    }

    /* the first partition record matches our span record. */
    TEST_EXPECT(
        0
            == memcmp(
                    &mbr.partition_record[0], &span_record,
                    sizeof(span_record)));

    /* all other records are unused. */
    for (int i = 1; i < 4; ++i)
    {
        TEST_EXPECT(
            0
                == memcmp(
                        &mbr.partition_record[i], &unused_record,
                        sizeof(unused_record)));
    }

    /* the signature is 0xAA55. */
    TEST_EXPECT(0xAA55 == mbr.signature);
}
