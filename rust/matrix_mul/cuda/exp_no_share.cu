#include "exp_no_share.cuh"


__global__
void MatExpKernel(Matrix<mat_float_t> src, Matrix<mat_float_t> tgt) {
    float_t tgt_val = 0.0;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row > src.height || col > src.width) return;

    for (std::size_t i = 0; i < src.width; ++i) {
        tgt_val += (src.elements[row * src.width + i]) 
                 * (src.elements[i * src.width + col]);
    }
    tgt.elements[row * tgt.width + col] = tgt_val;
}

void MatExp(const Matrix<mat_float_t> &src, Matrix<mat_float_t> &tgt) {

    dim3 dimBlock(CUDA_BLOCK_SIZE, CUDA_BLOCK_SIZE);
    dim3 dimGrid((src.width + dimBlock.x - 1) / dimBlock.x,
                 (src.height + dimBlock.y - 1) / dimBlock.y);

    MatExpKernel<<<dimGrid, dimBlock>>>(src, tgt);

}
