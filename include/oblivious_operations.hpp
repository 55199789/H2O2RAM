#pragma once

#include "types.hpp"
#include <immintrin.h>

namespace ORAM
{
    template <std::integral T>
    inline void oblivious_swap(T &left, T &right, bool flag)
    {
        T mask = ~((T)flag - 1);
        T *left_ptr = (T *)&left;
        T *right_ptr = (T *)&right;
        *left_ptr ^= *right_ptr;
        *right_ptr ^= *left_ptr & mask;
        *left_ptr ^= *right_ptr;
    }

    template <std::integral KeyType>
    inline void oblivious_swap(Block<KeyType, 4> &left, Block<KeyType, 4> &right, bool flag)
    {
        uint32_t mask = ~((uint32_t)flag - 1);
        uint32_t *left_ptr = (uint32_t *)&left;
        uint32_t *right_ptr = (uint32_t *)&right;
        *left_ptr ^= *right_ptr;
        *right_ptr ^= *left_ptr & mask;
        *left_ptr ^= *right_ptr;
    }

    template <std::integral KeyType>
    inline void oblivious_swap(Block<KeyType, 4> *left, Block<KeyType, 4> *right, bool flag)
    {
        oblivious_swap(*left, *right, flag);
    }

    template <std::integral KeyType, std::size_t BlockSize>
    inline void oblivious_swap(Block<KeyType, BlockSize> &left, Block<KeyType, BlockSize> &right, bool flag)
    {
        uint64_t mask = ~((uint64_t)flag - 1);
        uint64_t *left_ptr = (uint64_t *)&left;
        uint64_t *right_ptr = (uint64_t *)&right;
        for (size_t i = 0; i < BlockSize / 8; i++)
        {
            *left_ptr ^= *right_ptr;
            *right_ptr ^= *left_ptr & mask;
            *left_ptr ^= *right_ptr;
            left_ptr++;
            right_ptr++;
        }
        if constexpr (BlockSize & 7)
        {
            uint8_t *left_ptr_ = (uint8_t *)left_ptr;
            uint8_t *right_ptr_ = (uint8_t *)right_ptr;
            uint8_t mask_ = ~((uint8_t)flag - 1);
            for (size_t i = 0; i < (BlockSize & 7); i++)
            {
                *left_ptr_ ^= *right_ptr_;
                *right_ptr_ ^= *left_ptr_ & mask_;
                *left_ptr_ ^= *right_ptr_;
                left_ptr_++;
                right_ptr_++;
            }
        }
    }

    template <std::integral KeyType, std::size_t BlockSize>
    inline void oblivious_swap(Block<KeyType, BlockSize> *left, Block<KeyType, BlockSize> *right, bool flag)
    {
        oblivious_swap(*left, *right, flag);
    }

    template <typename T>
    inline void oblivious_swap(T &left, T &right, bool flag)
    {
        uint8_t mask = ~((uint8_t)flag - 1);
        uint8_t *left_ptr = (uint8_t *)&left;
        uint8_t *right_ptr = (uint8_t *)&right;
        for (size_t i = 0; i < sizeof(T); i++)
        {
            *left_ptr ^= *right_ptr;
            *right_ptr ^= *left_ptr & mask;
            *left_ptr ^= *right_ptr;
            left_ptr++;
            right_ptr++;
        }
    }

    // returns right if flag = 1;
    inline bool oblivious_select(bool left, bool right, bool flag)
    {
        return (left & !flag) | (right & flag);
    }

    template <std::integral T>
    inline T oblivious_select(T left, T right, bool flag)
    {
        return (left & ((T)flag - 1)) | (right & ~((T)flag - 1));
    }

    template <std::integral KeyType>
    inline Block<KeyType, 4> oblivious_select(Block<KeyType, 4> left,
                                              Block<KeyType, 4> right,
                                              bool flag)
    {
        Block<KeyType, 4> ret;
        uint32_t mask = ((uint32_t)flag - 1);
        uint32_t *left_ptr = (uint32_t *)&left;
        uint32_t *right_ptr = (uint32_t *)&right;
        uint32_t *ret_ptr = (uint32_t *)&ret;
        *ret_ptr = (*left_ptr & mask) | (*right_ptr & ~mask);
        return ret;
    }

    template <std::integral KeyType, std::size_t BlockSize>
    inline Block<KeyType, BlockSize> oblivious_select(const Block<KeyType, BlockSize> &left,
                                                      const Block<KeyType, BlockSize> &right,
                                                      bool flag)
    {
        Block<KeyType, BlockSize> ret;
        uint32_t mask = (uint32_t)flag - 1;
        uint32_t *left_ptr = (uint32_t *)&left;
        uint32_t *right_ptr = (uint32_t *)&right;
        uint32_t *ret_ptr = (uint32_t *)&ret;
        // for (size_t i = 0; i < (BlockSize + 7) / 8; i++)
        for (size_t i = 0; i < sizeof(Block<KeyType, BlockSize>) / 4; i++)
        {
            *ret_ptr = (*left_ptr & mask) | (*right_ptr & ~mask);
            left_ptr++;
            right_ptr++;
            ret_ptr++;
        }
        return ret;
    }

    inline void CSWAP8(const uint64_t cond, uint64_t &guy1, uint64_t &guy2)
    {
        asm volatile(
            "test %[mcond], %[mcond]\n\t"
            "mov %[i1], %%r9\n\t"
            "cmovnz %[i2], %[i1]\n\t"
            "cmovnz %%r9, %[i2]\n\t"
            : [i1] "=r"(guy1), [i2] "=r"(guy2)
            : [mcond] "r"(cond), "[i1]"(guy1), "[i2]"(guy2)
            : "r9");
    }

    inline void CMOV8_internal(const uint64_t cond, uint64_t &guy1,
                               const uint64_t &guy2)
    {
        asm volatile(
            "test %[mcond], %[mcond]\n\t"
            "cmovnz %[i2], %[i1]\n\t"
            : [i1] "=r"(guy1)
            : [mcond] "r"(cond), "[i1]"(guy1), [i2] "r"(guy2)
            :);
    }

    inline void CMOV4_internal(const uint64_t cond, uint32_t &guy1,
                               const uint32_t &guy2)
    {
        asm volatile(
            "test %[mcond], %[mcond]\n\t"
            "cmovnz %[i2], %[i1]\n\t"
            : [i1] "=r"(guy1)
            : [mcond] "r"(cond), "[i1]"(guy1), [i2] "r"(guy2)
            :);
    }

    inline void CMOV1(const bool &cond, uint8_t &val1, const uint8_t &val2)
    {
        uint32_t r1 = 0 | val1;
        uint32_t r2 = 0 | val2;
        CMOV4_internal(cond, r1, r2);
        val1 = r1 & 0xff;
    }

    inline void CMOV2(const bool &cond, uint16_t &val1, const uint16_t &val2)
    {
        uint32_t r1 = 0 | val1;
        uint32_t r2 = 0 | val2;
        CMOV4_internal(cond, r1, r2);
        val1 = r1 & 0xffff;
    }

    inline void CMOV4(const bool &cond, uint32_t &val1, const uint32_t &val2)
    {
        CMOV4_internal(cond, val1, val2);
    }

    inline void CMOV8(const bool &cond, uint64_t &val1, const uint64_t &val2)
    {
        CMOV8_internal(cond, val1, val2);
    }

    inline void CMOV_BOOL(const bool cond, bool &val1, const bool &val2)
    {
        uint32_t v1 = val1;
        CMOV4_internal(cond, v1, val2);
        val1 = v1;
    }

    template <typename T>
    inline void CMOV(const bool cond, T &val1, const T &val2)
    {
        if (cond)
        {
            val1 = val2;
        }
    }

    // Some CMOV specializations:
    //
    template <>
    inline void CMOV<uint64_t>(const bool cond, uint64_t &val1,
                               const uint64_t &val2)
    {
        CMOV8(cond, val1, val2);
    }

    template <>
    inline void CMOV<uint32_t>(const bool cond, uint32_t &val1,
                               const uint32_t &val2)
    {
        CMOV4(cond, val1, val2);
    }

    template <>
    inline void CMOV<uint16_t>(const bool cond, uint16_t &val1,
                               const uint16_t &val2)
    {
        CMOV2(cond, val1, val2);
    }

    template <>
    inline void CMOV<uint8_t>(const bool cond, uint8_t &val1,
                              const uint8_t &val2)
    {
        CMOV1(cond, val1, val2);
    }

    template <>
    inline void CMOV<bool>(const bool cond, bool &val1, const bool &val2)
    {
        CMOV_BOOL(cond, val1, val2);
    }

    template <>
    inline void CMOV<int>(const bool cond, int &val1, const int &val2)
    {
        // UNDONE(): Make this a reinterpret cast?
        //
        CMOV4(cond, (uint32_t &)val1, val2);
    }

    template <>
    inline void CMOV<short>(const bool cond, short &val1, const short &val2)
    {
        // UNDONE(): Make this a reinterpret cast?
        //
        CMOV2(cond, (uint16_t &)val1, val2);
    }

    template <>
    inline void CMOV<int8_t>(const bool cond, int8_t &val1,
                             const int8_t &val2)
    {
        // UNDONE(): Make this a reinterpret cast?
        //
        CMOV1(cond, (uint8_t &)val1, val2);
    }

    template <typename T>
    inline void CXCHG(const uint64_t &cond, T &A, T &B)
    {
        const T C = A;
        CMOV(cond, A, B);
        CMOV(cond, B, C);
    }

    template <const uint64_t sz>
    inline void CXCHG_internal(const bool cond, void *vec1, void *vec2)
    {
        static_assert(sz <= 64);
#if defined(__AVX512VL__)
        const __mmask8 blend_mask = (__mmask8)(!cond) - 1;
#endif
        if constexpr (sz == 64)
        {
#if false && defined(__AVX512VL__)
    /* alternative implementation
    __m512i vec1_temp, vec2_temp;
    std::memcpy(&vec1_temp, vec1, 64);
    std::memcpy(&vec2_temp, vec2, 64);
    const __m512i& vec1_after_swap =
        _mm512_mask_blend_epi64(blend_mask, vec1_temp, vec2_temp);
    const __m512i& vec2_after_swap =
        _mm512_mask_blend_epi64(blend_mask, vec2_temp, vec1_temp);
    std::memcpy(vec1, &vec1_after_swap, 64);
    std::memcpy(vec2, &vec2_after_swap, 64);
    */
    __m512i vec1_temp, vec2_temp;
    __m512i temp;
    std::memcpy(&vec1_temp, vec1, 64);
    std::memcpy(&vec2_temp, vec2, 64);
    __m512i mask = _mm512_set1_epi32(-cond);  // Create a mask based on the
    temp = _mm512_xor_si512(vec1_temp, vec2_temp);
    temp = _mm512_and_si512(temp, mask);
    vec1_temp = _mm512_xor_si512(vec1_temp, temp);
    vec2_temp = _mm512_xor_si512(vec2_temp, temp);
    std::memcpy(vec1, &vec1_temp, 64);
    std::memcpy(vec2, &vec2_temp, 64);

    // on skylake, it's faster to run 256bit instructions two times
#else
            CXCHG_internal<32>(cond, vec1, vec2);
            CXCHG_internal<32>(cond, (char *)vec1 + 32, (char *)vec2 + 32);
#endif
            return;
        }
        if constexpr (sz >= 32)
        {
#if defined(__AVX512VL__)
            __m256d vec1_temp, vec2_temp;
            std::memcpy(&vec1_temp, vec1, 32);
            std::memcpy(&vec2_temp, vec2, 32);
            const __m256d &vec1_after_swap =
                _mm256_mask_blend_pd(blend_mask, vec1_temp, vec2_temp);
            const __m256d &vec2_after_swap =
                _mm256_mask_blend_pd(blend_mask, vec2_temp, vec1_temp);
            std::memcpy(vec1, &vec1_after_swap, 32);
            std::memcpy(vec2, &vec2_after_swap, 32);
#elif defined(__AVX2__)
            /* alternative implementation
            __m256i vec1_temp, vec2_temp;
            std::memcpy(&vec1_temp, vec1, 32);
            std::memcpy(&vec2_temp, vec2, 32);
            __m256i mask = _mm256_set1_epi8(-cond);
            const __m256i& vec1_after_swap =
                _mm256_blendv_epi8(vec1_temp, vec2_temp, mask);
            const __m256i& vec2_after_swap =
                _mm256_blendv_epi8(vec2_temp, vec1_temp, mask);
            std::memcpy(vec1, &vec1_after_swap, 32);
            std::memcpy(vec2, &vec2_after_swap, 32);
            */
            __m256i vec1_temp, vec2_temp;
            __m256i temp;
            std::memcpy(&vec1_temp, vec1, 32);
            std::memcpy(&vec2_temp, vec2, 32);
            __m256i mask = _mm256_set1_epi32(-cond); // Create a mask based on cond
            temp = _mm256_xor_si256(vec1_temp, vec2_temp);
            temp = _mm256_and_si256(temp, mask);
            vec1_temp = _mm256_xor_si256(vec1_temp, temp);
            vec2_temp = _mm256_xor_si256(vec2_temp, temp);
            std::memcpy(vec1, &vec1_temp, 32);
            std::memcpy(vec2, &vec2_temp, 32);
#else
            CXCHG_internal<16>(cond, vec1, vec2);
            CXCHG_internal<16>(cond, (char *)vec1 + 16, (char *)vec2 + 16);
#endif
        }
        if constexpr (sz % 32 >= 16)
        {
#if defined(__AVX512VL__)
            constexpr uint64_t offset = 4 * (sz / 32);
            __m128d vec1_temp, vec2_temp;
            std::memcpy(&vec1_temp, (uint64_t *)vec1 + offset, 16);
            std::memcpy(&vec2_temp, (uint64_t *)vec2 + offset, 16);
            const __m128d &vec1_after_swap =
                _mm_mask_blend_pd(blend_mask, vec1_temp, vec2_temp);
            const __m128d &vec2_after_swap =
                _mm_mask_blend_pd(blend_mask, vec2_temp, vec1_temp);
            std::memcpy((uint64_t *)vec1 + offset, &vec1_after_swap, 16);
            std::memcpy((uint64_t *)vec2 + offset, &vec2_after_swap, 16);
#elif defined(__SSE2__)
            __m128i vec1_temp, vec2_temp;
            __m128i temp;
            std::memcpy(&vec1_temp, vec1, 16);
            std::memcpy(&vec2_temp, vec2, 16);
            __m128i mask = _mm_set1_epi16(-cond); // Create a mask based on cond
            temp = _mm_xor_si128(vec1_temp, vec2_temp);
            temp = _mm_and_si128(temp, mask);
            vec1_temp = _mm_xor_si128(vec1_temp, temp);
            vec2_temp = _mm_xor_si128(vec2_temp, temp);
            std::memcpy(vec1, &vec1_temp, 16);
            std::memcpy(vec2, &vec2_temp, 16);
#else
            CXCHG_internal<8>(cond, vec1, vec2);
            CXCHG_internal<8>(cond, (char *)vec1 + 8, (char *)vec2 + 8);
#endif
        }

        if constexpr (sz % 16 >= 8)
        {
            constexpr uint64_t offset = 2 * (sz / 16);
            uint64_t *curr1_64 = (uint64_t *)vec1 + offset;
            uint64_t *curr2_64 = (uint64_t *)vec2 + offset;
            CXCHG(cond, *curr1_64, *curr2_64);
        }
        if constexpr (sz % 8 >= 4)
        {
            constexpr uint64_t offset = 2 * (sz / 8);
            uint32_t *curr1_32 = (uint32_t *)vec1 + offset;
            uint32_t *curr2_32 = (uint32_t *)vec2 + offset;
            CXCHG(cond, *curr1_32, *curr2_32);
        }
        if constexpr (sz % 4 >= 2)
        {
            constexpr uint64_t offset = 2 * (sz / 4);
            uint16_t *curr1_16 = (uint16_t *)vec1 + offset;
            uint16_t *curr2_16 = (uint16_t *)vec2 + offset;
            CXCHG(cond, *curr1_16, *curr2_16);
        }
        if constexpr (sz % 2 >= 1)
        {
            constexpr uint64_t offset = 2 * (sz / 2);
            uint8_t *curr1_8 = (uint8_t *)vec1 + offset;
            uint8_t *curr2_8 = (uint8_t *)vec2 + offset;
            CXCHG(cond, *curr1_8, *curr2_8);
        }
    }

    template <typename T>
    inline void obliSwap(T &guy1, T &guy2, const bool mov)
    {
        // static_assert(sizeof(T)%8 == 0);
        __m512i *curr1 = (__m512i *)&guy1;
        __m512i *curr2 = (__m512i *)&guy2;
        for (uint64_t i = 0; i < sizeof(T) / 64; ++i)
        {
            CXCHG_internal<64>(mov, curr1, curr2);
            curr1++;
            curr2++;
        }
        constexpr uint64_t rem_size = sizeof(T) % 64;
        if constexpr (rem_size > 0)
        {
            CXCHG_internal<rem_size>(mov, curr1, curr2);
        }
    }

    template <typename T>
    inline void obliSwap(T *guy1, T *guy2, const bool mov)
    {
        // static_assert(sizeof(T)%8 == 0);
        __m512i *curr1 = (__m512i *)guy1;
        __m512i *curr2 = (__m512i *)guy2;
        for (uint64_t i = 0; i < sizeof(T) / 64; ++i)
        {
            CXCHG_internal<64>(mov, curr1, curr2);
            curr1++;
            curr2++;
        }
        constexpr uint64_t rem_size = sizeof(T) % 64;
        if constexpr (rem_size > 0)
        {
            CXCHG_internal<rem_size>(mov, curr1, curr2);
        }
    }
}