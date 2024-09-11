#pragma once
#include <assert.h>
#include <omp.h>
#include <algorithm>
#include <bitset>
#include <execution>
#include <iterator>
#include <numeric>
#include <map>
#include <mutex>
#include <random>
#include <tbb/parallel_invoke.h>
#include "oblivious_operations.hpp"
#include "osort.hpp"
#include "prp.hpp"

#include <iostream>
#include <iomanip>
#include "depth_counter.hpp"

namespace ORAM
{
    using bit = bool;
    template <std::unsigned_integral PermType = uint32_t>
    static inline std::vector<std::tuple<std::array<uint8_t,
                                                    AES_BLOCK_SIZE>,
                                         PermType,
                                         PermType>>
    create_forward_lookup(const PermType *perm, PermType n,
                          uint32_t depth,
                          const AESCSPRP &prp)
    {
        PermType k = n >> 1;
        std::vector<std::tuple<std::array<uint8_t,
                                          AES_BLOCK_SIZE>,
                               PermType,
                               PermType>>
            F;
        std::array<uint8_t, AES_BLOCK_SIZE> container;
        F.reserve(n);
        for (PermType i = 0; i < n; i++)
        {
            std::memcpy(container.data(), &i, sizeof(i));
            F.emplace_back(prp(container.data()),
                           i, perm[i] >> depth);
        }
        bitonic_sort(F.data(), 0, n, true);
        return F;
    }

    template <std::unsigned_integral PermType = uint32_t>
    static inline std::map<std::array<uint8_t,
                                      AES_BLOCK_SIZE>,
                           std::pair<PermType, PermType>>
    create_reverse_lookup(const std::vector<std::tuple<
                              std::array<uint8_t,
                                         AES_BLOCK_SIZE>,
                              PermType,
                              PermType>> &F,
                          const AESCSPRP &prp)
    {
        std::map<std::array<uint8_t,
                            AES_BLOCK_SIZE>,
                 std::pair<PermType, PermType>>
            R;
        std::array<uint8_t, AES_BLOCK_SIZE> container;
        for (PermType i = 0; i < F.size(); i++)
        {
            auto &[_, x, y] = F[i];
            std::memcpy(container.data(), &y, sizeof(y));
            R.emplace(std::make_pair(prp(container.data()), std::make_pair(x, i)));
        }
        return R;
    }

    template <std::unsigned_integral PermType = uint32_t>
    static inline void create_unselected_counts(PermType *U, PermType n)
    {
        if (n < 2)
            return;
        PermType k = n >> 1;
        U[k - 1] = k;
        create_unselected_counts(U, k);
        create_unselected_counts(U + k, k);
    }

    template <std::unsigned_integral PermType = uint32_t>
    static inline void dec_unselected_counts(PermType *U, PermType n, PermType l)
    {
        if (n < 2)
            return;
        PermType k = n >> 1;
        if (l < k)
        {
            --U[n - 1];
            if (n > 1)
                dec_unselected_counts(U, k, l);
        }
        else
            dec_unselected_counts(U + k, n - k, l - k);
    }

    template <std::unsigned_integral PermType = uint32_t>
    static inline std::tuple<PermType,
                             PermType,
                             PermType>
    forward_or_rand(const std::vector<std::tuple<
                        std::array<uint8_t,
                                   AES_BLOCK_SIZE>,
                        PermType,
                        PermType>> &F,
                    const std::vector<PermType> &U,
                    PermType f,
                    bool bit,
                    const AESCSPRP &prp)
    {
        PermType n = F.size();
        std::array<uint8_t, AES_BLOCK_SIZE> container;
        std::memcpy(container.data(), &f, sizeof(f));
        auto h = prp(container.data());
        PermType i = 0;
        PermType j = n - 1;
        PermType u = U[n - 1];
        // select a random number from 0 to u-1
        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<PermType> dist(0, u - 1);
        PermType rho = dist(rng);
        PermType l;
        for (;;)
        {
            l = (i + j) / 2;
            if (i == j)
                break;
            auto z = oblivious_select(std::get<0>(F[l]) < h, U[l] <= rho, bit);
            if (z == 0)
            {
                j = l;
                u = U[l];
            }
            else
            {
                i = l + 1;
                u = u - U[l];
                rho = rho - U[l];
            }
        }
        return {std::get<1>(F[l]), std::get<1>(F[l]), l};
    }

    template <std::unsigned_integral PermType = uint32_t>
    std::vector<bit> in_bits(const PermType *perm, PermType n, uint32_t depth)
    {
        // real length is k-1, but we return k bits for alignment
        PermType k = n >> 1;
        std::vector<bit> C(k);
        if (n <= 2)
            return C;
        std::vector<std::pair<PermType, bool>> S;
        std::vector<PermType> U(n);
        AESCSPRP forward_prp;
        AESCSPRP reverse_prp;
        auto F = create_forward_lookup(perm, n, depth, forward_prp);
        auto R = create_reverse_lookup(F, reverse_prp);
        create_unselected_counts(U.data(), n);
        // Perform initial back-and-forth with fixed input k − 1
        PermType f, g, l, s;
        std::tie(f, g, l) = forward_or_rand(F, U, k - 1, 0, forward_prp);
        dec_unselected_counts(U.data(), n, l);
        auto c = f;
        auto r = g + oblivious_select(-k, k, g < k);
        std::array<uint8_t, AES_BLOCK_SIZE> container;
        std::memcpy(container.data(), &r, sizeof(r));
        std::tie(s, l) = R[reverse_prp(container.data())];
        dec_unselected_counts(U.data(), n, l);
        f = s + oblivious_select(-k, k, s < k);
        // Repeat the back-and-forth process to compute the control bits
        for (PermType i = 0; i < k - 1; i++)
        {
            bool b = (f == c);
            std::tie(f, g, l) = forward_or_rand(F, U, f, b, forward_prp);
            dec_unselected_counts(U.data(), n, l);
            c = oblivious_select(c, f, b);
            r = g + oblivious_select(-k, k, g < k);
            std::memcpy(container.data(), &r, sizeof(r));
            std::tie(s, l) = R[reverse_prp(container.data())];
            dec_unselected_counts(U.data(), n, l);
            S.emplace_back(f - oblivious_select((PermType)0, k, f >= k), f >= k);
            f = s + oblivious_select(-k, k, s < k);
        }
        bitonic_sort(S.data(), 0, k - 1, true);
        for (PermType i = 0; i < k - 1; i++)
            C[i] = S[i].second;
        return C;
    }

    // total length is n * log2(n)
    template <std::unsigned_integral PermType = uint32_t>
    std::vector<bit> control_bits(PermType *perm, PermType n, uint32_t depth = 0)
    {
        if (n == 1)
            return {};
        assert(std::has_single_bit(n));
        PermType k = n >> 1;
        // Compute control bits for input-layer switches
        std::vector<bit> c_in = in_bits(perm, n, depth); // c_in of length k - 1 --> k
        // Apply input-layer switches to perm
        for (PermType i = 0; i < k - 1; ++i)
            obliSwap(perm[i], perm[k + i], c_in[i]);
        // Reduce perm values modulo k, storing bits to undo later
        for (PermType i = 0; i < n - 1; i++)
        {
            bool b = (perm[i] >> depth) >= k;
            perm[i] = ((perm[i] - (oblivious_select((PermType)0, k, b) << depth)) << 1) | b;
        }
        // Compute control bits for top and bottom subnetworks
        std::vector<bit> c_top = k > 1 ? control_bits(perm, k, depth + 1) : std::vector<bit>();
        std::vector<bit> c_bot = k > 1 ? control_bits(perm + k, k, depth + 1) : std::vector<bit>();
        // Compute control bits for output-layer switches
        std::vector<bit> c_out(n - k);
        for (PermType i = 0; i < n - k; ++i)
            c_out[i] = perm[i] & 1;
        // Apply output-layer switches to P
        for (PermType i = 0; i < n - k; ++i)
            obliSwap(perm[i], perm[k + i], c_out[i]);
        // Undo reduction of perm values modulo k
        for (PermType i = 0; i < n - 1; i++)
        {
            bool b = perm[i] & 1;
            perm[i] = (perm[i] >> 1) + (oblivious_select((PermType)0, k, b) << depth);
        }
        // Combine control bits
        std::vector<bit> c;
        c.reserve(c_in.size() + c_top.size() + c_bot.size() + c_out.size());
        c.insert(c.end(), c_in.begin(), c_in.end());
        c.insert(c.end(), c_top.begin(), c_top.end());
        c.insert(c.end(), c_bot.begin(), c_bot.end());
        c.insert(c.end(), c_out.begin(), c_out.end());
        return c;
    }

    template <std::random_access_iterator DataIt>
    void apply_perm(const std::vector<bit> &C,
                    DataIt data, const size_t n,
                    const size_t C_offset = 0)
    {
        if (n <= 1)
            return;
#if PAGE_LEVEL_OBLIVIOUS
        uintptr_t addr1 = reinterpret_cast<uintptr_t>(&*data);
        uintptr_t addr2 = reinterpret_cast<uintptr_t>(&*(data + n)) - 1;
        if ((addr1 / PAGE_SIZE) == (addr2 / PAGE_SIZE))
            return;
#endif
        // assert(std::has_single_bit(n));
        const size_t k = (n >> 1) + (n & 1);
        const size_t lg2 = std::floor(std::log2(n));
        const size_t C_top_len = (lg2 - 1) * k;
        // Apply input-layer switches to perm
        if (n > 2)
        {
#if PAGE_LEVEL_OBLIVIOUS
            addr2 = reinterpret_cast<uintptr_t>(&*(data + k - 1)) - 1;
            if ((addr1 / PAGE_SIZE) != (addr2 / PAGE_SIZE))
            {
#endif
                // if (n >= 1024)
                //     std::for_each(std::execution::par_unseq,
                //                   data,
                //                   data + k - 1,
                //                   [&C, data, C_offset, k](auto &d)
                //                   { obliSwap(d, data[k + &d - data],
                //                                    C[&d - data + C_offset]); });
                // else
                // std::for_each(data,
                //               data + k - 1,
                //               [&C, data, C_offset, k](auto &d)
                //               { obliSwap(d, data[k + &d - data],
                //                                C[&d - data + C_offset]); });
                for (size_t i = 0; i < k - 1; i++)
                    obliSwap(data[i], data[k + i], C[i + C_offset]);
                    // for (size_t i = 0; i < k - 1; i++)
                    //     if (C[i + C_offset])
                    //         std::swap(data[i], data[k + i]);
#if PAGE_LEVEL_OBLIVIOUS
            }
#endif
            // Apply top and bottom subnetworks
            // apply_perm(C, data, k, C_offset + k);
            // apply_perm(C, data + k, n - k, C_offset + k + C_top_len);
            if (n >= 64)
                tbb::parallel_invoke(
                    [&C, data, C_offset, k, C_top_len, n]
                    { apply_perm(C, data, k, C_offset + k); },
                    [&C, data, C_offset, k, C_top_len, n]
                    { apply_perm(C, data + k, n - k, C_offset + k + C_top_len); });
            else
            {
                apply_perm(C, data, k, C_offset + k);
                apply_perm(C, data + k, n - k, C_offset + k + C_top_len);
            }
        }
#if PAGE_LEVEL_OBLIVIOUS
        addr2 = reinterpret_cast<uintptr_t>(&*(data + n - k)) - 1;
        if ((addr1 / PAGE_SIZE) != (addr2 / PAGE_SIZE))
        {
#endif
            // if (n >= 1024)
            //     std::for_each(std::execution::par_unseq,
            //                   data,
            //                   data + n - k,
            //                   [&C, data, C_offset, C_top_len, k](auto &d)
            //                   { obliSwap(d, data[k + &d - data],
            //                                    C[&d - data + C_offset + C_top_len * 2]); });
            // else
            // std::for_each(data,
            //               data + n - k,
            //               [&C, data, C_offset, C_top_len, k](auto &d)
            //               { obliSwap(d, data[k + &d - data],
            //                                C[&d - data + C_offset + C_top_len * 2]); });
            for (size_t i = 0; i < n - k; i++)
                obliSwap(data[i], data[k + i], C[i + C_offset + C_top_len * 2]);
                // for (size_t i = 0; i < n - k; i++)
                //     if (C[i + C_offset + C_top_len * 2])
                //         std::swap(data[i], data[k + i]);

#if PAGE_LEVEL_OBLIVIOUS
        }
#endif
    }

    template <std::unsigned_integral T = uint32_t>
    class OSorter
    {
    private:
        T n;
        std::vector<bit> C;

    public:
        OSorter(T n) : n(n)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            // std::vector<T> P(n);
            // std::iota(P.begin(), P.end(), 0);
            // std::shuffle(P.begin(), P.end(), gen);
            // C = control_bits(P.data(), n);
            // generate n log2(n) random bits,
            // it should be implemented in a producer-consumer model,
            // but here we just generate them for simplicity,
            // it does not affect the online performance as those bits are precomputed
            C.reserve(n * std::bitset<sizeof(T) * 8>(n - 1).count());
            for (T i = 0; i < n * std::bitset<sizeof(T) * 8>(n - 1).count(); i++)
                C.push_back(gen() & 1);
        }

        template <std::random_access_iterator DataIt, class Compare>
        void operator()(DataIt data, size_t n, Compare cmp) const
        {
            if (n > this->n)
            {
                std::cerr << "n: " << n << ", this->n: " << this->n << std::endl;
                assert(n <= this->n);
            }
            // bitonic_sort(data, 0, n, 1, cmp);
            apply_perm(C, data, n);
            if (n < 1024)
                std::sort(std::execution::seq,
                          data, data + n, cmp);
            else
                std::sort(std::execution::par_unseq,
                          data, data + n, cmp);
        }

        T get_n() const
        {
            return n;
        }
    };
}