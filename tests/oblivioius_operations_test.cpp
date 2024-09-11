#include <random>
#include <vector>
#include <gtest/gtest.h>
#include "oblivious_operations.hpp"

TEST(OOperationTest, ObliviouSelectInt)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis_data(1, 10000);
    std::uniform_int_distribution<uint32_t> dis_flag(0, 1);
    int test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        uint32_t x = dis_data(gen);
        uint32_t y = dis_data(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
}

TEST(OOperationTest, ObliviouSelectBool)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis_data(0, 1);
    std::uniform_int_distribution<uint8_t> dis_flag(0, 1);
    int test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        bool x = dis_data(gen);
        bool y = dis_data(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
}

TEST(OOperationTest, ObliviouSelectBlock4)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis_data(1, 10000);
    std::uniform_int_distribution<uint32_t> dis_flag(0, 1);
    int test_cases = 500;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<uint32_t> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t> y;
        y.id = dis_data(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
}

TEST(OOperationTest, ObliviouSelectBlocks)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis_data(1, 10000);
    std::uniform_int_distribution<uint8_t> dis_val(0, std::numeric_limits<uint8_t>::max());
    std::uniform_int_distribution<uint8_t> dis_flag(0, 1);
    int test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 8;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
    test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 64;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
    test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 128;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
    test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 1024;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
    test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 8192;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
    test_cases = 100;
    while (test_cases--)
    {
        bool flag = dis_flag(gen);
        constexpr int B = 65536;
        ORAM::Block<uint32_t, B> x;
        x.id = dis_data(gen);
        ORAM::Block<uint32_t, B> y;
        y.id = dis_data(gen);
        EXPECT_EQ(sizeof(y), B);
        for (auto &i : x.value)
            i = dis_val(gen);
        for (auto &i : y.value)
            i = dis_val(gen);
        auto ret = ORAM::oblivious_select(x, y, flag);
        if (flag)
            EXPECT_EQ(ret, y);
        else
            EXPECT_EQ(ret, x);
    }
}

TEST(OOperationTest, ObliviouSwapBlockInt)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<int> left, right;
        left.id = dis_data(gen);
        right.id = dis_data(gen);
        auto _left = left;
        auto _right = right;
        ORAM::oblivious_swap(left, right, flag);
        if (flag)
        {
            EXPECT_EQ(left, _right);
            EXPECT_EQ(right, _left);
        }
        else
        {
            EXPECT_EQ(left, _left);
            EXPECT_EQ(right, _right);
        }
    }
}

TEST(OOperationTest, ObliviouSwapBlockUint64_t)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis_data(1, 10000);
    std::uniform_int_distribution<int> dis_flag(0, 1);
    int test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<uint64_t> left, right;
        left.id = dis_data(gen);
        right.id = dis_data(gen);
        auto _left = left;
        auto _right = right;
        ORAM::oblivious_swap(left, right, flag);
        if (flag)
        {
            EXPECT_EQ(left, _right);
            EXPECT_EQ(right, _left);
        }
        else
        {
            EXPECT_EQ(left, _left);
            EXPECT_EQ(right, _right);
        }
    }
}

TEST(OOperationTest, ObliviouSwapBlockMisc)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis_data(1, std::numeric_limits<size_t>::max());
    std::uniform_int_distribution<int> dis_flag(0, 1);
    std::uniform_int_distribution<uint8_t> dis_val(0, std::numeric_limits<uint8_t>::max());
    int test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<size_t, 17> left, right;
        left.id = dis_data(gen);
        right.id = dis_data(gen);
        for (auto &i : left.value)
            i = dis_val(gen);
        for (auto &i : right.value)
            i = dis_val(gen);
        auto _left = left;
        auto _right = right;
        ORAM::oblivious_swap(left, right, flag);
        if (flag)
        {
            EXPECT_EQ(left, _right);
            EXPECT_EQ(right, _left);
        }
        else
        {
            EXPECT_EQ(left, _left);
            EXPECT_EQ(right, _right);
        }
    }
    test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<size_t, 65536> left, right;
        left.id = dis_data(gen);
        right.id = dis_data(gen);
        for (auto &i : left.value)
            i = dis_val(gen);
        for (auto &i : right.value)
            i = dis_val(gen);
        auto _left = left;
        auto _right = right;
        ORAM::oblivious_swap(left, right, flag);
        if (flag)
        {
            EXPECT_EQ(left, _right);
            EXPECT_EQ(right, _left);
        }
        else
        {
            EXPECT_EQ(left, _left);
            EXPECT_EQ(right, _right);
        }
    }
    test_cases = 100;
    while (test_cases--)
    {
        int flag = dis_flag(gen);
        ORAM::Block<size_t, 65535> left, right;
        left.id = dis_data(gen);
        right.id = dis_data(gen);
        for (auto &i : left.value)
            i = dis_val(gen);
        for (auto &i : right.value)
            i = dis_val(gen);
        auto _left = left;
        auto _right = right;
        ORAM::oblivious_swap(left, right, flag);
        if (flag)
        {
            EXPECT_EQ(left, _right);
            EXPECT_EQ(right, _left);
        }
        else
        {
            EXPECT_EQ(left, _left);
            EXPECT_EQ(right, _right);
        }
    }
}