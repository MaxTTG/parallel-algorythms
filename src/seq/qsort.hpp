#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <queue>
#include <vector>

namespace seq {

template <typename iter>
void qsortIter(iter begin, iter end) {
    if (begin == end || std::distance(begin, end) <= 1)
        return;

    std::queue<std::pair<iter, iter>> q;
    q.push({begin, end - 1});

    while (!q.empty()) {
        auto [leftIt, rightIt] = q.front();
        q.pop();

        iter midIt       = leftIt + (std::distance(leftIt, rightIt) / 2);
        auto partitioner = *midIt;

        iter i = leftIt;
        iter j = rightIt;

        while (i <= j) {
            while (*i < partitioner)
                ++i;
            while (*j > partitioner)
                --j;

            if (i >= j)
                break;

            std::swap(*i, *j);
            ++i;
            --j;
        }

        if (leftIt < j)
            q.push({leftIt, j});
        if (j + 1 < rightIt)
            q.push({j + 1, rightIt});
    }
}

void qsort(std::vector<int>& vec) {
    seq::qsortIter(vec.begin(), vec.end());
}

}  // namespace seq
