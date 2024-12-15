#pragma once

#define LOCAL_FRONTIERS
// 500 - 2.45
// 400 - 2.46
// 300 - 2.78
// 200 - 3.29
// 100 - 2.23
#ifdef LOCAL_FRONTIERS
#include <omp.h>

#include <vector>
#endif

// #define PREFIX_SCAN_SPLIT
// 500 - 1.98
// 400 - 1.93
// 300 - 2.26
// 200 - 2.65
// 100 - 2.12
#ifdef PREFIX_SCAN_SPLIT
#include <omp.h>

#include <QtConcurrent/QtConcurrentFilter>
#include <atomic>
#include <functional>
#include <vector>
#endif

namespace par {

static const int64_t NOT_VISITED = -1;
static const int8_t THREAD_NUM   = 4;

#ifdef LOCAL_FRONTIERS
std::vector<int64_t> bfs(int64_t start, const std::vector<std::vector<int64_t>>& edges) {
    omp_set_num_threads(THREAD_NUM);

    std::vector<int64_t> dist(edges.size(), NOT_VISITED);
    dist[start] = 0;

    std::vector<int64_t> curLevel{start};
    std::vector<std::vector<int64_t>*> localNextLevels(THREAD_NUM);

    while (!curLevel.empty()) {
#pragma omp parallel
        {
            int thread_id              = omp_get_thread_num();
            localNextLevels[thread_id] = new std::vector<int64_t>();
            auto& localNextLevel       = localNextLevels[thread_id];
            localNextLevel->clear();

#pragma omp for schedule(static)
            for (size_t i = 0; i < curLevel.size(); ++i) {
                int64_t node     = curLevel[i];
                int64_t nextDist = dist[node] + 1;

                for (int64_t neighbor : edges[node]) {
                    if (dist[neighbor] == NOT_VISITED) {
                        dist[neighbor] = nextDist;
                        localNextLevel->push_back(neighbor);
                    }
                }
            }
        }

        curLevel.clear();
        for (const auto& localLevel : localNextLevels) {
            curLevel.insert(curLevel.end(), localLevel->begin(), localLevel->end());
        }
    }

    for (auto& v : localNextLevels) {
        delete v;
    }

    return dist;
}
#endif

#ifdef PREFIX_SCAN_SPLIT
std::vector<int64_t> bfs(int64_t start, const std::vector<std::vector<int64_t>>& edges) {
    omp_set_num_threads(THREAD_NUM);
    std::vector<int64_t> distances(edges.size(), NOT_VISITED);
    distances[start] = 0;

    std::vector<int64_t> curLevel{start};

    while (!curLevel.empty()) {
        std::vector<uint64_t> levelSizes(curLevel.size());

#pragma omp parallel for schedule(static)
        for (size_t i = 0; i < curLevel.size(); ++i) {
            levelSizes[i] = edges[curLevel[i]].size();
        }

        std::vector<uint64_t> prefSizes(curLevel.size() + 1, 0);
        for (size_t i = 0; i < curLevel.size(); ++i) {
            prefSizes[i + 1] = prefSizes[i] + levelSizes[i];
        }

        std::vector<int64_t> newLevel(prefSizes.back(), NOT_VISITED);

#pragma omp parallel for schedule(static)
        for (size_t i = 0; i < curLevel.size(); ++i) {
            int64_t node = curLevel[i];
            int64_t dist = distances[node] + 1;
            size_t idx   = prefSizes[i];

            for (size_t j = 0; j < edges[node].size(); ++j) {
                int64_t neighbor = edges[node][j];

                if (distances[neighbor] == NOT_VISITED) {
                    distances[neighbor] = dist;
                    newLevel[idx + j]   = neighbor;
                }
            }
        }

        std::vector<std::vector<int64_t>*> localNextLevels(THREAD_NUM);
#pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            auto& local   = localNextLevels[thread_id];
            local         = new std::vector<int64_t>();
#pragma omp for schedule(static)
            for (size_t i = 0; i < newLevel.size(); ++i) {
                if (newLevel[i] != NOT_VISITED) {
                    local->push_back(newLevel[i]);
                }
            }
        }

        curLevel.clear();
        for (const auto& localLevel : localNextLevels) {
            curLevel.insert(curLevel.end(), localLevel->begin(), localLevel->end());
        }
    }

    return distances;
}

#endif

}  // namespace par
