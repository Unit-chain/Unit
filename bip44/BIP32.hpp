#include <openssl/ec.h>
#include "openssl/ecdsa.h"
#include "openssl/bn.h"
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/hmac.h>
#include <boost/multiprecision/cpp_int.hpp>
#include "SHA256.h"
#include "base58.hpp"

#define HARDENED_VALUE 2147483648
// #define NMODULO boost::multiprecision::uint512_t("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F")
#define GMODULO boost::multiprecision::uint512_t("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141")

using uint512 = boost::multiprecision::uint512_t;

class BIP32
{
private:
    /// @brief Contains information from secp256k1 EC
    struct secp256k1_result
    {
        std::string private_key;
        std::string point_x;
        std::string point_y;
        std::string public_key;
        secp256k1_result(std::string _prv, std::string _x, std::string _y, std::string _pbc)
        {
            private_key = _prv;
            point_x = _x;
            point_y = _y;
            public_key = _pbc;
        }
    };
    std::string to_base58();
    secp256k1_result get_secp256k1(std::string);

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

public:
    /// @brief Result of generating BIP32 node
    struct BIP32NodeResult
    {
        std::string depth;         // hex
        std::string keypair_index; // hex
        std::string ecdsa_pub;     // hex
        std::string ecdsa_prv;     // hex
        std::string hex_prv_key;
        std::string hex_pub_key;
        std::string xprv_base58;
        std::string xpub_base58;

        /// @brief Constructor of the BIP32 result struct. Contains everything to generate child keys.
        /// @param d depth of the child node ("00", "01", "02"...)
        /// @param pair_i index of the keys pair. can be hardened or not.
        /// hardened values use macro HARDENED. ("00000001", "00000002", ... , "80000000", "80000001"...)
        ///
        /// @param h_prv hex representation of extended private key
        /// @param h_pub hex representation of extended public key
        /// @param b_prv base58 representation of extended private key
        /// @param b_pub base58 representation of extended public key
        /// @param ecdsa_pk hex representaion of private key
        /// @param ecdsa_pv hex representaion of public key
        BIP32NodeResult(std::string d, std::string pair_i, std::string h_prv, std::string h_pub, std::string b_prv, std::string b_pub, std::string ecdsa_pk, std::string ecdsa_pv)
        {
            depth = d;
            keypair_index = pair_i;
            hex_prv_key = h_prv;
            hex_pub_key = h_pub;
            xprv_base58 = b_prv;
            xpub_base58 = b_pub;
            ecdsa_pub = ecdsa_pk;
            ecdsa_prv = ecdsa_pv;
        }

        /// @brief Empty constructor. Use it when error has happened.
        BIP32NodeResult()
        {
            depth = "";
            keypair_index = "";
            hex_prv_key = "";
            hex_pub_key = "";
            xprv_base58 = "";
            xpub_base58 = "";
            ecdsa_pub = "";
            ecdsa_prv = "";
        }
    };

    /// @brief generating master-node for BIP32.
    /// @param seed from BIP38
    /// @return
    BIP32NodeResult gen_master(std::string seed);

    /// @brief generate child from parent's private key
    /// @param res parent's node
    /// @param i desired keypair index
    /// @param d desired depth of the node
    /// @return
    BIP32NodeResult gen_child_from_xprv(BIP32NodeResult res, uint32_t i, std::string d);
};

BIP32::BIP32NodeResult BIP32::gen_master(std::string seed)
{

    unsigned char digest[64];
    unsigned int sha_len = 64;
    unsigned char key[12] = {'B', 'i', 't', 'c', 'o', 'i', 'n', ' ', 's', 'e', 'e', 'd'};
    unsigned char seed_to_chr[seed.length() / 2];
    for (uint16_t i = 0; i < seed.length(); i += 2)
    {
        uint8_t byte = hex2dec(seed.substr(i, 2));
        seed_to_chr[i / 2] = byte;
    }

    std::string hmac_result_hex = hmac_512(key, 12, seed_to_chr, (seed.length() / 2));

    std::string I_left = hmac_result_hex.substr(0, 64);
    std::string I_right = hmac_result_hex.substr(64, 64);

    std::string xpriv = "0488ade4"; // hex format
    std::string xpub = "0488b21e";  // hex format

    std::string depth = "00"; // hex

    std::string fingerprint = "00000000"; // hex and 0 for master

    std::string childnumber = "00000000"; // hex and 0 for master

    std::string chaincode = I_right;

    BIP32::secp256k1_result sign = get_secp256k1(I_left);

    if (sign.point_x == "")
        return BIP32::BIP32NodeResult();

    // std::string reversed_prv_key = sign.private_key;
    std::string data_prv = "00" + I_left;

    std::string ext_xprv_raw = xpriv + depth + fingerprint + childnumber + chaincode + data_prv;

    uint8_t prv_fro_sha[ext_xprv_raw.length() / 2];
    for (size_t i = 0; i < ext_xprv_raw.length(); i += 2)
        prv_fro_sha[i / 2] = hex2dec(ext_xprv_raw.substr(i, 2));

    SHA256_Legacy sha;
    std::string round_one = sha(prv_fro_sha, ext_xprv_raw.length() / 2);
    uint8_t round_one_next_sha[round_one.length() / 2];

    for (size_t i = 0; i < round_one.length(); i += 2)
        round_one_next_sha[i / 2] = hex2dec(round_one.substr(i, 2));

    std::string round_two = sha(round_one_next_sha, round_one.length() / 2);

    std::string ext_priv = ext_xprv_raw + round_two.substr(0, 8);

    base58 b58;
    std::vector<uint8_t> ext_priv_bytes;
    for (uint16_t i = 0; i < ext_priv.length(); i += 2)
    {
        uint8_t byte = hex2dec(ext_priv.substr(i, 2));
        ext_priv_bytes.push_back(byte);
    }
    std::string xprv_base58_result = b58.EncodeBase58(ext_priv_bytes, b58.base58map);

    /***
     *
     * FINALLY PUBLIC KEY
     *
     ***/

    std::string G_y_last_digit = sign.point_y.substr(sign.point_y.length() - 1, 1);
    std::string first_byte = (int)hex2dec(G_y_last_digit) & 1 ? "03" : "02";
    std::string ext_pub_raw = xpub + depth + fingerprint + childnumber + chaincode + first_byte + sign.point_x;

    uint8_t pub_fro_sha[ext_pub_raw.length() / 2];
    for (size_t i = 0; i < ext_pub_raw.length(); i += 2)
        pub_fro_sha[i / 2] = hex2dec(ext_pub_raw.substr(i, 2));
    std::string round_one_pub = sha(pub_fro_sha, ext_pub_raw.length() / 2);
    uint8_t round_one_next_sha_pub[round_one_pub.length() / 2];

    for (size_t i = 0; i < round_one_pub.length(); i += 2)
        round_one_next_sha_pub[i / 2] = hex2dec(round_one_pub.substr(i, 2));

    std::string round_two_pub = sha(round_one_next_sha_pub, round_one_pub.length() / 2);

    std::string ext_pub = ext_pub_raw + round_two_pub.substr(0, 8);

    std::vector<uint8_t> ext_pub_bytes;
    for (uint16_t i = 0; i < ext_pub.length(); i += 2)
    {
        uint8_t byte = hex2dec(ext_pub.substr(i, 2));
        ext_pub_bytes.push_back(byte);
    }
    std::string xpub_base58_result = b58.EncodeBase58(ext_pub_bytes, b58.base58map);

    return BIP32::BIP32NodeResult(depth, "00000000", ext_priv, ext_pub, xprv_base58_result, xpub_base58_result, sign.public_key, I_left);
}

BIP32::BIP32NodeResult BIP32::gen_child_from_xprv(BIP32::BIP32NodeResult parent_node, uint32_t keypair_index, std::string depth)
{
    std::string parent_prv_version = parent_node.hex_prv_key.substr(0, 8);
    std::string parent_pub_version = parent_node.hex_pub_key.substr(0, 8);
    std::string parent_depth = parent_node.hex_prv_key.substr(8, 2);
    std::string parent_fingerprint = parent_node.hex_prv_key.substr(10, 8);
    std::string parent_child_number = parent_node.hex_prv_key.substr(18, 8);
    std::string parent_chaincode = parent_node.hex_prv_key.substr(26, 64);
    std::string parent_private_key = parent_node.ecdsa_prv;
    // std::string parent_public_key = parent_node.hex_pub_key.substr();

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(8) << std::hex << keypair_index;
    std::string keypair_index_hex = ss.str();

    std::string hmac_result;

    if (keypair_index >= HARDENED_VALUE)
    {
        // hardened
        unsigned char parent_chaincode_to_chr[parent_chaincode.length() / 2];
        for (uint8_t i = 0; i < parent_chaincode.length(); i += 2)
        {
            uint8_t byte = hex2dec(parent_chaincode.substr(i, 2));
            parent_chaincode_to_chr[i / 2] = byte;
        }
        std::string data = "00" + parent_private_key + keypair_index_hex;
        unsigned char data_to_chr[data.length() / 2];
        for (uint8_t i = 0; i < data.length(); i += 2)
        {
            uint8_t byte = hex2dec(data.substr(i, 2));
            data_to_chr[i / 2] = byte;
        }

        hmac_result = hmac_512(parent_chaincode_to_chr, parent_chaincode.length() / 2, data_to_chr, data.length() / 2);
    }
    else
    {
        // not hardened
        unsigned char parent_chaincode_to_chr[parent_chaincode.length() / 2];
        for (uint8_t i = 0; i < parent_chaincode.length(); i += 2)
        {
            uint8_t byte = hex2dec(parent_chaincode.substr(i, 2));
            parent_chaincode_to_chr[i / 2] = byte;
        }
        BIP32::secp256k1_result curve_k = get_secp256k1(parent_private_key);
        if (curve_k.point_x == "")
            return BIP32::BIP32NodeResult();
        std::string G_y_last_digit_k = curve_k.point_y.substr(curve_k.point_y.length() - 1, 1);
        std::string first_byte_k = (int)hex2dec(G_y_last_digit_k) & 1 ? "03" : "02";
        std::string data = first_byte_k + curve_k.point_x + keypair_index_hex;

        unsigned char data_to_chr[data.length() / 2];
        for (uint8_t i = 0; i < data.length(); i += 2)
        {
            uint8_t byte = hex2dec(data.substr(i, 2));
            data_to_chr[i / 2] = byte;
        }

        hmac_result = hmac_512(parent_chaincode_to_chr, parent_chaincode.length() / 2, data_to_chr, data.length() / 2);
    }

    std::string I_left = hmac_result.substr(0, 64);
    std::string I_right = hmac_result.substr(64, 64);

    std::string chaincode = I_right;

    std::string parent_public_key = parent_node.ecdsa_pub;

    SHA256_Legacy sha256;
    uint8_t parent_public_key_bytes[parent_public_key.length() / 2];
    for (size_t i = 0; i < parent_public_key.length(); i += 2)
        parent_public_key_bytes[i / 2] = hex2dec(parent_public_key.substr(i, 2));
    std::string sha256_parent_public_key = sha256(parent_public_key_bytes, parent_public_key.length() / 2);
    unsigned char sha256_parent_public_key_bytes[sha256_parent_public_key.length() / 2];
    for (uint8_t i = 0; i < sha256_parent_public_key.length(); i += 2)
    {
        uint8_t byte = hex2dec(sha256_parent_public_key.substr(i, 2));
        sha256_parent_public_key_bytes[i / 2] = byte;
    }
    unsigned char *ripemd160_parent_public_key = RIPEMD160(sha256_parent_public_key_bytes, sha256_parent_public_key.length() / 2, NULL);
    std::stringstream ripemd160_stream;
    for (uint8_t i = 0; i < 20; i++)
        ripemd160_stream << std::setfill('0') << std::setw(2) << std::hex << (int)ripemd160_parent_public_key[i];
    std::string ripemd160_parent_public_key_hex = ripemd160_stream.str();

    std::string fingerprint = ripemd160_parent_public_key_hex.substr(0, 8);

    uint512 parsed_I_left("0x" + I_left);
    if (parsed_I_left >= GMODULO || parsed_I_left == 0)
        return BIP32::BIP32NodeResult();
    uint512 parsed_k("0x" + parent_private_key);
    uint512 child_prv_key_decimal = ((parsed_k + parsed_I_left) % GMODULO);
    std::stringstream ss1;
    ss1 << std::setfill('0') << std::setw(64) << std::hex << child_prv_key_decimal;
    std::string child_data_prv = ss1.str();

    // std::cout << "KEYPAIR INDEX: " << keypair_index_hex << std::endl;

    std::string child_ext_private_key_raw = parent_prv_version + depth + fingerprint + keypair_index_hex + chaincode + "00" + child_data_prv;

    uint8_t prv_fro_sha[child_ext_private_key_raw.length() / 2];
    for (size_t i = 0; i < child_ext_private_key_raw.length(); i += 2)
        prv_fro_sha[i / 2] = hex2dec(child_ext_private_key_raw.substr(i, 2));
    std::string round_one = sha256(prv_fro_sha, child_ext_private_key_raw.length() / 2);
    uint8_t round_one_next_sha[round_one.length() / 2];
    for (size_t i = 0; i < round_one.length(); i += 2)
        round_one_next_sha[i / 2] = hex2dec(round_one.substr(i, 2));
    std::string round_two = sha256(round_one_next_sha, round_one.length() / 2);
    std::string child_ext_private_key = child_ext_private_key_raw + round_two.substr(0, 8);

    base58 b58;
    std::vector<uint8_t> child_ext_private_key_bytes;
    for (uint16_t i = 0; i < child_ext_private_key.length(); i += 2)
    {
        uint8_t byte = hex2dec(child_ext_private_key.substr(i, 2));
        child_ext_private_key_bytes.push_back(byte);
    }
    std::string child_ext_private_key_base58 = b58.EncodeBase58(child_ext_private_key_bytes, b58.base58map);

    BIP32::secp256k1_result curve = get_secp256k1(child_data_prv);
    // std::cout << "^POINT X:^ " << curve.point_x << std::endl;
    if (curve.point_x == "")
    {
        return BIP32::BIP32NodeResult();
    }
    std::string G_y_last_digit = curve.point_y.substr(curve.point_y.length() - 1, 1);
    std::string first_byte = (int)hex2dec(G_y_last_digit) & 1 ? "03" : "02";
    std::string ext_pub_raw = parent_pub_version + depth + fingerprint + keypair_index_hex + chaincode + first_byte + curve.point_x;

    uint8_t pub_fro_sha[ext_pub_raw.length() / 2];
    for (size_t i = 0; i < ext_pub_raw.length(); i += 2)
        pub_fro_sha[i / 2] = hex2dec(ext_pub_raw.substr(i, 2));
    std::string round_one_pub = sha256(pub_fro_sha, ext_pub_raw.length() / 2);
    uint8_t round_one_next_sha_pub[round_one_pub.length() / 2];

    for (size_t i = 0; i < round_one_pub.length(); i += 2)
        round_one_next_sha_pub[i / 2] = hex2dec(round_one_pub.substr(i, 2));

    std::string round_two_pub = sha256(round_one_next_sha_pub, round_one_pub.length() / 2);

    std::string ext_pub = ext_pub_raw + round_two_pub.substr(0, 8);

    std::vector<uint8_t> ext_pub_bytes;
    for (uint16_t i = 0; i < ext_pub.length(); i += 2)
    {
        uint8_t byte = hex2dec(ext_pub.substr(i, 2));
        ext_pub_bytes.push_back(byte);
    }
    std::string xpub_base58_result = b58.EncodeBase58(ext_pub_bytes, b58.base58map);

    return BIP32::BIP32NodeResult(depth, keypair_index_hex, child_ext_private_key, ext_pub, child_ext_private_key_base58, xpub_base58_result, first_byte + curve.point_x, child_data_prv);
}

BIP32::secp256k1_result BIP32::get_secp256k1(std::string seed)
{
    EC_KEY *key_pair_obj = nullptr;
    int ret_error;
    BIGNUM *priv_key = NULL;
    // set private key from 32 byte seed
    BN_hex2bn(&priv_key, (seed).c_str());
    EC_POINT *pub_key;
    EC_GROUP *secp256k1_group;
    char *pub_key_char, *priv_key_char;

    // Generate secp256k1 key pair
    key_pair_obj = EC_KEY_new_by_curve_name(NID_secp256k1);
    secp256k1_group = (EC_GROUP *)EC_KEY_get0_group(key_pair_obj);
    pub_key = EC_POINT_new(secp256k1_group);

    // Set private key
    if (!EC_KEY_set_private_key(key_pair_obj, priv_key))
    {
        return BIP32::secp256k1_result("", "", "", "");
    }

    // Calculate public key
    if (!EC_POINT_mul(secp256k1_group, pub_key, priv_key, NULL, NULL, nullptr))
    {
        return BIP32::secp256k1_result("", "", "", "");
    }
    EC_KEY_set_public_key(key_pair_obj, pub_key);

    // priv_key_char = BN_bn2hex(priv_key);
    pub_key_char = EC_POINT_point2hex(secp256k1_group, pub_key, POINT_CONVERSION_COMPRESSED, nullptr);

    BIGNUM *x = BN_new();
    BIGNUM *y = BN_new();
    if (!EC_POINT_get_affine_coordinates(secp256k1_group, pub_key, x, y, NULL))
    {
        BN_free(x);
        BN_free(y);
        EC_GROUP_free(secp256k1_group);
        return BIP32::secp256k1_result("", "", "", "");
    }
    EC_GROUP_free(secp256k1_group);

    BIP32::secp256k1_result sig(seed, BN_bn2hex(x), BN_bn2hex(y), pub_key_char);

    BN_free(x);
    BN_free(y);
    return sig;
}
