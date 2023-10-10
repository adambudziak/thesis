import numpy as np
import pandas
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline
from scipy.stats import norm
import sys
import re

from math import log, sqrt



def main():
    if len(sys.argv) < 2:
        print(f'usage {sys.argv[0]} n1_d1.dat')
        return

    fname = sys.argv[1]
    m = re.match(r'(\d+)_(\d+).*.dat', fname)
    n, d = m.groups()[0], m.groups()[1]

    data = pandas.read_csv(fname, sep='\t', header=None)
    data.columns = ['x', 'y']
    x, y = data.x, data.y

    cs = CubicSpline(x, y)

    plt.plot(x, y, 'o', label='data')
    xs = np.arange(0, len(x), 0.01)
    plt.plot(xs, cs(xs), label='d')
    plt.plot(xs, cs(xs, 1), label="d'")
    plt.plot(xs, cs(xs, 2), label="d''")
    plt.plot(xs, cs(xs, 3), label="d'''")

    plt.legend(loc='upper right', ncol=2)
    
    plt.show()


if __name__ == '__main__':
    main()
