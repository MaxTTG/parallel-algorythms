#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace gen {

template <typename T = int64_t>
auto generateVec(size_t n, const T& min_val = std::numeric_limits<T>::min(),
                 const T& max_val = std::numeric_limits<T>::max()) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<T> dist{min_val, max_val};

    std::vector<T> vec(n);
    std::generate(vec.begin(), vec.end(), [&]() -> T { return dist(mersenne_engine); });

    return vec;
}

///

size_t toIndex(const std::vector<size_t>& coords, const std::vector<size_t>& dims) {
    int64_t index = 0, multiplier = 1;
    for (size_t i = 0; i < dims.size(); ++i) {
        index += coords[i] * multiplier;
        multiplier *= dims[i];
    }
    return index;
}

std::vector<std::vector<size_t>> generateNeighbors(const std::vector<size_t>& coords, const std::vector<size_t>& dims) {
    std::vector<std::vector<size_t>> neighbors;
    for (size_t i = 0; i < dims.size(); ++i) {
        if (coords[i] + 1 < dims[i]) {
            auto neighbor = coords;
            neighbor[i]++;
            neighbors.push_back(neighbor);
        }

        if (coords[i] > 0) {
            auto neighbor = coords;
            neighbor[i]--;
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

std::vector<std::vector<int64_t>> generateCube(const std::vector<size_t>& dims) {
    std::cout << "Generating cube ";
    size_t totalVertices = 1;
    for (size_t dim : dims) {
        std::cout << dim << "x";
        totalVertices *= dim;
    }
    std::cout << std::endl;

    std::vector<std::vector<int64_t>> adjacencyList(totalVertices);
    std::vector<size_t> coords(dims.size(), 0);

    while (true) {
        size_t currentIndex = toIndex(coords, dims);

        for (const auto& neighbor : generateNeighbors(coords, dims)) {
            size_t neighborIndex = toIndex(neighbor, dims);
            adjacencyList[currentIndex].push_back(neighborIndex);
        }

        size_t dim = 0;
        while (dim < dims.size() && ++coords[dim] == dims[dim]) {
            coords[dim] = 0;
            dim++;
        }

        if (dim == dims.size())
            break;
    }

    return adjacencyList;
}

}  // namespace gen
