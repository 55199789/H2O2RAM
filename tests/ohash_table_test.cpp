#include <gtest/gtest.h>
#include "hash_planner.hpp"
#include "ohash_tiers.hpp"
#include "types.hpp"

TEST(ObliviousHashTableTest, ObliviousHashTableLinearScan)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 50;
    uint32_t n = LINEAR_SCAN_THRESHOLD / 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 512>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, 512> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}

TEST(ObliviousHashTableTest, ObliviousHashTableSmall)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 50;
    uint32_t n = LINEAR_SCAN_THRESHOLD;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 512>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, 512> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
    test_cases = 100;
    n = LINEAR_SCAN_THRESHOLD * 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 512>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, 512> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}

TEST(ObliviousHashTableTest, ObliviousHashTableLarge1)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 5;
    uint32_t n = MAJOR_BIN_SIZE;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 512>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, 512> oht(n);
        // oht.build(data.data());
        // EXPECT_EQ(oht[-1].dummy(), true);
        // EXPECT_EQ(oht[-2].dummy(), true);
        // for (uint32_t i = 0; i < n; i++)
        //     EXPECT_EQ(oht[i].id, i);
        // EXPECT_EQ(oht[-1].dummy(), true);
        // EXPECT_EQ(oht[-2].dummy(), true);
        // for (uint32_t i = 0; i < n; i++)
        //     EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}

TEST(ObliviousHashTableTest, ObliviousHashTableLarge2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 5;
    uint32_t n = MAJOR_BIN_SIZE * 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 512>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, 512> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}

TEST(ObliviousHashTableTest, ObliviousHashTableHuge1)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 1;
    constexpr int B = 64;
    uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, B> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}

TEST(ObliviousHashTableTest, ObliviousHashTableHuge2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 1;
    constexpr int B = 64;
    uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, B>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::OTwoTierHash<uint32_t, B> oht(n);
        oht.build(data.data());
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].id, i);
        EXPECT_EQ(oht[-1].dummy(), true);
        EXPECT_EQ(oht[-2].dummy(), true);
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(oht[i].dummy(), true);

        // test extract()
        oht.build(data.data());
        auto ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(ret[i].id, i);

        oht.build(data.data());
        std::vector<uint32_t> idx(n);
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), gen);
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(oht[idx[i]].id, idx[i]);
        ret = oht.extract();
        std::sort(ret.begin(), ret.end(),
                  [](const auto &a, const auto &b)
                  { return a.id < b.id; });
        std::sort(idx.begin() + n / 2, idx.end());
        for (uint32_t i = 0; i < n / 2; i++)
            EXPECT_EQ(ret[i].id, idx[i + n / 2]);
    }
}