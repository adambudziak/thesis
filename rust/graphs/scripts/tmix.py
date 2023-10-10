from scipy.stats import norm
from math import log, sqrt
import sys
from decimal import Decimal, getcontext

def tmix(s, n, d, c, ppf):
    lambda_ = 2 * (d*(d-1)).sqrt() / (d - 2) ** Decimal(1.5)
    lhs = d / (d - 2) * n.log10() / (d-1).log10()
    rhs = (lambda_ + c) * ppf * (n.log10() / (d-1).log10()).sqrt()
    # lambda_ = sqrt(d*(d-1))/(d-2) ** 1.5
    # lhs = d / (d - 2) * log(n, d-1)
    # rhs = (lambda_ + c) * ppf * log(n, d-1)
    return lhs - rhs


def main():
    if len(sys.argv) < 6:
        print(f'usage {sys.argv} s n d c ppf')
        return

    getcontext().prec = 272

    s, n, d, c, ppf = (Decimal(a) for a in sys.argv[1:6])
    print(tmix(s, n, d, c, ppf))


if __name__ == '__main__':
    main()
