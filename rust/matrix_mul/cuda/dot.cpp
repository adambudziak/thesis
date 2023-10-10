#include "dot.h"

#include <iostream>
#include <fstream>
#include <regex>

enum class ReadDotState {
    Vertices,
    DirectedEdges,
    UndirectedEdges
};

Matrix<mat_float_t> initialize_matrix(std::size_t size) {
    std::cout << "initialize_matrix " << size << '\n';
    Matrix<mat_float_t> matrix;
    matrix.width = size;
    matrix.height = size;
    matrix.elements = (mat_float_t*)malloc(size * size * sizeof(mat_float_t));
    return matrix;
};

Matrix<mat_float_t> adj_matrix_from_dot(const std::string& filepath)
{
    std::ifstream file{};
    file.open(filepath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filepath << '\n';
    }

    std::regex vertex_r{"\\s*([[:digit:]]+)\\s*"};
    std::regex undirected_edge_r{"\\s*([[:digit:]]+)\\s*--\\s*([[:digit:]]+)"};
    std::regex directed_edge_r{"\\s*([[:digit:]]+)\\s*->\\s*([[:digit:]]+)"};

    std::smatch matches;

    std::string line;

    std::size_t size = 0;

    Matrix<mat_float_t> result;
    result.width = 0;
    result.height = 0;

    ReadDotState read_state { ReadDotState::Vertices };

    while (std::getline(file, line)) {
        if (std::regex_search(line, matches, undirected_edge_r)) {
            if (read_state == ReadDotState::Vertices) {
                read_state = ReadDotState::UndirectedEdges;
                result = initialize_matrix(size);
            } else if (read_state == ReadDotState::DirectedEdges) {
                std::cerr << "Invalid .dot file (directed/undirected mashup).\n";
                std::exit(1);
            }
            std::size_t first = std::stoull(matches[1]);
            std::size_t second = std::stoull(matches[2]);
            result.elements[first * result.width + second] = 1;
            result.elements[second * result.width + first] = 1;
        } else if (std::regex_search(line, matches, directed_edge_r)) {
            if (read_state == ReadDotState::Vertices) {
                read_state = ReadDotState::DirectedEdges;
                result = initialize_matrix(size);
            } else if (read_state == ReadDotState::UndirectedEdges) {
                std::cerr << "Invalid .dot file (undirected/directed mashup).\n";
                std::exit(1);
            }
            std::size_t first = std::stoull(matches[1]);
            std::size_t second = std::stoull(matches[2]);
            result.elements[first * result.width + second] = 1;
        } else if (std::regex_search(line, matches, vertex_r)) {
            if (read_state != ReadDotState::Vertices) {
                std::cerr << "Invalid .dot file (vertex after edge)\n";
                std::exit(1);
            }
            std::size_t vertex_id = std::stoull(matches[0]);
            if (size < vertex_id + 1) size = vertex_id + 1;
        }
    }
    return result;
}

arb_mat_struct initialize_arb_matrix(std::size_t size) {
    arb_mat_struct m;
    arb_mat_init(&m, size, size);
    return m;
}

arb_mat_struct adj_arb_matrix_from_dot(const std::string& filepath)
{
    std::ifstream file{};
    file.open(filepath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filepath << '\n';
    }

    std::regex vertex_r{"\\s*([[:digit:]]+)\\s*"};
    std::regex undirected_edge_r{"\\s*([[:digit:]]+)\\s*--\\s*([[:digit:]]+)"};
    std::regex directed_edge_r{"\\s*([[:digit:]]+)\\s*->\\s*([[:digit:]]+)"};

    std::smatch matches;

    std::string line;

    std::size_t size = 0;

    arb_mat_struct result;

    ReadDotState read_state { ReadDotState::Vertices };
    arb_struct arb;


    while (std::getline(file, line)) {
        if (std::regex_search(line, matches, undirected_edge_r)) {
            if (read_state == ReadDotState::Vertices) {
                read_state = ReadDotState::UndirectedEdges;
                result = initialize_arb_matrix(size);
            } else if (read_state == ReadDotState::DirectedEdges) {
                std::cerr << "Invalid .dot file (directed/undirected mashup).\n";
                std::exit(1);
            }
            std::size_t first = std::stoull(matches[1]);
            std::size_t second = std::stoull(matches[2]);
            arb_init(&arb);
            arb_one(&arb);
            result.entries[first * size + second] = arb;
            arb_init(&arb);
            arb_one(&arb);
            result.entries[second * size + first] = arb;
        } else if (std::regex_search(line, matches, directed_edge_r)) {
            if (read_state == ReadDotState::Vertices) {
                read_state = ReadDotState::DirectedEdges;
                result = initialize_arb_matrix(size);
            } else if (read_state == ReadDotState::UndirectedEdges) {
                std::cerr << "Invalid .dot file (undirected/directed mashup).\n";
                std::exit(1);
            }
            std::size_t first = std::stoull(matches[1]);
            std::size_t second = std::stoull(matches[2]);
            arb_init(&arb);
            arb_one(&arb);
            result.entries[first * size + second] = arb;
        } else if (std::regex_search(line, matches, vertex_r)) {
            if (read_state != ReadDotState::Vertices) {
                std::cerr << "Invalid .dot file (vertex after edge)\n";
                std::exit(1);
            }
            std::size_t vertex_id = std::stoull(matches[0]);
            if (size < vertex_id + 1) size = vertex_id + 1;
        }
    }
    return result;
}
