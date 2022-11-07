#pragma once
#include "string"
#include "cmath"
#include <openssl/ec.h>
#include "openssl/ecdsa.h"
#include "openssl/bn.h"
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>
#include "base58.hpp"
#include "SHA256.h"
#include <boost/multiprecision/cpp_int.hpp>
#include "addresses.hpp"

inline uint8_t hex2dec(std::string hex)
{
    uint8_t result = 0;
    for (int i = 0; i < hex.length(); i++)
    {
        if (hex[i] >= 48 && hex[i] <= 57)
        {
            result += (hex[i] - 48) * pow(16, hex.length() - i - 1);
        }
        else if (hex[i] >= 65 && hex[i] <= 70)
        {
            result += (hex[i] - 55) * pow(16, hex.length() - i - 1);
        }
        else if (hex[i] >= 97 && hex[i] <= 102)
        {
            result += (hex[i] - 87) * pow(16, hex.length() - i - 1);
        }
    }
    return result;
}
inline std::string to_hex(const std::string input)
{
    static const char *lut = "0123456789abcdef0123456789ABCDEF";
    const char *symbol = lut;
    const size_t length = input.size();
    std::string output;
    output.reserve(2 * length);
    for (size_t i = 0; i < length; ++i)
    {
        const uint8_t c = input[i];
        output.push_back(symbol[c >> 4]);
        output.push_back(symbol[c & 15]);
    }
    return output;
}
inline std::string reverse_hex_str(std::string &str)
{
    std::string result;
    result.reserve(str.size());

    for (std::size_t i = str.size(); i != 0; i -= 2)
    {
        result.append(str, i - 2, 2);
    }

    return result;
}

inline std::string hmac_512(unsigned char *key, int key_length, unsigned char *message, size_t message_length)
{
    unsigned char digest[64];
    unsigned int sha_len = 64;
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key, key_length, EVP_sha512(), NULL);
    HMAC_Update(ctx, message, message_length);
    HMAC_Final(ctx, digest, &sha_len);
    std::string hmac_result(digest, digest + sizeof digest / sizeof digest[0]);
    std::string hmac_result_hex = to_hex(hmac_result);
    HMAC_CTX_free(ctx);
    return hmac_result_hex;
}
