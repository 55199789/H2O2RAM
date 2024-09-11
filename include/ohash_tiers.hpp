#pragma once
#include <sys/resource.h>
#include <concepts>
#include "ocompact.hpp"
#include "ohash_bin.hpp"
#include "prf.hpp"

#include "timer.hpp"
#include <fstream>

static long getMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

namespace ORAM
{
    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class OTwoTierHash : public OHashBase<KeyType, BlockSize>
    {
    private:
        const KeyType n;
        KeyType dummy_access_ctr;
        bool _empty;

        KeyType bin_size;
        KeyType delta_inv_log2;
        KeyType epsilon_inv;
        const KeyType bin_num;

        AESPRF<uint32_t> prf;
        std::vector<uint32_t> bin_loads;
        std::vector<ObliviousBin<KeyType, BlockSize>> major_bins;
        ObliviousBin<KeyType, BlockSize> overflow_bin;
        std::mt19937 gen;
        std::vector<Block<KeyType, BlockSize>> extracted_data;
        std::vector<uint32_t> sample_secrete_loads(KeyType n_)
        {
            std::vector<uint32_t> secrete_loads(this->bin_num);
            // binomial distribution
            for (KeyType i = 0; i < this->bin_num - 1; i++)
            {
                std::binomial_distribution<KeyType> dist(n_, 1.0 / (this->bin_num - i));
                secrete_loads[i] = dist(this->gen);
                n_ -= secrete_loads[i];
            }
            secrete_loads[this->bin_num - 1] = n_;
            return secrete_loads;
        }

        static inline KeyType compute_bin_size(const KeyType epsilon_inv)
        {
            return epsilon_inv * epsilon_inv * 1024;
        }

    public:
        static std::tuple<KeyType, double> compute_epsilon_inv(
            Block<KeyType, BlockSize> *data,
            const KeyType n,
            const KeyType delta_inv_log2 = DELTA_INV_LOG2)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint32_t> dist(0, n - 1);
            KeyType epsilon_inv = 2;
            auto bin_size = compute_bin_size(epsilon_inv);
            double min_time = std::numeric_limits<double>::max();
            KeyType ret_eps_inv = 2;
            DepthCounter _;
            while (bin_size < n)
            {
                for (KeyType i = 0; i < n; i++)
                    data[i].id = i;
                std::shuffle(data, data + n, gen);
                OTwoTierHash<KeyType, BlockSize> oht(n, delta_inv_log2, epsilon_inv);
                Timer t;
                oht.build(data);
                for (KeyType i = 0; i < n; i++)
                    oht[dist(gen)];
                oht.extract();
                double cur_time = t.get_total_time();
                if (cur_time < min_time)
                {
                    min_time = cur_time;
                    ret_eps_inv = epsilon_inv;
                }
                epsilon_inv <<= 1;
                bin_size = compute_bin_size(epsilon_inv);
                std::cout << _ << "epsilon_inv: " << epsilon_inv << ", bin_size: " << bin_size << ", time: " << cur_time << ", memory usage: " << getMemoryUsage() << " KB" << std::endl;
            }
            return {ret_eps_inv, min_time};
        }

        OTwoTierHash(const KeyType n,
                     // some parameters
                     const KeyType delta_inv_log2 = DELTA_INV_LOG2,
                     const KeyType epsilon_inv = EPSILON_INV)
            : n(n),
              _empty(true),
              bin_size(compute_bin_size(epsilon_inv)),
              delta_inv_log2(delta_inv_log2),
              epsilon_inv(epsilon_inv),
              bin_num(n > bin_size ? KeyType(2) * n / bin_size : KeyType(1)), // public
              prf(bin_num),
              bin_loads(bin_num),
              major_bins(bin_num,
                         ObliviousBin<KeyType,
                                      BlockSize>(bin_num == 1 ? n : bin_size / 2,
                                                 n / bin_num,
                                                 delta_inv_log2)),
              overflow_bin(bin_num > 1 ? n / epsilon_inv : 0,
                           bin_num > 1 ? n : 0,
                           delta_inv_log2),
              gen(std::random_device{}())
        {
            // std::cout << "bin_size: " << bin_size << std::endl
            //           << std::endl;
            assert(std::has_single_bit(n));
        }

        virtual void build(Block<KeyType, BlockSize> *data)
        {
            // Timer t;
            // for small hash tables with only one bin,
            // we can directly build the cuckoo hash table
            dummy_access_ctr = 0;
            _empty = false;
            extracted_data.clear();
            if (this->bin_num == 1)
            {
                bin_loads[0] = n;
                major_bins[0].build(data);
                return;
            }
            std::vector<size_t> shuffle_key(this->n);
            std::generate(shuffle_key.begin(), shuffle_key.end(),
                          [this]()
                          { 
                            static std::uniform_int_distribution<size_t> dis(0, std::numeric_limits<size_t>::max());
                            return dis(this->gen); });
            auto shuffle_key_st = shuffle_key.data();
            osorter(data,
                    n,
                    [data, shuffle_key_st](const auto &a, const auto &b)
                    {bool ret = shuffle_key_st[&a - data] < shuffle_key_st[&b - data];
                    return ret; });
            // std::cout << "building hash table with " << n << " entries" << std::endl;
            this->prf.reset();
            std::uniform_int_distribution<uint32_t> dist(0, this->bin_num - 1);
            std::vector<Block<KeyType, BlockSize>> buffer(KeyType(2) * n);
            // fill bin_loads with 0s
            std::fill(bin_loads.begin(), bin_loads.end(), 0);
            // it's ok to non-obliviously distribute the data,
            // as data is assumed to be obliviously shuffled,
            // see FutORAMa (CCS'23) for further details
            for (KeyType i = 0; i < this->n; i++)
            {
                KeyType bin_id = prf(data[i].id);
                CMOV(data[i].dummy(), bin_id, (KeyType)dist(this->gen));
                // KeyType bin_id = oblivious_select(prf(data[i].id),
                //                                   dist(this->gen),
                //                                   data[i].dummy());
                bin_loads[bin_id]++;
                buffer[bin_id * bin_size + bin_loads[bin_id] - 1] = data[i];
            }
            // sample random secrete loads
            std::vector<KeyType> indices(bin_num);
            std::iota(indices.begin(), indices.end(), 0);
            std::vector<uint32_t> secrete_loads = sample_secrete_loads(n - n / epsilon_inv);
            std::vector<Block<KeyType, BlockSize>> overflow_data(n / epsilon_inv * 2ll);
            std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](KeyType i)
                          {
                KeyType bin_st = bin_size * i;
                KeyType overflow_st = (bin_size / epsilon_inv) * i;
                // assert(secrete_loads[i] >= bin_loads[i] - bin_size / epsilon_inv);
                // assert(bin_loads[i] - secrete_loads[i] <= bin_size / 2);
                auto tmp = bin_st + KeyType(bin_loads[i] - bin_size / epsilon_inv);
                for (KeyType j = tmp; j < bin_st + (KeyType)bin_loads[i]; j++)
                {
                    obliSwap(buffer[j],
                             overflow_data[overflow_st + j - tmp],
                             j >= bin_st + secrete_loads[i]);
                } });
            // build major bins, w/ overwhleming probability, the major bins are less than half
            std::for_each(std::execution::par_unseq, indices.begin(), indices.end(), [&](KeyType i)
                          { major_bins[i].build(buffer.data() + i * bin_size); });
            // build overflow piles
            std::vector<uint8_t> flags(overflow_data.size());
            for (KeyType i = 0; i < overflow_data.size(); i++)
                flags[i] = !overflow_data[i].dummy();
            ocompact_by_half(overflow_data.begin(), flags.begin(),
                             overflow_data.size(),
                             OCOMPACT_Z);
            // only the first half of the overflow data is real
            overflow_data.resize(overflow_data.size() / 2);
            assert(overflow_data.size() == n / epsilon_inv);

            overflow_bin.build(overflow_data.data());
        }

        virtual Block<KeyType, BlockSize> operator[](KeyType key)
        {
            CMOV(key == KeyType(-1), key, --dummy_access_ctr);
            if (bin_num > 1)
            {
                auto ret = overflow_bin[key];
                if (ret.dummy())
                {
                    KeyType bin_id = prf(key);
                    ret = major_bins[bin_id][key];
                }
                else
                {
                    --dummy_access_ctr;
                    KeyType bin_id = prf(dummy_access_ctr);
                    major_bins[bin_id][dummy_access_ctr];
                }
                return ret;
            }
            else
                return major_bins[0][key];
        }

        virtual std::vector<Block<KeyType, BlockSize>> &data()
        {
            // should never be called
            assert(false);
            return extracted_data;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &extract()
        {
            _empty = true;
            if (bin_num == 1)
            {
                extracted_data = major_bins[0].extract();
                return extracted_data;
            }
            // Timer t;
            std::vector<std::pair<KeyType, Block<KeyType, BlockSize>>> tmp;
            Block<KeyType, BlockSize> dummy_block;
            auto &data = overflow_bin.extract();
            tmp.reserve(data.size());
            for (auto &e : data)
                tmp.emplace_back(oblivious_select(prf(e.id),
                                                  bin_num,
                                                  e.dummy()),
                                 e);
            // std::cout << "Time for overflow pile: " << t.get_interval_time() << std::endl;
            KeyType group_size = bin_size / epsilon_inv;
            for (KeyType i = 0; i < bin_num; i++)
                for (KeyType j = 0; j < group_size; j++)
                    tmp.emplace_back(i, dummy_block);
            // std::cout << "Time for constructing tmp: " << t.get_interval_time() << std::endl;
            osorter(tmp.data(), tmp.size(), [](const auto &a, const auto &b)
                    {
                        // if(a.first!=b.first)
                        //         return a.first < b.first; 
                        // if(a.second.dummy()!=b.second.dummy())
                        //     return !a.second.dummy();
                        // return a.second.id < b.second.id; 
                        bool cond1= a.first != b.first;
                        bool ret1 = a.first < b.first;
                        bool cond2 = a.second.dummy() != b.second.dummy();
                        bool ret2 = !a.second.dummy();
                        bool ret3 = a.second.id < b.second.id;
                        return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            // std::cout << "Time for osorting tmp: " << t.get_interval_time() << std::endl;
            KeyType cnt = 1;
            KeyType prev_first = tmp[0].first;
            for (KeyType i = 1; i < tmp.size(); i++)
            {
                ++cnt;
                CMOV(tmp[i].first == prev_first, cnt, KeyType(1));
                // cnt = oblivious_select(KeyType(1),
                //                        cnt + 1,
                //                        tmp[i].first == prev_first);
                prev_first = tmp[i].first;
                CMOV(cnt > group_size, tmp[i].first, bin_num);
            }
            // std::cout << "Time for reassigning bin ids: " << t.get_interval_time() << std::endl;
            osorter(tmp.data(), tmp.size(), [](const auto &a, const auto &b)
                    {
                        bool cond1 = a.first != b.first;
                        bool ret1 = a.first < b.first;
                        bool cond2 = a.second.dummy() != b.second.dummy();
                        bool ret2 = !a.second.dummy();
                        bool ret3 = a.second.id < b.second.id;
                        return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            extracted_data.resize(std::accumulate(bin_loads.begin(), bin_loads.end(), 0));
            // std::cout << "Time for osorting tmp: " << t.get_interval_time() << std::endl;

            std::for_each(std::execution::par_unseq, major_bins.begin(), major_bins.end(), [&](auto &bin)
                          {
                // auto bin_data = bin.data();
                auto bin_data = bin.extract();
                auto tmp_size = bin_data.size();
                bin_data.resize(tmp_size + group_size);
                // Assuming tmp and group_size are accessible here and bin_size is defined
                for (KeyType j = 0; j < group_size; j++)
                    bin_data[j + tmp_size] = tmp[(&bin - &major_bins[0]) * group_size + j].second;

                osorter(bin_data.data(), bin_data.size(),
                        [](const auto &a, const auto &b)
                        {
                            bool cond1 = a.dummy() != b.dummy();
                            bool ret1 = !a.dummy();
                            bool ret2 = a.id < b.id;
                            return (cond1 & ret1) | (!cond1 & ret2);
                        });

                // Assuming bin_loads is appropriately synchronized for access
                std::copy(bin_data.begin(), bin_data.begin() + bin_loads[&bin - &major_bins[0]],
                        extracted_data.begin() + std::accumulate(bin_loads.begin(), bin_loads.begin() + (&bin - &major_bins[0]), 0)); });
            // std::cout << "Time for extracting data: " << t.get_interval_time() << std::endl
            //           << std::endl;
            return extracted_data;
        }

        bool empty() const
        {
            return _empty;
        }

        // only for completeness, actually not used
        bool operator==(const OTwoTierHash &other) const
        {
            if (n != other.n)
                return false;
            if (bin_num != other.bin_num)
                return false;
            if (epsilon_inv != other.epsilon_inv)
                return false;
            if (major_bins.size() != other.major_bins.size())
                return false;
            for (size_t i = 0; i < major_bins.size(); i++)
                if (major_bins[i] != other.major_bins[i])
                    return false;
            return true;
        }

        virtual OHashBase<KeyType, BlockSize> *clone()
        {
            auto ret = new OTwoTierHash<KeyType, BlockSize>(this->n, this->delta_inv_log2, this->epsilon_inv);
            ret->dummy_access_ctr = this->dummy_access_ctr;
            ret->_empty = this->_empty;
            ret->bin_size = this->bin_size;
            ret->delta_inv_log2 = this->delta_inv_log2;
            ret->epsilon_inv = this->epsilon_inv;
            ret->prf = this->prf;
            ret->bin_loads = this->bin_loads;
            ret->major_bins = this->major_bins;
            ret->overflow_bin = this->overflow_bin;
            ret->gen = this->gen;
            ret->extracted_data = this->extracted_data;
            return ret;
        }
    };
}