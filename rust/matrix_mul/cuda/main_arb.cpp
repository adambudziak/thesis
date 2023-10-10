#include <iostream>
#include <cmath>

#include <arb_mat.h>

#include "matrix.h"
#include "dot.h"
// double value = arf_get_d(arb_midref(arb_mat_entry(&mat, i, j)), ARF_RND_NEAR);

void print_arb_mat(const arb_mat_struct& mat) {
    for (int i = 0; i < std::min(10l, arb_mat_nrows(&mat)); ++i) {
        for (int j = 0; j < std::min(10l, arb_mat_ncols(&mat)); ++j) {
            char *str_ = arb_get_str(
                arb_mat_entry(&mat, i, j), 10, 0
            );
            std::cout << str_ << ' ';
        }
        std::cout << '\n';
    }
}

std::size_t get_degree(const arb_mat_struct& m) {
    std::size_t deg = 0;
    for (int i = 0; i < arb_mat_ncols(&m); ++i) {
        if (arb_is_nonzero(&m.entries[i])) ++deg;
    }
    return deg;
}

int main() {
    arb_mat_struct mat = adj_arb_matrix_from_dot("../../10000_16.dot");
    arb_mat_struct swap_mat;
    arb_mat_init(&swap_mat, arb_mat_nrows(&mat), arb_mat_ncols(&mat));
    arb_mat_set(&swap_mat, &mat);

    arb_struct degree;
    arb_init(&degree);
    arb_set_d(&degree, get_degree(mat));

    arb_mat_scalar_div_arb(&swap_mat, &mat, &degree, 128);
    arb_mat_swap(&swap_mat, &mat);

    std::cout << arb_mat_ncols(&mat) << '\n';
    std::cout << arb_mat_nrows(&mat) << '\n';

    print_arb_mat(mat);
    std::cout << '\n';
    print_arb_mat(swap_mat);
    std::cout << '\n';

    flint_set_num_threads(6);

    std::cout << "size of arb_struct: " << sizeof(arb_struct) << '\n';
    std::cout << "Threads count: " << flint_get_num_threads() << '\n';
    arb_mat_mul_threaded(&swap_mat, &mat, &mat, 32);
    // arb_mat_sqr(&swap_mat, &mat, 32);
    arb_mat_swap(&swap_mat, &mat);

    print_arb_mat(mat);
    std::cout << '\n';
    print_arb_mat(swap_mat);
    std::cout << '\n';

    arb_mat_clear(&mat);
    arb_mat_clear(&swap_mat);
}
