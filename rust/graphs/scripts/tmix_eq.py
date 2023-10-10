import numpy as np
import pandas
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline
from scipy.stats import norm
import sys
import re

from math import log, sqrt


def from_eq(n, d, alpha):
    L = d / (d - 2) * log(n, d-1)
    R = sqrt(log(n, d-1))
    lambda_ = sqrt(d*(d-1)) / (d - 2) ** 1.5
    def inner(s):
        nonlocal L
        nonlocal R
        nonlocal lambda_
        R_ = R * norm.ppf(s)
        return L - (lambda_ + alpha) * R_
    return inner


def main():
    if len(sys.argv) < 4:
        print(f'usage {sys.argv[0]} n d fname')
        return

    n, d = int(sys.argv[1]), int(sys.argv[2])
    fname = sys.argv[3]

    tmix_fn = from_eq(n, d, 10)
    xs = np.arange(0, 1, 0.0001)
    ys = [tmix_fn(x) for x in xs]

    data = pandas.read_csv(fname, sep='\t', header=None)
    data.columns = ['x', 'y']
    x, y = data.x, data.y


    plt.plot(ys, xs, label='tmix(s)')
    plt.plot(x, y, label=fname)
    plt.show()


if __name__ == '__main__':
    main()

