/**
 * @file main.cpp
 * @author m-tarazanov@list.ru
 * @brief Нужно реализовать quicksort.
            От Вас требуется написать последовательную
            версию алгоритма (seq) и параллельную версию (par).
            Взять случайный массив из 10^8 элементов и отсортировать.
            (Усреднить по 5 запускам) Сравнить время работы par на
            4 процессах и seq на одном процессе - у Вас должно быть раза в 3 быстрее.
            Также нужно сопроводить тестами на корректность работы алгоритма.
 * @version 0.1
 */

#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "src/gen.hpp"
#include "src/par/qsort.hpp"
#include "src/seq/qsort.hpp"

#define VECSIZE 100000000
#define RUNNUMS 5

enum SEQ_OR_PAR {
    SEQ,
    PAR,
};

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

template <typename Func, typename... Args>
size_t benchmark(const std::string& label, Func func, Args&&... args) {
    std::cout << "Banchmarking " << label << "..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << label << " took " << duration << " microseconds" << std::endl;
    return duration;
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

int main() {
    std::vector<std::pair<size_t, size_t>> runs;
    for (int i = 0; i < RUNNUMS; ++i) {
        std::cout << std::endl << "Run #" << i + 1 << std::endl;
        runs.push_back(run());
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
