import numpy as np
import scipy.sparse as sparse
import networkx as nx
import matplotlib.pyplot as plt
import decimal

import sys


def compute_pow(fname, n, d, steps, directed):
    uniform = np.ones(n) / n

    graph = nx.drawing.nx_agraph.read_dot(fname)

    adj = sparse.csr_matrix(nx.adjacency_matrix(graph), dtype='float64')
    adj /= d
    if directed:
        adj /= 2
    base = adj.copy()
    adj = adj.todense()

    from ipdb import set_trace
    set_trace()

    x, y = list(range(steps)), []

    for i in x:
        print(adj.diagonalize())
        tvd = abs(adj - uniform).sum(axis=1).max() / 2
        y.append(tvd)
        print(f'{i}\t{tvd}')
        print(f'{i}\t{tvd}', file=sys.stderr)
        adj *= base

    return
    plt.plot(x, y)
    plt.show()


def main():
    if len(sys.argv) < 5:
        print(f'usage: {sys.argv[0]} filename n d steps directed[?]')
        return

    fname = sys.argv[1]
    n = int(sys.argv[2])
    d = int(sys.argv[3])
    steps = int(sys.argv[4])
    directed = len(sys.argv) >= 6 and sys.argv[5] == 'directed'
    compute_pow(fname, n, d, steps, directed)


if __name__ == '__main__':
    main()
