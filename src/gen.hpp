#pragma once

#include <algorithm>
#include <fstream>
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

}  // namespace gen
