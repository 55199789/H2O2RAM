#include <benchmark/benchmark.h>
#include <vector>
#include "hash_planner.hpp"
#include "ohash_bucket.hpp"
#include "ocuckoo_hash.hpp"
class OLinearScan256 : public benchmark::Fixture
{
public:
    using IndexType = size_t;
    std::random_device rd;
    std::vector<ORAM::Block<IndexType, 256>> raw_data;
    size_t n;
    void SetUp(const ::benchmark::State &state) override
    {
        n = state.range(0);
        std::mt19937 gen(rd());
        raw_data.resize(n);
        for (size_t i = 0; i < n; i++)
            raw_data[i].id = i;
        std::shuffle(raw_data.begin(), raw_data.end(), gen);
    }

    void TearDown(const ::benchmark::State &) override
    {
    }
};

BENCHMARK_DEFINE_F(OLinearScan256, OLinearScan)
(benchmark::State &state)
{
    for (auto _ : state)
    {

        for (size_t i = 0; i < 128 * n; i++)
            for (size_t j = 0; j < n; j++)
                benchmark::DoNotOptimize(raw_data[i].id >= 0);
    }
}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{
    size_t step = 1;
    for (size_t n = 13; n <= 18; n++)
    {
        size_t N = 1 << n;
        b->Args({(int64_t)N});
    }
}

BENCHMARK_REGISTER_F(OLinearScan256, OLinearScan)->Apply(CustomizedArgsN)->UseRealTime();
