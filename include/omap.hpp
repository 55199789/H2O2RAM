#pragma once
#include <string>
#include "oram.hpp"
#include "hash_planner.hpp"

namespace ORAM
{
    template <typename KeyType, typename ValueType>
    class ObliviousMap
    {
    private:
        ORAM::ObliviousRAM<size_t, ValueType> oram;

    public:
        // implement an oblivious & cryptographically secure hash alg is orthogonal to our task
        inline void insert(const KeyType &key, const ValueType &value)
        {
            size_t hash = std::hash<KeyType>{}(key);
            hash &= ~(1UL << (sizeof(size_t) * 8 - 1));
            oram.insert(hash, value);
        }

        inline void erase(const KeyType &key)
        {
            size_t hash = std::hash<KeyType>{}(key);
            hash &= ~(1UL << (sizeof(size_t) * 8 - 1));
            oram.erase(hash);
        }

        inline ValueType &operator[](const KeyType &key)
        {
            size_t hash = std::hash<KeyType>{}(key);
            hash &= ~(1UL << (sizeof(size_t) * 8 - 1));
            return oram[hash];
        }
    };

    template <typename ValueType>
    class ObliviousMap<uint16_t, ValueType>
    {
    private:
        ORAM::ObliviousRAM<uint16_t, ValueType> oram;

    public:
        // implement an oblivious & cryptographically secure hash alg is orthogonal to our task
        inline void insert(const uint16_t key, const ValueType &value)
        {
            oram.insert(key, value);
        }

        inline void erase(const uint16_t key)
        {
            oram.erase(key);
        }

        inline ValueType &operator[](const uint16_t key)
        {
            return oram[key];
        }
    };

    template <typename ValueType>
    class ObliviousMap<uint32_t, ValueType>
    {
    private:
        ORAM::ObliviousRAM<uint32_t, ValueType> oram;

    public:
        // implement an oblivious & cryptographically secure hash alg is orthogonal to our task
        inline void insert(const uint32_t key, const ValueType &value)
        {
            oram.insert(key, value);
        }

        inline void erase(const uint32_t key)
        {
            oram.erase(key);
        }

        inline ValueType &operator[](const uint32_t key)
        {
            return oram[key];
        }
    };

    template <typename ValueType>
    class ObliviousMap<uint64_t, ValueType>
    {
    private:
        ORAM::ObliviousRAM<uint64_t, ValueType> oram;

    public:
        // implement an oblivious & cryptographically secure hash alg is orthogonal to our task
        inline void insert(const uint64_t key, const ValueType &value)
        {
            oram.insert(key, value);
        }

        inline void erase(const uint64_t key)
        {
            oram.erase(key);
        }

        inline ValueType &operator[](const uint64_t key)
        {
            return oram[key];
        }
    };
}