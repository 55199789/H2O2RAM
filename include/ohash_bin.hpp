#pragma once

#include <concepts>
#include "ohash_base.hpp"
#include "olinear_scan.hpp"
#include "oshuffle.hpp"

#include "timer.hpp"
namespace ORAM
{
    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    OHashBase<KeyType, BlockSize> *determine_hash(KeyType n = 1,
                                                  KeyType op_num = 1,
                                                  KeyType delta_inv_log2 = DELTA_INV_LOG2);

    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class ObliviousBin
    {
    private:
        KeyType n = 0;
        OHashBase<KeyType, BlockSize> *hash = nullptr;
        bool _empty;

    public:
        ObliviousBin(KeyType n = 1, KeyType op_num = 1,
                     KeyType delta_inv_log2 = DELTA_INV_LOG2)
            : n(n),
              hash(determine_hash<KeyType, BlockSize>(n, op_num,
                                                      delta_inv_log2)),
              _empty(true)
        {
            // std::cout << "\tconstructing ObliviousBin with n=" << n << std::endl;
        }

        // copy constructor makes a deep copy
        ObliviousBin(const ObliviousBin &obin) : n(obin.n), _empty(obin._empty)
        {
            // std::cout << "copy constructor" << std::endl;
            if (obin.hash != nullptr)
                hash = obin.hash->clone();
            else
                hash = nullptr;
        }

        // operator= makes a deep copy
        ObliviousBin &operator=(const ObliviousBin &obin)
        {
            // std::cout << "copy operator" << std::endl;
            if (this != &obin)
            {
                n = obin.n;
                delete hash;
                if (obin.hash != nullptr)
                    hash = obin.hash->clone();
                else
                    hash = nullptr;
                _empty = obin._empty;
            }
            return *this;
        }

        ~ObliviousBin()
        {
            delete hash;
        }

        inline void build(Block<KeyType, BlockSize> *data)
        {
            _empty = false;
            hash->build(data);
        }

        inline Block<KeyType, BlockSize> operator[](KeyType i)
        {
            return hash->operator[](i);
        }

        inline std::vector<Block<KeyType, BlockSize>> &data()
        {
            return hash->data();
        }

        inline std::vector<Block<KeyType, BlockSize>> &extract()
        {
            _empty = true;
            return hash->extract();
        }

        inline bool empty() const
        {
            return _empty;
        }

        inline bool is_linear_scan() const
        {
            return dynamic_cast<OLinearScan<KeyType, BlockSize> *>(hash) != nullptr;
        }
    };
}