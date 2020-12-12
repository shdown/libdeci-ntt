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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

static void die_oom(void)
{
    fprintf(stderr, "Out of memory.\n");
    abort();
}

static void *xmalloc(size_t n, size_t m)
{
    if (m && n > SIZE_MAX / m)
        die_oom();
    size_t sz = n * m;
    void *r = malloc(sz);
    if (!r && sz)
        die_oom();
    return r;
}

static inline size_t div_ceil_zu(size_t a, size_t b)
{
    return a / b + !!(a % b);
}

static char *read_number_str(void)
{
    char *buf = NULL;
    size_t nbuf = 0;
    ssize_t r = getline(&buf, &nbuf, stdin);
    if (r < 0) {
        if (ferror(stdin)) {
            perror("getline");
        } else {
            fprintf(stderr, "Got EOF.\n");
        }
        abort();
    }

    if (r && buf[r - 1] == '\n') {
        --r;
        buf[r] = '\0';
    }

    if (!r) {
        fprintf(stderr, "Got empty line.\n");
        abort();
    }
    for (size_t i = 0; i < (size_t) r; ++i) {
        unsigned char c = buf[i];
        if (c < '0' || c > '9') {
            fprintf(stderr, "Got non-numeric data.\n");
            abort();
        }
    }

    return buf;
}

static deci_UWORD parse_word(const char *s, size_t ns)
{
    deci_UWORD r = 0;
    for (size_t i = 0; i < ns; ++i) {
        r *= 10;
        r += s[i] - '0';
    }
    return r;
}

static void parse_str(deci_UWORD *out, const char *s, size_t ns)
{
    size_t i = ns;
    while (i >= DECI_BASE_LOG) {
        i -= DECI_BASE_LOG;
        *out++ = parse_word(s + i, DECI_BASE_LOG);
    }
    if (i) {
        *out++ = parse_word(s, i);
    }
}

static void pretty_print(const deci_UWORD *w, size_t nw)
{
    // normalize
    while (nw && w[nw - 1] == 0)
        --nw;

    if (!nw) {
        printf("0\n");
        return;
    }

    size_t i = nw - 1;
    printf("%" PRIu32, (uint32_t) w[i]);
    while (i) {
        --i;
        printf("%0*" PRIu32, DECI_BASE_LOG, (uint32_t) w[i]);
    }
    printf("\n");
}

static void print_usage_and_exit(void)
{
    fprintf(stderr, "USAGE: driver {mul | sqr}\n");
    exit(2);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        print_usage_and_exit();
    }
    const char *arg = argv[1];
    if (strcmp(arg, "mul") == 0) {

        char *a = read_number_str();
        size_t na = strlen(a);

        char *b = read_number_str();
        size_t nb = strlen(b);

        size_t nwa = div_ceil_zu(na, DECI_BASE_LOG);
        deci_UWORD *wa = xmalloc(nwa, sizeof(deci_UWORD));
        parse_str(wa, a, na);

        size_t nwb = div_ceil_zu(nb, DECI_BASE_LOG);
        deci_UWORD *wb = xmalloc(nwb, sizeof(deci_UWORD));
        parse_str(wb, b, nb);

        size_t ns = decintt_mul_nscratch_bytes(nwa, nwb);
        void *s = xmalloc(ns, 1);

        size_t nwout = nwa + nwb;
        deci_UWORD *wout = xmalloc(nwout, sizeof(deci_UWORD));
        decintt_mul(wa, nwa, wb, nwb, wout, s);

        pretty_print(wout, nwout);

    } else if (strcmp(arg, "sqr") == 0) {

        char *a = read_number_str();
        size_t na = strlen(a);

        size_t nwa = div_ceil_zu(na, DECI_BASE_LOG);
        deci_UWORD *wa = xmalloc(nwa, sizeof(deci_UWORD));
        parse_str(wa, a, na);

        size_t ns = decintt_sqr_nscratch_bytes(nwa);
        void *s = xmalloc(ns, 1);

        size_t nwout = nwa * 2;
        deci_UWORD *wout = xmalloc(nwout, sizeof(deci_UWORD));
        decintt_sqr(wa, nwa, wout, s);

        pretty_print(wout, nwout);

    } else {
        print_usage_and_exit();
    }
}
