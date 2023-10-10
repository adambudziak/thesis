#ifndef MULT_NO_SHARE_H
#define MULT_NO_SHARE_H

#include <stdio.h>

typedef struct {
    int width;
    int height;
    float* elements;
} Matrix;

#define BLOCK_SIZE 16

__global__ void MatMulKernel(const Matrix, const Matrix, Matrix);

#endif
