#include <gtest/gtest.h>
#include <ranges>
#include "types.hpp"

TEST(TypesTest, DefaultSize)
{
    EXPECT_EQ(sizeof(ORAM::Block<int>), sizeof(int));
}

TEST(TypesTest, VariableSize)
{
    EXPECT_EQ(sizeof(ORAM::Block<int, 10>), 12);
    EXPECT_EQ(sizeof(ORAM::Block<int, 17>), 20);
    EXPECT_EQ(sizeof(ORAM::Block<int, 65536>), 65536);
    EXPECT_EQ(sizeof(ORAM::Block<int, 1234>), 1236);
    EXPECT_EQ(sizeof(ORAM::Block<size_t, 17>), 24);
}

TEST(TypesTest, OtherKeyType)
{
    EXPECT_EQ(sizeof(ORAM::Block<size_t>), sizeof(size_t));
    EXPECT_EQ(sizeof(ORAM::Block<uint16_t>), sizeof(uint16_t));
    EXPECT_EQ(sizeof(ORAM::Block<uint32_t>), sizeof(uint32_t));
    EXPECT_EQ(sizeof(ORAM::Block<int64_t>), sizeof(int64_t));
}