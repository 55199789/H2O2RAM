
#include <benchmark/benchmark.h>
#include <vector>
#include <map>
#include "graphs.hpp"
#include "types.hpp"

class OSSSPDataFixture : public benchmark::Fixture
{
public:
    size_t n;
    size_t m;
    // using target_type = ORAM::Block<uint32_t, 2048>;
    using target_type = uint32_t;
    std::map<std::pair<uint32_t, uint32_t>, int>
        raw_edges;
    std::random_device rd;
    void SetUp(const ::benchmark::State &state) override
    {
        n = state.range(0) / 4;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, n - 1);
        std::uniform_int_distribution<int> dist_w(0, std::numeric_limits<int>::max() / (8 * n));
        m = n * 4;
        for (int i = 0; i < m / 2; i++)
        {
            int u = dist(gen);
            int v = dist(gen);
            if (u == v || raw_edges.find({u, v}) != raw_edges.end())
            {
                i--;
                continue;
            }
            int w = dist_w(gen);
            raw_edges[{u, v}] = w;
            raw_edges[{v, u}] = w;
        }
    }

    void TearDown(const ::benchmark::State &) override
    {
        raw_edges.clear();
    }
};

BENCHMARK_DEFINE_F(OSSSPDataFixture, OSSSP)
(benchmark::State &state)
{
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, n - 1);
    for (auto _ : state)
    {
        auto head = init_head(n);
        std::vector<Edge> _edges(m);
        // auto edges = init_edge(m);
        size_t cnt = 0;
        for (auto &p : raw_edges)
            add_edge(_edges, head, p.first.first, p.first.second, p.second, cnt++);
        auto edges = init_edge(_edges);
        auto root = dist(gen);
        auto dist1 = shortest_path(edges, head, root);
        benchmark::DoNotOptimize(dist1);
    }
}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{
    for (size_t i = 8; i <= 18; i += 2) // n := 2**i
    {
        size_t n = 1ll << i;
        b->Args({(int64_t)n});
    }
}
BENCHMARK_REGISTER_F(OSSSPDataFixture, OSSSP)->Apply(CustomizedArgsN)->MeasureProcessCPUTime()->UseRealTime();
