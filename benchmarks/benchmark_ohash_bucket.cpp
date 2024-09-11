#include <benchmark/benchmark.h>
#include <vector>
#include "hash_planner.hpp"
#include "ohash_bucket.hpp"
class OHashBucket256 : public benchmark::Fixture
{
public:
    using IndexType = size_t;
    std::random_device rd;
    std::vector<ORAM::Block<IndexType, 256>> raw_data;
    size_t bucket_num;
    size_t n;
    size_t bucket_size;
    void SetUp(const ::benchmark::State &state) override
    {
        n = state.range(0);
        std::mt19937 gen(rd());
        raw_data.resize(n);
        for (size_t i = 0; i < n; i++)
            raw_data[i].id = i;
        std::shuffle(raw_data.begin(), raw_data.end(), gen);
        auto [t, _l, _m] = ORAM::OHashBucket<size_t, 256>::compute_appropriate_bucket_num(raw_data.data(),
                                                                                          n,
                                                                                          n * 128, 64);
        bucket_num = _m;
        bucket_size = _l;
    }

    void TearDown(const ::benchmark::State &) override
    {
    }
};

BENCHMARK_DEFINE_F(OHashBucket256, OHashBucket)
(benchmark::State &state)
{
    for (auto _ : state)
    {
        ORAM::OHashBucket<size_t, 256> obucket(n, bucket_num, bucket_size);
        obucket.build(raw_data.data());
        for (size_t i = 0; i < 128 * n; i++)
            obucket[i];
        obucket.extract();
    }
}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{
    size_t step = 1;
    for (size_t n = 1; n <= 18; n++)
    {
        size_t N = 1 << n;
        b->Args({(int64_t)N});
    }
}

BENCHMARK_REGISTER_F(OHashBucket256, OHashBucket)->Apply(CustomizedArgsN)->UseRealTime();
