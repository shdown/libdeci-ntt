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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "deci.h"

#if DECI_WE_ARE_64_BIT
#   define FFT_LIMB_BITS    64
#   define FFT_BASE         14
#   define FFT_ULIMB_C(X)   UINT64_C(X)
typedef uint64_t            FFT_ULIMB;
typedef int64_t             FFT_SLIMB;
typedef unsigned __int128   FFT_DOUBLE_ULIMB;
typedef __int128            FFT_DOUBLE_SLIMB;
#else
#   define FFT_LIMB_BITS    32
#   define FFT_BASE         5
#   define FFT_ULIMB_C(X)   UINT32_C(X)
typedef uint32_t            FFT_ULIMB;
typedef int32_t             FFT_SLIMB;
typedef uint64_t            FFT_DOUBLE_ULIMB;
typedef int64_t             FFT_DOUBLE_SLIMB;
#endif

extern const size_t DECINTT_MAX_DIGITS;

void decintt_fft(
        FFT_ULIMB *a1, FFT_ULIMB *b1,
        FFT_ULIMB *a2, FFT_ULIMB *b2,
        FFT_ULIMB *scratch,
        size_t n);

void decintt_fft_fourstep(
        FFT_ULIMB *a1, FFT_ULIMB *b1,
        FFT_ULIMB *a2, FFT_ULIMB *b2,
        FFT_ULIMB *scratch,
        size_t n);

void decintt_fft_recover_answer(
        FFT_ULIMB *out, size_t nout,
        FFT_ULIMB *a1, FFT_ULIMB *a2);
