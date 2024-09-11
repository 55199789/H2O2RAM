#pragma once

#include "ohash_base.hpp"
#include "types.hpp"

namespace ORAM
{
    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class OLinearScan : public OHashBase<KeyType, BlockSize>
    {
    private:
        std::vector<Block<KeyType, BlockSize>> entries;
        KeyType n;

    public:
        OLinearScan(KeyType n) : entries(n), n(n) {}
        virtual void build(Block<KeyType, BlockSize> *data)
        {
            std::copy(data, data + n, this->entries.begin());
        }

        inline virtual Block<KeyType, BlockSize> operator[](KeyType i)
        {
            Block<KeyType, BlockSize> ret;
            for (auto &_ : entries)
            {
                CMOV(_.id == i, ret, _);
                CMOV(_.id == i, _.id, _.id | (KeyType(1) << (8 * sizeof(KeyType) - 1)));
            }
            return ret;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &data()
        {
            return this->entries;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &extract()
        {
            return this->entries;
        }

        virtual OHashBase<KeyType, BlockSize> *clone()
        {
            auto ret = new OLinearScan<KeyType, BlockSize>(this->n);
            ret->entries = this->entries;
            return ret;
        }
    };
}