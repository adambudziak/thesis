"""
author: Adam Budziak
"""

from math import log
import sys


def _compute_T(NS, NT):
    max_lhs = 40 * log(2 * NS ** 2)
    max_rhs = (10 * log(NS/9)) / log(1 + (7/144)*((7/9) * NS ** 2 - NS) / NT ** 2)
    offset = (144 * NT * log(2 * NS ** 2)) / NS

    return max(max_lhs, max_rhs) + offset


def _compute_dTV(NS, T, r):
    return NS ** (1 - 2 * r / T)


def _compute_r(NS, T, threshold):
    return T / 2 * (1 - log(threshold)/log(NS))


def main():
    if len(sys.argv) < 4:
        print(f'{sys.argv[0]} usage: |NS| |NT| threshold')
        sys.exit(1)

    NS = int(sys.argv[1])
    NT = int(sys.argv[2])
    threshold = float(sys.argv[3])

    T = _compute_T(NS, NT)

    r = int(_compute_r(NS, T, threshold))
    dtv = _compute_dTV(NS, T, r)

    if dtv > threshold:
        r += 1
        dtv = _compute_dTV(NS, T, r)

    print(f'C = {NS/NT}, r={r}, dtv={dtv}')


if __name__ == '__main__':
    main()
