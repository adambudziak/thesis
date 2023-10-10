import numpy as np
import pandas
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline
from scipy.stats import norm
from math import log, sqrt
import sys
import re

def find_tmix(s, xs, cs):
    for x in xs:
        if cs(x) < s:
            return x


def get_tmix_fn(xs, cs):
    def inner(s):
        return find_tmix(s, xs, cs)
    return inner


def get_alpha_fn(tmix_fn, n, d):
    L = d / (d - 2) * log(n, d-1)
    R = sqrt(log(n, d-1))
    lambda_ = sqrt(d*(d-1)) / (d - 2) ** 1.5
    def inner(s):
        nonlocal L
        nonlocal R
        nonlocal lambda_
        R_ = R * norm.ppf(s)
        return L / R_ - tmix_fn(s) / R_ - lambda_
    return inner


def draw_unknown_graph(fname):
    try:
        data = pandas.read_csv(fname, sep='\t', header=None)
    except Exception as e:
        print(f'Cannot read {fname}')
        return
    data.columns = ['x', 'y']
    x, y = data.x, data.y

    cs = CubicSpline(x, y)
    xs = np.arange(0, len(x), 0.01)

    ss = np.logspace(0.1, 0.9, num=1000, base=2) - 1
    alphas = [cs(s) for s in ss]
    plt.plot(ss, alphas, label=fname)
    print(fname, 'done')


def draw_n_d_graph(fname, n, d):
    try:
        data = pandas.read_csv(fname, sep='\t', header=None)
    except Exception as e:
        print(f'Cannot read {fname}')
        return
    data.columns = ['x', 'y']
    x, y = data.x, data.y

    cs = CubicSpline(x, y)
    xs = np.arange(0, len(x), 0.01)

    tmix_fn = get_tmix_fn(xs, cs)
    alpha_fn = get_alpha_fn(get_tmix_fn(xs, cs), n, d)
    ss = np.logspace(0.1, 0.9, num=1000, base=2) - 1
    alphas = [alpha_fn(s) for s in ss]
    plt.plot(ss, alphas, label=f'alpha(s, {n}, {d})')
    print(n, d, 'done')


def main():
    if len(sys.argv) < 2:
        print(f'usage {sys.argv[0]} n1_d1.dat [n2_d2.dat ...]')
        return
    
    fnames = sys.argv[1:]
    for fname in fnames:
        m = re.match(r'(\d+)_(\d+).*.dat', fname)
        try:
            n, d = int(m.groups()[0]), int(m.groups()[1])
        except Exception as e:
            draw_unknown_graph(fname)
        else:
            draw_n_d_graph(fname, n, d)

    plt.legend(loc='upper right', ncol=2)
    plt.axhline(0, color='black')
    plt.axvline(0, color='black')
    plt.show()
    


if __name__ == '__main__':
    main()
