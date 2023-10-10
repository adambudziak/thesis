"""
author: Adam Budziak
"""

import argparse
import ast


def permutations_to_dot(permutations):
    result = 'graph {\n'

    for i in range(len(permutations[0])):
        result += f'\n\t{i};'

    for permutation in permutations:
        for v_from, v_to in enumerate(permutation):
            result += f'\n\t{v_from} -- {v_to};'

    result += '\n}'

    return result



def main_with_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('input_file')
    parser.add_argument('output_file')
    args = parser.parse_args()
    return main(args.input_file, args.output_file)


def main(input_file, output_file):

    with open(input_file) as file:
        permutations = ast.literal_eval(file.readline())

    dot = permutations_to_dot(permutations)
    with open(output_file, 'w') as file:
        file.write(dot)

if __name__ == '__main__':
    main_with_args()
