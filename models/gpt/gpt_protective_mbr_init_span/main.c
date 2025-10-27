/**
 * \file models/gpt/gpt_protective_mbr_init_span/main.c
 *
 * \brief Model checks for \ref gpt_protective_mbr_init_span.
 *
 * \copyright 2025 Justin Handville.  Please see LICENSE.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <libfat32/model_check/assert.h>
#include <libfat32/status.h>
#include <libfat32/gpt.h>

FAT32_IMPORT_gpt;

size_t nondet_size();

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int retval;
    gpt_protective_mbr mbr;

    /* initialize the record. */
    retval = gpt_protective_mbr_init_span(&mbr, nondet_size());
    if (STATUS_SUCCESS != retval)
    {
        /* this method only fails with a bad size error. */
        MODEL_ASSERT(FAT32_ERROR_GPT_BAD_SIZE == retval);
        return 1;
    }

    return 0;
}
