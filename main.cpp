/**
 * @file main.cpp
 * @author m-tarazanov@list.ru
 * @version 0.1
 */

#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "src/gen.hpp"
#include "src/par/bfs.hpp"
#include "src/par/qsort.hpp"
#include "src/seq/bfs.hpp"
#include "src/seq/qsort.hpp"

#define VECSIZE 100
#define RUNNUMS 5
#define CUBE_EDGE 500

enum SEQ_OR_PAR {
    SEQ,
    PAR,
};

template <typename Func, typename... Args>
auto benchmark(const std::string& label, Func func, Args&&... args) {
    std::cout << "Benchmarking " << label << "..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>) {
        func(std::forward<Args>(args)...);
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << label << " took " << duration << " microseconds" << std::endl;
        return duration;
    } else {
        auto result   = func(std::forward<Args>(args)...);
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << label << " took " << duration << " microseconds" << std::endl;
        return std::make_pair(duration, result);
    }
}

namespace task1 {

void printVec(const std::vector<int>& vec) {
    for (auto&& i : vec) {
        std::cout << i << ' ';
    }

    std::cout << std::endl << "=============" << std::endl;
}

bool isSorted(const std::vector<int>& vec) {
    for (size_t i = 1; i < vec.size(); i++) {
        if (vec[i - 1] > vec[i]) {
            std::cerr << "ERROR: vec[" << i - 1 << "] = " << vec[i - 1] << ", vec[" << i << "] = " << vec[i] << "; But "
                      << vec[i - 1] << " > " << vec[i] << std::endl;

            return false;
        }
    }

    std::cout << "Vector is sorted!" << std::endl;

    return true;
}

template <typename T>
size_t checkQuickSort(SEQ_OR_PAR type, std::vector<T>& vec) {
    size_t time;
    switch (type) {
    case SEQ:
        time = benchmark("Sequental", seq::qsort, vec);
        break;
    case PAR:
        time = benchmark("Parallel", par::qsort, vec, 4);
        break;
    default:
        throw std::logic_error("Wrong type");
    }

    seq::qsort(vec);
    isSorted(vec);
    return time;
}

auto run() {
    auto vec1 = gen::generateVec<int>(VECSIZE);
    auto vec2(vec1);

    auto seqTime = checkQuickSort(SEQ, vec1);
    auto parTime = checkQuickSort(PAR, vec2);
    std::cout << "Acceleration: " << (double)seqTime / parTime << std::endl;

    return std::make_pair(seqTime, parTime);
}

int qsortMain() {
    std::vector<std::pair<size_t, size_t>> runs;
    for (int i = 0; i < RUNNUMS; ++i) {
        std::cout << std::endl << "Run #" << i + 1 << std::endl;
        runs.emplace_back(run());
    }

    // clang-format off
    std::cout << std::endl
              << "=====================================" << std::endl
              << "seq (mcsec) | par(mcsec)| accelerate " << std::endl
              << "=====================================" << std::endl;
    // clang-format on

    size_t avgSeq = 0;
    size_t avgPar = 0;

    for (int i = 0; i < RUNNUMS; ++i) {
        avgSeq += runs[i].first;
        avgPar += runs[i].second;
        std::cout << '\t' << runs[i].first << "\t|\t" << runs[i].second << "\t|\t"
                  << (double)runs[i].first / runs[i].second << std::endl;
    }

    avgSeq /= RUNNUMS;
    avgPar /= RUNNUMS;
    std::cout << "=====================================" << std::endl;
    std::cout << "Average seq result: " << avgSeq << std::endl;
    std::cout << "Average par result: " << avgPar << std::endl;
    std::cout << "Average accelerate: " << (double)avgSeq / avgPar << std::endl;

    return 0;
}

}  // namespace task1

namespace task2 {

template <typename T>
bool vectorsEquals(const std::vector<T>& vec1, const std::vector<T>& vec2) {
    assert(vec1.size() == vec2.size());
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] != vec2[i]) {
            std::cout << "Векторы не равны: vec1[" << i << "] = " << vec1[i] << "; vec2[" << i << "] = " << vec2[i]
                      << std::endl;
            return false;
        }
    }

    std::cout << "Векторы равны!" << std::endl;
    return true;
}

int bfsMain() {
#ifdef PREFIX_SCAN_SPLIT
    std::cout << "PREFIX_SCAN_SPLIT" << std::endl;
#endif

#ifdef LOCAL_FRONTIERS
    std::cout << "LOCAL_FRONTIERS" << std::endl;
#endif

    const auto cube = gen::generateCube({CUBE_EDGE, CUBE_EDGE, CUBE_EDGE});

    std::vector<std::pair<size_t, size_t>> runs;
    for (int i = 0; i < RUNNUMS; ++i) {
        std::cout << std::endl << "Run #" << i + 1 << std::endl;
        auto [seqTime, seqRes] = benchmark("seq::BFS", seq::bfs, 0, cube);
        auto [parTime, parRes] = benchmark("par::BFS", par::bfs, 0, cube);
        assert(vectorsEquals(seqRes, parRes));
        runs.push_back({seqTime, parTime});
    }

    // clang-format off
    std::cout << std::endl
              << "=====================================" << std::endl
              << "seq (mcsec) | par(mcsec)| accelerate " << std::endl
              << "=====================================" << std::endl;
    // clang-format on

    size_t avgSeq = 0;
    size_t avgPar = 0;

    for (int i = 0; i < RUNNUMS; ++i) {
        avgSeq += runs[i].first;
        avgPar += runs[i].second;
        std::cout << '\t' << runs[i].first << "\t|\t" << runs[i].second << "\t|\t"
                  << (double)runs[i].first / runs[i].second << std::endl;
    }

    avgSeq /= RUNNUMS;
    avgPar /= RUNNUMS;
    std::cout << "=====================================" << std::endl;
    std::cout << "Average seq result: " << avgSeq << std::endl;
    std::cout << "Average par result: " << avgPar << std::endl;
    std::cout << "Average accelerate: " << (double)avgSeq / avgPar << std::endl;

    return 0;
}

void tests() {
    // Тест 1: Один узел
    {
        std::vector<std::vector<int64_t>> edges = {{}};
        assert(vectorsEquals(seq::bfs(0, edges), std::vector<int64_t>{0}));
        assert(vectorsEquals(par::bfs(0, edges), std::vector<int64_t>{0}));
    }

    // Тест 2: Два узла с ребром
    {
        std::vector<std::vector<int64_t>> edges = {{1}, {}};
        assert(vectorsEquals(seq::bfs(0, edges), std::vector<int64_t>{0, 1}));
        assert(vectorsEquals(par::bfs(0, edges), std::vector<int64_t>{0, 1}));
    }

    // Тест 3: Несвязный граф
    {
        std::vector<std::vector<int64_t>> edges = {{}, {}};
        assert(vectorsEquals(seq::bfs(0, edges), std::vector<int64_t>{0, seq::NOT_VISITED}));
        assert(vectorsEquals(par::bfs(0, edges), std::vector<int64_t>{0, par::NOT_VISITED}));
    }

    // Тест 4: Кольцевой граф
    {
        std::vector<std::vector<int64_t>> edges = {{1}, {2}, {0}};
        assert(vectorsEquals(seq::bfs(0, edges), std::vector<int64_t>{0, 1, 2}));
        assert(vectorsEquals(par::bfs(0, edges), std::vector<int64_t>{0, 1, 2}));
    }

    // Тест 5: Случайный граф
    {
        std::vector<std::vector<int64_t>> edges = {
            {1, 2},  // 0 -> 1, 0 -> 2
            {3},     // 1 -> 3
            {3},     // 2 -> 3
            {}       // 3
        };

        assert(vectorsEquals(seq::bfs(0, edges), std::vector<int64_t>{0, 1, 1, 2}));
        assert(vectorsEquals(par::bfs(0, edges), std::vector<int64_t>{0, 1, 1, 2}));
    }

    std::cout << "All tests passed!" << std::endl;
}

}  // namespace task2

int main() {
    // return task1::qsortMain();
    task2::tests();
    return task2::bfsMain();
}
