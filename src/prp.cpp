#include "prp.hpp"
#include <assert.h>
#include <random>
namespace ORAM
{
    AESCSPRP::AESCSPRP()
    {
        std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint64_t> dist;
        uint64_t key[2] = {dist(rng), dist(rng)};
        AES_set_encrypt_key((uint8_t *)key, AES_BLOCK_SIZE * 8, &enc_key);
    }

    std::array<uint8_t, AES_BLOCK_SIZE> AESCSPRP::operator()(
        const uint8_t *input) const
    {
        std::array<uint8_t, AES_BLOCK_SIZE> output;
        AES_ecb_encrypt(input, output.data(), &enc_key, AES_ENCRYPT);
        return output;
    }
}