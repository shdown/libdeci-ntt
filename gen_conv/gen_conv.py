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


def mk_const_uword(n):
    return f'{n}u'


def mk_const_ulimb(n):
    return f'FFT_ULIMB_C({n})'


# From base 10^n to base 10^m.
# Requires m/2 < n < m.
def gen_conv_to(n, m, func_name, func_quals=''):
    print(f'{func_quals} FFT_ULIMB *{func_name}(deci_UWORD *wa, deci_UWORD *wa_end, FFT_ULIMB *out)')
    print('{')
    print(' while (wa != wa_end) {')
    print('  deci_UWORD x = *wa++;')

    i = 0
    while True:
        sz_x = n - i
        sz_y = m - sz_x
        if sz_y > n:
            sz_z = sz_y - n
            sz_y = n
            assert sz_z < n
            print('  {')
            print('   deci_UWORD y = *wa++;')
            print('   deci_UWORD z = *wa++;')
            print(f'   deci_UWORD zq = z / {mk_const_uword(10 ** sz_z)};')
            print(f'   deci_UWORD zr = z % {mk_const_uword(10 ** sz_z)};')
            print(f'   *out++ = x + y * {mk_const_ulimb(10 ** sz_x)} + zr * {mk_const_ulimb(10 ** (sz_x + sz_y))};')
            print('   x = zq;')
            print('  }')
        elif sz_y == n:
            print('  {')
            print('   deci_UWORD y = *wa++;')
            print(f'   *out++ = x + y * {mk_const_ulimb(10 ** sz_x)};')
            # No need to update 'x': the next value of i, that is '(i+m)%n', is going to be zero.
            print('  }')
        else:
            print('  {')
            print('   deci_UWORD y = *wa++;')
            print(f'   deci_UWORD yq = y / {mk_const_uword(10 ** sz_y)};')
            print(f'   deci_UWORD yr = y % {mk_const_uword(10 ** sz_y)};')
            print(f'   *out++ = x + yr * {mk_const_ulimb(10 ** sz_x)};')
            print('   x = yq;')
            print('  }')

        i = (i + m) % n
        if i == 0:
            break

    print(' }')
    print(' return out;')
    print('}')


# From base 10^m to base 10^n.
# Requires m/2 < n < m.
def gen_conv_from(n, m, func_name, func_quals=''):
    print(f'{func_quals} deci_UWORD *{func_name}(FFT_ULIMB *a, FFT_ULIMB *a_end, deci_UWORD *out)')
    print('{')
    print(' while (a != a_end) {')
    print('  FFT_ULIMB x = *a++;')

    i = 0
    while True:
        sz_x = m - i
        if sz_x > n:
            print('  {')
            print(f'   *out++ = x % {mk_const_uword(10 ** n)};')
            print(f'   x /= {mk_const_uword(10 ** n)};')
            print('  }')
        elif sz_x == n:
            print('  {')
            print(f'   *out++ = x;')
            # No need to update 'x': the next value of i, that is '(i+n)%m', is going to be zero.
            print('  }')
        else:
            sz_next_piece = n - sz_x
            print('  {')
            print('   FFT_ULIMB y = *a++;')
            print(f'   FFT_ULIMB yq = y / {mk_const_uword(10 ** sz_next_piece)};')
            print(f'   deci_UWORD yr = y % {mk_const_uword(10 ** sz_next_piece)};')
            print(f'   *out++ = x + yr * {mk_const_uword(10 ** sz_x)};')
            print(f'   x = yq;')
            print('  }')

        i = (i + n) % m
        if i == 0:
            break

    print(' }')
    print(' return out;')
    print('}')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('n', type=int)
    parser.add_argument('m', type=int)
    args = parser.parse_args()
    n, m = args.n, args.m

    print(f'''\
// Auto-generated; do not edit.

// gen_conv params: deci_UWORD => base 10^{n}, FFT_ULIMB => base 10^{m}.

#if DECI_BASE_LOG != {n}
# error "Unsupported DECI_BASE_LOG (expected {n})."
#endif
''')

    gen_conv_to(n=n, m=m, func_name=f'raw_conv_to', func_quals='static')
    gen_conv_from(n=n, m=m, func_name=f'raw_conv_from', func_quals='static')


if __name__ == '__main__':
    main()
