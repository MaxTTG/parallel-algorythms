#pragma once

#include <queue>
#include <vector>

namespace seq {

static const int64_t NOT_VISITED = -1;

std::vector<int64_t> bfs(int64_t start, std::vector<std::vector<int64_t>> const& edges) {
    std::vector<int64_t> dist(edges.size(), NOT_VISITED);
    dist[start] = 0;
    std::queue<int64_t> q;
    q.push(start);
    while (!q.empty()) {
        auto cur = q.front();
        q.pop();
        for (auto next : edges[cur]) {
            if (dist[next] == NOT_VISITED) {
                dist[next] = dist[cur] + 1;
                q.push(next);
            }
        }
    }

    return dist;
}

}  // namespace seq
