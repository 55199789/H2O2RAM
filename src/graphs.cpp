#include "graphs.hpp"
#include <queue>
#include "hash_planner.hpp"
#include "oblivious_operations.hpp"
#include "oheap.hpp"

ORAM::ObliviousRAM<uint32_t, int> init_head(int n)
{
    ORAM::ObliviousRAM<uint32_t, int> oram_head(n, -1);
    return oram_head;
}

ORAM::ObliviousRAM<uint32_t, Edge> init_edge(std::vector<Edge> &edges_) //  init_edge(int m)
{
    ORAM::ObliviousRAM<uint32_t, Edge> oram_edges(edges_.begin(), edges_.end());
    return oram_edges;
}

ORAM::ObliviousRAM<uint32_t, Edge> init_edge(int m)
{
    ORAM::ObliviousRAM<uint32_t, Edge> oram_edges(m);
    return oram_edges;
}

void add_edge(std::vector<Edge> &edges, // ORAM::ObliviousRAM<uint32_t, Edge> &edges,
              ORAM::ObliviousRAM<uint32_t, int> &head,
              int source, int to, int weight, int idx)
{
    // assume edges has been initialized
    // std::cout << "add " << source << " " << to << " " << idx << std::endl;
    assert(edges.size() > idx);
    auto &e = edges[idx];
    auto &h = head[source];
    e.to = to;
    e.next = h;
    e.weight = weight;
    h = idx;
}

void add_edge(ORAM::ObliviousRAM<uint32_t, Edge> &edges,
              ORAM::ObliviousRAM<uint32_t, int> &head,
              int source, int to, int idx)
{
    // assume edges has been initialized
    // std::cout << "add " << source << " " << to << " " << idx << std::endl;
    assert(edges.size() > idx);
    auto &e = edges[idx];
    auto &h = head[source];
    e.to = to;
    e.next = h;
    h = idx;
}

ORAM::ObliviousRAM<uint32_t, int> shortest_path(const ORAM::ObliviousRAM<uint32_t, Edge> &edges,
                                                const ORAM::ObliviousRAM<uint32_t, int> &head,
                                                int source)
{
    int n = head.size();
    int m = edges.size();
    ORAM::ObliviousRAM<uint32_t, int> dist(n, -1);
    MinHeap<std::pair<int, int>> pq;
    dist[source] = 0;
    pq.push({0, source});
    bool inner_loop = false;
    int cur_u;
    int dist_u;
    Edge cur_e;
    // use if-else clauses to align with GraphOS's implementation
    for (int i = 0; i < (2 * n + m); i++)
    {
        if (!inner_loop)
        {
            std::pair<int, int> _ = pq.top();
            pq.pop();
            cur_u = _.second;
            dist_u = _.first;
            ORAM::CMOV(dist[cur_u] == dist_u, inner_loop, true);
            ORAM::CMOV(dist[cur_u] == dist_u, cur_e, edges[head[cur_u]]);
        }
        else
        {
            int cur_v = cur_e.to;
            int cur_dist = dist_u + cur_e.weight;
            auto &dist_v = dist[cur_v];

            bool cond1 = (dist_v == -1) | (cur_dist < dist_v);
            ORAM::CMOV(cond1, dist_v, cur_dist);
            ORAM::CMOV(!cond1, cur_dist, std::numeric_limits<int>::max());
            pq.push({cur_dist, cur_v});
            inner_loop = cur_e.next != -1;
            int next = 0;
            ORAM::CMOV(inner_loop, next, cur_e.next);
            ORAM::CMOV(inner_loop, cur_e, edges[next]);
        }
    }
    return dist;
}

std::vector<int> dijkstra(const std::vector<std::vector<int>> &graph, int source)
{
    int n = graph.size();
    std::vector<int> dist(n, -1);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
    dist[source] = 0;
    pq.push({0, source});
    while (!pq.empty())
    {
        int u = pq.top().second;
        assert(dist[u] == pq.top().first);
        pq.pop();
        for (auto v : graph[u])
        {
            if (dist[v] == -1 || dist[v] > dist[u] + 1)
            {
                dist[v] = dist[u] + 1;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}