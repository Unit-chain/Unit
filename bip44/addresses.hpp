#include "SHA256.h"
#include "cmath"

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
    std::string pub2addr(std::string pub_key);
};

std::string Addresses::pub2addr(std::string pub_key)
{
    SHA256_Legacy sha256;

    std::string address_hex = "16D8EE";

    uint8_t pub_key_bytes[pub_key.length() / 2];
    for (size_t i = 0; i < pub_key.length(); i += 2)
        pub_key_bytes[i / 2] = hex2dec(pub_key.substr(i, 2));
    std::string sha256_pub_key = sha256(pub_key_bytes, pub_key.length() / 2);

    uint8_t sha256_pub_key_bytes[sha256_pub_key.length() / 2];
    for (size_t i = 0; i < sha256_pub_key.length(); i += 2)
        sha256_pub_key_bytes[i / 2] = hex2dec(sha256_pub_key.substr(i, 2));
    std::string sha256_pub_key_round_one = sha256(sha256_pub_key_bytes, sha256_pub_key.length() / 2);

    uint8_t sha256_pub_key_round_one_bytes[sha256_pub_key_round_one.length() / 2];
    for (size_t i = 0; i < sha256_pub_key_round_one.length(); i += 2)
        sha256_pub_key_round_one_bytes[i / 2] = hex2dec(sha256_pub_key_round_one.substr(i, 2));
    std::string sha256_pub_key_round_two = sha256(sha256_pub_key_round_one_bytes, sha256_pub_key_round_one.length() / 2);

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