#pragma once
#include <omp.h>
#include <stdlib.h>
#include <algorithm>
#include <mutex>
#include <random>
#include <vector>
#include "oshuffle.hpp"
using namespace std;

namespace ORAM
{
    enum EdgeFlag : uint8_t
    {
        REVERSE_IF_POSSIBLE_BK,
        REVERSE_IF_POSSIBLE,
        REVERSABLE,
        NA
    };

    // #pragma pack(push, 1)
    template <std::unsigned_integral T = uint32_t>
    struct BiEdge
    {
        T u, v;
        uint16_t ctr;
        uint8_t dir; // 1 towards table, 0 towards keys
        EdgeFlag flag;
        BiEdge(T u = 0, T v = 0)
            : u(u), v(v),
              ctr(0), dir(1), flag(NA)
        {
        }
    };
    // #pragma pack(pop)

    template <std::unsigned_integral T = uint32_t, uint8_t group = 0>
    struct CompareEdge
    {
    };

    template <std::unsigned_integral T>
    struct CompareEdge<T, 0>
    {
        bool operator()(const BiEdge<T> &a, const BiEdge<T> &b) const
        {
            // if (a.u != b.u)
            //     return a.u < b.u;
            // directs to left first
            // if (a.dir != b.dir)
            //     return a.dir == 0;
            // // flag equals to REVERSABLE first
            // if (a.flag != b.flag)
            //     return a.flag == REVERSABLE;
            // return a.ctr < b.ctr;
            bool cond1 = a.dir != b.dir;
            bool ret1 = a.dir == 0;
            bool cond2 = a.flag != b.flag;
            bool ret2 = a.flag == REVERSABLE;
            bool ret3 = a.ctr < b.ctr;
            return (cond1 & ret1) | (!cond1 & cond2 & ret2) | (!cond1 & !cond2 & ret3);
        }
    };

    template <std::unsigned_integral T>
    struct CompareEdge<T, 1>
    {
        bool operator()(const BiEdge<T> &a, const BiEdge<T> &b) const
        {
            // if (a.v != b.v)
            //     return a.v < b.v;
            // if (a.dir != b.dir)
            //     return a.dir == 0;
            // return a.ctr > b.ctr;
            bool cond1 = a.v != b.v;
            bool ret1 = a.v < b.v;
            bool cond2 = a.dir != b.dir;
            bool ret2 = a.dir == 0;
            bool ret3 = a.ctr > b.ctr;
            return (cond1 & ret1) | (!cond1 & cond2 & ret2) | (!cond1 & !cond2 & ret3);
        }
    };

    template <std::unsigned_integral T>
    std::vector<T> omatcher(std::vector<BiEdge<T>> &edges,
                            const T left_cnt,
                            const T k)
    {
        assert(left_cnt * k == edges.size());
        BiEdge<T> **edges_by_bucket = new BiEdge<T> *[k];
        for (T i = 0; i < k; i++)
            edges_by_bucket[i] = new BiEdge<T>[left_cnt];
        std::vector<T> matchL(left_cnt, -1);
        // Assuming 2n nodes in the right part, divided into k buckets
        T matches;
        const T UB = 3 * std::ceil(std::log2(left_cnt)) + 3;
        std::vector<T> indices;
        for (int z = 0; z < UB; z++)
        {
            // edges are created in order for the first time
            // public info
            if (z) [[likely]]
            {
                indices.resize(k);
                std::iota(indices.begin(), indices.end(), 0);
                std::for_each(std::execution::par_unseq,
                              indices.begin(),
                              indices.end(),
                              [&](auto i)
                              {
                                  osorter(edges_by_bucket[i], left_cnt,
                                          [](const BiEdge<T> &a, const BiEdge<T> &b)
                                          { return a.u < b.u; });

                                  for (T j = 0, _ = 0; j < left_cnt; j++, _ += k)
                                      edges[_ + i] = edges_by_bucket[i][j];
                              });
                indices.resize(edges.size() / k);
                std::iota(indices.begin(), indices.end(), 0);
                std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](size_t idx)
                              { osorter(edges.data() + idx * k, k, CompareEdge<T, 0>()); });
            }
            else
            {
                indices.resize(edges.size() / k);
                std::iota(indices.begin(), indices.end(), 0);
            }
            // It's public that each v in the left has degree k
            std::for_each(std::execution::par_unseq,
                          indices.begin(),
                          indices.end(), [&](auto _)
                          { 
                            const T i = _ * k;
                            auto &e0 = edges[i];
                            matchL[e0.u] = e0.v;
                            e0.ctr += e0.dir;
                            e0.dir = 0;
                            for (T j = 1; j < k; ++j)
                            {
                                auto &e = edges[i + j];
                                CMOV((e0.flag == REVERSE_IF_POSSIBLE) & (e.flag == REVERSABLE),
                                     e0.flag, NA);
                                CMOV((e0.flag == REVERSE_IF_POSSIBLE) & (e.flag == REVERSABLE),
                                     e0.dir, (uint8_t)1);
                                CMOV((e0.flag == REVERSE_IF_POSSIBLE) & (e.flag == REVERSABLE),
                                     e.flag, NA);
                                CMOV((e0.flag == REVERSE_IF_POSSIBLE) & (e.flag == REVERSABLE),
                                     e.ctr, uint16_t(e.ctr + 1));
                                CMOV((e0.flag == REVERSE_IF_POSSIBLE) & (e.flag == REVERSABLE),
                                     e.dir, (uint8_t)0);
                            }
                            CMOV(e0.flag == REVERSE_IF_POSSIBLE_BK, e0.flag, REVERSE_IF_POSSIBLE);
                            osorter(edges.data() + i,
                                    k,
                                    [](const BiEdge<T> &a, const BiEdge<T> &b)
                                    {
                                        return a.v < b.v;
                                    });
                            for (T j = 0; j < k; j++)
                                edges_by_bucket[j][i / k] = edges[i + j]; });
            std::for_each(std::execution::par_unseq,
                          edges_by_bucket, edges_by_bucket + k,
                          [left_cnt](auto &bucket)
                          {
                              osorter(bucket, left_cnt, CompareEdge<T, 1>());
                          });
            matches = 0;
            if (indices.size() < k) [[unlikely]]
            {
                indices.resize(k);
                std::iota(indices.begin(), indices.end(), 0);
            }
            // for (T i = 0; i < k; i++)
            std::for_each(std::execution::par_unseq,
                          indices.begin(),
                          indices.begin() + k,
                          [&](auto i)
                          {
                              // osorter(edges.data(), left_cnt * k, CompareEdge<T, 1>());
                              auto e0 = edges_by_bucket[i] + left_cnt - 1;
                              auto prev_flag = NA;
                              for (T j = 0; j < left_cnt; j++)
                              {
                                  auto e = edges_by_bucket[i] + j;
                                  const bool first_edge = e->v != e0->v;
                                  auto cur_flag = prev_flag;
                                  CMOV(first_edge & (e->dir == 1), cur_flag, REVERSABLE);
                                  CMOV(first_edge & (e->dir != 1), cur_flag, NA);
                                  CMOV(!first_edge & (e->dir == 0), e->dir, (uint8_t)1);
                                  CMOV(!first_edge & (e->dir == 0), e->flag, REVERSE_IF_POSSIBLE_BK);

                                  CMOV(first_edge, e0, e);
                                  prev_flag = e->flag = cur_flag;
                              }
                          });
            for (T i = 0; i < k; i++)
                for (T j = 0; j < left_cnt; ++j)
                {
                    auto e = edges_by_bucket[i] + j;
                    matches += !e->dir;
                }
        }
        // if (matches != left_cnt)
        //     std::cerr << "Warning: " << matches << " matches out of " << left_cnt << std::endl;
        assert(matches == left_cnt);
        // delete edges_by_bucket
        for (T i = 0; i < k; i++)
            delete[] edges_by_bucket[i];
        delete[] edges_by_bucket;
        return matchL;
    }

    // for testing
    template <std::unsigned_integral T>
    std::vector<T> no_match(std::vector<BiEdge<T>> &edges, T N, T k)
    {
        T M = N * 2;
        std::vector<std::vector<T>> adj_left(N);
        std::vector<T> matchL(N, -1), matchR(M, -1);
        std::vector<T> root(N), prev(N), qq(N);

        for (auto &e : edges)
            adj_left[e.u].push_back(e.v);

        T updated_cnt = 0;
        T max_dist = 0;
        for (bool updated = true; updated;)
        {
            updated = false;
            updated_cnt++;
            T qi, qj;
            qi = qj = 0;
            memset(root.data(), -1, N * sizeof(T));
            memset(prev.data(), -1, N * sizeof(T));
            for (T i = 0; i < N; i++)
                if (matchL[i] == -1)
                    qq[qj++] = i, root[i] = i, prev[i] = i;
            while (qi < qj)
            {
                T u = qq[qi++];
                if (matchL[root[u]] != -1)
                    continue;

                for (T v : adj_left[u])
                {
                    if (matchR[v] == -1)
                    {
                        T dist = 0;
                        while (v != -1)
                            matchR[v] = u, std::swap(matchL[u], v), u = prev[u], dist++;

                        max_dist = max(max_dist, dist);
                        updated = true;
                        break;
                    }

                    if (prev[matchR[v]] == -1)
                        v = matchR[v], prev[v] = u, root[v] = root[u], qq[qj++] = v;
                }
            }
        }
        for (auto &e : matchL)
        assert(e != -1);
        return matchL;
    }
}