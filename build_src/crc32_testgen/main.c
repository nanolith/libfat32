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
static int crc_recursive_loop_function_create(
    Z3_func_decl* fn, generator_context* ctx);
static int crc_function_create(generator_context* ctx);
static int generate_unit_tests(generator_context* ctx, FILE* out);
static int generate_unit_test_frontmatter(generator_context* ctx, FILE* out);
static int generate_williams_test(generator_context* ctx, FILE* out);
static int generate_empty_test(generator_context* ctx, FILE* out);
static int generate_constants_exercise_test(generator_context* ctx, FILE* out);
static int generate_full_constants_test(generator_context* ctx, FILE* out);

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

    /* verify that we have an output file. */
    if (argc != 2)
    {
        fprintf(
            stderr, "error: expecting one argument, the output filename.\n");
        retval = 1;
        goto done;
    }

    /* open the output file for writing. */
    FILE* out = fopen(argv[1], "w");
    if (NULL == out)
    {
        fprintf(stderr, "error: could not open %s for writing.\n", argv[1]);
        retval = 2;
        goto done;
    }

    /* create the context and load the script. */
    retval = context_create(&ctx);
    if (0 != retval)
    {
        goto cleanup_out;
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

    /* generate unit tests. */
    retval = generate_unit_tests(ctx, out);
    if (0 != retval)
    {
        goto cleanup_ctx;
    }

    /* success. */
    retval = 0;
    goto cleanup_ctx;

cleanup_ctx:
    context_release(ctx);

cleanup_out:
    fclose(out);

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
        retval = 2;
        goto cleanup_tmp;
    }

    /* create the context. */
    tmp->ctx = Z3_mk_context(cfg);
    if (NULL == tmp->ctx)
    {
        fprintf(stderr, "error: could not create Z3 context.\n");
        retval = 3;
        goto cleanup_config;
    }

    /* create the bv8 type. */
    tmp->bv8 = Z3_mk_bv_sort(tmp->ctx, 8);
    if (NULL == tmp->bv8 || Z3_OK != Z3_get_error_code(tmp->ctx))
    {
        fprintf(stderr, "error: could not create BitVec 8 type.\n");
        retval = 4;
        goto cleanup_config;
    }

    /* create the bv32 type. */
    tmp->bv32 = Z3_mk_bv_sort(tmp->ctx, 32);
    if (NULL == tmp->bv32 || Z3_OK != Z3_get_error_code(tmp->ctx))
    {
        fprintf(stderr, "error: could not create BitVec 32 type.\n");
        retval = 5;
        goto cleanup_config;
    }

    /* create the array type. */
    tmp->array = Z3_mk_array_sort(tmp->ctx, tmp->bv32, tmp->bv8);
    if (NULL == tmp->array || Z3_OK != Z3_get_error_code(tmp->ctx))
    {
        fprintf(stderr, "error: could not create array type.\n");
        retval = 6;
        goto cleanup_config;
    }

    /* create the crc function. */
    retval = crc_function_create(tmp);
    if (0 != retval)
    {
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
    if (0 == bits || bits > 64)
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
    if (NULL == s || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not create Z3 solver instance.\n");
        retval = 1;
        goto done;
    }

    /* we will initialize our array with zeroes. */
    Z3_ast v_zero = mk_bv_from_uint64(ctx, 8, 0);
    if (NULL == v_zero || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not create constant zero.\n");
        retval = 2;
        goto cleanup_s;
    }

    /* create a data array for holding our input data. */
    Z3_ast input_array = Z3_mk_const_array(ctx->ctx, ctx->bv32, v_zero);
    if (NULL == input_array || Z3_OK != Z3_get_error_code(ctx->ctx))
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
        if (NULL == idx || Z3_OK != Z3_get_error_code(ctx->ctx))
        {
            fprintf(stderr, "Could not create store index.\n");
            retval = 4;
            goto cleanup_s;
        }

        /* create the value to store at this array index. */
        Z3_ast value = mk_bv_from_uint64(ctx, 8, bdata[i]);
        if (NULL == value || Z3_OK != Z3_get_error_code(ctx->ctx))
        {
            fprintf(stderr, "Could not create store value.\n");
            retval = 5;
            goto cleanup_s;
        }

        /* create the store. */
        input_array = Z3_mk_store(ctx->ctx, input_array, idx, value);
        if (NULL == input_array || Z3_OK != Z3_get_error_code(ctx->ctx))
        {
            fprintf(stderr, "Could not update input array.\n");
            retval = 6;
            goto cleanup_s;
        }
    }

    /* create the input length. */
    Z3_ast length = mk_bv_from_uint64(ctx, 32, size);
    if (NULL == length || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not create input array length.\n");
        retval = 7;
        goto cleanup_s;
    }

    /* create the function invocation. */
    Z3_ast args[2] = { input_array, length };
    Z3_ast fn_call = Z3_mk_app(ctx->ctx, ctx->crc_fn, 2, args);
    if (NULL == fn_call || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not create function call.\n");
        retval = 0;
        goto cleanup_s;
    }

    /* create the result variable. */
    Z3_ast crc_result = Z3_mk_fresh_const(ctx->ctx, "result", ctx->bv32);
    if (NULL == crc_result || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not make result constant.\n");
        retval = 9;
        goto cleanup_s;
    }

    /* create the equality portion of the assertion. */
    Z3_ast eq = Z3_mk_eq(ctx->ctx, crc_result, fn_call);
    if (NULL == eq || Z3_OK != Z3_get_error_code(ctx->ctx))
    {
        fprintf(stderr, "Could not make equality.\n");
        retval = 10;
        goto cleanup_s;
    }

    /* make the assertion. */
    Z3_solver_assert(ctx->ctx, s, eq);

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
    /* XXX - for some reason, this causes heap corruption. */
    //Z3_solver_dec_ref(ctx->ctx, s);

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

/**
 * \brief Create the CRC recursive loop function, returning the function
 * declaration as an output parameter.
 *
 * \param fn                Pointer to the function decl pointer to receive this
 *                          function on success.
 * \param ctx               The context for this operation.
 *
 * \code
 *
 * (define-fun-rec crc-recursive-loop
 *   (
 *     (data (Array (_ BitVec 32) (_ BitVec 8))) ; input array
 *     (len (_ BitVec 32))                       ; length
 *     (idx (_ BitVec 32))                       ; current index
 *     (crc-in (_ BitVec 32))                    ; current crc state
 *   )
 *   (_ BitVec 32)
 *
 *   (ite (bvuge idx len)
 *         ; base case
 *         crc-in
 *
 *         ; recursive case
 *         (crc-recursive-loop
 *           data
 *           len
 *           (bvadd idx #x00000001)
 *           (crc-byte-step crc-in (select data idx)))))
 *
 * \endcode
 *
 * \returns 0 on success and non-zero on failure.
 */
static int crc_recursive_loop_function_create(
    Z3_func_decl* fn, generator_context* ctx)
{
    int retval;

    /* create the recursive loop function symbol. */
    Z3_symbol loop_sym = Z3_mk_string_symbol(ctx->ctx, "crc-recursive-loop");
    if (NULL == loop_sym)
    {
        fprintf(stderr, "error: could not create loop function symbol.\n");
        retval = 1;
        goto done;
    }

    /* create the function domain. */
    Z3_sort loop_domain[4] = { ctx->array, ctx->bv32, ctx->bv32, ctx->bv32 };

    /* create the function declaration. */
    Z3_func_decl loop_decl =
        Z3_mk_rec_func_decl(ctx->ctx, loop_sym, 4, loop_domain, ctx->bv32);
    if (NULL == loop_decl)
    {
        fprintf(stderr, "error: could not create loop function decl.\n");
        retval = 2;
        goto done;
    }

    /* create the data argument binding. */
    Z3_ast arg_data = Z3_mk_bound(ctx->ctx, 3, ctx->array);
    if (NULL == arg_data)
    {
        fprintf(stderr, "error: could not create data array argument.\n");
        retval = 3;
        goto done;
    }

    /* create the len argument binding. */
    Z3_ast arg_len = Z3_mk_bound(ctx->ctx, 2, ctx->bv32);
    if (NULL == arg_len)
    {
        fprintf(stderr, "error: could not create len argument.\n");
        retval = 4;
        goto done;
    }

    /* create the idx argument binding. */
    Z3_ast arg_idx = Z3_mk_bound(ctx->ctx, 1, ctx->bv32);
    if (NULL == arg_idx)
    {
        fprintf(stderr, "error: could not create idx argument.\n");
        retval = 5;
        goto done;
    }

    /* create the crc argument binding. */
    Z3_ast arg_crc = Z3_mk_bound(ctx->ctx, 0, ctx->bv32);
    if (NULL == arg_crc)
    {
        fprintf(stderr, "error: could not create crc argument.\n");
        retval = 5;
        goto done;
    }

    /* Create a 32-bit one value. */
    Z3_ast c1 = mk_bv_from_uint64(ctx, 32, 1);
    if (NULL == c1)
    {
        fprintf(stderr, "error: could not create c1 constant.\n");
        retval = 6;
        goto done;
    }

    /* if (idx >= len) */
    Z3_ast cond = Z3_mk_bvuge(ctx->ctx, arg_idx, arg_len);
    if (NULL == cond)
    {
        fprintf(stderr, "error: could not make condition.\n");
        retval = 7;
        goto done;
    }

    /* then: return crc-in. */
    Z3_ast then_branch = arg_crc;

    /* else: recursive case... */

    /* increment index. */
    Z3_ast next_idx = Z3_mk_bvadd(ctx->ctx, arg_idx, c1);
    if (NULL == next_idx)
    {
        fprintf(stderr, "error: could not create next idx.\n");
        retval = 8;
        goto done;
    }

    /* select the byte at the current index. */
    Z3_ast byte = Z3_mk_select(ctx->ctx, arg_data, arg_idx);
    if (NULL == byte)
    {
        fprintf(stderr, "error: could not select byte from array.\n");
        retval = 9;
        goto done;
    }

    /* compute the new CRC by folding in this byte value. */
    Z3_ast next_crc;
    retval = crc_byte_step_block_create(&next_crc, ctx, arg_crc, byte);
    if (0 != retval)
    {
        goto done;
    }

    /* call this function recursively. */
    Z3_ast else_args[4] = { arg_data, arg_len, next_idx, next_crc };
    Z3_ast else_branch  = Z3_mk_app(ctx->ctx, loop_decl, 4, else_args);
    if (NULL == else_branch)
    {
        fprintf(stderr, "error: could not create recursive call.\n");
        retval = 10;
        goto done;
    }

    /* create the body of this recursive function. */
    Z3_ast loop_body = Z3_mk_ite(ctx->ctx, cond, then_branch, else_branch);
    if (NULL == loop_body)
    {
        fprintf(stderr, "error: could not create if-then-else.\n");
        retval = 11;
        goto done;
    }

    /* add the definition to our recursive function. */
    Z3_ast param_vars[4] = { arg_data, arg_len, arg_idx, arg_crc };
    Z3_add_rec_def(ctx->ctx, loop_decl, 4, param_vars, loop_body);
    /* check for errors... */

    /* success. */
    retval = 0;
    *fn = loop_decl;
    goto done;

done:
    return retval;
}

/**
 * \brief Create a CRC-32 function using the Z3 API.
 *
 * \param ctx           The context for this operation.
 *
 * \code
 *
 * (define-fun crc-of-array
 *         ((data (Array (_ BitVec 32) (_ BitVec 8))) (len (_ BitVec 32)))
 *         (_ BitVec 32)
 *   (let ((intermediate-crc
 *              (crc-recursive-loop data len #x00000000 initial-val)))
 *     (bvxor intermediate-crc final-xor)))
 *
 * \endcode
 *
 * \returns 0 on success and non-zero on failure.
 */
static int crc_function_create(generator_context* ctx)
{
    int retval;

    /* create the function name symbol. */
    Z3_symbol fn_sym = Z3_mk_string_symbol(ctx->ctx, "crc-of-array");
    if (NULL == fn_sym)
    {
        fprintf(stderr, "error: could not create function symbol.\n");
        retval = 1;
        goto done;
    }

    /* create the parameter types (domain) for the function. */
    Z3_sort fn_domain[2] = { ctx->array, ctx->bv32 };

    /* create the function declaration. */
    Z3_func_decl fn_decl =
        Z3_mk_rec_func_decl(ctx->ctx, fn_sym, 2, fn_domain, ctx->bv32);
    if (NULL == fn_decl)
    {
        fprintf(stderr, "error: could not create function declaration.\n");
        retval = 2;
        goto done;
    }

    /* first argument. */
    Z3_ast arg_data = Z3_mk_bound(ctx->ctx, 1, ctx->array);
    if (NULL == arg_data)
    {
        fprintf(stderr, "error: could not create first argument.\n");
        retval = 3;
        goto done;
    }

    /* second argument. */
    Z3_ast arg_len = Z3_mk_bound(ctx->ctx, 0, ctx->bv32);
    if (NULL == arg_len)
    {
        fprintf(stderr, "error: could not create second argument.\n");
        retval = 4;
        goto done;
    }

    /* create zero constant. */
    Z3_ast c0 = mk_bv_from_uint64(ctx, 32, 0);
    if (NULL == c0)
    {
        fprintf(stderr, "error: could not create zero constant.\n");
        retval = 5;
        goto done;
    }

    /* create 0xffffffff value. */
    Z3_ast cff = mk_bv_from_uint64(ctx, 32, 0xFFFFFFFF);
    if (NULL == cff)
    {
        fprintf(stderr, "error: could not create 0xffffffff constant.\n");
        retval = 6;
        goto done;
    }

    /* create the loop function. */
    Z3_func_decl loop_fn = NULL;
    retval = crc_recursive_loop_function_create(&loop_fn, ctx);
    if (NULL == loop_fn)
    {
        goto done;
    }

    /* call the loop function. */
    Z3_ast loop_fn_args[4] = { arg_data, arg_len, c0, cff };
    Z3_ast loop_fn_result = Z3_mk_app(ctx->ctx, loop_fn, 4, loop_fn_args);
    if (NULL == loop_fn_result)
    {
        fprintf(stderr, "error: could not call loop function.\n");
        retval = 7;
        goto done;
    }

    /* create the function body. */
    Z3_ast fn_body = Z3_mk_bvxor(ctx->ctx, loop_fn_result, cff);
    if (NULL == fn_body)
    {
        fprintf(stderr, "error: could not create function body.\n");
        retval = 8;
        goto done;
    }

    /* create the function definition. */
    Z3_ast fn_args[2] = { arg_data, arg_len };
    Z3_add_rec_def(ctx->ctx, fn_decl, 2, fn_args, fn_body);
    /* TODO - check result. */

    /* success. */
    ctx->crc_fn = fn_decl;
    retval = 0;
    goto done;

done:
    return retval;
}

/**
 * \brief Generate the unit test suite for crc32.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_unit_tests(generator_context* ctx, FILE* out)
{
    (void)ctx;
    int retval;

    /* generate the front matter for the unit test file. */
    retval = generate_unit_test_frontmatter(ctx, out);
    if (0 != retval)
    {
        goto done;
    }

    /* generate the Ross N. Williams test. */
    retval = generate_williams_test(ctx, out);
    if (0 != retval)
    {
        goto done;
    }

    /* generate empty file test. */
    retval = generate_empty_test(ctx, out);
    if (0 != retval)
    {
        goto done;
    }

    /* generate a unit test exercising the constants table. */
    retval = generate_constants_exercise_test(ctx, out);
    if (0 != retval)
    {
        goto done;
    }

    /* generate a unit test exercising the whole constants table in a single
     * calculation. */
    retval = generate_full_constants_test(ctx, out);
    if (0 != retval)
    {
        goto done;
    }

    /* success. */
    retval = 0;
    goto done;

done:
    return retval;
}

/**
 * \brief Generate the front matter for the unit test source file.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_unit_test_frontmatter(generator_context* ctx, FILE* out)
{
    (void)ctx;

    fprintf(out, "#include <libfat32/crc.h>\n");
    fprintf(out, "#include <minunit/minunit.h>\n\n");
    fprintf(out, "#include <stdint.h>\n\n");
    fprintf(out, "FAT32_IMPORT_crc;\n\n");
    fprintf(out, "TEST_SUITE(crc32);\n\n");

    return 0;
}

/**
 * \brief Generate the 123456789 test vector from Ross N. Williams' paper.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_williams_test(generator_context* ctx, FILE* out)
{
    (void)ctx;

    fprintf(out, "/**\n"
                 " * The test vector from the Ross N. Williams 1993 paper.\n"
                 " */\n"
                 "TEST(crc32_base_test)\n"
                 "{\n"
                 "    const uint32_t EXPECTED_RESULT = 0xcbf43926;\n"
                 "    const char* input = \"123456789\";\n\n"
                 "    TEST_ASSERT(EXPECTED_RESULT == crc32(input, 9));\n"
                 "}\n\n");

    return 0;
}

/**
 * \brief Generate the empty file test vector.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_empty_test(generator_context* ctx, FILE* out)
{
    int retval;
    uint32_t test_result = 0;

    /* run a canonical crc on an empty test vector. */
    retval = canonical_crc(&test_result, ctx, "", 0);
    if (0 != retval)
    {
        return retval;
    }

    fprintf(out, "/**\n"
                 " * Empty file test vector.\n"
                 " */\n"
                 "TEST(crc32_empty_file)\n"
                 "{\n"
                 "    const uint32_t EXPECTED_RESULT = 0x%08x;\n"
                 "    const char* input = \"\";\n\n"
                 "    TEST_ASSERT(EXPECTED_RESULT == crc32(input, 0));\n"
                 "}\n\n", test_result);

    return 0;
}

/**
 * \brief Generate a unit test exercising the constants table.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_constants_exercise_test(generator_context* ctx, FILE* out)
{
    int retval;
    uint32_t test_results[256];

    /* run a canonical crc on all possible byte input values. */
    for (int i = 0; i < 256; ++i)
    {
        uint8_t input[1] = { i };

        /* run a canonical crc on this input constant. */
        retval = canonical_crc(&test_results[i], ctx, input, 1);
        if (0 != retval)
        {
            return retval;
        }
    }

    /* begin test. */
    fprintf(out, "/**\n"
                 " * Constants test vector.\n"
                 " */\n"
                 "TEST(crc32_constants_test)\n"
                 "{\n"
                 "    const uint32_t EXPECTED_RESULTS[256] = {");

    /* output expected results. */
    for (int i = 0; i < 256; ++i)
    {
        /* add a blank line every six constants. */
        if (i % 6 == 0)
        {
            fprintf(out, "\n        ");
        }

        fprintf(out, "0x%08x, ", test_results[i]);
    }

    /* end test. */
    fprintf(out, "};\n\n"
                 "    for (int i = 0; i < 256; ++i)\n"
                 "    {\n"
                 "        uint8_t input[1] = { (uint8_t)i };\n"
                 "        TEST_ASSERT(EXPECTED_RESULTS[i] == "
                              "crc32(input, 1));\n"
                 "    }\n"
                 "}\n");

    return 0;
}

/**
 * \brief Generate a unit test checking for any propagation issues in the
 * constants table.
 *
 * \param ctx           The generator context for this operation.
 * \param out           The output file for this operation.
 *
 * \returns 0 on success and non-zero on failure.
 */
static int generate_full_constants_test(generator_context* ctx, FILE* out)
{
    int retval;
    uint32_t test_result;
    uint8_t input[256];

    /* generate the input */
    for (int i = 0; i < 256; ++i)
    {
        input[i] = i;
    }

    /* run a canonical crc on this input. */
    retval = canonical_crc(&test_result, ctx, input, 256);
    if (0 != retval)
    {
        return retval;
    }

    /* begin test. */
    fprintf(out, "/**\n"
                 " * Constants test vector.\n"
                 " */\n"
                 "TEST(crc32_full_constant_test_vector)\n"
                 "{\n"
                 "    const uint32_t EXPECTED_RESULT = 0x%08x;\n"
                 "    const uint8_t input[256] = {",
                 test_result);

    /* output input values. */
    for (int i = 0; i < 256; ++i)
    {
        /* add a blank line every 12 constants. */
        if (i % 12 == 0)
        {
            fprintf(out, "\n        ");
        }

        fprintf(out, "0x%02x, ", input[i]);
    }

    /* end test. */
    fprintf(out, "};\n\n"
                 "    TEST_ASSERT(EXPECTED_RESULT == crc32(input, 256));\n"
                 "}\n");

    return 0;
}
