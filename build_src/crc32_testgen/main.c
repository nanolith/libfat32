/**
 * \file crc32_testgen/main.c
 *
 * \brief Generate test vectors for CRC-32 using a reference implementation in
 * Z3.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <z3.h>

/**
 * \brief This is the SMT-LIB implementation of the bit-wise CRC-32 which we use
 * to derive our test vectors.
 */
static const char smt_crc32_script[] = {
#embed "crc32.smt2"
    ,0 /* ASCII zero this data. */
};

/**
 * \brief The test vector generator context.
 */
typedef struct generator_context generator_context;
struct generator_context
{
    Z3_context ctx;
    Z3_sort bv8;
    Z3_sort bv32;
    Z3_sort array;
    Z3_func_decl crc_fn;
};

/* forward decls. */
static int context_create(generator_context** ctx);
static void context_release(generator_context* ctx);
static Z3_ast mk_bv_from_uint64(
    generator_context* ctx, unsigned int bits, uint64_t value);

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
    generator_context* ctx;

    /* create the context and load the script. */
    retval = context_create(&ctx);
    if (0 != retval)
    {
        goto done;
    }

    /* TODO - implement. */
    (void)mk_bv_from_uint64;
    goto cleanup_ctx;

cleanup_ctx:
    context_release(ctx);

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
static int context_create(generator_context** ctx)
{
    int retval;
    generator_context* tmp;
    Z3_config cfg;
    Z3_ast_vector parsed;

    /* allocate memory for the context. */
    tmp = (generator_context*)malloc(sizeof(*tmp));
    if (NULL == tmp)
    {
        fprintf(stderr, "error: could not allocate generator context.\n");
        retval = 1;
        goto done;
    }
    memset(tmp, 0, sizeof(*tmp));

    /* create the config. */
    cfg = Z3_mk_config();
    if (NULL == cfg)
    {
        fprintf(stderr, "error: could not create Z3 config.\n");
        retval = 1;
        goto cleanup_tmp;
    }

    /* create the context. */
    tmp->ctx = Z3_mk_context(cfg);
    if (NULL == tmp->ctx)
    {
        fprintf(stderr, "error: could not create Z3 context.\n");
        retval = 2;
        goto cleanup_config;
    }

    /* parse the input. */
    parsed =
        Z3_parse_smtlib2_string(
            tmp->ctx, smt_crc32_script, 0, NULL, NULL, 0, NULL, NULL);
    if (NULL == parsed)
    {
        fprintf(stderr, "error: could not load reference CRC-32 script.\n");
        retval = 3;
        goto cleanup_config;
    }

    /* we don't actually need to reference these AST values. */
    Z3_ast_vector_dec_ref(tmp->ctx, parsed);

    /* create the bv8 type. */
    tmp->bv8 = Z3_mk_bv_sort(tmp->ctx, 8);
    if (NULL == tmp->bv8)
    {
        fprintf(stderr, "error: could not create BitVec 8 type.\n");
        retval = 4;
        goto cleanup_config;
    }

    /* create the bv32 type. */
    tmp->bv32 = Z3_mk_bv_sort(tmp->ctx, 32);
    if (NULL == tmp->bv32)
    {
        fprintf(stderr, "error: could not create BitVec 32 type.\n");
        retval = 5;
        goto cleanup_config;
    }

    /* create the array type. */
    tmp->array = Z3_mk_array_sort(tmp->ctx, tmp->bv32, tmp->bv8);
    if (NULL == tmp->array)
    {
        fprintf(stderr, "error: could not create array type.\n");
        retval = 6;
        goto cleanup_config;
    }

    /* get the function name for our Z3 CRC-32 function. */
    Z3_symbol fn_sym = Z3_mk_string_symbol(tmp->ctx, "crc-of-array");
    if (NULL == fn_sym)
    {
        fprintf(stderr, "error: could not create function symbol.\n");
        retval = 7;
        goto cleanup_config;
    }

    /* define the parameter types. */
    Z3_sort param_types[2] = { tmp->array, tmp->bv32 };

    /* get the function declaration for our Z3 CRC-32 function. */
    tmp->crc_fn = Z3_mk_func_decl(tmp->ctx, fn_sym, 2, param_types, tmp->bv32);
    if (NULL == tmp->crc_fn)
    {
        fprintf(stderr, "error: could not create function decl.\n");
        retval = 8;
        goto cleanup_config;
    }

    /* success. */
    *ctx = tmp;
    tmp = NULL;
    retval = 0;
    goto cleanup_config;

cleanup_config:
    Z3_del_config(cfg);

cleanup_tmp:
    if (NULL != tmp)
    {
        context_release(tmp);
    }

done:
    return retval;
}

/**
 * \brief Release a context.
 *
 * \param ctx           The context to release.
 */
static void context_release(generator_context* ctx)
{
    if (NULL != ctx->ctx)
    {
        Z3_del_context(ctx->ctx);
    }

    free(ctx);
}

/**
 * \brief Create a bit-vector numeral from a uint64_t value and a bit length.
 *
 * \param ctx           The generator context for this operation.
 * \param bits          The bit length of this bit vector.
 * \param value         The uint64_t value to encode as a bit vector.
 *
 * \returns the bit vector, as a Z3_ast, or NULL if this conversion failed.
 */
static Z3_ast mk_bv_from_uint64(
    generator_context* ctx, unsigned int bits, uint64_t value)
{
    bool bit_array[64];

    /* for our purposes, an empty bit vector is meaningless. */
    if (0 == bits)
    {
        return NULL;
    }

    /* encode bits. */
    for (unsigned int i = 0; i < bits; ++i)
    {
        /* the bits array is big-endian, so we start from the MSB back. */
        unsigned int index = (bits - 1) + i;

        /* convert this bit in the value into a boolean in the array. */
        bit_array[i] =
            ((value >> index) & 1)
                ? true
                : false;
    }

    return Z3_mk_bv_numeral(ctx->ctx, bits, bit_array);
}
