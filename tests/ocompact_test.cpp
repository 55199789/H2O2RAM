#include <iomanip>
#include <random>
#include <vector>
#include <gtest/gtest.h>
#include "ocompact.hpp"
#include "types.hpp"
#include <unordered_set>

TEST(DISABLED_OCompactTest, OptimalCompact)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 100000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 10;
    while (test_cases--)
    {
        int n = dis_data(gen);
        if (n <= 2)
            continue;
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        ORAM::compact(data.begin(), data.end(), flags.begin());
        int cnt = 0;
        for (int i = 0; i < m; i++)
            EXPECT_NE(s.find(data[i].id), s.end());
        for (int i = m; i < n; i++)
            EXPECT_EQ(s.find(data[i].id), s.end());
    }
}

TEST(DISABLED_OCompactTest, SortsDataBasedOnFlag)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 100000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 10;
    while (test_cases--)
    {
        int n = dis_data(gen);
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);

        ORAM::compact_by_sort(data.begin(), data.end(), flags.begin());
        int cnt = 0;
        for (int i = 1; i < n; i++)
            cnt += flags[i] != flags[i - 1];
        EXPECT_EQ(cnt, 1);
    }
}

// constexpr size_t ns[] = {1, 2, 4, 8, 16, 32, 64, 128,
//                          256, 512, 1024, 2048, 4096,
//                          8192, 16384, 32768, 65536, 131072,
//                          262144, 524288, 1048576, 2097152};
TEST(DISABLED_OCompactTest, OCompactPower2_2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    constexpr size_t n = 2;
    while (test_cases--)
    {
        int m = 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_32)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 1000;
    constexpr size_t n = 32;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        // for (auto &i : data)
        //     std::cout << std::setw(4) << i.id << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;
        // std::cout << std::endl;

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        // for (auto &i : data)
        //     std::cout << std::setw(4) << i.id << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_64)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    constexpr size_t n = 64;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        // for (auto &i : data)
        //     std::cout << std::setw(4) << i.id << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;
        // std::cout << std::endl;

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        // for (auto &i : data)
        //     std::cout << std::setw(4) << i.id << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_512)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    constexpr size_t n = 512;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        // std::cout << m << std::endl;
        // for (int i = 0; i < n; i++)
        //     std::cout << std::setw(4) << i << ' ';
        // std::cout << std::endl;
        // for (auto &i : data)
        //     std::cout << std::setw(4) << i.id << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;
        // std::cout << std::endl;

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        // for (int i = 0; i < n; i++)
        //     std::cout << std::setw(4) << i << ' ';
        // std::cout << std::endl;
        // for (auto &i : data)
        //     std::cout << std::setw(4) << (int)flags[i.id] << ' ';
        // std::cout << std::endl;
        // for (auto &i : flags)
        //     std::cout << std::setw(4) << (int)i << ' ';
        // std::cout << std::endl;

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_1024)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    constexpr size_t n = 1024;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_2048)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    constexpr size_t n = 2048;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactPower2_65536)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 65536);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 10;
    constexpr size_t n = 65536;
    while (test_cases--)
    {
        int m = dis_data(gen) % (n - 1) + 1;
        std::vector<ORAM::Block<int, 4 * 1024>> data(n);
        std::vector<uint8_t> flags(n);
        for (int i = 0; i < n; i++)
            data[i].id = i;
        for (int i = 0; i < m; i++)
            flags[i] = 1;
        std::shuffle(flags.begin(), flags.end(), gen);
        std::shuffle(data.begin(), data.end(), gen);
        std::unordered_set<int> s;
        for (int i = 0; i < n; i++)
            if (flags[i])
                s.insert(data[i].id);

        ORAM::or_compact_power_2(data.begin(), flags.begin(), n);

        for (int i = 0; i < m; i++)
        {
            EXPECT_NE(s.find(data[i].id), s.end());
            EXPECT_EQ(flags[i], 1);
        }
        for (int i = m; i < n; i++)
        {
            EXPECT_EQ(s.find(data[i].id), s.end());
            EXPECT_NE(flags[i], 1);
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactByHalf)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 65536);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    for (size_t n = 8192; n <= 8192; n *= 2)
    // for (size_t n = 2; n <= (1 << 29); n *= 2)
    {
        // int test_cases = 18000000;
        int test_cases = 100;
        while (test_cases--)
        {
            int m = n / 2;
            std::vector<ORAM::Block<int, 16>> data(n);
            std::vector<uint8_t> flags(n);
            for (int i = 0; i < n; i++)
                data[i].id = i;
            for (int i = 0; i < m; i++)
                flags[i] = 1;
            std::shuffle(flags.begin(), flags.end(), gen);
            std::shuffle(data.begin(), data.end(), gen);
            std::unordered_set<int> s;
            for (int i = 0; i < n; i++)
                if (flags[i])
                    s.insert(data[i].id);

            ORAM::ocompact_by_half(data.begin(), flags.begin(), n,
                                   OCOMPACT_Z);

            for (int i = 0; i < m; i++)
            {
                EXPECT_NE(s.find(data[i].id), s.end());
                EXPECT_EQ(flags[i], 1);
            }
            for (int i = m; i < n; i++)
            {
                EXPECT_EQ(s.find(data[i].id), s.end());
                EXPECT_NE(flags[i], 1);
            }
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactByHalfExp)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 65536);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    // for (size_t n = 8192; n <= 8192; n *= 2)
    for (size_t n = 2; n <= (1 << 16); n *= 2)
    {
        // int test_cases = 18000000;
        int test_cases = 100;
        while (test_cases--)
        {
            int m = n / 2;
            std::vector<ORAM::Block<int, 16>> data(n);
            std::vector<uint8_t> flags(n);
            for (int i = 0; i < n; i++)
                data[i].id = i;
            for (int i = 0; i < m; i++)
                flags[i] = 1;
            std::shuffle(flags.begin(), flags.end(), gen);
            std::shuffle(data.begin(), data.end(), gen);
            std::unordered_set<int> s;
            for (int i = 0; i < n; i++)
                if (flags[i])
                    s.insert(data[i].id);

            ORAM::ocompact_by_half(data.begin(), flags.begin(), n,
                                   OCOMPACT_Z);

            for (int i = 0; i < m; i++)
            {
                EXPECT_NE(s.find(data[i].id), s.end());
                EXPECT_EQ(flags[i], 1);
            }
            for (int i = m; i < n; i++)
            {
                EXPECT_EQ(s.find(data[i].id), s.end());
                EXPECT_NE(flags[i], 1);
            }
        }
    }
}

TEST(DISABLED_OCompactTest, OCompactTwoCompactedArr)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, (1 << 16));
    std::uniform_int_distribution<int> dis_flag(0, 1);
    for (size_t n = 2; n <= (1 << 16); n *= 2)
        for (size_t bit = 0; (1 << bit) <= n; bit++)
        {
            for (int test_case = 0; test_case < 10; test_case++)
            {
                // std::cout << "n: " << n << " bit: " << bit << std::endl;
                int m1 = dis_data(gen) % (n / 2 + 1);
                int m2 = dis_data(gen) % (n / 2 + 1);
                std::vector<ORAM::Block<int, 16>> data(n);
                std::vector<uint8_t> flags(n);
                for (int i = 0; i < n; i++)
                    data[i].id = i;
                for (int i = 0; i < m1; i++)
                    flags[i] = 1;
                for (int i = 0; i < m2; i++)
                    flags[i + n / 2] = 1;

                ORAM::ocompact_two_compacted_arrays(data.begin(), flags.begin(),
                                                    data.begin() + n / 2, flags.begin() + n / 2,
                                                    n / 2,
                                                    bit);

                for (int i = 0; i < m1 + m2; i++)
                    EXPECT_EQ(flags[i], 1);
                for (int i = m1 + m2; i < n; i++)
                    EXPECT_NE(flags[i], 1);
            }
        }
}

TEST(DISABLED_OCompactTest, RelaxedOCompact)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, (1 << 16));
    std::uniform_int_distribution<int> dis_flag(0, 1);
    for (size_t n = 4; n <= (1 << 10); n *= 2)
        for (size_t Z = 2; Z <= std::min(n, (size_t)4096); Z *= 2)
        {
            for (int test_case = 0; test_case < 100; test_case++)
            {
                int m = dis_data(gen) % (n + 1);
                // std::cout << "n: " << n << ", Z: " << Z << ", m: " << m << std::endl;
                std::vector<ORAM::Block<int, 16>> data(n);
                std::vector<uint8_t> flags(n);
                for (int i = 0; i < n; i++)
                    data[i].id = i;
                for (int i = 0; i < m; i++)
                    flags[i] = 1;
                std::shuffle(flags.begin(), flags.end(), gen);
                std::shuffle(data.begin(), data.end(), gen);
                std::unordered_set<int> s;
                for (int i = 0; i < n; i++)
                    if (flags[i])
                        s.insert(data[i].id);

                ORAM::relaxed_ocompact(data.begin(), flags.begin(),
                                       n,
                                       Z);

                for (int i = 0; i < m; i++)
                {
                    EXPECT_NE(s.find(data[i].id), s.end());
                    EXPECT_EQ(flags[i], 1);
                }
                for (int i = m; i < n; i++)
                {
                    EXPECT_EQ(s.find(data[i].id), s.end());
                    EXPECT_NE(flags[i], 1);
                }
            }
        }
}