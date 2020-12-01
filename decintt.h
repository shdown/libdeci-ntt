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

#include <stddef.h>
#include "deci.h"

// Returns ((size_t) -1) if spans of given sizes can not be multiplied using this library.
size_t decintt_mul_nscratch_bytes(size_t nwa, size_t nwb);

// Multiplies (wa ... wa+nwa) by (wb ... wb+nwb), writing result into (out ... out+nwa+nwb).
//
// 'scratch' must:
//   * be aligned on 'sizeof(deci_DOUBLE_UWORD)';
//   * have capacity of 'decintt_mul_nscratch_bytes(nwa, nwb)'.
void decintt_mul(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *wb, size_t nwb,
        deci_UWORD *out,
        void *scratch);

// Returns ((size_t) -1) if span of given size can not be squared using this library.
size_t decintt_sqr_nscratch_bytes(size_t nwa);

// Multiplies (wa ... wa+nwa) by itself, writing result into (out ... out+2*nwa).
//
// 'scratch' must:
//   * be aligned on 'sizeof(deci_DOUBLE_UWORD)';
//   * have capacity of 'decintt_sqr_nscratch_bytes(nwa)'.
void decintt_sqr(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *out,
        void *scratch);
