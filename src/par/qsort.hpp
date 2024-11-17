#pragma once

#include <QRunnable>
#include <QThreadPool>
#include <vector>
//
#include <cmath>
#include <iostream>

#include "../seq/qsort.hpp"

namespace par {

class QSortTask: public QRunnable {
public:
    QSortTask(std::vector<int>& vec, size_t left, size_t right, QThreadPool* pool)
        : _vec(vec), _left(left), _right(right), _pool(pool) {}

    void run() override {
        if (_left >= _right) {
            return;
        }

        if (_right - _left < _TRASHOLD) {
            seq::qsortIter(_vec.begin() + _left, _vec.begin() + _right);
            return;
        }

        auto partitioner = _vec[(_left + _right) / 2];
        size_t i         = _left;
        size_t j         = _right;

        while (i <= j) {
            while (_vec[i] < partitioner)
                ++i;
            while (_vec[j] > partitioner)
                --j;

            if (i >= j)
                break;

            std::swap(_vec[i], _vec[j]);
            ++i;
            --j;
        }

        if (_left < j)
            _pool->start(new QSortTask(_vec, _left, j, _pool));
        if (j + 1 < _right)
            _pool->start(new QSortTask(_vec, j + 1, _right, _pool));
    }

private:
    std::vector<int>& _vec;
    size_t _left;
    size_t _right;

    QThreadPool* _pool = nullptr;

    static const size_t _TRASHOLD = 1000;
};

void qsort(std::vector<int>& vec, size_t maxThreads = 4) {
    if (vec.empty())
        return;

    QThreadPool pool;
    if (maxThreads <= 0) {
        maxThreads = QThreadPool::globalInstance()->maxThreadCount();
    }

    pool.setMaxThreadCount(maxThreads);

    pool.start(new QSortTask(vec, 0, vec.size() - 1, &pool));
    pool.waitForDone();
}

}  // namespace par
