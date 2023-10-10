import numpy as np
import matplotlib.pyplot as plt
import pandas

from scipy.interpolate import CubicSpline

import sys
import re

def main():
    if len(sys.argv) < 2:
        print(f'usage: {sys.argv[0]} [i] n1_d1.dat [n2_d2.dat ...]')
        return

    interpolate = False

    fnames = sys.argv[1:]
    if fnames[0] == 'i':
        interpolate = True
        fnames = fnames[1:]

    for fname in fnames:
        m = re.match(r'(\d+)_(\d+).*.dat', fname)
        n, d = m.groups()[0], m.groups()[1]
        data = pandas.read_csv(fname, sep='\t', header=None)
        data.columns = ['x', 'y']
        x, y = data.x, data.y
        if interpolate:
            cs = CubicSpline(x, y)
            xs = np.arange(0, len(x), 0.01)
            plt.plot(xs, cs(xs), label=f'alpha(s, {n}, {d})')
        else:
            plt.plot(x, y, label=f'alpha(s, {n}, {d})')

    plt.legend(loc='upper right', ncol=2)
    plt.show()


if __name__ == '__main__':
    main()




