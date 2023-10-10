import numpy as np
import pandas
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline
from scipy.stats import norm
from math import log, sqrt
import sys
import re
import decimal
from decimal import Decimal

def find_tmix(s, xs, ys):
    for x, y in zip(xs, ys):
        if y <= s:
            return x


def get_tmix_fn(xs, ys):
    def inner(s):
        return find_tmix(s, xs, ys)
    return inner


def get_alpha_fn(tmix_fn, n, d):
    L = d / (d - 2) * n.log10() / (d-1).log10()
    R = (n.log10() / (d-1).log10()).sqrt()
    lambda_ = 2 * (d*(d-1)).sqrt() / (d - 2) ** Decimal(1.5)
    def inner(s, ppf):
        nonlocal L
        nonlocal R
        nonlocal lambda_
        R_ = R * ppf
        return L / R_ - tmix_fn(s) / R_ - lambda_
    return inner


def main():
    if len(sys.argv) < 4:
        print(f'usage {sys.argv[0]} n1_d1.dat point ppf')
        return

    decimal.getcontext().prec = 256

    fname = sys.argv[1]
    point = Decimal(sys.argv[2])
    ppf = Decimal(sys.argv[3])
    
    m = re.match(r'(\d+)_(\d+).*.dat', fname)
    n, d = Decimal(m.groups()[0]), Decimal(m.groups()[1])
    try:
        data = pandas.read_csv(fname, sep='\t', header=None, dtype=str)
    except Exception as e:
        print(f'Cannot read {fname}, reason: ', e)
        return
    data.columns = ['x', 'y']
    xs, ys = data.x, data.y
    xs = list(map(int, xs))
    ys = list(map(Decimal, ys))
    alpha = get_alpha_fn(get_tmix_fn(xs, ys), n, d)(point, ppf)
    print(alpha)

    

if __name__ == '__main__':
    main()
