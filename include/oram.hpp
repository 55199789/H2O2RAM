#pragma once
#include <map>
#include <memory>
#include <vector>
#include "ocompact.hpp"
#include "ohash_tiers.hpp"
#include "oram_iterator.hpp"

namespace ORAM
{
    template <std::unsigned_integral IndexType,
              typename ValueType>
    class ObliviousRAM
    {
    public:
        using value_type = ValueType;
        using reference = ValueType &;
        using const_reference = const ValueType &;
        using size_type = IndexType;
        using difference_type = typename std::make_signed<size_type>::type;
        using iterator = ObliviousRAMIterator<size_type, ValueType>;
        using const_iterator = const ObliviousRAMIterator<size_type, ValueType>;
        using reverse_iterator = ObliviousRAMIteratorReverse<size_type, ValueType>;
        using const_reverse_iterator = const ObliviousRAMIteratorReverse<size_type, ValueType>;

    protected:
        using BlockType = Block<size_type, sizeof(ValueType) + sizeof(size_type)>;

    private:
        size_type _size;
        size_type _capacity;
        iterator _begin;
        iterator _end;
        mutable std::vector<ObliviousBin<size_type, sizeof(ValueType) + sizeof(size_type)>>
            hash_tables;
        // mutable std::vector<OTwoTierHash<size_type, sizeof(ValueType) + sizeof(size_type)>>
        //     hash_tables;
#if !CACHE_OBLIVIOUS
        mutable std::vector<BlockType> _linear_scan_buffer;
#else
        mutable std::map<IndexType, BlockType> _linear_scan_buffer;
#endif
        mutable IndexType _dummy_ctr;
        mutable size_type _buffer_cnt;
        mutable std::mt19937 gen;

        size_type linear_scan_threshold;
        size_type delta_inv_log2;

        std::vector<uint8_t> generate_flags(size_t n) const
        {
            // generate n/2 0 and n/2 1
            std::vector<uint8_t> flags(n);
            size_t x = n / 2;
            size_t y = n / 2;
            for (size_t i = 0; i < n; i++)
            {
                // generate 1 with p = y/(x+y)
                double p = static_cast<double>(y) / (x + y);
                std::bernoulli_distribution dist(p);
                flags[i] = dist(gen);
                y = oblivious_select(y, y - 1, flags[i]);
                x = oblivious_select(x, x - 1, !flags[i]);
            }
            return flags;
        }

        void clear_buffer_if_full() const
        {
            if (_buffer_cnt == linear_scan_threshold)
                [[unlikely]]
            {
                // Timer t;
#if !CACHE_OBLIVIOUS
                std::vector<BlockType> extracted_data(_linear_scan_buffer.begin(),
                                                      _linear_scan_buffer.end());
#else
                assert(_linear_scan_buffer.size() == linear_scan_threshold);
                std::vector<BlockType> extracted_data;
                for (auto &p : _linear_scan_buffer)
                    extracted_data.emplace_back(p.second);
                _linear_scan_buffer.clear();
                _dummy_ctr = 0;
#endif
                uint32_t L = 0;
                for (; L < hash_tables.size() && !hash_tables[L].empty(); L++)
                    ;
                extracted_data.resize(linear_scan_threshold * (1 << L));
                // use std::for_each and std::execution to extract in parallel
                auto tmp_ptr = extracted_data.data();
                std::for_each(std::execution::par_unseq,
                              hash_tables.begin(), hash_tables.begin() + L,
                              [tmp_ptr](auto &table)
                              {
                                  auto &current_data = table.extract();
                                  std::copy(current_data.begin(), current_data.end(),
                                            tmp_ptr + current_data.size());
                              });
                if (L == hash_tables.size())
                {
                    std::vector<uint8_t> flags(extracted_data.size());
                    if (extracted_data.size() > 8192)
                        std::transform(std::execution::par_unseq,
                                       extracted_data.begin(), extracted_data.end(),
                                       flags.begin(),
                                       [](const BlockType &b)
                                       { return !b.dummy(); });
                    else
                        std::transform(std::execution::seq,
                                       extracted_data.begin(), extracted_data.end(),
                                       flags.begin(),
                                       [](const BlockType &b)
                                       { return !b.dummy(); });
                    ocompact_by_half(extracted_data.data(),
                                     flags.data(),
                                     extracted_data.size(),
                                     OCOMPACT_Z);
                    --L;
                }
                hash_tables[L].build(extracted_data.data());
                _buffer_cnt = 0;
            }
        }

    public:
        ObliviousRAM(size_type _size = 0,
                     // default val constructor
                     value_type val = value_type(),
                     // some parameters
                     size_type linear_scan_threshold = LINEAR_SCAN_THRESHOLD,
                     size_type delta_inv_log2 = DELTA_INV_LOG2)
            : _size(_size),
              _begin(this, 0),
              _end(this, _size),
              _dummy_ctr(0),
              _buffer_cnt(0),
              gen(std::random_device{}()),
              linear_scan_threshold(linear_scan_threshold),
              delta_inv_log2(delta_inv_log2)

        {
            hash_tables.reserve(64);
            _capacity = linear_scan_threshold;
            while (_capacity < std::max(_size, (size_type)MIN_CAPACITY))
            {
                hash_tables.emplace_back(_capacity, _capacity, delta_inv_log2);
                // is instance of
                if (hash_tables.back().is_linear_scan())
                {
                    hash_tables.pop_back();
                    this->linear_scan_threshold = _capacity;
                }
                _capacity <<= 1;
            }
            this->linear_scan_threshold <<= 1;
            hash_tables.emplace_back(_capacity, _capacity, delta_inv_log2);
            _linear_scan_buffer.resize(this->linear_scan_threshold);
            if (_size == 0)
                return;
            BlockType *blocks = new BlockType[_capacity];
            if (_capacity - _size > 2048)
                std::for_each(std::execution::par_unseq, blocks, blocks + _capacity,
                              [blocks, &val](BlockType &b)
                              {
                                  b.id = &b - blocks;
                                  std::memcpy(&b.value, &val, sizeof(value_type));
                              });
            else
                std::for_each(blocks, blocks + _capacity,
                              [blocks, &val](BlockType &b)
                              {
                                  b.id = &b - blocks;
                                  std::memcpy(&b.value, &val, sizeof(value_type));
                              });
            hash_tables.back().build(blocks);
            delete[] blocks;
        }

        template <typename _InputIt,
                  typename = std::_RequireInputIter<_InputIt>>
            requires(std::is_same_v<typename std::iterator_traits<_InputIt>::value_type,
                                    ValueType>)
        ObliviousRAM(_InputIt first, _InputIt last,
                     // some parameters
                     size_type linear_scan_threshold = LINEAR_SCAN_THRESHOLD,
                     size_type delta_inv_log2 = DELTA_INV_LOG2)
            : _dummy_ctr(0),
              _buffer_cnt(0),
              gen(std::random_device{}()),
              linear_scan_threshold(linear_scan_threshold),
              delta_inv_log2(delta_inv_log2)
        {
            hash_tables.reserve(64);
            _size = std::distance(first, last);
            _begin = iterator(this, 0);
            _end = iterator(this, _size);
            _capacity = linear_scan_threshold;
            while (_capacity < std::max(_size, (size_type)MIN_CAPACITY))
            {
                // std::cout << "capacity: " << _capacity << ", memory usage: " << getMemoryUsage() << " KB" << std::endl;
                hash_tables.emplace_back(_capacity, _capacity, delta_inv_log2);
                if (hash_tables.back().is_linear_scan())
                {
                    hash_tables.pop_back();
                    this->linear_scan_threshold = _capacity;
                }
                _capacity <<= 1;
            }
            // std::cout << "total: " << _capacity * sizeof(*first) / 1024 << " KB, memory usage: " << getMemoryUsage() << " KB" << ", ratio: " << 1.0 * getMemoryUsage() / _capacity / sizeof(*first) * 1024 << std::endl;
            this->linear_scan_threshold <<= 1;
            hash_tables.emplace_back(_capacity, _capacity, delta_inv_log2);
            _linear_scan_buffer.resize(this->linear_scan_threshold);
            // std::cout << "linear_scan_threshold: " << this->linear_scan_threshold << std::endl;
            BlockType *blocks = new BlockType[_capacity];
            // std::cout << "after new total: " << _capacity * sizeof(*first) / 1024 << " KB, memory usage: " << getMemoryUsage() << " KB" << ", ratio: " << 1.0 * getMemoryUsage() / _capacity / sizeof(*first) * 1024 << std::endl;
            if (_size > 2048)
                std::transform(std::execution::par_unseq, first, last, blocks,
                               [first](const ValueType &v)
                               { return BlockType(v, &v - &*first); });
            else
                std::transform(first, last, blocks,
                               [first](const ValueType &v)
                               { return BlockType(v, &v - &*first); });
            if (_capacity - _size > 2048)
                std::for_each(std::execution::par_unseq, blocks + _size, blocks + _capacity,
                              [blocks](BlockType &b)
                              { b.id = &b - blocks; });
            else
                std::for_each(blocks + _size, blocks + _capacity,
                              [blocks](BlockType &b)
                              { b.id = &b - blocks; });

            hash_tables.back().build(blocks);
            // std::cout << "after build total: " << _capacity * sizeof(*first) / 1024 << " KB, memory usage: " << getMemoryUsage() << " KB" << ", ratio: " << 1.0 * getMemoryUsage() / _capacity / sizeof(*first) * 1024 << std::endl;
            delete[] blocks;
        }

        // implement copy constructor and copy assignment operator
        ObliviousRAM(const ObliviousRAM &other)
            : _size(other._size),
              _capacity(other._capacity),
              _begin(this, 0),
              _end(this, other._size),
              hash_tables(other.hash_tables),
              _linear_scan_buffer(other._linear_scan_buffer),
              _dummy_ctr(other._dummy_ctr),
              _buffer_cnt(other._buffer_cnt),
              gen(std::random_device{}()),
              linear_scan_threshold(other.linear_scan_threshold),
              delta_inv_log2(other.delta_inv_log2)
        {
        }

        /**
         * lookup, core of our design
         * we postponed the rebuilding process to the next inovke
         * to support oram[x] = c; no oram[x] = oram[y] = c!
         **/
        reference operator[](size_type index) const
        {
            clear_buffer_if_full();
            BlockType res;
#if !CACHE_OBLIVIOUS
            // scan the buffer
            for (size_t i = 0; i < _buffer_cnt; i++)
            {
                auto &_ = _linear_scan_buffer[i];
                CMOV(_.id == index, res, _);
                CMOV(_.id == index, _.id, IndexType(-1));
            }
#else
            auto it = _linear_scan_buffer.find(index);
            if (it == _linear_scan_buffer.end())
            {
                res = BlockType();
            }
            else
            {
                auto &_ = it->second;
                res = _;
                _.id = IndexType(-1);
            }
#endif
            for (auto &table : hash_tables)
            {
                // adv knows it
                if (table.empty())
                    continue;
                IndexType cur_idx = index;
                CMOV(!res.dummy(), cur_idx, IndexType(-1));
                // IndexType cur_idx = oblivious_select(index, IndexType(-1), !res.dummy());
                auto cur_res = table[cur_idx];
                CMOV(res.dummy(), res, cur_res);
                // res = oblivious_select(res, cur_res, res.dummy());
            }
            // write back to the buffer
#if !CACHE_OBLIVIOUS
            // assert(!res.dummy());
            // CMOV(res.dummy(), res.id, index);
            _linear_scan_buffer[_buffer_cnt++] = res;
            return *(value_type *)(&(_linear_scan_buffer[_buffer_cnt - 1].value));
#else
            _buffer_cnt++;
            // it = _linear_scan_buffer.find(index);
            if (it != _linear_scan_buffer.end() || index == IndexType(-1))
            {
                it->second = std::move(res);
                _linear_scan_buffer[--_dummy_ctr];
                return *(value_type *)(&it->second.value);
            }
            else
            {
                _linear_scan_buffer[index] = std::move(res);
                return *(value_type *)(&(_linear_scan_buffer[index].value));
            }
#endif
        }

        ObliviousRAM &operator=(const ObliviousRAM &other)
        {
            ObliviousRAM tmp(other);
            swap(tmp);
            return *this;
        }

        // implement move constructor and move assignment operator
        ObliviousRAM(ObliviousRAM &&other) noexcept
            : _size(std::move(other._size)),
              _capacity(std::move(other._capacity)),
              _begin(std::move(other._begin)),
              _end(std::move(other._end)),
              hash_tables(std::move(other.hash_tables)),
              _linear_scan_buffer(std::move(other._linear_scan_buffer)),
              _dummy_ctr(std::move(other._dummy_ctr)),
              _buffer_cnt(std::move(other._buffer_cnt)),
              gen(std::move(other.gen)),
              linear_scan_threshold(std::move(other.linear_scan_threshold)),
              delta_inv_log2(std::move(other.delta_inv_log2))
        {
        }

        iterator push(const ValueType &value)
        {
            return insert(_size, value);
        }

        iterator insert(IndexType index, const ValueType &value)
        {
            if (_size == _capacity)
            {
                _capacity <<= 1;
                hash_tables.emplace_back(_capacity, _capacity, delta_inv_log2);
            }
            clear_buffer_if_full();
            _size++;
            _linear_scan_buffer[_buffer_cnt++] = {value, index};
            _end = iterator(this, _size);
            return iterator(this, index);
        }

        // erase
        void erase(IndexType index)
        {
            clear_buffer_if_full();
            BlockType res;
            // scan the buffer
            for (size_t i = 0; i < _buffer_cnt; i++)
            {
                auto &_ = _linear_scan_buffer[i];
                CMOV(_.id == index, res, _);
                CMOV(_.id == index, _.id, IndexType(-1));
            }
            for (auto &table : hash_tables)
            {
                // adv knows it
                if (table.empty())
                    continue;
                IndexType cur_idx = index;
                CMOV(!res.dummy(), cur_idx, IndexType(-1));
                auto cur_res = table[cur_idx];
                CMOV(res.dummy(), res, cur_res);
            }
        }

        // front
        const_reference front() const
        {
            return operator[](0);
        }

        // pop_back
        void pop_back()
        {
            if (_size == 0)
                return;
            // throw std::out_of_range("pop_back on empty container");
            // if (_size == _capacity >> 1)
            // {
            //     _capacity >>= 1;
            //     hash_tables.pop_back();
            // }
            _size--;
            _end = iterator(this, _size);
        }

        ObliviousRAM &operator=(ObliviousRAM &&other) noexcept
        {
            swap(other);
            return *this;
        }

        iterator begin()
        {
            return _begin;
        }
        iterator end()
        {
            return _end;
        }

        const_iterator cbegin() const
        {
            return _begin;
        }

        const_iterator cend() const
        {
            return _end;
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(_size - 1);
        }

        reverse_iterator rend()
        {
            return reverse_iterator(-1);
        }

        /**
         * insecure implementation for completeness
         * */
        bool operator==(const ObliviousRAM &other) const
        {
            return _size == other._size && _capacity == other._capacity && _buffer_cnt == other._buffer_cnt && _linear_scan_buffer == other._linear_scan_buffer && hash_tables == other.hash_tables;
        }

        /**
         * insecure implementation for completeness
         * */
        bool operator!=(const ObliviousRAM &other) const
        {
            return !(*this == other);
        }

        void swap(ObliviousRAM &other)
        {
            std::swap(_size, other._size);
            std::swap(_capacity, other._capacity);
            std::swap(_begin, other._begin);
            std::swap(_end, other._end);
            std::swap(hash_tables, other.hash_tables);
            std::swap(_linear_scan_buffer, other._linear_scan_buffer);
            std::swap(_dummy_ctr, other._dummy_ctr);
            std::swap(_buffer_cnt, other._buffer_cnt);
            std::swap(gen, other.gen);
            std::swap(linear_scan_threshold, other.linear_scan_threshold);
            std::swap(delta_inv_log2, other.delta_inv_log2);
        }

        size_type maxsize() const
        {
            return std::numeric_limits<size_type>::max();
        }

        size_type size() const
        {
            return _size;
        }

        bool empty() const
        {
            return _size == 0;
        }
    };
}

namespace std
{
    // swap(a, b)
    template <std::unsigned_integral IndexType,
              typename ValueType>
    inline void std::swap(ORAM::ObliviousRAM<IndexType, ValueType> &a,
                          ORAM::ObliviousRAM<IndexType, ValueType> &b)
    {
        a.swap(b);
    }
}