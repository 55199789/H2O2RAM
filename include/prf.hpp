#pragma once
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <string.h>
#include <concepts>

template <std::unsigned_integral KeyType>
class AESPRF
{
private:
    AES_KEY key;
    KeyType range;

public:
    AESPRF(KeyType range = 0) : range(range)
    {
        reset();
    }

    // define operator =
    AESPRF &operator=(const AESPRF &other)
    {
        if (this != &other)
            range = other.range;
        return *this;
    }

    void reset()
    {
        uint8_t aes_key[AES_BLOCK_SIZE];
        RAND_bytes(aes_key, AES_BLOCK_SIZE);
        AES_set_encrypt_key(aes_key, 128, &key);
    }

    KeyType operator()(KeyType input) const
    {
        uint8_t in[AES_BLOCK_SIZE] = {};
        uint8_t out[AES_BLOCK_SIZE] = {};

        std::memcpy(in, &input, sizeof(input));
        AES_encrypt(in, out, &key);

        KeyType output;
        std::memcpy(&output, out, sizeof(output));

        return output % range;
    }
};