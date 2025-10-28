/**
 * \file models/gpt/gpt_protective_mbr_write_shadow/main.c
 *
 * \brief Model checks for \ref gpt_protective_mbr_write shadow impl.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/gpt.h>

FAT32_IMPORT_gpt;

size_t nondet_size();

size_t record_size()
{
    size_t ret = nondet_size();
    if (ret > 40)
    {
        ret = 40;
    }

    return ret;
};

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    uint8_t data[600];
    gpt_protective_mbr mbr;

    /* create a record. */
    __CPROVER_havoc_object(&mbr);
    MODEL_ASSUME(property_gpt_protective_mbr_valid(&mbr));

    /* write the record. */
    retval = gpt_protective_mbr_write(data, record_size(), &mbr);
    if (STATUS_SUCCESS != retval)
    {
        /* this method only fails with one of the following errors. */
        MODEL_ASSERT(FAT32_ERROR_GPT_BAD_SIZE == retval);

        return 1;
    }

    return 0;
}
