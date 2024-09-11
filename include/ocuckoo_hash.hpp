#pragma once

#include <concepts>
#include "ohash_base.hpp"
#include "obipartite_matching.hpp"
#include "oshuffle.hpp"
#include "prf.hpp"

#include "timer.hpp"
namespace ORAM
{
    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class OCuckooHash : public OHashBase<KeyType, BlockSize>
    {
    private:
        KeyType n;
        KeyType delta_inv_log2;
        uint32_t prf_cnt;
        KeyType bucket_size;
        std::vector<AESPRF<uint32_t>> prfs;
        std::vector<Block<KeyType, BlockSize>> entries;

        void compute_prf_cnt(const KeyType delta_inv_log2)
        {
            // 2^{-64}
            if (delta_inv_log2 <= 64)
            {
                if (n < 8)
                    prf_cnt = 3;
                if (n < 32)
                    prf_cnt = 5;
                else if (n < 1024)
                    prf_cnt = 4;
                else
                    prf_cnt = 3;
            }
            // 2^{-96}
            else if (delta_inv_log2 <= 96)
            {
                if (n < 8)
                    prf_cnt = 3;
                if (n < 32)
                    prf_cnt = 6;
                else if (n < 256)
                    prf_cnt = 5;
                else if (n < 65536)
                    prf_cnt = 4;
                else
                    prf_cnt = 3;
            }
            // 2^{-128}
            else
            {
                if (n < 8)
                    prf_cnt = 3;
                if (n < 16)
                    prf_cnt = 7;
                else if (n < 64)
                    prf_cnt = 6;
                else if (n < 2048)
                    prf_cnt = 5;
                else if (n < 2097152)
                    prf_cnt = 4;
                else
                    prf_cnt = 3;
            }
        }

    public:
        OCuckooHash(KeyType n,
                    KeyType delta_inv_log2 = DELTA_INV_LOG2)
            : n(n), delta_inv_log2(delta_inv_log2)
        {
            compute_prf_cnt(delta_inv_log2);
        }

        virtual void build(Block<KeyType, BlockSize> *data)
        {
            // Timer t;
            // std::cout << "\thash_bin n: " << n << std::endl;
            prfs.clear();
            bucket_size = 2 * n / prf_cnt;
            for (uint32_t i = 0; i < prf_cnt; i++)
                prfs.emplace_back(bucket_size);

            std::vector<BiEdge<KeyType>> edges;
            edges.reserve(n * prf_cnt);
            KeyType dummy_ctr = 0;
            for (KeyType i = 0; i < n; i++)
            {
                for (KeyType j = 0; j < prf_cnt; j++)
                {
                    KeyType dest_id = prfs[j](data[i].dummy() ? --dummy_ctr : data[i].id) + j * bucket_size;
                    edges.emplace_back(i, dest_id);
                }
            }
            // std::cout << "\t\tconstructing edge time: " << t.get_interval_time() << std::endl;
            auto matches = omatcher(edges, n, (KeyType)prf_cnt);
            // std::cout << "\t\tomatching time: " << t.get_interval_time() << std::endl;
            // auto matches = no_match(edges, n, prf_cnt);
            std::vector<std::pair<KeyType, Block<KeyType, BlockSize>>> tmp;
            Block<KeyType, BlockSize> dummy_block;
            tmp.reserve(n * 3ll);
            for (KeyType i = 0; i < n; i++)
                tmp.emplace_back(matches[i], data[i]);
            for (KeyType i = 0; i < 2 * n; i++)
                tmp.emplace_back(i, dummy_block);
            osorter(tmp.data(), 3 * n, [](const auto &a, const auto &b)
                    {
                        // if(a.first!=b.first)
                        //     return a.first < b.first; 
                        // if (a.second.dummy() != b.second.dummy())
                        //     return !a.second.dummy();
                        // return a.second.id < b.second.id; 
                        bool cond1 = a.first != b.first;
                        bool ret1 = a.first < b.first;
                        bool cond2 = a.second.dummy() != b.second.dummy();
                        bool ret2 = !a.second.dummy();
                        bool ret3 = a.second.id < b.second.id; 
                        return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            KeyType n_2 = 2 * n;
            for (KeyType i = 1; i < 3 * n; i++)
                // tmp[i].first = oblivious_select(tmp[i].first,
                //                                 2 * n,
                //                                 tmp[i - 1].first == tmp[i].first);
                CMOV(tmp[i - 1].first == tmp[i].first, tmp[i].first, n_2);
            osorter(tmp.data(), 3 * n, [](const auto &a, const auto &b)
                    {
                        // if(a.first!=b.first)
                        //     return a.first < b.first; 
                        // if (a.second.dummy() != b.second.dummy())
                        //     return !a.second.dummy();
                        // return a.second.id < b.second.id; 
                        bool cond1 = a.first != b.first;
                        bool ret1 = a.first < b.first;
                        bool cond2 = a.second.dummy() != b.second.dummy();
                        bool ret2 = !a.second.dummy();
                        bool ret3 = a.second.id < b.second.id;
                        return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            entries.clear();
            for (KeyType i = 0; i < 2 * n; i++)
                entries.emplace_back(tmp[i].second);
        }

        virtual Block<KeyType, BlockSize> operator[](KeyType i)
        {
            Block<KeyType, BlockSize> ret;
            for (uint32_t j = 0; j < prf_cnt; j++)
            {
                auto &_ = entries[prfs[j](i) + j * bucket_size];
                CMOV(_.id == i, ret, _);
                CMOV(_.id == i, _.id, _.id | (KeyType(1) << (8 * sizeof(KeyType) - 1)));
                // ret = oblivious_select(ret, _, _.id == i);
                // _.id = oblivious_select(_.id,
                //                         _.id | (KeyType(1) << (8 * sizeof(KeyType) - 1)),
                //                         _.id == i);
            }
            return ret;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &data()
        {
            return entries;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &extract()
        {
            // generate flags
            std::vector<uint8_t> flags(entries.size());
            for (KeyType i = 0; i < entries.size(); i++)
                flags[i] = !entries[i].dummy();
            // ocompact
            ocompact_by_half(entries.begin(), flags.begin(),
                             entries.size(),
                             OCOMPACT_Z);
            entries.resize(entries.size() / 2);
            return entries;
        }

        virtual OHashBase<KeyType, BlockSize> *clone()
        {
            auto ret = new OCuckooHash<KeyType, BlockSize>(this->n, this->delta_inv_log2);
            ret->entries = this->entries;
            ret->prfs = this->prfs;
            return ret;
        }
    };
}