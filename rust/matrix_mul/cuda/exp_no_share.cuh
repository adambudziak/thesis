#pragma once

#include "matrix.h"

#define CUDA_BLOCK_SIZE 16

__global__
void MatExpKernel(const Matrix<mat_float_t>, Matrix<mat_float_t>);

void MatExp(const Matrix<mat_float_t>& src, Matrix<mat_float_t>& tgt);
