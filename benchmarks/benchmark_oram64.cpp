

#include <benchmark/benchmark.h>
#include <vector>
#include "hash_planner.hpp"
#include "oram.hpp"
#include "types.hpp"

class ORAMDataFixture64 : public benchmark::Fixture
{
public:
    size_t n;
    using IndexType = size_t;
    std::vector<ORAM::Block<IndexType, 64 - sizeof(IndexType)>>
        raw_data;
    std::random_device rd;
    ORAM::ObliviousRAM<size_t, ORAM::Block<IndexType, 64 - sizeof(IndexType)>> *oram;
    void SetUp(const ::benchmark::State &state) override
    {
        std::mt19937 gen(rd());
        n = state.range(0);
        raw_data.resize(n);
        std::uniform_int_distribution<> dist(0, 1);
        for (size_t i = 0; i < n; i++)
                raw_data[i].id = i;
        std::shuffle(raw_data.begin(), raw_data.end(), gen);
        oram = new ORAM::ObliviousRAM<size_t,
                                      ORAM::Block<IndexType,
                                                  64 - sizeof(IndexType)>>(raw_data.begin(),
                                                                           raw_data.end());
    }

    void TearDown(const ::benchmark::State &) override
    {
        raw_data.clear();
        raw_data.shrink_to_fit();
        delete oram;
    }
};

BENCHMARK_DEFINE_F(ORAMDataFixture64, ORAM)
(benchmark::State &state)
{
    for (auto _ : state)
    {
        for (uint32_t i = 0; i < n; i++)
            assert((*oram)[i].id == raw_data[i].id);
    }
}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{
    for (size_t i = 8; i <= 25; i++) // n := 2**i
    {
        size_t n = 1ll << i;
        b->Args({(int64_t)n});
    }
}

BENCHMARK_REGISTER_F(ORAMDataFixture64, ORAM)->Apply(CustomizedArgsN)->MeasureProcessCPUTime()->UseRealTime();
