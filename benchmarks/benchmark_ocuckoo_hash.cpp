#include <benchmark/benchmark.h>
#include <vector>
#include "hash_planner.hpp"
#include "ohash_bucket.hpp"
#include "ocuckoo_hash.hpp"
class OHashCuckoo256 : public benchmark::Fixture
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

BENCHMARK_DEFINE_F(OHashCuckoo256, OCuckooHash)
(benchmark::State &state)
{
    for (auto _ : state)
    {
        ORAM::OCuckooHash<size_t, 256> obucket(n);
        obucket.build(raw_data.data());
        for (size_t i = 0; i < 128 * n; i++)
            obucket[i];
        obucket.extract();
    }
}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{
    size_t step = 1;
    for (size_t n = 3; n <= 18; n++)
    {
        size_t N = 1 << n;
        b->Args({(int64_t)N});
    }
}

BENCHMARK_REGISTER_F(OHashCuckoo256, OCuckooHash)->Apply(CustomizedArgsN)->UseRealTime();
