#include "multNoShare.cuh"

#include <stdlib.h>
#include <bsd/stdlib.h>
#include <iostream>

__global__ void MatMulKernel(Matrix A, Matrix B, Matrix C) {
    float Cvalue = 0.0;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row > A.height || col > B.width) return;

    for (int e = 0; e < A.width; ++e) {
        Cvalue += (A.elements[row * A.width + e]) * (B.elements[e * B.width + col]);
    }
    C.elements[row * C.width + col] = Cvalue;
}

void MatMul(const Matrix A, const Matrix B, Matrix C) {
    Matrix d_A;
    d_A.width = A.width;
    d_A.height = A.height;
    size_t size = A.width * A.height + sizeof(float);
    cudaError_t err = cudaMalloc(&d_A.elements, size);
    std::cerr << "CUDA malloc A: " << cudaGetErrorString(err) << '\n';
    /* printf("CUDA malloc A: %s\n", cudaGetErrorString(err)); */
    err = cudaMemcpy(d_A.elements, A.elements, size, cudaMemcpyHostToDevice);
    printf("Copy A to device: %s\n", cudaGetErrorString(err));

    Matrix d_B;
    d_B.width = B.width;
    d_B.height = B.height;
    size = B.width * B.height * sizeof(float);
    err = cudaMalloc(&d_B.elements, size);
    printf("CUDA malloc B:%s\n", cudaGetErrorString(err));
    err = cudaMemcpy(d_B.elements, B.elements, size, cudaMemcpyHostToDevice);
    printf("Copy B to device: %s\n", cudaGetErrorString(err));

    Matrix d_C;
    d_C.width = C.width;
    d_C.height = C.height;
    size = C.width * C.height + sizeof(float);
    err = cudaMalloc(&d_C.elements, size);
    printf("CUDA malloc C:%s\n", cudaGetErrorString(err));

    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((B.width + dimBlock.x - 1) / dimBlock.x,
                 (A.height + dimBlock.y - 1) / dimBlock.y);

    MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);
    err = cudaThreadSynchronize();
    printf("Run kernel: %s\n", cudaGetErrorString(err));

    err = cudaMemcpy(C.elements, d_C.elements, size, cudaMemcpyDeviceToHost);
    printf("Copy C off of the device: %s\n", cudaGetErrorString(err));

    cudaFree(d_A.elements);
    cudaFree(d_B.elements);

}

void print_matrix(Matrix *m) {
    for(int i = 0; i < min(10, m->height); ++i) {
        for(int j = 0; j < min(10, m->width); ++j) {
            printf("%f ", m->elements[i*m->width + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    Matrix A, B, C;
    int a1, a2, b1, b2;
    a1 = atoi(argv[1]);
    a2 = atoi(argv[2]);
    b1 = a2;
    b2 = atoi(argv[3]);

    A.height = a1;
    A.width = a2;
    A.elements = (float*)malloc(A.width * A.height * sizeof(float));
    
    B.height = b1;
    B.width = b2;
    B.elements = (float*)malloc(B.width * B.height * sizeof(float));

    C.height = A.height;
    C.width = B.width;
    C.elements = (float*)malloc(C.width * C.height * sizeof(float));

    for(int i = 0; i < A.height; ++i)
        for(int j = 0; j < A.width; ++j)
            A.elements[i*A.width + j] = (float)(arc4random() % 3);

    for(int i = 0; i < B.height; ++i)
        for(int j = 0; j < B.width; ++j)
            B.elements[i*B.width + j] = (float)(arc4random() % 2);

    MatMul(A, B, C);

    print_matrix(&A);
    print_matrix(&B);
    print_matrix(&C);
}
