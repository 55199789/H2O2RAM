#include <gtest/gtest.h>
#include "hash_planner.hpp"
#include "oram.hpp"
#include "types.hpp"

TEST(ObliviousRAMTest, ObliviousRAMLinearScan)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 100;
    uint32_t n = LINEAR_SCAN_THRESHOLD / 2;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}

TEST(ObliviousRAMTest, ObliviousRAMSmall)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    int test_cases = 10;
    uint32_t n = LINEAR_SCAN_THRESHOLD;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
    test_cases = 10;
    n = LINEAR_SCAN_THRESHOLD * 2;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}

TEST(ObliviousRAMTest, ObliviousRAMLarge1)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 10;
    uint32_t n = MAJOR_BIN_SIZE;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}

TEST(ObliviousRAMTest, ObliviousRAMLarge2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 10;
    uint32_t n = MAJOR_BIN_SIZE * 2;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}

TEST(ObliviousRAMTest, ObliviousRAMHuge1)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 5;
    constexpr int B = 64;
    uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}

TEST(ObliviousRAMTest, ObliviousRAMHuge2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    int test_cases = 5;
    constexpr int B = 64;
    uint32_t n = MAJOR_BIN_SIZE * EPSILON_INV * 4;
    while (test_cases--)
    {
        std::vector<int> data(n);
        std::vector<uint8_t> flags(n);
        for (uint32_t i = 0; i < n; i++)
            data[i] = i;
        std::shuffle(data.begin(), data.end(), gen);
        ORAM::ObliviousRAM<uint32_t, int> oram(data.begin(), data.end());

        for (int _ = 0; _ < 4; _++)
            for (uint32_t i = 0; i < n; i++)
                EXPECT_EQ(oram[i], data[i]);
    }
}