#pragma once
#include "utils.hpp"

class Addresses
{
private:
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

public:
    std::string pub2addr(const std::string& pub_key);
};

std::string Addresses::pub2addr(const std::string& pub_key)
{
    SHA256_Legacy sha256;

    std::string address_hex = "16D8EE";

    BIGNUM* bn_r1 = BN_new();
    BN_hex2bn(&bn_r1, pub_key.c_str());
    uint8_t r1_bytes[pub_key.length() / 2];
    BN_bn2bin(bn_r1, r1_bytes);
    std::string sha256_pub_key = sha256(r1_bytes, pub_key.length() / 2);

    BIGNUM* bn_r2 = BN_new();
    BN_hex2bn(&bn_r2, sha256_pub_key.c_str());
    uint8_t r2_bytes[sha256_pub_key.length() / 2];
    BN_bn2bin(bn_r2, r2_bytes);
    std::string sha256_pub_key_round_one = sha256(r2_bytes, sha256_pub_key.length() / 2);

    BIGNUM* bn_r3 = BN_new();
    BN_hex2bn(&bn_r3, sha256_pub_key_round_one.c_str());
    uint8_t r3_bytes[sha256_pub_key_round_one.length() / 2];
    BN_bn2bin(bn_r3, r3_bytes);
    std::string sha256_pub_key_round_two = sha256(r3_bytes, sha256_pub_key_round_one.length() / 2);
    BN_free(bn_r1);
    BN_free(bn_r2);
    BN_free(bn_r3);

    address_hex += sha256_pub_key.substr(0, 32) + sha256_pub_key_round_two.substr(0, 8);

    base58 b58;
    std::vector<uint8_t> address_bytes;
    for (uint16_t i = 0; i < address_hex.length(); i += 2)
    {
        uint8_t byte = hex2dec(address_hex.substr(i, 2));
        address_bytes.push_back(byte);
    }
    std::string address_base58 = b58.EncodeBase58(address_bytes, b58.base58map);
    return address_base58;
}