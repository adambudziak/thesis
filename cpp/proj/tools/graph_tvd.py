"""
author: Adam Budziak
"""

import numpy as np
import scipy.sparse as sparse
import networkx as nx
from mpmath import mp

from math import log

HIGH_PRECISION = True
DIRECTED = True


N = 100
D = 16


graph = nx.drawing.nx_agraph.read_dot(f'gd{N}-{D}.gv')

adj = sparse.csr_matrix(nx.adjacency_matrix(graph), dtype='longdouble')
adj /= (D / 2) if DIRECTED else D
adj = adj.todense()

uniform = np.ones(N, dtype='longdouble') / N

if HIGH_PRECISION:
    uniform = np.array([[mp.mpf(1)/N] for _ in range(N)])
    mp.dps = 120
    adjmp = mp.matrix(adj)
    adj = np.matrix(adjmp.tolist(), dtype=object)
    del adjmp

np.show_config()
from ipdb import set_trace
set_trace()


for i in range(9):
    print(f'Number of steps: {2 ** i}')
    tvd = abs(adj - uniform).sum(axis=1).max() / 2
    print(f'log 1/TVD: {log(1/tvd, 2)}')
    adj = np.linalg.matrix_power(adj, 2)
