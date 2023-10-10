#include "matrix.h"
#include "utils.cuh"
#include "exp_no_share.cuh"

#include "dot.h"

/* #include <bsd/stdlib.h> */


void print_matrix(Matrix<mat_float_t> &m) {
    for(int i = 0; i < min(10, int(m.height)); ++i) {
        for(int j = 0; j < min(10, int(m.width)); ++j) {
            std::cout << m.elements[i * m.width + j] << ' ';
        }
        std::cout << '\n';
    }
}
/*  */
/* void random_matrix(Matrix<mat_float_t>& m) { */
/*     for (std::size_t i = 0; i < m.height; ++i) { */
/*         for (std::size_t j = 0; j < m.width; ++j) { */
/*             m.elements[i * m.width + j] = (mat_float_t)(arc4random() % 5); */
/*         } */
/*     } */
/* } */


std::size_t get_degree(Matrix<mat_float_t> &m) {
    std::size_t deg = 0;
    for (std::size_t i = 0; i < m.width; ++i)
        if (m.elements[i] != 0.0) ++deg;
    return deg;
}

__global__
void normalize_matrix(Matrix<mat_float_t> m, std::size_t degree) {
    std::size_t row = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row > m.height || col > m.width) return;
    m.elements[row * m.width + col] /= degree;
}

__global__
void pow2_matrix(const Matrix<mat_float_t> src, Matrix<mat_float_t> tgt) {
    std::size_t row = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row > src.height || col > src.width) return;
    
    mat_float_t tgt_val = 0.0;
    for (std::size_t i = 0; i < src.width; ++i) {
        tgt_val += (src.elements[row * src.width + i])
                 * (src.elements[i * src.width + col]);
    }
    tgt.elements[row * tgt.width + col] = tgt_val;
}

__global__
void matrix_tvd(const Matrix<mat_float_t> src, double uniform, double* tvd_vector) {

}

double compute_tvd(Matrix<mat_float_t> src, Matrix<mat_float_t> d_src) {
    dim3 dimBlock(CUDA_BLOCK_SIZE, CUDA_BLOCK_SIZE);
    dim3 dimGrid((src.width + dimBlock.x - 1) / dimBlock.x,
                 (src.height + dimBlock.y - 1) / dimBlock.y);
    std::size_t d = get_degree(src);
    normalize_matrix<<<dimGrid, dimBlock>>>(d_src, get_degree(src));
    std::size_t size = src.width * src.height * sizeof(mat_float_t);
    Matrix<mat_float_t> d_swap_mat;
    d_swap_mat.width = src.width;
    d_swap_mat.height = src.height;
    cudaError_t err = cudaMalloc(&d_swap_mat.elements, size);
    if (err != 0) {
        std::cerr << "Failed to allocate swap matrix!\n";
        std::cerr << cudaGetErrorString(err) << '\n';
        std::exit(1);
    }
    pow2_matrix<<<dimGrid, dimBlock>>>(d_src, d_swap_mat);
    return 0.0;
}


int main(int argc, char **argv) {
    Matrix<mat_float_t> src, tgt;
    /* int width, height; */
    /* width = std::atoi(argv[1]); */
    /* height = std::atoi(argv[2]); */
    /*  */
    /* src.width = width; */
    /* src.height = height; */
    /*  */
    /* tgt.width = width; */
    /* tgt.height = height; */
    /*  */
    /* std::size_t size = width * height * sizeof(mat_float_t); */

    /* src.elements = (mat_float_t*)malloc(size); */
    /* tgt.elements = (mat_float_t*)malloc(size); */

    /* random_matrix(src); */

    src = adj_matrix_from_dot(argv[1]);

    std::cout << src.width << "   " << src.height << '\n';
    print_matrix(src);

    std::size_t size = src.width * src.height * sizeof(mat_float_t);

    tgt.width = src.width;
    tgt.height = src.height;
    tgt.elements = (mat_float_t*)malloc(size);

    Matrix<mat_float_t> d_src = copy_matrix_to_device(src);

    compute_tvd(src, d_src);
    /* Matrix<mat_float_t> d_tgt = copy_matrix_to_device(tgt); */

    /* MatExp(d_src, d_tgt); */
    /*  */
    cudaMemcpy(src.elements, d_src.elements, size, cudaMemcpyDeviceToHost);
    /* cudaMemcpy(tgt.elements, d_tgt.elements, size, cudaMemcpyDeviceToHost); */

    cudaFree(d_src.elements);
    /* cudaFree(d_tgt.elements); */

    print_matrix(src);
    /* print_matrix(tgt); */

    free(src.elements);
    free(tgt.elements);
}
