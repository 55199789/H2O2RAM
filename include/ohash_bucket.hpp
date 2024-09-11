#pragma once

#include <math.h>
#include <nlopt.h>
#include <stdint.h>
#include <execution>
#include "ohash_base.hpp"
#include "prf.hpp"
#include "types.hpp"
namespace ORAM
{
    static long double log2_factorial(size_t n)
    {
        static const std::array<long double, 1 << 20> log2_factorial = []()
        {
            std::array<long double, 1 << 20> ret;
            ret[0] = 0;
            for (uint32_t i = 1; i < (1 << 20); ++i)
            {
                ret[i] = ret[i - 1] + log2(i);
            }
            return ret;
        }();
        if (n < 2)
            return 0;
        if (n < log2_factorial.size())
            return log2_factorial[n];
        // n * math.log2(n) - n * math.log2(math.e) + 0.5 * math.log2(2 * math.pi * n)
        // return n * log2(n / exp(1)) + 0.5 * log2(2 * M_PI * n);
        return n * log2(n / exp(1)) + (2 * log2(n) + 2 + log2(1 + 2 * n)) / 6.0 + 0.5 * log2(M_PI);
    }

    static long double log2_nCr(size_t n, size_t r)
    {
        if (r == n)
            return 0;
        if (r > n)
            return -10000;
        return log2_factorial(n) - (log2_factorial(r) + log2_factorial(n - r));
    }

    static long double bin_load(size_t n, size_t m, size_t k)
    {
        if (n == k)
            return std::pow(1.0 / m, k);
        if (k == 0)
            return std::pow(1 - 1.0 / m, n);
        long double ret = log2_nCr(n, k) + (n - k) * std::log2(m - 1) - n * std::log2(m);
        return std::pow(2, ret);
    }

    static long double fail_prob(size_t n, size_t m, size_t k)
    {
        long double st = bin_load(n, m, k);
        long double ret = st;
        for (size_t t = k + 1; t <= n; ++t)
        {
            st *= (n - t + 1.0) / t / (m - 1);
            ret += st;
        }
        return m * ret;
    }

    static size_t compute_bucket_size(size_t n, size_t m, int delta_inv_log2)
    {
        size_t left = 2;
        size_t right = n;
        while (left < right)
        {
            size_t mid = (left + right) / 2;
            if (fail_prob(n, m, mid) <= std::pow(2, -delta_inv_log2))
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }
        return right;
    }

    static double osort_time_complexity(size_t n)
    {
        // get available threads
        return n / 4.0 * log2(n) * (log2(n) + 1);
    }

    // Numerical differentiation for the gradient approximation
    static double numerical_derivative(std::function<double(double)> func, double x, double h = 1)
    {
        return (func(x + h) - func(x - h)) / (2 * h);
    }

    static double objective_function(unsigned _, const double *x, double *grad, void *my_func_data)
    {
        int k = static_cast<int>(std::round(x[0]));
        size_t n = ((size_t *)my_func_data)[0];
        size_t op_num = ((size_t *)my_func_data)[1];
        size_t delta_inv_log2 = ((size_t *)my_func_data)[2];
        int lnk = compute_bucket_size(n, k, delta_inv_log2);

        // Compute the function value
        double result = 2 * osort_time_complexity(k * lnk + n) + 2 * (k + op_num) * lnk + osort_time_complexity(k * lnk);

        // Compute the gradient if required
        if (grad)
        {
            auto func = [&](double k_val) -> double
            {
                int k_int = static_cast<int>(std::round(k_val));
                int lnk_val = compute_bucket_size(n, k_int, delta_inv_log2);
                return 2 * osort_time_complexity(k_int * lnk_val + n) + 2 * (k_int + op_num) * lnk_val + osort_time_complexity(k_int * lnk_val);
            };

            // Numerical differentiation
            grad[0] = numerical_derivative(func, x[0]);
        }

        return result;
    }

    template <std::integral KeyType,
              std::size_t BlockSize = sizeof(KeyType)>
    class OHashBucket : public OHashBase<KeyType, BlockSize>
    {
    private:
        const KeyType n;
        const KeyType bucket_num;
        const KeyType bucket_size;
        AESPRF<uint32_t> prf;
        std::vector<Block<KeyType, BlockSize>> entries;

    public:
        static std::tuple<double, KeyType, KeyType> compute_appropriate_bucket_num(
            Block<KeyType, BlockSize> *data,
            KeyType n,
            KeyType op_num,
            KeyType delta_inv_log2 = DELTA_INV_LOG2)
        {
            KeyType bucket_num_l = 10;
            KeyType bucket_num_r = [n, delta_inv_log2]()
            {
                KeyType left = 10;
                KeyType right = n;
                KeyType div = (size_t(n) * size_t(BlockSize) >> 30) >= 1 ? 2 : 4;
                while (left + 1 < right)
                {
                    KeyType mid = (left + right) / 2;

                    if (compute_bucket_size(n, mid, delta_inv_log2) / div * mid <= n)
                        left = mid;
                    else
                        right = mid - 1;
                }
                return left;
            }();
            KeyType step = std::pow(n, 0.5);
            double lr = 0.01;
            double min_time = std::numeric_limits<double>::max();
            KeyType ret_bucket_size;
            KeyType ret_bucket_num;
            // decrease bucket_num_r until bucket_size * bucket_num_r < 4*n
            // binary search bucket_num_r

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<KeyType> dis(0, n - 1);
            auto compute_time = [&](KeyType bucket_num)
            {
                KeyType bucket_size = compute_bucket_size(n, bucket_num, delta_inv_log2);
                // std::cout << "n: " << n << ", bucket_num: " << bucket_num << ", bucket_size: " << bucket_size << std::endl;
                double total_time = 0;
                constexpr int T = 3;
                for (int _ = 0; _ < T; _++)
                {
                    for (KeyType i = 0; i < n; i++)
                        data[i].id = i;
                    std::shuffle(data, data + n, gen);
                    OHashBucket<KeyType, BlockSize> obucket(n, bucket_num, bucket_size);
                    Timer t;
                    obucket.build(data);
                    for (KeyType i = 0; i < op_num; i++)
                        obucket[dis(gen)];
                    obucket.extract();
                    auto cur_time = t.get_total_time();
                    total_time += cur_time;
                }
                return total_time / T;
            };
            KeyType delta, bucket_x1, bucket_x2;
            double x1_time;
            double x2_time;

            delta = bucket_num_r - bucket_num_l;
            bucket_x1 = bucket_num_l + 0.3819660112501051 * delta;
            bucket_x2 = bucket_num_r - 0.3819660112501051 * delta;
            x1_time = compute_time(bucket_x1);
            x2_time = compute_time(bucket_x2);
            while (bucket_num_l + step < bucket_num_r)
            {
                // std::cout << "\tl: " << bucket_num_l << ", x1: " << bucket_x1 << ", time: " << x1_time
                //           << ", x2: " << bucket_x2 << ", time: " << x2_time << ", r: " << bucket_num_r
                //           << std::endl;
                if (x1_time < x2_time)
                {
                    bucket_num_r = bucket_x2;
                    bucket_x2 = bucket_x1;
                    x2_time = x1_time;
                    delta = bucket_num_r - bucket_num_l;
                    bucket_x1 = bucket_num_l + 0.3819660112501051 * delta;
                    x1_time = compute_time(bucket_x1);
                }
                else
                {
                    bucket_num_l = bucket_x1;
                    bucket_x1 = bucket_x2;
                    x1_time = x2_time;
                    delta = bucket_num_r - bucket_num_l;
                    bucket_x2 = bucket_num_r - 0.3819660112501051 * delta;
                    x2_time = compute_time(bucket_x2);
                }
            }
            // std::cout << "\tl: " << bucket_num_l << ", x1: " << bucket_x1 << ", time: " << x1_time
            //           << ", x2: " << bucket_x2 << ", time: " << x2_time << ", r: " << bucket_num_r
            //           << std::endl
            //           << std::endl;
            ret_bucket_num = (bucket_num_l + bucket_num_r) / 2;
            ret_bucket_size = compute_bucket_size(n, ret_bucket_num, delta_inv_log2);
            min_time = (x1_time + x2_time) / 2;
            // bucket_num_l = ret_bucket_num < 100 ? 2 : ret_bucket_num - 100;
            // bucket_num_r = ret_bucket_num + 100;
            // // std::cout << "\tret_bucket_num: " << ret_bucket_num << " ret_bucket_size: " << ret_bucket_size << ", time: " << min_time << std::endl;
            // for (KeyType bucket_num = bucket_num_l; bucket_num < bucket_num_r; bucket_num++)
            // {
            //     if (bucket_num <= 2)
            //         continue;
            //     KeyType bucket_size = compute_bucket_size(n, bucket_num, delta_inv_log2);
            //     if (bucket_size * bucket_num < n)
            //         continue;
            //     auto cur_time = compute_time(bucket_num);
            //     if (cur_time < min_time)
            //     {
            //         std::cout << "\tbucket_num: " << bucket_num << " time: " << cur_time << std::endl;
            //         min_time = cur_time;
            //         ret_bucket_num = bucket_num;
            //         ret_bucket_size = bucket_size;
            //     }
            // }
            return {min_time, ret_bucket_size, ret_bucket_num};
        }

        OHashBucket(KeyType n,
                    KeyType bucket_num,
                    KeyType bucket_size) : n(n),
                                           bucket_num(bucket_num),
                                           bucket_size(bucket_size),
                                           prf(bucket_num),
                                           entries((size_t)bucket_size * (size_t)bucket_num)
        {
        }

        ~OHashBucket()
        {
            // release the memory manually
            entries.clear();
            entries.shrink_to_fit();
        }

        virtual void build(Block<KeyType, BlockSize> *data)
        {
            prf.reset();
            // allocate n datum to the buckets based on the PRF
            std::vector<std::pair<KeyType, Block<KeyType, BlockSize>>> tmp;
            Block<KeyType, BlockSize> dummy_block;
            KeyType d_idx = bucket_num * bucket_size;
            tmp.reserve(n + d_idx);
            for (KeyType i = 0; i < n; ++i)
                tmp.emplace_back(prf(data[i].id), data[i]);
            for (KeyType i = 0; i < bucket_num; ++i)
                for (KeyType j = 0; j < bucket_size; ++j)
                    tmp.emplace_back(i, dummy_block);
            osorter(tmp.begin(), tmp.size(), [](auto &a, auto &b)
                    {bool cond1 = a.first != b.first;
                    bool ret1 = a.first < b.first;
                    bool cond2 = a.second.dummy() != b.second.dummy();
                    bool ret2 = !a.second.dummy();
                    bool ret3 = a.second.id < b.second.id; 
                    return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            KeyType cnt = 1;
            KeyType prev_first = tmp[0].first;
            for (KeyType i = 1; i < tmp.size(); i++)
            {
                ++cnt;
                CMOV(tmp[i].first != prev_first, cnt, (KeyType)1);
                prev_first = tmp[i].first;
                CMOV(cnt > bucket_size, tmp[i].first, bucket_num);
            }
            osorter(tmp.data(), tmp.size(), [](auto &a, auto &b)
                    {bool cond1 = a.first != b.first;
                    bool ret1 = a.first < b.first;
                    bool cond2 = a.second.dummy() != b.second.dummy();
                    bool ret2 = !a.second.dummy();
                    bool ret3 = a.second.id < b.second.id;
                    return (cond1 & ret1) | (!cond1 & ((cond2 & ret2) | (!cond2 & ret3))); });
            this->entries.resize(d_idx);
            for (KeyType i = 0; i < d_idx; i++)
                this->entries[i] = tmp[i].second;
        }

        virtual Block<KeyType, BlockSize> operator[](KeyType key)
        {
            Block<KeyType, BlockSize> ret;
            KeyType bucket_id = prf(key);
            KeyType st = bucket_id * bucket_size;
            for (KeyType i = 0; i < bucket_size; i++)
            {
                auto &_ = entries[st + i];
                CMOV(_.id == key, ret, _);
                CMOV(_.id == key, _.id, _.id | (KeyType(1) << (8 * sizeof(KeyType) - 1)));
            }
            return ret;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &data()
        {
            return entries;
        }

        virtual std::vector<Block<KeyType, BlockSize>> &extract()
        {
            osorter(entries.begin(), entries.size(), [](auto &a, auto &b)
                    {bool cond1 = a.dummy() != b.dummy();
                    bool ret1 = !a.dummy();
                    bool ret2 = a.id < b.id;
                    return (cond1 & ret1) | (!cond1 & ret2); });
            entries.resize(n);
            return entries;
        }

        virtual OHashBase<KeyType, BlockSize> *clone()
        {
            auto ret = new OHashBucket<KeyType, BlockSize>(this->n, this->bucket_num, this->bucket_size);
            ret->prf = this->prf;
            ret->entries = this->entries;
            return ret;
        }
    };
} // namespace ORAM