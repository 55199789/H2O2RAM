#pragma once
#include <openssl/aes.h>
#include <stdint.h>
#include <array>

namespace ORAM
{
    class AESCSPRP
    {
    private:
        AES_KEY enc_key;

    public:
        AESCSPRP();
        std::array<uint8_t, AES_BLOCK_SIZE> operator()(const uint8_t *input) const;
    };
}