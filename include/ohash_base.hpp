#pragma once

#include <stdint.h>
#include <concepts>
#include "oshuffle.hpp"
namespace ORAM
{

    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class OHashBase
    {
    public:
        virtual void build(Block<KeyType, BlockSize> *data) = 0;
        virtual Block<KeyType, BlockSize> operator[](KeyType key) = 0;
        virtual std::vector<Block<KeyType, BlockSize>> &data() = 0;
        virtual std::vector<Block<KeyType, BlockSize>> &extract() = 0;
        virtual OHashBase<KeyType, BlockSize> *clone() = 0;
    };
}