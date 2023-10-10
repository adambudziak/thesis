#pragma once

#include <string>

#include "matrix.h"
#include <arb_mat.h>

Matrix<mat_float_t> adj_matrix_from_dot(const std::string& str);
arb_mat_struct adj_arb_matrix_from_dot(const std::string& str);

