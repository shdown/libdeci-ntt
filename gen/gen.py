#!/usr/bin/env python3
#
# Copyright (C) 2020  libdeci-ntt developers
#
# This file is part of libdeci-ntt.
#
# libdeci-ntt is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libdeci-ntt is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with libdeci-ntt.  If not, see <https://www.gnu.org/licenses/>.
#

import argparse

from isprime import is_prime
from utils import egcd, canonical_mod, prim_root, mod_pow, modp_inv, decompose_pow2
import fastdiv


class Platform:
    def __init__(self, bitwidth, k_min, base_log):
        self.bitwidth = bitwidth
        self.k_min = k_min
        self.base_log = base_log


BASE = 10


PLATFORM_32 = Platform(
    bitwidth=32,
    k_min=25,
    base_log=5)


PLATFORM_64 = Platform(
    bitwidth=64,
    k_min=35,
    base_log=14)


def generate_primes_aux(k_min, k_max, p_max, npick):
    assert npick > 0
    for k in range(k_min, k_max + 1):
        c_start = p_max >> k
        if c_start % 2 == 0:
            c_start -= 1
        while c_start % 3 != 0:
            c_start -= 2
        nleft = npick
        for c in range(c_start, -3, -6):
            p = (c << k) + 1
            if is_prime(p):
                yield p
                nleft -= 1
                if not nleft:
                    break


def find_primes_for_platform(platform, nprimes):
    result = list(generate_primes_aux(
        k_min=platform.k_min,
        k_max=platform.bitwidth - 2,
        p_max=1 << (platform.bitwidth - 1),
        npick=nprimes))
    if len(result) < nprimes:
        raise ValueError(f'cannot find {nprimes} primes')
    result.sort()
    result = result[-nprimes:]
    return result


def extract_k_from_prime(p):
    k, _ = decompose_pow2(p - 1)
    return k


def print_consts_for_platform(platform, prime1, prime2):
    print('// Auto-generated; do not edit.')

    MONT_R = 1 << platform.bitwidth

    for number, prime in [(1, prime1), (2, prime2)]:
        neg_mont_p1, _ = egcd(prime, MONT_R)
        mont_p1 = canonical_mod(-neg_mont_p1, MONT_R)
        x = prim_root(prime)

        k = extract_k_from_prime(prime)

        print(f'static const FFT_ULIMB FFT_ARRAY_{number}[] =')
        print('{')
        for i in range(k):
            value = (1 << i) % prime
            print(f' {modp_inv(value, prime)},')
        print('};')

        print(f'static const Field FFT_field_{number} =')
        print('{')

        print(f' /*p=*/ {prime},')
        print(f' /*k=*/ {k},')
        print(f' /*mont_p1=*/ {mont_p1},')
        print(f' /*mont_rmodp=*/ {MONT_R % prime},')
        print(f' /*mont_r2modp=*/ {MONT_R ** 2 % prime},')

        def to_mont(a):
            return a * MONT_R % prime

        def print_root_data(name, order):
            root = mod_pow(x, (prime - 1) // order, prime)
            inv_root = modp_inv(root, prime)
            print(f' /*{name}_M=*/ {to_mont(root)},')
            print(f' /*inv_{name}_M=*/ {to_mont(inv_root)},')

        print_root_data('root',  order=1 << k)
        print_root_data('root3', order=3)
        print_root_data('rootA', order=3 << k)

        print(f' /*inv_3=*/ {modp_inv(3, prime)},')
        print(f' /*inv_2=*/ FFT_ARRAY_{number},')
        print('};')

    crt_r12 = modp_inv(prime1, prime2) * MONT_R % prime2
    print(f'static const FFT_ULIMB FFT_CRT_R12_FACTOR = {crt_r12};')


def print_thresholds_for_platform(platform, prime1, prime2):
    print('// Auto-generated; do not edit.')

    product = prime1 * prime2
    min_k = min(extract_k_from_prime(p) for p in [prime1, prime2])
    max_transform_len = 3 << min_k

    def make_const(n):
        if n <= 0xFFFFFFFF:
            return f'UINT32_C({n})'
        return f'UINT64_C({n})'

    radix = BASE ** platform.base_log
    max_n = (product - 1) // ((radix - 1) ** 2)
    max_n = min(max_n, max_transform_len // 2)
    ndigits = max_n * platform.base_log

    print('#define NDIGITS(X_) ((X_) < SIZE_MAX ? (X_) : SIZE_MAX)')
    print(f'const size_t DECINTT_MAX_DIGITS = NDIGITS({make_const(ndigits)});')
    print('#undef NDIGITS')


def print_fastdiv_for_platform(platform, prime1, prime2):
    print('// Auto-generated; do not edit.')

    def issue_srl(expr, nbits):
        if nbits:
            return f'({expr}) >> {nbits}'
        else:
            return expr

    def issue_const(x):
        return f'UINT{platform.bitwidth}_C({x})'

    def get_lo_hi(x):
        lo = x & ((1 << platform.bitwidth) - 1)
        hi = x >> platform.bitwidth
        return lo, hi

    radix = BASE ** platform.base_log
    c = fastdiv.fastdiv(d=radix, N=platform.bitwidth * 2)

    print(f'''
void decintt_fft_recover_answer(
        FFT_ULIMB *out, size_t nout, FFT_ULIMB *a1, FFT_ULIMB *a2)''')
    print('{')
    print(' FFT_DOUBLE_ULIMB carry = 0;')
    print(' for (size_t i = 0; i < nout; ++i) {')
    print('  carry += crt2(a1[i], a2[i]);')

    if isinstance(c, fastdiv.CaseEasy):
        f0, f1 = get_lo_hi(c.factor)
        q = issue_srl('carry', c.sh_pre)
        q = f'big_mulh({q}, {issue_const(f0)}, {issue_const(f1)})'
        q = issue_srl(q, c.sh_post)
        print(f'  FFT_DOUBLE_ULIMB q = {q};')
        print(f'  out[i] = carry - q * {issue_const(radix)};')
        print(f'  carry = q;')

    elif isinstance(c, fastdiv.CaseHard):
        f0, f1 = get_lo_hi(c.factor)
        print(f'  FFT_DOUBLE_ULIMB t1 = big_mulh(carry, {issue_const(f0)}, {issue_const(f1)});')
        q = 't1 + ((carry - t1) >> 1)'
        q = issue_srl(q, c.sh)
        print(f'  FFT_DOUBLE_ULIMB q = {q};')
        print(f'  out[i] = carry - q * {issue_const(radix)};')
        print(f'  carry = q;')

    elif isinstance(c, fastdiv.CasePowerOfTwo):
        mask = (1 << c.ell) - 1
        print(f'  out[i] = carry & {issue_const(mask)};')
        print(f'  carry >>= {c.ell};')

    else:
        raise NotImplementedError(f'not implemented for {c}')

    print(' }')
    print('}')


def main():
    platforms = {
        '32': PLATFORM_32,
        '64': PLATFORM_64,
    }
    actions = {
        'print_fastdiv': print_fastdiv_for_platform,
        'print_consts': print_consts_for_platform,
        'print_thresholds': print_thresholds_for_platform,
    }

    parser = argparse.ArgumentParser()

    parser.add_argument(
        'platform',
        help='platform word size',
        choices=platforms)

    parser.add_argument(
        'action',
        help='action to perform',
        choices=actions)

    args = parser.parse_args()

    platform = platforms[args.platform]
    action = actions[args.action]

    p1, p2 = find_primes_for_platform(platform, nprimes=2)
    action(platform, p1, p2)


if __name__ == '__main__':
    main()
