
#include <omp.h>
#include <algorithm>
#include <bitset>
#include <iostream>
#include <execution>
#include <vector>
#include <numeric>
#include <random>
#include <unistd.h>
#include <variant>
#include <ranges>
#include <queue>
#include <tbb/global_control.h>
#include "oblivious_operations.hpp"
#include "iterator_stride.hpp"
#include "ocompact.hpp"
#include "ohash_tiers.hpp"
#include "omap.hpp"
#include "oshuffle.hpp"
#include "types.hpp"
#include "timer.hpp"
#include "hash_planner.hpp"
#include "oram.hpp"
#include "graphs.hpp"
#include <iomanip>
#include <unordered_set>
#include <set>
using namespace std;
int benchmark_ocompact();
void benchmark_ohahsbin();

// static long getMemoryUsage()
// {
//     struct rusage usage;
//     getrusage(RUSAGE_SELF, &usage);
//     return usage.ru_maxrss;
// }
void benchmark_omap()
// int main(int argc, char **argv)
{
    std::cout << "CACHE: " << CACHE_OBLIVIOUS << ", PAGE: " << PAGE_LEVEL_OBLIVIOUS << std::endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 1;
    // uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    uint32_t n = 1024; // argc >= 2 ? std::atoi(argv[1]) : 1024;
    // using target_type = ORAM::Block<uint32_t, 1020>;
    using target_type = int;
    constexpr int B = 4;
    using value_type = size_t; // ORAM::Block<uint32_t, B>;
    while (test_cases--)
    {
        Timer t;
        std::vector<value_type> data(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        std::cout << "Preprocessing: " << t.get_interval_time() << " s" << std::endl;
        std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
        uint32_t T = n; // std::min(100000u, n);
        std::map<uint32_t, value_type> map;
        for (uint32_t i = 0; i < T; i++)
            map.emplace(dis(gen), data[i]);
        auto _ = t.get_interval_time();
        std::cout << "MAP Insertion: " << _ << " s" << ", per op: " << _ / T * 1000 << " ms" << std::endl;

        // avl_array<uint32_t, ORAM::Block<uint32_t, B>, uint32_t, true> avl(n);
        ORAM::ObliviousMap<uint32_t, value_type> omap;
        std::cout << "Init: " << t.get_interval_time() << " s" << std::endl;
        value_type val;
        for (auto &x : map)
            omap.insert(x.first, x.second);
        _ = t.get_interval_time();
        std::cout << "DOMAP Insertion: " << _ << " s" << ", per op: " << _ / T * 1000 << " ms" << std::endl;
        for (auto &x : map)
        {
            val = value_type(omap[x.first]);
            // avl.find(data[i].id, val);
            if (val != x.second)
            {
                std::cout << "val: " << val << ", data[i]: " << x.second << std::endl;
                assert(false);
            }
        }
        _ = t.get_interval_time();
        std::cout << "DOMAP Find: " << _ << " s" << ", per op: " << _ / T * 1000 << " ms" << std::endl;
    }
}
void benchmark_pq()
// int main(int argc, char **argv)
{
    uint32_t x = 1;
    uint32_t y = 0;
    bool cond = 0;
    double t1 = 0, t2 = 0;
    Timer t;
    x = ORAM::oblivious_select(x, y, cond);
    t1 = t.get_interval_time();
    ORAM::CMOV(cond, x, y);
    t2 = t.get_interval_time();
    std::cout << "x: " << x << ", y: " << y << std::endl;
    // return 0;
    // std::cout << "CMOV: " << t1 << "s" << std::endl;
    // std::cout << "oblivious_select: " << t2 << "s" << std::endl;

    std::cout << "CACHE: " << CACHE_OBLIVIOUS << ", PAGE: " << PAGE_LEVEL_OBLIVIOUS << std::endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 1;
    // uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    uint32_t n = 1024; // argc >= 2 ? std::atoi(argv[1]) : 1024; // 1 << 16;
    uint32_t m = n * 4;
    std::cout << "n: " << n << ", m: " << m
              << ", graph size: 2^" << std::log2(m + n) << std::endl;
    // using target_type = ORAM::Block<uint32_t, 1020>;
    using target_type = int;
    while (test_cases--)
    {
        std::vector<std::vector<int>> graph(n);
        std::uniform_int_distribution<> dist(0, n - 1);
        std::set<std::pair<int, int>> tmps;
        Timer t;
        auto head = init_head(n);
        std::vector<Edge> edges_(m); // = init_edge(m);
        // auto edges = init_edge(m);
        int cnt = 0;
        std::cout << "Init: " << t.get_interval_time() << " s" << std::endl;
        for (int i = 0; i < m / 2; i++)
        {
            int u = dist(gen);
            int v = dist(gen);
            if (u == v || tmps.find({u, v}) != tmps.end())
            {
                i--;
                continue;
            }
            tmps.insert({u, v});
            tmps.insert({v, u});
            // std::cout << "i: " << i << ", u: " << u << ", v: " << v << std::endl;
            // add_edge(edges, head, u, v, cnt++);
            // add_edge(edges, head, v, u, cnt++);
            add_edge(edges_, head, u, v, 1, cnt++);
            add_edge(edges_, head, v, u, 1, cnt++);
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        auto edges = init_edge(edges_);
        std::cout << "Graph: " << t.get_interval_time() << " s" << std::endl;
        auto root = dist(gen);
        auto dist1 = shortest_path(edges, head, root);
        auto _ = t.get_interval_time();
        std::cout << "ObliviousRAM: " << _ << " s" << ", per op: " << " us" << std::endl;
        auto dist2 = dijkstra(graph, root);
        _ = t.get_interval_time();
        std::cout << "Dijkstra: " << _ << " s" << ", per op: " << " us" << std::endl;
        // output the graph
        // for (int i = 0; i < n; i++)
        // {
        //     std::cout << i << ": ";

        //     for (auto &x : graph[i])
        //         std::cout << x << " ";
        //     std::cout << std::endl;
        // }
        // for (int i = 0; i < n; i++)
        //     if (dist1[i] != dist2[i])
        //     {
        //         std::cout << "i: " << i << ", dist1[i]: " << dist1[i] << ", dist2[i]: " << dist2[i] << std::endl;
        //         assert(dist1[i] == dist2[i]);
        //     }
    }
}

void benchmark_oram()
// int main(int argc, char **argv)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    using KeyType = uint32_t;
    uint32_t n = 1024; // argc >= 2 ? std::atoi(argv[1]) : 1 << 21; // 1 << 16;
    std::uniform_int_distribution<KeyType> dis(0, n - 1);
    constexpr int BlockSize = 16;
    std::vector<ORAM::Block<KeyType, BlockSize>> data(n);
    uint32_t op_num = n;
    auto compute_time = [&](KeyType bucket_num)
    {
        KeyType bucket_size = ORAM::compute_bucket_size(n, bucket_num, 64);
        double total_time = 0;
        constexpr int T = 8;
        for (int _ = 0; _ < T; _++)
        {
            for (KeyType i = 0; i < n; i++)
                data[i].id = i;
            std::shuffle(data.begin(), data.end(), gen);
            ORAM::OHashBucket<KeyType, BlockSize> obucket(n, bucket_num, bucket_size);
            Timer t;
            obucket.build(data.data());
            for (KeyType i = 0; i < op_num; i++)
                obucket[dis(gen)];
            obucket.extract();
            auto cur_time = t.get_total_time();
            total_time += cur_time;
        }
        return total_time / T;
    };
    for (KeyType bucket_num = 1; bucket_num <= n; bucket_num++)
    {
        auto cur_time = compute_time(bucket_num);
        std::cout << "bucket_num: " << bucket_num << ", time: " << cur_time << std::endl;
    }
}
// void benchmark_oram()

int main(int argc, char **argv)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 1;
    // uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    uint32_t n = argc >= 2 ? std::atoi(argv[1]) : 1 << 21; // 1 << 16;
    std::cout << "n: " << n << std::endl;
    constexpr int B = 12;
    while (test_cases--)
    {
        Timer t;
        // std::vector<int> data(n);
        // for (uint32_t i = 0; i < n; i++)
        //     data[i] = i;
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::vector<uint8_t> flags(n);
        std::shuffle(data.begin(), data.end(), gen);
        std::cout << "Preprocessing: " << t.get_interval_time() << " s" << std::endl;
        ORAM::ObliviousRAM<uint32_t, ORAM::Block<uint32_t, B>> oram(data.begin(), data.end());
        auto max_mem_usage = getMemoryUsage();
        std::cout << "total: " << n * sizeof(oram[0]) / 1024 << " KB, memory usage: " << getMemoryUsage() << " KB" << ", ratio: " << 1.0 * getMemoryUsage() / n / sizeof(data[0]) * 1024 << std::endl;
        std::cout << "Init: " << t.get_interval_time() << " s" << std::endl;
        // ProfilerStart("test_capture.prof");
        uint32_t idx;
        std::uniform_int_distribution<uint32_t> dis(0, n - 1);
        constexpr uint32_t T = 1;
        for (int _ = 0; _ < T; _++)
            for (uint32_t i = 0; i < n; i++)
            {
                // idx = dis(gen);
                idx = i;
                assert(oram[idx] == data[idx]);
                // max_mem_usage = std::max(max_mem_usage, getMemoryUsage());
            }
        std::cout << "total: " << n * sizeof(oram[0]) / 1024 << " KB, memory usage: " << getMemoryUsage() << " KB" << ", ratio: " << 1.0 * getMemoryUsage() / n / sizeof(data[0]) * 1024 << std::endl;
        // ProfilerStop();
        auto _ = t.get_interval_time();
        std::cout << "ObliviousRAM: " << _ << " s" << ", per op: " << _ / T / n * 1000000 << " us" << std::endl;
    }
}

void benchmark_ohashtable2()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 5;
    constexpr int B = 64;
    uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    std::cout << "n: " << n << std::endl;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        Timer t;
        ORAM::OTwoTierHash<uint32_t, B> oht(n, EPSILON_INV);
        std::cout << "Init: " << t.get_interval_time() << " s" << std::endl;
        oht.build(data.data());
        assert(oht[-1].dummy() == true);
        assert(oht[-2].dummy() == true);
        for (uint32_t i = 0; i < n; i++)
        {
            auto _ = oht[i];
            if (_.id != i)
            {
                std::cout << "i: " << i << ", oht[i].id: " << _.id << std::endl;
                assert(false);
            }
        }
        auto _ = t.get_interval_time();
        std::cout << "OTwoTierHash: " << _ << " s" << ", per op: " << _ / n * 1000000 << " us" << std::endl;
        assert(oht[-1].dummy() == true);
        assert(oht[-2].dummy() == true);
        for (uint32_t i = 0; i < n; i++)
            assert(oht[i].dummy() == true);

        // create a std::set
        std::map<int, ORAM::Block<uint32_t, B>> s;
        for (uint32_t i = 0; i < n; i++)
            s.insert({i, data[i]});
        t.get_interval_time();
        for (uint32_t i = 0; i < n; i++)
            assert(s.find(i)->second == data[i]);
        _ = t.get_interval_time();
        std::cout << "Map: " << _ << " s" << ", per op: " << _ / n * 1000000 << " us" << std::endl;

        std::sort(data.begin(), data.end(), [](auto &a, auto &b)
                  { return a.id < b.id; });
        t.get_interval_time();
        ORAM::Block<uint32_t, B> dummy;
        for (uint32_t i = 0; i < n; i++)
            data[i] = oblivious_select(data[i], dummy, data[i].id == i);
        _ = t.get_interval_time();
        std::cout << "PlainBin: " << _ << "s" << ", per op: " << _ / n * 1000000 << " us" << std::endl;
    }
}
void benchmark_ohashtable()
{
    // benchmark_ohahsbin();
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 10;
    uint32_t n = MAJOR_BIN_SIZE * 2;
    constexpr int B = 512;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        Timer t;
        ORAM::OTwoTierHash<uint32_t, B> oht(n, EPSILON_INV);
        std::cout << "Init: " << t.get_interval_time() << " s" << std::endl;
        oht.build(data.data());
        for (uint32_t i = 0; i < n; i++)
        {
            auto _ = oht[i];
            if (_.id != i)
            {
                std::cout << "i: " << i << ", oht[i].id: " << _.id << std::endl;
                assert(false);
            }
        }
        std::cout << "OTwoTierHash: " << t.get_interval_time() << " s" << std::endl;
        assert(oht[-1].dummy() == true);
        assert(oht[-2].dummy() == true);
        for (uint32_t i = 0; i < n; i++)
            assert(oht[i].dummy() == true);
    }
}

void benchmark_ohahsbin()
{
    uint32_t n = MAJOR_BIN_SIZE * 2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis_data(1, 100000);
    int test_cases = 10;
    double avg_time = 0;
    while (test_cases--)
    {
        constexpr int B = 512;
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        Timer t;
        ORAM::ObliviousBin<uint32_t, B> obin(n, n);
        obin.build(data.data());
        // for (uint32_t i = 0; i < n; i++)
        //     assert(obin[i].id == i);
        auto tmp = t.get_interval_time();
        avg_time += tmp;
        // std::cout << "ObliviousBin: " << tmp << "s" << std::endl;
        std::sort(data.begin(), data.end(), [](auto &a, auto &b)
                  { return a.id < b.id; });
        t.get_interval_time();
        for (uint32_t i = 0; i < n; i++)
            assert(data[i].id == i);
        // std::cout << "PlainBin: " << t.get_interval_time() << "s" << std::endl;
    }
    std::cout << "Average time: " << avg_time / 10 << "s" << std::endl;
}

void benchmark_osorter()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    // for (uint32_t n = 4; n <= (1 << 6); n <<= 1)
    uint32_t n = 1 << 20;
    {
        std::vector<uint32_t> P(n);
        std::iota(P.begin(), P.end(), 0);
        std::shuffle(P.begin(), P.end(), gen);
        Timer t;
        // auto C = ORAM::control_bits(P.data(), n);
        std::vector<ORAM::bit> C(n * std::bitset<64>(n - 1).count());
        for (auto b : C)
            b = std::rand() & 1;
        std::cout << "control_bits: " << t.get_interval_time() << "s" << std::endl;
        P.clear();
        t.get_interval_time();
        // output C
        std::vector<ORAM::Block<uint32_t, 32>> data(n);
#pragma omp parallel for
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        t.get_interval_time();

        ORAM::apply_perm(C, data.data(), n);
        std::cout << "apply_perm: " << t.get_interval_time() << "s" << std::endl;

        ORAM::OTwoTierHash<uint32_t, 32> oht(n, 16);
        std::cout << "sorting: " << t.get_interval_time() << "s" << std::endl;
        oht.build(data.data());

        // std::sort(std::execution::par_unseq,
        //           data.begin(), data.end(), [](auto &a, auto &b)
        //           { return a.id < b.id; });
        std::cout << "sorting: " << t.get_interval_time() << "s" << std::endl;
        // for (auto &x : data)
        //     std::cout << x.id << " ";
        // std::cout << std::endl;
    }
    // benchmark_ocompact();
}
int benchmark_rotate()
{
#ifndef _OMP_H
    std::cout << "OpenMP is not supported!" << std::endl;
#endif
    std::mt19937 gen(std::random_device{}());
    size_t n = 1ll << 23;
    constexpr size_t B = 1024;
    using NumTp = uint32_t;
    const NumTp b = n / OCOMPACT_Z;
    std::uniform_int_distribution<NumTp> dist(0, b - 1);
    std::vector<uint8_t> flags(n);
    std::vector<ORAM::Block<int, B>> data(n);
    auto data_first = data.begin();
    auto flag_first = flags.begin();
    double t1;
    {
        Timer t;
        ORAM::IteratorStride<ORAM::Block<int, B>> data_stride_iter(&*data_first, b);
        if (b >= 32768 / sizeof(*data_first))
        {
            std::for_each(std::execution::par,
                          data_stride_iter,
                          data_stride_iter + OCOMPACT_Z,
                          [&gen, b,
                           data_first,
                           flag_first](auto &x)
                          {
                              std::uniform_int_distribution<NumTp> dist(0, b - 1);
                              NumTp r = dist(gen);
                              auto data_st_addr = &x;
                              size_t offset = data_st_addr - &(*data_first);
                              auto flag_st_addr = flag_first + offset;
                              std::rotate(data_st_addr,
                                          data_st_addr + r,
                                          data_st_addr + b);
                              std::rotate(flag_st_addr,
                                          flag_st_addr + r,
                                          flag_st_addr + b);
                          });
        }
        else
        {
            std::for_each(data_stride_iter,
                          data_stride_iter + OCOMPACT_Z,
                          [&dist, &gen, b,
                           data_first,
                           flag_first](auto &x)
                          {
                              NumTp r = dist(gen);
                              auto data_st_addr = &x;
                              size_t offset = data_st_addr - &(*data_first);
                              auto flag_st_addr = flag_first + offset;
                              std::rotate(data_st_addr,
                                          data_st_addr + r,
                                          data_st_addr + b);
                              std::rotate(flag_st_addr,
                                          flag_st_addr + r,
                                          flag_st_addr + b);
                          });
        }
        t1 = t.get_total_time();
        std::cout << "tbbparallel rotating " << b << " groups takes " << t1 << "s" << std::endl;
    }
    {
        Timer t;
#pragma omp parallel for if (b >= 32768 / sizeof(*data_first))
        for (NumTp i = 0; i < OCOMPACT_Z; i++)
        {
            NumTp start = i * b;
            NumTp end = (i + 1) * b;
            NumTp r = dist(gen);
            std::rotate(data_first + start,
                        data_first + start + r,
                        data_first + end);
            std::rotate(flag_first + start,
                        flag_first + start + r,
                        flag_first + b);
        }
        t1 = t.get_total_time();
        std::cout << "ompparallel rotating " << b << " groups takes " << t1 << "s" << std::endl;
    }

    {
        Timer t;
        for (NumTp i = 0; i < OCOMPACT_Z; i++)
        {
            NumTp start = i * b;
            NumTp end = (i + 1) * b;
            NumTp r = dist(gen);
            std::rotate(data_first + start,
                        data_first + start + r,
                        data_first + end);
            std::rotate(flag_first + start,
                        flag_first + start + r,
                        flag_first + b);
        }
        t1 = t.get_total_time();
        std::cout << "searialized rotating " << b << " groups takes " << t1 << "s" << std::endl;
    }

    return 0;
}

int benchmark_ocompact()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    using sum_type = uint32_t;
    // constexpr size_t n = 1 << 29;
    // constexpr size_t B = 1 << 4;
    std::cout << "# threads: " << omp_get_max_threads() << std::endl;
    constexpr size_t n = 1 << 16;
    constexpr size_t B = 1 << 11;
    {
        std::uniform_int_distribution<int> dis_data(0, n);
        std::uniform_int_distribution<int> dis_flag(0, 1);
        // int m = dis_data(gen);
        int m = n / 2;
        std::cout << "Generating data..." << std::endl;
        std::vector<ORAM::Block<int, B>> data(n);
        std::vector<uint8_t> flags(n);
#pragma omp parallel for
        for (int i = 0; i < n; i++)
            data[i].id = i;
#pragma omp parallel for
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        double t1, t2, t3;
        std::cout << "Data generated!" << std::endl;
        {
            auto data_copy = data;
            auto flags_copy = flags;
            {
                Timer t;
                ORAM::compact(data_copy.begin(), data_copy.end(), flags_copy.begin());
                t1 = t.get_total_time();
            }
            std::cout << "optimal compact: " << t1 << "s" << std::endl;
        }
        // {
        //     auto data_copy = data;
        //     auto flags_copy = flags;
        //     {
        //         Timer t;
        //         ORAM::compact_by_sort(data_copy.begin(), data_copy.end(), flags_copy.begin());
        //         t2 = t.get_total_time();
        //     }
        //     std::cout << "compact_by_sort: " << t2 << "s" << std::endl;
        // }
        {
            auto data_copy = data;
            auto flags_copy = flags;
            {
                Timer t;
                ORAM::or_compact_power_2(data_copy.begin(), flags_copy.begin(), n);
                t3 = t.get_total_time();
            }
            std::cout << "or_compact_power_2: " << t3 << "s" << std::endl;
        }
        {
            auto data_copy = data;
            auto flags_copy = flags;
            {
                Timer t;
                ORAM::ocompact_by_half(data_copy.begin(), flags_copy.begin(), n,
                                       OCOMPACT_Z);
                t3 = t.get_total_time();
            }
            std::cout << "ocompact_by_half: " << t3 << "s" << std::endl;
        }
        {
            auto data_copy = data;
            auto flags_copy = flags;
            {
                Timer t;
                ORAM::ocompact_by_half(data_copy.begin(), flags_copy.begin(), n,
                                       OCOMPACT_Z);
                t3 = t.get_total_time();
            }
            std::cout << "ocompact_by_half: " << t3 << "s" << std::endl;
        }
        {
            auto data_copy = data;
            auto flags_copy = flags;
            {
                Timer t;
                ORAM::relaxed_ocompact(data_copy.begin(), flags_copy.begin(), n,
                                       (size_t)OCOMPACT_Z);
                t3 = t.get_total_time();
            }
            std::cout << "relaxed_ocompact: " << t3 << "s" << std::endl;
        }
    }
    return 0;
}

static void garbage1()
{

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis_data(0, 1);
    size_t left = 1, right = 100000000;
    using sum_type = uint32_t;
    // while (left < right)
    for (size_t n = 2 << 10; n < (1ll << 35); n <<= 1)
    {
        // size_t n = (right + left) / 2;
        std::vector<ORAM::Byte> a(n);
        std::cout << "n: " << n << std::endl;
        for (auto &i : a)
            i = (ORAM::Byte)dis_data(gen);
        double t1, t2, t3;
        {
            std::vector<sum_type> offset_partial_sum(a.size());
            Timer t;
            std::transform_inclusive_scan(
                std::execution::seq,
                a.begin(), a.end(),
                offset_partial_sum.begin(),
                std::plus<sum_type>(),
                [](ORAM::Byte x)
                { return x; });
            t1 = t.get_total_time();
            std::cout << "transform w/ seq: \t" << t1 << std::endl;
        }

        std::vector<ORAM::Byte> b(10000000);
        for (auto &i : b)
            i = (ORAM::Byte)dis_data(gen);
        {
            std::vector<sum_type> offset_partial_sum(a.size());
            Timer t;
            std::transform_inclusive_scan(
                std::execution::par_unseq,
                a.begin(), a.end(),
                offset_partial_sum.begin(),
                std::plus<sum_type>(),
                [](ORAM::Byte x)
                { return x; });
            t2 = t.get_total_time();
            std::cout << "transform w/ par_unseq: " << t2 << std::endl;
        }
        if (t2 < t1)
        {
            right = n;
            // break;
        }
        else
        {
            left = n + 1;
        }
    }
}