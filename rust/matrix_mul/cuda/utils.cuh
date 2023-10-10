#pragma once

#include <iostream>

#include "matrix.h"

template <typename Float_T>
Matrix<Float_T> copy_matrix_to_device(const Matrix<Float_T> &src) {
    Matrix<Float_T> tgt;
    tgt.width = src.width;
    tgt.height = src.height;
    std::size_t size = src.width * src.height * sizeof(Float_T);
    cudaError_t err = cudaMalloc(&tgt.elements, size);
    if (err != 0) {
        std::cerr << "Failed to allocate matrix on device.\n";
        std::cerr << cudaGetErrorString(err) << '\n';
        std::exit(1);
    }

    err = cudaMemcpy(tgt.elements, src.elements, size, cudaMemcpyHostToDevice);
    if (err != 0) {
        std::cerr << "Failed to copy matrix from host to device.\n";
        std::cerr << cudaGetErrorString(err) << '\n';
        std::exit(1);
    }

    return tgt;
}
