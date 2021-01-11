/*
 * Copyright (C) 2020  libdeci-ntt developers
 *
 * This file is part of libdeci-ntt.
 *
 * libdeci-ntt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdeci-ntt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libdeci-ntt.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "decintt.h"

#include "decintt_private.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#if DECI_WE_ARE_64_BIT
#   include "decintt_conv_64.inc"
#else
#   include "decintt_conv_32.inc"
#endif

static inline size_t div_ceil_zu(size_t a, size_t b)
{
    return a / b + !!(a % b);
}

static inline size_t calc_fft_size(size_t n)
{
    if (n > SIZE_MAX / 2)
        return -1;

    size_t s = 4;
    while (s < n) {
        // s < n <= SIZE_MAX/2, so it's safe to double 's' here.
        s *= 2;
    }

    size_t s_fourstep = s / 4 * 3;
    return (n <= s_fourstep) ? s_fourstep : s;
}

// For s = 3*2^k, returns true.
// For s = 2^k, returns false.
// For any other value of s, including ((size_t) -1), the result is undefined.
static inline bool is_fourstep_size(size_t s)
{
    return s & (s >> 1);
}

size_t decintt_mul_nscratch_bytes(size_t nwa, size_t nwb)
{
    if (nwa > SIZE_MAX / DECI_BASE_LOG)
        return -1;
    size_t ndigits_a = nwa * DECI_BASE_LOG;

    if (nwb > SIZE_MAX / DECI_BASE_LOG)
        return -1;
    size_t ndigits_b = nwb * DECI_BASE_LOG;

    if (ndigits_a > DECINTT_MAX_DIGITS)
        return -1;

    if (ndigits_b > DECINTT_MAX_DIGITS)
        return -1;

    size_t nlimbs_a = div_ceil_zu(ndigits_a, FFT_BASE);
    size_t nlimbs_b = div_ceil_zu(ndigits_b, FFT_BASE);

    // We have
    //     nlimbs_a  <=  SIZE_MAX/FFT_BASE+1  <  SIZE_MAX/2.
    // Same for 'nlimbs_b'. So it's safe to add them together.
    size_t nlimbs_r = nlimbs_a + nlimbs_b;
    size_t s = calc_fft_size(nlimbs_r);

    if (s > (SIZE_MAX / sizeof(FFT_ULIMB) - 4) / 5)
        return -1;
    return (s * 5 + 4) * sizeof(FFT_ULIMB);
}

size_t decintt_sqr_nscratch_bytes(size_t nwa)
{
    if (nwa > SIZE_MAX / DECI_BASE_LOG)
        return -1;
    size_t ndigits_a = nwa * DECI_BASE_LOG;

    if (ndigits_a > DECINTT_MAX_DIGITS)
        return -1;

    size_t nlimbs_a = div_ceil_zu(ndigits_a, FFT_BASE);

    // We have
    //     nlimbs_a  <=  SIZE_MAX/FFT_BASE+1  <  SIZE_MAX/2.
    // Do it's safe to double 'a' here.
    size_t nlimbs_r = nlimbs_a * 2;
    size_t s = calc_fft_size(nlimbs_r);

    if (s > (SIZE_MAX / sizeof(FFT_ULIMB) - 2) / 3)
        return -1;
    return (s * 3 + 2) * sizeof(FFT_ULIMB);
}

// Assumes nout >= ceil(nwa * DECI_BASE_LOG / FFT_BASE).
static void conv_to(deci_UWORD *wa, size_t nwa, FFT_ULIMB *out, size_t nout)
{
    FFT_ULIMB *out_end = out + nout;

    size_t nleftover = nwa % FFT_BASE;
    deci_UWORD *leftover = wa + nwa - nleftover;

    out = raw_conv_to(wa, leftover, out);

    deci_UWORD buf_in  [FFT_BASE];
    FFT_ULIMB  buf_out [DECI_BASE_LOG];

    for (size_t i = 0; i < nleftover; ++i)
        buf_in[i] = leftover[i];
    for (size_t i = nleftover; i < FFT_BASE; ++i)
        buf_in[i] = 0;

    (void) raw_conv_to(buf_in, buf_in + FFT_BASE, buf_out);

    size_t ncopy = out_end - out;
    if (ncopy > DECI_BASE_LOG)
        ncopy = DECI_BASE_LOG;

    for (size_t i = 0; i < ncopy; ++i)
        out[i] = buf_out[i];

    for (out += ncopy; out != out_end; ++out)
        *out = 0;
}

static void conv_from(FFT_ULIMB *a, size_t na, deci_UWORD *out, size_t nout)
{
    deci_UWORD *out_end = out + nout;

    while (na && a[na - 1] == 0)
        --na;

    size_t nleftover = na % DECI_BASE_LOG;
    if (!nleftover && na)
        nleftover = DECI_BASE_LOG;

    FFT_ULIMB *leftover = a + na - nleftover;

    out = raw_conv_from(a, leftover, out);

    FFT_ULIMB  buf_in  [DECI_BASE_LOG];
    deci_UWORD buf_out [FFT_BASE];

    for (size_t i = 0; i < nleftover; ++i)
        buf_in[i] = leftover[i];
    for (size_t i = nleftover; i < DECI_BASE_LOG; ++i)
        buf_in[i] = 0;

    (void) raw_conv_from(buf_in, buf_in + DECI_BASE_LOG, buf_out);

    size_t ncopy = out_end - out;
    if (ncopy > FFT_BASE)
        ncopy = FFT_BASE;

    for (size_t i = 0; i < ncopy; ++i)
        out[i] = buf_out[i];

    for (out += ncopy; out != out_end; ++out)
        *out = 0;
}

void decintt_mul(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *wb, size_t nwb,
        deci_UWORD *out,
        void *scratch_v)
{
    size_t nlimbs_a = div_ceil_zu(nwa * DECI_BASE_LOG, FFT_BASE);
    size_t nlimbs_b = div_ceil_zu(nwb * DECI_BASE_LOG, FFT_BASE);
    size_t nlimbs_r = nlimbs_a + nlimbs_b;

    size_t s = calc_fft_size(nlimbs_r);

    FFT_ULIMB *scratch_0 = scratch_v;
    FFT_ULIMB *scratch_1 = scratch_0 + s + 1;
    FFT_ULIMB *scratch_2 = scratch_1 + s + 1;
    FFT_ULIMB *scratch_3 = scratch_2 + s + 1;
    FFT_ULIMB *scratch_4 = scratch_3 + s + 1;

    conv_to(wa, nwa, scratch_0, s);
    memcpy(scratch_2, scratch_0, sizeof(FFT_ULIMB) * s);

    conv_to(wb, nwb, scratch_1, s);
    memcpy(scratch_3, scratch_1, sizeof(FFT_ULIMB) * s);

    if (is_fourstep_size(s)) {
        decintt_fft_fourstep(
            scratch_0, scratch_1, scratch_2, scratch_3, scratch_4, s);
    } else {
        decintt_fft(
            scratch_0, scratch_1, scratch_2, scratch_3, scratch_4, s);
    }

    decintt_fft_recover_answer(scratch_0, nlimbs_r, scratch_0, scratch_2);

    conv_from(scratch_0, nlimbs_r, out, nwa + nwb);
}

void decintt_sqr(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *out,
        void *scratch_v)
{
    size_t nlimbs_a = div_ceil_zu(nwa * DECI_BASE_LOG, FFT_BASE);
    size_t nlimbs_r = nlimbs_a * 2;
    size_t s = calc_fft_size(nlimbs_r);

    FFT_ULIMB *scratch_0 = scratch_v;
    FFT_ULIMB *scratch_1 = scratch_0 + s + 1;
    FFT_ULIMB *scratch_2 = scratch_1 + s + 1;

    conv_to(wa, nwa, scratch_0, s);
    memcpy(scratch_1, scratch_0, sizeof(FFT_ULIMB) * s);

    if (is_fourstep_size(s)) {
        decintt_fft_fourstep(
            scratch_0, scratch_0, scratch_1, scratch_1, scratch_2, s);
    } else {
        decintt_fft(
            scratch_0, scratch_0, scratch_1, scratch_1, scratch_2, s);
    }

    decintt_fft_recover_answer(scratch_0, nlimbs_r, scratch_0, scratch_1);

    conv_from(scratch_0, nlimbs_r, out, nwa * 2);
}
