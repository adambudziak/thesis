"""
author: Adam Budziak
"""

import os
import ast

from tempfile import mkstemp
import subprocess
import argparse
import networkx as nx
from networkx.drawing.nx_pydot import write_dot
import pylab


def draw_dfa(conf):
    G = nx.DiGraph()
    print(conf)

    G.add_edges_from(key for key in conf)
    edge_labels = conf

    pos=nx.spring_layout(G)
    nx.draw_networkx_edge_labels(G,pos,edge_labels=edge_labels)
    write_dot(G, 'graph.dot')
    nx.draw(G, pos)
    pylab.show()


def dfa_to_digraph(dfa):
    edges = dfa[1]
    result = {}
    for node, targets in edges.items():
        for char, t_node in targets.items():
            result.setdefault((node, t_node), []).append(char)

    return {key: ', '.join(val) for key, val in result.items()}


def digraph_to_dot(dfa, conf):

    print(dfa[0])
    states = [i for i in range(len(dfa[0]))]
    result = [str(states[0]) + " [style=filled, fillcolor=red" + (", shape=doublecircle];" if 0 in dfa[2] else "];")]
    result += [str(state) + (" [shape=doublecircle];" if state in dfa[2] else "[shape=circle];") for state in states]
    result += [(str(pair[0]) + " -> " + str(pair[1])) + f' [label="{label}" ];' for pair, label in conf.items()]
    return 'digraph { \n\trankdir=LR;\n' + '\n'.join(result) + '\n}'


def main_with_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('exec_path')
    parser.add_argument('regexp')
    args = parser.parse_args()
    return main(args.exec_path, args.regexp)


def main(exec_path, regexp):
    dfa_file = mkstemp()[1]

    subprocess.run([
        exec_path,
        f'--regexp={regexp}',
        f'--output_file={dfa_file}'
    ])

    dot_file = 'graph.dot'

    with open(dfa_file) as file:
        states = ast.literal_eval(file.readline())
        edges = ast.literal_eval(file.readline())
        accepting = ast.literal_eval(file.readline())
        dfa = (states[:-1], edges, accepting)

    dot = digraph_to_dot(dfa, dfa_to_digraph(dfa))
    with open(dot_file, 'w') as file:
        file.write(dot)

    subprocess.run(['dot', '-Tx11', 'graph.dot'])

    os.remove(dfa_file)


if __name__ == "__main__":
    #  main('../../build/proj/clients/DfaFromRegex', 'a[b-c]d')
    main_with_args()

