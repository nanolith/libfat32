/**
 * \file models/gpt/gpt_protective_mbr_partition_record_init_clear_shadow/main.c
 *
 * \brief Model checks for \ref gpt_protective_mbr_partition_record_init_clear
 * shadow impl.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/gpt.h>

FAT32_IMPORT_gpt;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    gpt_protective_mbr_partition_record rec;

    /* initialize the record. */
    retval = gpt_protective_mbr_partition_record_init_clear(&rec);
    /* this method can only succeed. */
    MODEL_ASSERT(STATUS_SUCCESS == retval);

    return 0;
}
