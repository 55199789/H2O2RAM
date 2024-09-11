#pragma once
#include <stdint.h>
#include <unistd.h>
#include <array>
#include <concepts>
#include <cstring>

#ifndef CACHE_OBLIVIOUS
#define CACHE_OBLIVIOUS 0
#endif

#ifndef PAGE_LEVEL_OBLIVIOUS
#define PAGE_LEVEL_OBLIVIOUS 0
#endif

#define MIN_CAPACITY 65536

#define osorter stateless_osorter
#define CONFIG_FILE "hash_map.bin"
#define TIME_FILE "hash_time.bin"

#define OCOMPACT_Z 512
#define EPSILON_INV 8
#define MAJOR_BIN_SIZE (EPSILON_INV * EPSILON_INV * 1024)
#define OVERFLOW_PILE_BIN_SIZE 512
// #define LINEAR_SCAN_THRESHOLD 256 // large block size
#define LINEAR_SCAN_THRESHOLD 128 // small block size
#define SMALL_HASH_TABLE_THRESHOLD LINEAR_SCAN_THRESHOLD
#define DELTA_INV_LOG2 64

#define PARTIAL_SUM_PARALLEL_THRESHOLD (1 << 21)
namespace ORAM
{
    using Byte = uint8_t;
    const size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);

    template <std::integral KeyType, std::size_t BlockSize = sizeof(KeyType)>
        requires(BlockSize >= sizeof(KeyType) && std::is_standard_layout_v<KeyType>)
    struct Block
    {
        KeyType id;
        Byte value[BlockSize - sizeof(KeyType)];
        Block(KeyType id = -1) : id(id) {}
        bool operator==(const Block<KeyType, BlockSize> &other) const
        {
            bool ret = (id == other.id);
            for (size_t i = 0; i < BlockSize - sizeof(KeyType); i++)
                ret &= (value[i] == other.value[i]);
            return ret;
        }

        // Constructor from any arithmetic type that fits into the value array
        template <typename T>
            requires(sizeof(T) == BlockSize - sizeof(KeyType))
        Block(T input, KeyType id = -1) : id(id)
        {
            std::memcpy(value, &input, sizeof(T)); // Copy new data
        }

        bool dummy() const
        {
            // check if the msb is 1
            return id & (1ll << (sizeof(KeyType) * 8 - 1));
        }

        // Template function to get the value as a basic type
        // template <typename T>
        //     requires(sizeof(T) == sizeof(value))
        // explicit operator T() const
        // {
        //     T output;
        //     std::memcpy(&output, value, sizeof(T));
        //     return output;
        // }

        // Template function to get the value as a basic type
        template <typename T>
            requires(sizeof(T) == BlockSize && std::is_standard_layout_v<T>)
        explicit operator T() const
        {
            T output;
            std::memcpy(&output, this, sizeof(T));
            return output;
        }
    };

    template <typename T>
    concept RandomEngine = requires(T a) {
        typename T::result_type;
        {
            a.min()
        } -> std::same_as<typename T::result_type>;
        {
            a.max()
        } -> std::same_as<typename T::result_type>;
        {
            a()
        } -> std::same_as<typename T::result_type>;
    };

    template <typename T>
    concept reversible_random_access_iterator = std::random_access_iterator<T> &&
                                                requires(T t) {
                                                    t.reverse();
                                                };
} // namespace ORAM