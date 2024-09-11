#pragma once
#include <limits.h>
#include <unistd.h>
#include <concepts>
#include <fstream>
#include "ocuckoo_hash.hpp"
#include "ohash_bucket.hpp"
#include "ohash_tiers.hpp"
#include "olinear_scan.hpp"
#include "oshuffle.hpp"
#include "timer.hpp"

namespace ORAM
{
    static std::string getExecutablePath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        // get its folder
        for (int i = count; i >= 0; i--)
        {
            if (result[i] == '/')
            {
                result[i + 1] = '\0';
                break;
            }
        }
        return std::string(result);
    }

    template <std::integral KeyType,
              std::size_t BlockSize>
    OHashBase<KeyType, BlockSize> *determine_hash(KeyType n,
                                                  KeyType op_num,
                                                  KeyType delta_inv_log2)
    {
        DepthCounter dp_cnt;
        static std::map<std::tuple<KeyType,
                                   KeyType,
                                   KeyType>,
                        std::tuple<std::string,
                                   KeyType,
                                   KeyType>>
            hash_map = []()
        {
            // {n, op_num, delta} -> hash
            auto folder = getExecutablePath();
            std::map<std::tuple<KeyType,
                                KeyType,
                                KeyType>,
                     std::tuple<std::string,
                                KeyType,
                                KeyType>>
                hash_map;
            std::ifstream file(folder + CONFIG_FILE + std::to_string(BlockSize));
            if (!file.is_open())
            {
                std::cerr << "file " << folder + CONFIG_FILE + std::to_string(BlockSize) << " not found" << std::endl;
                return hash_map;
            }
            KeyType a, b, c;
            while (file >> a)
            {
                file >> b >> c;
                std::string type;
                file >> type;
                // std::cout << a << " " << b << " " << c << " " << type << std::endl;
                if (type == "linear")
                    hash_map[{a, b, c}] = {type, 0, 0};
                else if (type == "bucket")
                {
                    KeyType bucket_num, bucket_size;
                    file >> bucket_num >> bucket_size;
                    hash_map[{a, b, c}] = {type, bucket_num, bucket_size};
                    // new OHashBucket<KeyType, BlockSize>(a, bucket_num, bucket_size);
                }
                else if (type == "cuckoo")
                    hash_map[{a, b, c}] = {type, 0, 0}; // new OCuckooHash<KeyType, BlockSize>(a, c);
                else if (type == "two_tier")
                {
                    KeyType epsilon_inv;
                    file >> epsilon_inv;
                    hash_map[{a, b, c}] = {type, epsilon_inv, 0}; // new OTwoTierHash<KeyType, BlockSize>(a, c, epsilon_inv);
                }
                else
                    std::cerr << "unknown type: " << type << std::endl;
            }
            // std::cout << "hash_map size: " << hash_map.size() << std::endl;
            return hash_map;
        }();
        if (n == 0)
            return nullptr;
        if (hash_map.find({n, op_num, delta_inv_log2}) != hash_map.end())
        {
            auto [type, _, __] = hash_map[{n, op_num, delta_inv_log2}];
            if (type == "linear")
                return new OLinearScan<KeyType, BlockSize>(n);
            if (type == "cuckoo")
                return new OCuckooHash<KeyType, BlockSize>(n, delta_inv_log2);
            if (type == "bucket")
                return new OHashBucket<KeyType, BlockSize>(n,
                                                           _,
                                                           __);
            if (type == "two_tier")
                return new OTwoTierHash<KeyType, BlockSize>(n,
                                                            delta_inv_log2,
                                                            _);
            std::cerr << "unknown type: " << std::get<0>(hash_map[{n, op_num, delta_inv_log2}]) << std::endl;
            return nullptr;
        }
        std::cout << dp_cnt << "n: " << n << ", op_num: " << op_num << std::endl;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<KeyType> dis(0, n - 1);
        OHashBase<KeyType, BlockSize> *hash = nullptr;
        std::vector<Block<KeyType, BlockSize>> data(n);
        for (KeyType i = 0; i < n; i++)
            data[i] = Block<KeyType, BlockSize>(i);
        std::shuffle(data.begin(), data.end(), gen);
        double cuckoo_time = std::numeric_limits<double>::max();
        double linear_time = std::numeric_limits<double>::max();
        double bucket_time = std::numeric_limits<double>::max();
        double two_tier_time = std::numeric_limits<double>::max();
        // ocuckoo_hash
        OCuckooHash<KeyType, BlockSize> cuckoo(n, delta_inv_log2);

        auto folder = getExecutablePath();
        std::ofstream time_file(folder + TIME_FILE + std::to_string(BlockSize), std::ios::app);
        if (!time_file.is_open())
        {
            std::cerr << "file " << folder + TIME_FILE + std::to_string(BlockSize) << " not found" << std::endl;
            exit(-1);
        }
        if (n > 1024 && op_num > n)
        {
            Timer t;
            cuckoo.build(data.data());
            for (KeyType i = 0; i < op_num; i++)
                cuckoo[dis(gen)];
            cuckoo.extract();
            cuckoo_time = t.get_total_time();
            time_file << dp_cnt << "n: " << n << " op_num: " << op_num << " cuckoo_time: " << cuckoo_time << std::endl;
        }
        // obucket
        KeyType bucket_size, bucket_num;
        std::tie(bucket_time,
                 bucket_size,
                 bucket_num) = OHashBucket<KeyType,
                                           BlockSize>::compute_appropriate_bucket_num(data.data(),
                                                                                      n,
                                                                                      op_num,
                                                                                      delta_inv_log2);
        time_file << dp_cnt << "n: " << n << " op_num: " << op_num << " bucket_time: " << bucket_time << std::endl;
        // olinear_scan
        if (n < (1 << 16))
        {
            for (KeyType i = 0; i < n; i++)
                data[i].id = i;
            std::shuffle(data.begin(), data.end(), gen);

            OLinearScan<KeyType, BlockSize> linear(n);
            Timer t;
            linear.build(data.data());
            for (KeyType i = 0; i < op_num; i++)
                linear[dis(gen)];
            linear.extract();
            linear_time = t.get_total_time();
            time_file << dp_cnt << "n: " << n << " op_num: " << op_num << " linear_time: " << linear_time << std::endl;
        }

        KeyType epsilon_inv;
        // if (dp_cnt.get_depth() == 1)
        {
            std::tie(epsilon_inv,
                     two_tier_time) = OTwoTierHash<KeyType,
                                                   BlockSize>::compute_epsilon_inv(data.data(),
                                                                                   n,
                                                                                   delta_inv_log2);
            time_file << dp_cnt << "n: " << n << " op_num: " << op_num << " two_tier_time: " << two_tier_time << ", epsilon: " << epsilon_inv << std::endl;
        }

        // choose the minimum one
        folder = getExecutablePath();
        std::ofstream file(folder + CONFIG_FILE + std::to_string(BlockSize), std::ios::app);
        file << n << " " << op_num << " " << delta_inv_log2;
        auto min_time = std::min({cuckoo_time, linear_time, bucket_time, two_tier_time});
        std::string type;
        KeyType _ = 0, __ = 0;
        if (min_time == cuckoo_time)
        {
            type = "cuckoo";
            hash = new OCuckooHash<KeyType, BlockSize>(n, delta_inv_log2);
            file << " cuckoo" << std::endl;
            std::cout << dp_cnt << "alg: cuckoo_hash" << std::endl;
        }
        else if (min_time == linear_time)
        {
            type = "linear";
            hash = new OLinearScan<KeyType, BlockSize>(n);
            file << " linear" << std::endl;
            std::cout << dp_cnt << "alg: linear_scan" << std::endl;
        }
        else if (min_time == bucket_time)
        {
            type = "bucket";
            _ = bucket_num;
            __ = bucket_size;
            hash = new OHashBucket<KeyType, BlockSize>(n, bucket_num, bucket_size);
            file << " bucket " << bucket_num << " " << bucket_size << std::endl;
            std::cout << dp_cnt << "alg: hash_bucket w/ bucket_num: " << bucket_num << ", bucket_size: " << bucket_size << std::endl;
        }
        else
        {
            type = "two_tier";
            _ = epsilon_inv;
            hash = new OTwoTierHash<KeyType, BlockSize>(n, delta_inv_log2, epsilon_inv);
            file << " two_tier " << epsilon_inv << std::endl;
            std::cout << dp_cnt << "alg: oblivious two_tier w/ epsilon: " << epsilon_inv << std::endl;
        }
        file.close();
        hash_map[{n, op_num, delta_inv_log2}] = std::make_tuple(type, _, __);
        return hash;
    }
}