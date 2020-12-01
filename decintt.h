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

size_t decintt_mul_nscratch_bytes(size_t nwa, size_t nwb);

void decintt_mul(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *wb, size_t nwb,
        deci_UWORD *out,
        void *scratch);

size_t decintt_sqr_nscratch_bytes(size_t nwa);

void decintt_sqr(
        deci_UWORD *wa, size_t nwa,
        deci_UWORD *out,
        void *scratch);
