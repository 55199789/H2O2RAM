template = """

#include <benchmark/benchmark.h>
#include <vector>
#include "hash_planner.hpp"
#include "oram.hpp"
#include "types.hpp"

class ORAMDataFixture{block_size} : public benchmark::Fixture
{{
public:
    size_t n;
    using IndexType = size_t;
    std::vector<ORAM::Block<IndexType, {block_size} - sizeof(IndexType)>>
        raw_data;
    std::random_device rd;
    ORAM::ObliviousRAM<size_t, ORAM::Block<IndexType, {block_size} - sizeof(IndexType)>> *oram;
    void SetUp(const ::benchmark::State &state) override
    {{
        std::mt19937 gen(rd());
        n = state.range(0);
        raw_data.resize(n);
        std::uniform_int_distribution<> dist(0, 1);
        for (size_t i = 0; i < n; i++)
                raw_data[i].id = i;
        std::shuffle(raw_data.begin(), raw_data.end(), gen);
        oram = new ORAM::ObliviousRAM<size_t,
                                      ORAM::Block<IndexType,
                                                  {block_size} - sizeof(IndexType)>>(raw_data.begin(),
                                                                           raw_data.end());
    }}

    void TearDown(const ::benchmark::State &) override
    {{
        raw_data.clear();
        raw_data.shrink_to_fit();
        delete oram;
    }}
}};

BENCHMARK_DEFINE_F(ORAMDataFixture{block_size}, ORAM)
(benchmark::State &state)
{{
    for (auto _ : state)
    {{
        for (uint32_t i = 0; i < n; i++)
            assert((*oram)[i].id == raw_data[i].id);
    }}
}}

static void CustomizedArgsN(benchmark::internal::Benchmark *b)
{{
    for (size_t i = 8; i <= {cnt_ub}; i++) // n := 2**i
    {{
        size_t n = 1ll << i;
        b->Args({{(int64_t)n}});
    }}
}}

BENCHMARK_REGISTER_F(ORAMDataFixture{block_size}, ORAM)->Apply(CustomizedArgsN)->MeasureProcessCPUTime()->UseRealTime();
"""

import math

def run_benchmarks(block_sizes, template):
    for block_size in block_sizes:
        command = template.format(block_size=block_size, 
                        cnt_ub=31 - int(math.log2(block_size)))
        with open(f"benchmark_oram{block_size}.cpp", "w") as f:
            f.write(command)

block_sizes = [16,32,64,128,256,512,1024,4096,8192,16384,32768,65536]

run_benchmarks(block_sizes, template)