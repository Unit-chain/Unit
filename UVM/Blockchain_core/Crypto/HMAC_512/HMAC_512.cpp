//
// Created by Kirill Zhukov on 22.06.2022.
//

#include "HMAC_512.h"

std::string unit::HMAC_512::get_hash(const std::string &input) {
    uint8_t digest[SHA512::DIGEST_SIZE];
    std::fill(digest, digest + SHA512::DIGEST_SIZE, '\0');
    SHA512 ctx = SHA512();
    ctx.init();
    ctx.update((uint8_t*)input.c_str(), input.size());
    ctx.final(digest);
    return {(const char*)digest, SHA512::DIGEST_SIZE};
}
std::string unit::HMAC_512::get_hmac(std::string key, const std::string &msg, const bool is_hex) {
    size_t block_size = SHA512::SHA384_512_BLOCK_SIZE;
    if(key.size() < block_size) {
        key.resize(block_size, '\0');
    }
    std::string ikeypad;
    std::string okeypad;
    ikeypad.reserve(block_size);
    okeypad.reserve(block_size);
    for(size_t i = 0; i < block_size; ++i) {
        ikeypad.push_back('\x36' ^ key[i]);
        okeypad.push_back('\x5c' ^ key[i]);
    }
    if(is_hex) return to_hex(get_hash(okeypad + get_hash(ikeypad + msg)));
    return get_hash(okeypad + get_hash(ikeypad + msg));
}

std::string unit::HMAC_512::to_hex(const std::string &input) {
    static const char *lut = "0123456789abcdef0123456789ABCDEF";
    const char *symbol = lut;
    const size_t length = input.size();
    std::string output;
    output.reserve(2 * length);
    for (size_t i = 0; i < length; ++i) {
        const uint8_t c = input[i];
        output.push_back(symbol[c >> 4]);
        output.push_back(symbol[c & 15]);
    }
    return output;
}
