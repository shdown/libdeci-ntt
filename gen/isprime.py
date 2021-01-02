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

from utils import decompose_pow2, mod_pow


# See: https://arxiv.org/pdf/1509.00864


_BASES = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37]


_LIMIT = 1 << 64


def _miller_rabin_round(n, a):
    r, d = decompose_pow2(n - 1)
    x = mod_pow(a, d, n)
    if x == 1 or x == n - 1:
        return True
    for _ in range(r - 1):
        x = x * x % n
        if x == n - 1:
            return True
    return False


def is_prime(n):
    if n >= _LIMIT:
        raise ValueError('n is too large')

    if n <= _BASES[-1]:
        return n in _BASES

    if (n & 1) == 0:
        return False

    for base in _BASES:
        if not _miller_rabin_round(n, base):
            return False
    return True
