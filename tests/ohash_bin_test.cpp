#include <gtest/gtest.h>
#include "hash_planner.hpp"
#include "ohash_tiers.hpp"
#include "types.hpp"

TEST(ObliviousBinTest, ObliviousBinLinearScan)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 100;
    uint32_t n = LINEAR_SCAN_THRESHOLD / 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousBin<uint32_t, 4 * 1024> obin(n, n);
        obin.build(data.data());
        auto &entries = obin.data();
        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(entries[i].id, data[i].id);
        EXPECT_EQ(obin[-1].dummy(), true);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].id, i);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].dummy(), true);
    }
}

TEST(ObliviousBinTest, ObliviousBinSmall)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 100;
    uint32_t n = LINEAR_SCAN_THRESHOLD;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousBin<uint32_t, 4 * 1024> obin(n, n);
        obin.build(data.data());
        auto &entries = obin.data();
        for (uint32_t i = 0; i < n; i++)
        {
            uint32_t cnt = 0;
            for (auto &entry : entries)
                cnt += entry.id == data[i].id;
            EXPECT_EQ(cnt, 1);
        }
        EXPECT_EQ(obin[-1].dummy(), true);
        EXPECT_EQ(obin[-2].dummy(), true);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].id, i);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].dummy(), true);
    }
    test_cases = 100;
    n = LINEAR_SCAN_THRESHOLD * 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousBin<uint32_t, 4 * 1024> obin(n, n);
        obin.build(data.data());
        auto &entries = obin.data();
        for (uint32_t i = 0; i < n; i++)
        {
            uint32_t cnt = 0;
            for (auto &entry : entries)
                cnt += entry.id == data[i].id;
            EXPECT_EQ(cnt, 1);
        }
        EXPECT_EQ(obin[-1].dummy(), true);
        EXPECT_EQ(obin[-2].dummy(), true);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].id, i);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].dummy(), true);
    }
}

TEST(ObliviousBinTest, ObliviousBinLarge)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 10;
    uint32_t n = MAJOR_BIN_SIZE / 2;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 64>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousBin<uint32_t, 64> obin(n, n);
        obin.build(data.data());
        auto &entries = obin.data();
        // for (uint32_t i = 0; i < 100; i++)
        // {
        //     uint32_t cnt = 0;
        //     for (auto &entry : entries)
        //         cnt += entry.id == data[i].id;
        //     EXPECT_EQ(cnt, 1);
        // }
        EXPECT_EQ(obin[-1].dummy(), true);
        EXPECT_EQ(obin[-2].dummy(), true);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].id, i);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].dummy(), true);
    }
    test_cases = 10;
    n = MAJOR_BIN_SIZE;
    while (test_cases--)
    {
        std::vector<ORAM::Block<uint32_t, 64>> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i].id = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousBin<uint32_t, 64> obin(n, n);
        obin.build(data.data());
        auto &entries = obin.data();
        // for (uint32_t i = 0; i < 100; i++)
        // {
        //     uint32_t cnt = 0;
        //     for (auto &entry : entries)
        //         cnt += entry.id == data[i].id;
        //     EXPECT_EQ(cnt, 1);
        // }
        EXPECT_EQ(obin[-1].dummy(), true);
        EXPECT_EQ(obin[-2].dummy(), true);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].id, i);

        for (uint32_t i = 0; i < n; i++)
            EXPECT_EQ(obin[i].dummy(), true);
    }
}