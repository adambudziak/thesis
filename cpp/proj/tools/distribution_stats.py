"""
author: Adam Budziak
"""

from typing import Tuple, List
import argparse
import json
import re
import os
import matplotlib.pyplot as plt
from textwrap import dedent
import subprocess
from tempfile import mkdtemp
import shutil
import time


class Configuration:
    def __init__(self, n, d, walk_length, key, iv, input=None, output_file=None):
        self.n = n
        self.d = d
        self.walk_length = walk_length
        self.key = key
        self.iv = iv
        self.input = input
        self.output_file = output_file


CRYPTER_PATH = None
OUTPUT_DIR = '.'

DEFAULT_KEY = "babababababababababababababababa"
DEFAULT_IV = "bebebebebebebebebebebebebebebebe"

TEST_CONFIGURATIONS: List[Configuration] = (
    Configuration(8, 4, 4, DEFAULT_KEY, DEFAULT_IV),
    Configuration(8, 4, 64, DEFAULT_KEY, DEFAULT_IV)
)


def generate_png(data_file: str):
    with open(data_file) as input_file:
        n = int(input_file.readline())
        line = input_file.readline()
        values = json.loads(line)
        xs = list(range(n))

    count = [0] * n
    for num in values:
        count[num] += 1

    # plt.scatter(list(range(len(values))), values, marker='o')
    # plt.subplots()
    plt.bar(xs, count)
    # plt.show()
    plt.savefig(os.path.splitext(data_file)[0])
    plt.clf()


def generate_pdf(crypter_output_file: str):
    tex_figure_format = '''
    \\begin{{figure}}[H]
        \\centering
        \\includegraphics[width=\\textwidth]{{{0}}}
        \\caption{{ {1} }}
    \\end{{figure}}
    '''

    tex_header = '''
    \\documentclass{{article}}
    \\usepackage{{polski}}
    \\usepackage{{float}}
    \\usepackage[margin=1.2in]{{geometry}}
    \\usepackage[utf8]{{inputenc}}
    \\usepackage{{graphicx}}
    \\usepackage{{subcaption}}
    
    \\begin{{document}}
    {0}
    \\end{{document}}
    '''

    def fname_to_caption(fname: str) -> str:
        pattern = r'n([0-9]+)_d([0-9]+)_wl([0-9]+)_([0-9]+)c'
        params = re.split(pattern, os.path.basename(fname))[1:-1]
        return 'n={}, d={}, walk-length={}, msg-length={}'.format(*params)

    content = []

    for conf in TEST_CONFIGURATIONS:
        image_fname = os.path.basename(os.path.splitext(conf.output_file)[0]) + '.png'
        content.append(tex_figure_format.format(image_fname, fname_to_caption(image_fname)))

    texfilename = os.path.join(crypter_output_file, 'stats.tex')

    with open(texfilename, 'w') as texfile:
        texfile.write(dedent(tex_header.format('\n\n'.join(content))))

    subprocess.run(['pdflatex', texfilename], cwd=crypter_output_file, stdout=subprocess.DEVNULL)

    pdf_filename = os.path.join(crypter_output_file, 'stats.pdf')
    shutil.move(pdf_filename, os.path.join(OUTPUT_DIR, 'stats_{}.pdf'.format(time.strftime("%Y%m%d-%H%M%S"))))


def auto_output_file(conf: Configuration) -> str:
    return f'n{conf.n}_d{conf.d}_wl{conf.walk_length}_{len(conf.input)}c.output'


def run_crypter(conf: Configuration):
    subprocess.run([
        CRYPTER_PATH,
        f'--key="{conf.key}"',
        f'--iv="{conf.iv}"',
        f'--n="{conf.n}"',
        f'--d="{conf.d}"',
        f'--walk_length="{conf.walk_length}"',
        f'--message="{conf.input}"',
        f'--output_file="{conf.output_file}"'
    ])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('crypter_path')
    parser.add_argument('input_file')
    parser.add_argument('output_dir')

    args = parser.parse_args()
    global CRYPTER_PATH
    global OUTPUT_DIR
    CRYPTER_PATH = args.crypter_path
    OUTPUT_DIR = args.output_dir

    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    with open(args.input_file) as input_file:
        input_ = input_file.readline().strip()

    tmpdir = mkdtemp()

    for conf in TEST_CONFIGURATIONS:
        conf.input = input_
        conf.output_file = os.path.join(tmpdir, auto_output_file(conf))
        run_crypter(conf)
        generate_png(conf.output_file)

    generate_pdf(tmpdir)
    shutil.rmtree(tmpdir)


if __name__ == '__main__':
    main()

