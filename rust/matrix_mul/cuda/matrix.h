#pragma once

#include <cstddef>

using mat_float_t = double;

template <typename Float_T>
struct Matrix {
    std::size_t width;
    std::size_t height;
    Float_T* elements;
};
