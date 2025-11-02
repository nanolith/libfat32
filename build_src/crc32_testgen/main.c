/**
 * \file crc32_testgen/main.c
 *
 * \brief Generate test vectors for CRC-32 using a reference implementation in
 * Z3.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <stdio.h>
#include <z3.h>

/**
 * \brief This is the SMT-LIB implementation of the bit-wise CRC-32 which we use
 * to derive our test vectors.
 */
static const char smt_crc32_script[] = {
#embed "crc32.smt2"
    ,0 /* ASCII zero this data. */
};

/* forward decls. */
static int context_create(Z3_context* ctx);

/**
 * \brief Entry point for CRC-32 test vector generator.
 *
 * \param argc      Argument count (unused).
 * \param argv      Argument vector (unused).
 *
 * \returns 0 on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    int retval;
    Z3_context ctx;

    /* create the context and load the script. */
    retval = context_create(&ctx);
    if (0 != retval)
    {
        goto done;
    }

    /* TODO - implement. */
    goto cleanup_ctx;

cleanup_ctx:
    Z3_del_context(ctx);

done:
    return retval;
}

/**
 * \brief Create the context for our reference implementation in Z3.
 *
 * \param ctx           Pointer to the context pointer to set on success.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int context_create(Z3_context* ctx)
{
    int retval;
    Z3_config cfg;
    Z3_context tmp;
    Z3_ast_vector parsed;

    /* create the config. */
    cfg = Z3_mk_config();
    if (NULL == cfg)
    {
        fprintf(stderr, "error: could not create Z3 config.\n");
        retval = 1;
        goto done;
    }

    /* create the context. */
    tmp = Z3_mk_context(cfg);
    if (NULL == tmp)
    {
        fprintf(stderr, "error: could not create Z3 context.\n");
        retval = 2;
        goto cleanup_config;
    }

    /* parse the input. */
    parsed =
        Z3_parse_smtlib2_string(
            tmp, smt_crc32_script, 0, NULL, NULL, 0, NULL, NULL);
    if (NULL == parsed)
    {
        fprintf(stderr, "error: could not load reference CRC-32 script.\n");
        retval = 3;
        goto cleanup_context;
    }

    /* we don't actually need to reference these AST values. */
    Z3_ast_vector_dec_ref(tmp, parsed);

    /* success. */
    *ctx = tmp;
    retval = 0;
    goto cleanup_config;

cleanup_context:
    Z3_del_context(tmp);

cleanup_config:
    Z3_del_config(cfg);

done:
    return retval;
}
