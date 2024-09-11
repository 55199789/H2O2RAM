#include <gtest/gtest.h>
#include <ranges>
#include <numeric>
#include "oshuffle.hpp"

TEST(DISABLED_TypesTest, Controbits)
{
    // only test whehter it'll crash
    for (uint32_t n = 1; n <= (1 << 10); n <<= 1)
    {
        std::vector<uint32_t> P(n);
        std::iota(P.begin(), P.end(), 0);
        auto C = ORAM::control_bits(P.data(), n);
    }
}

TEST(DISABLED_TypesTest, ApplyPerm)
{
    // only test whehter it'll crash
    for (uint32_t n = 1; n <= (1 << 10); n <<= 1)
    {
        std::vector<uint32_t> P(n);
        std::iota(P.begin(), P.end(), 0);
        auto C = ORAM::control_bits(P.data(), n);
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        ORAM::apply_perm(C, data.data(), n);

        sort(data.begin(), data.end(), [](auto &a, auto &b)
             { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(data[i].id, i);
    }
}