/**
 * \file crc32_testgen/main.c
 *
 * \brief Generate test vectors for CRC-32 using a reference implementation in
 * Z3.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#include <assert.h>
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
static int canonical_crc(
    uint32_t* result, generator_context* ctx, const void* data, size_t size);
static int crc_bit_step_block_create(
    Z3_ast* fn, generator_context* ctx, Z3_ast crc_in);
static int crc_byte_step_block_create(
    Z3_ast* fn, generator_context* ctx, Z3_ast crc_in, Z3_ast byte_in);

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

    /* run a canonical crc on our basic test vector. */
    uint32_t test_result = 0;
    retval = canonical_crc(&test_result, ctx, "123456789", 9);
    if (0 != retval)
    {
        goto cleanup_ctx;
    }

    /* verify that this matches. */
    if (0xcbf43926 != test_result)
    {
        printf("Expected 0x%08x and got 0x%08x.\n", 0xcbf43926, test_result);
        retval = 1;
        goto cleanup_ctx;
    }

    /* TODO - fill out generation part. */
    retval = 0;
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

    (void)crc_bit_step_block_create;
    (void)crc_byte_step_block_create;

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
        /* convert this bit in the value into a boolean in the array. */
        if ((value >> i) & 1)
        {
            bit_array[i] = true;
        }
        else
        {
            bit_array[i] = false;
        }
    }

    return Z3_mk_bv_numeral(ctx->ctx, bits, bit_array);
}

/**
 * \brief Perform a canonical CRC-32, using the Z3 source.
 *
 * \param result            On success, the result is stored here.
 * \param ctx               The context for creating the solver for this
 *                          invocation.
 * \param data              The data on which this CRC is performed.
 * \param size              The size of this data in bytes.
 *
 * \returns the CRC-32 of this data.
 */
static int canonical_crc(
    uint32_t* result, generator_context* ctx, const void* data, size_t size)
{
    int retval;
    Z3_solver s;
    Z3_model m;

    /* make working with this data more convenient. */
    const uint8_t* bdata = (const uint8_t*)data;

    /* create a solver instance for running this computation. */
    s = Z3_mk_solver(ctx->ctx);
    if (NULL == s)
    {
        fprintf(stderr, "Could not create Z3 solver instance.\n");
        retval = 1;
        goto done;
    }

    /* we will initialize our array with zeroes. */
    Z3_ast v_zero = mk_bv_from_uint64(ctx, 8, 0);
    if (NULL == v_zero)
    {
        fprintf(stderr, "Could not create constant zero.\n");
        retval = 2;
        goto cleanup_s;
    }

    /* create a data array for holding our input data. */
    Z3_ast input_array = Z3_mk_const_array(ctx->ctx, ctx->bv32, v_zero);
    if (NULL == input_array)
    {
        fprintf(stderr, "Could not create input array.\n");
        retval = 3;
        goto cleanup_s;
    }

    /* populate this array. */
    for (size_t i = 0; i < size; ++i)
    {
        /* create the store index. */
        Z3_ast idx = mk_bv_from_uint64(ctx, 32, i);
        if (NULL == idx)
        {
            fprintf(stderr, "Could not create store index.\n");
            retval = 4;
            goto cleanup_s;
        }

        /* create the value to store at this array index. */
        Z3_ast value = mk_bv_from_uint64(ctx, 8, bdata[i]);
        if (NULL == value)
        {
            fprintf(stderr, "Could not create store value.\n");
            retval = 5;
            goto cleanup_s;
        }

        /* create the store. */
        input_array = Z3_mk_store(ctx->ctx, input_array, idx, value);
        if (NULL == input_array)
        {
            fprintf(stderr, "Could not update input array.\n");
            retval = 6;
            goto cleanup_s;
        }
    }

    /* create the input length. */
    Z3_ast length = mk_bv_from_uint64(ctx, 32, size);
    if (NULL == length)
    {
        fprintf(stderr, "Could not create input array length.\n");
        retval = 7;
        goto cleanup_s;
    }

    /* create the function invocation. */
    Z3_ast args[2] = { input_array, length };
    Z3_ast fn_call = Z3_mk_app(ctx->ctx, ctx->crc_fn, 2, args);
    if (NULL == fn_call)
    {
        fprintf(stderr, "Could not create function call.\n");
        retval = 0;
        goto cleanup_s;
    }

    /* create the result variable. */
    Z3_ast crc_result = Z3_mk_fresh_const(ctx->ctx, "result", ctx->bv32);
    if (NULL == crc_result)
    {
        fprintf(stderr, "Could not make result constant.\n");
        retval = 9;
        goto cleanup_s;
    }

    /* create the equality portion of the assertion. */
    Z3_ast eq = Z3_mk_eq(ctx->ctx, crc_result, fn_call);
    if (NULL == eq)
    {
        fprintf(stderr, "Could not make equality.\n");
        retval = 10;
        goto cleanup_s;
    }

    /* make the assertion. */
    Z3_solver_assert(ctx->ctx, s, eq);

    printf("Trace: %s\n", Z3_ast_to_string(ctx->ctx, eq));

    /* check for satisfiability. */
    if (Z3_L_TRUE != Z3_solver_check(ctx->ctx, s))
    {
        fprintf(stderr, "Could not get satisfiability for CRC calculation.\n");
        retval = 11;
        goto cleanup_s;
    }

    /* get the model of the solution. */
    m = Z3_solver_get_model(ctx->ctx, s);
    if (NULL == m)
    {
        fprintf(stderr, "Could not get model.\n");
        retval = 12;
        goto cleanup_s;
    }

    /* get the result value. */
    Z3_ast result_ast;
    if (!Z3_model_eval(ctx->ctx, m, crc_result, true, &result_ast))
    {
        fprintf(stderr, "Could not resolve CRC value via model.\n");
        retval = 13;
        goto cleanup_m;
    }

    /* is this a numeral value? */
    if (Z3_get_ast_kind(ctx->ctx, result_ast) != Z3_NUMERAL_AST)
    {
        fprintf(stderr, "Unexpected kind.\n");
        retval = 14;
        goto cleanup_m;
    }

    /* get the result as a numeral. */
    uint64_t numeral_result;
    if (!Z3_get_numeral_uint64(ctx->ctx, result_ast, &numeral_result))
    {
        fprintf(stderr, "Could not resolve CRC result as a numeral.\n");
        retval = 14;
        goto cleanup_m;
    }

    *result = numeral_result;
    retval = 0;
    goto cleanup_m;

cleanup_m:
    Z3_model_dec_ref(ctx->ctx, m);

cleanup_s:
    Z3_solver_dec_ref(ctx->ctx, s);

done:
    return retval;
}

/**
 * \brief Create a block of code equivalent to the crc-bit-step function used to
 * build the crc-byte-step function.
 *
 * \param fn            Pointer to the AST node to set to this ast definition.
 * \param ctx           The context to use for this operation.
 * \param crc_in        The CRC input parameter for this shift.
 *
 * \code
 *
 * (define-fun crc-bit-step ((crc-in (_ BitVec 32))) (_ BitVec 32)
 *   (let ((shifted (bvlshr crc-in bv-one-32)))
 *     (let ((bitval ((_ extract 0 0) crc-in)))
 *       (ite (= bitval #b1)
 *         (bvxor shifted poly)
 *         shifted))))
 *
 * \endcode
 *
 * \returns 0 on success and non-zero on failure.
 */
static int crc_bit_step_block_create(
    Z3_ast* fn, generator_context* ctx, Z3_ast crc_in)
{
    int retval;

    /* create the polynomial constant. */
    Z3_ast polynomial = mk_bv_from_uint64(ctx, 32, 0xedb88320);
    if (NULL == polynomial)
    {
        fprintf(stderr, "error: could not make polynomial constant.\n");
        retval = 1;
        goto done;
    }

    /* create the 32-bit one constant. */
    Z3_ast one_u32 = mk_bv_from_uint64(ctx, 32, 1);
    if (NULL == one_u32)
    {
        fprintf(stderr, "error: could not make 32-bit one constant.\n");
        retval = 2;
        goto done;
    }

    /* create the 1-bit one constant. */
    Z3_ast one_u1 = mk_bv_from_uint64(ctx, 1, 1);
    if (NULL == one_u1)
    {
        fprintf(stderr, "error: could not make 1-bit one constant.\n");
        retval = 3;
        goto done;
    }

    /* right shift the CRC input by one. */
    Z3_ast shift_op = Z3_mk_bvlshr(ctx->ctx, crc_in, one_u32);
    if (NULL == shift_op)
    {
        fprintf(stderr, "error: could not make shift operation.\n");
        retval = 4;
        goto done;
    }

    /* get the least-significant bit. */
    Z3_ast lsb = Z3_mk_extract(ctx->ctx, 0, 0, crc_in);
    if (NULL == lsb)
    {
        fprintf(stderr, "error: could not extract lsb.\n");
        retval = 4;
        goto done;
    }

    /* create the comparison. */
    Z3_ast cmp = Z3_mk_eq(ctx->ctx, lsb, one_u1);
    if (NULL == cmp)
    {
        fprintf(stderr, "error: could not make comparison.\n");
        retval = 5;
        goto done;
    }

    /* create the "then" branch. */
    Z3_ast then_branch = Z3_mk_bvxor(ctx->ctx, shift_op, polynomial);
    if (NULL == then_branch)
    {
        fprintf(stderr, "error: could not make then branch.\n");
        retval = 6;
        goto done;
    }

    /* create the "else" branch. */
    Z3_ast else_branch = shift_op;

    /* make the if-then-else expression. */
    Z3_ast ite_expr = Z3_mk_ite(ctx->ctx, cmp, then_branch, else_branch);
    if (NULL == ite_expr)
    {
        fprintf(stderr, "error: could not make if-the-else expression.\n");
        retval = 7;
        goto done;
    }

    /* success. */
    *fn = ite_expr;
    retval = 0;
    goto done;

done:
    return retval;
}

/**
 * \brief Create a block of code equivalent to the crc-byte-step function used
 * to build the crc-recursive-loop function.
 *
 * \param fn            Pointer to the AST node to set to this ast definition.
 * \param ctx           The context to use for this operation.
 * \param crc_in        The CRC input parameter for this step.
 * \param byte_in       The byte input parameter for this step.
 *
 * \code
 *
 * (define-fun crc-byte-step
 *         ((crc-in (_ BitVec 32)) (byte-in-8b (_ BitVec 8))) (_ BitVec 32)
 *   (let ((s0 (bvxor crc-in ((_ zero_extend 24) byte-in-8b))))
 *     (let ((s1 (crc-bit-step s0)))
 *       (let ((s2 (crc-bit-step s1)))
 *         (let ((s3 (crc-bit-step s2)))
 *           (let ((s4 (crc-bit-step s3)))
 *             (let ((s5 (crc-bit-step s4)))
 *               (let ((s6 (crc-bit-step s5)))
 *                 (let ((s7 (crc-bit-step s6)))
 *                   (let ((s8 (crc-bit-step s7)))
 *                     s8))))))))))
 *
 * \endcode
 *
 * \returns 0 on success and non-zero on failure.
 */
static int crc_byte_step_block_create(
    Z3_ast* fn, generator_context* ctx, Z3_ast crc_in, Z3_ast byte_in)
{
    int retval;

    /* zero-extend the input byte. */
    Z3_ast zero_extend = Z3_mk_zero_ext(ctx->ctx, 24, byte_in);
    if (NULL == zero_extend)
    {
        fprintf(stderr, "error: could not make zero-extend expression.\n");
        retval = 1;
        goto done;
    }

    /* s0: xor the zero-extended byte and the crc input. */
    Z3_ast s = Z3_mk_bvxor(ctx->ctx, crc_in, zero_extend);
    if (NULL == s)
    {
        fprintf(stderr, "error: could not make s0 expression.\n");
        retval = 2;
        goto done;
    }

    /* perform the crc bit step on each bit of this byte. */
    for (int i = 0; i < 8; ++i)
    {
        retval = crc_bit_step_block_create(&s, ctx, s);
        if (0 != retval)
        {
            goto done;
        }
    }

    /* success. */
    *fn = s;
    goto done;

done:
    return retval;
}
