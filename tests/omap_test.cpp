#include <gtest/gtest.h>
#include "hash_planner.hpp"
#include "omap.hpp"
#include "types.hpp"

TEST(ObliviousMapTest, ObliviousMap2)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 2;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap8)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 8;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap32)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 32;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap128)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 128;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap512)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 512;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap2048)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 2048;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap8192)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 8192;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap32768)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 32768;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap131072)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 131072;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap524288)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 524288;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap2097152)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 2097152;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap8388608)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 8388608;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMap33554432)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<int>::max());
    int n = 33554432;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<uint32_t, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[dis(gen)] = dis(gen);

        ORAM::ObliviousMap<uint32_t, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

static std::string generate_random_string(size_t length)
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string random_string(length, 0);
    for (size_t i = 0; i < length; ++i)
        random_string[i] = characters[dis(gen)];

    return random_string;
}
TEST(ObliviousMapTest, ObliviousMapStr4)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 4;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr16)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 16;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr64)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 64;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr256)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 256;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr1024)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 1024;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr4096)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 4096;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr16384)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 16384;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr65536)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 65536;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr262144)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 262144;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr1048576)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 1048576;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr4194304)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 4194304;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr16777216)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 16777216;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}

TEST(ObliviousMapTest, ObliviousMapStr67108864)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, std::numeric_limits<int>::max());
    uint32_t n = 67108864;
    int test_cases = n <= 65536 ? 10 : 1;
    while (test_cases--)
    {
        std::unordered_map<std::string, int> data;
        for (uint32_t i = 0; i < n; i++)
            data[generate_random_string(16)] = dis(gen);

        ORAM::ObliviousMap<std::string, int> omap;

        for (auto [key, value] : data)
            omap.insert(key, value);

        for (int _ = 0; _ < 2; _++)
            for (auto [key, value] : data)
                EXPECT_EQ(omap[key], value);
    }
}