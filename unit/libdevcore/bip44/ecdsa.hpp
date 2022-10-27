#pragma once
#include "utils.hpp"

/// @brief Result of signing message with secp256k1 curve. Contains r-value, s-value and messages hash (SHA256)
struct ECDSASignResult
{
    std::string r;
    std::string s;
    std::string message_hash;
    ECDSASignResult(std::string _r, std::string _s, std::string _msghash)
    {
        r = _r;
        s = _s;
        message_hash = _msghash;
    }
};

/// @brief Signing message with private key (not extended)
/// @param message message as string
/// @param prv_key private key in hex representaion
/// @return r value, s value and message hash
ECDSASignResult ecdsa_sign_message(std::string msg, std::string prv_key)
{
    EC_KEY *key_pair_obj = nullptr;
    int ret_error;
    BIGNUM *priv_key = NULL;
    // set private key from 32 byte seed
    BN_hex2bn(&priv_key, (prv_key).c_str());
    // std::cout << "prv to sig: " << BN_bn2hex(priv_key) << std::endl;
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
        EC_KEY_free(key_pair_obj);
        return ECDSASignResult("", "", "");
    }

    // Calculate public key
    if (!EC_POINT_mul(secp256k1_group, pub_key, priv_key, NULL, NULL, nullptr))
    {
        EC_KEY_free(key_pair_obj);
        return ECDSASignResult("", "", "");
    }
    EC_KEY_set_public_key(key_pair_obj, pub_key);

    const char *message = msg.c_str();
    unsigned char buffer_digest[SHA256_DIGEST_LENGTH];
    uint8_t *digest;
    uint8_t *signature;
    uint32_t signature_len;
    int verification;

    // secp256k1_group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    pub_key_char = EC_POINT_point2hex(secp256k1_group, pub_key, POINT_CONVERSION_COMPRESSED, nullptr);
    priv_key_char = BN_bn2hex(priv_key);

    // Sign message
    signature_len = ECDSA_size(key_pair_obj); // the signature size depends on the key
    signature = (uint8_t *)OPENSSL_malloc(signature_len);
    digest = SHA256((const unsigned char *)message, strlen(message), buffer_digest);
    ret_error = ECDSA_sign(0, (const uint8_t *)digest, SHA256_DIGEST_LENGTH, signature, &signature_len, key_pair_obj);

    BIGNUM* bn_sig = BN_new();
    BN_bin2bn(signature, signature_len, bn_sig);
    std::string signature_string(BN_bn2hex(bn_sig));
    BN_free(bn_sig);

    int r_length = hex2dec(signature_string.substr(6, 2)) * 2;
    std::string r_sig = signature_string.substr(8, r_length);
    std::string s_sig = signature_string.substr(8 + r_length + 4, hex2dec(signature_string.substr(8 + r_length + 2, 2)) * 2);

    BIGNUM* bn_digest = BN_new();
    BN_bin2bn(digest, SHA256_DIGEST_LENGTH, bn_digest);
    std::string sha_string(BN_bn2hex(bn_digest));
    BN_free(bn_digest);

    verification = ECDSA_verify(0, digest, SHA256_DIGEST_LENGTH, signature, signature_len, key_pair_obj);

    OPENSSL_free(signature);
    EC_KEY_free(key_pair_obj);

    if (verification == 0)
        return ECDSASignResult("", "", "");

    return ECDSASignResult(r_sig, s_sig, sha_string);
}


/// @brief Method to verify signature with r value, s value, message and wallet address
/// @param r r value from signature. It is X-coord of point R
/// @param s s value from signature. It is signature proof
/// @param message message wich we decide to check
/// @param address address of the wallet signer in base58 encoding
/// @return result of verification (bool)
bool ecdsa_verify_signature(std::string r, std::string s, std::string msg, std::string address)
{
    BN_CTX *bn_ctx = BN_CTX_new();

    //n = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    BIGNUM *nmod_bn = BN_new();
    BN_hex2bn(&nmod_bn, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");

    BIGNUM *r_bn = BN_new();
    BN_hex2bn(&r_bn, r.c_str());

    BIGNUM *s_bn = BN_new();
    BN_hex2bn(&s_bn, s.c_str());

    BIGNUM *r_inversed_bn = BN_new();
    BN_mod_inverse(r_inversed_bn, r_bn, nmod_bn, bn_ctx);

    BIGNUM *s_inversed_bn = BN_new();
    BN_mod_inverse(s_inversed_bn, s_bn, nmod_bn, bn_ctx);

    BIGNUM *z_bn = BN_new();

    BIGNUM *u1_bn = BN_new();
    BIGNUM *u2_bn = BN_new();

    EC_GROUP *secp256k1_group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    EC_POINT *R1_point = EC_POINT_new(secp256k1_group);
    EC_POINT *R2_point = EC_POINT_new(secp256k1_group);
    int err = EC_POINT_set_compressed_coordinates(secp256k1_group, R1_point, r_bn, 0, nullptr);
    int err2 = EC_POINT_set_compressed_coordinates(secp256k1_group, R2_point, r_bn, 1, nullptr);
    int proofr1 = EC_POINT_is_on_curve(secp256k1_group, R1_point, nullptr);
    int proofr2 = EC_POINT_is_on_curve(secp256k1_group, R2_point, nullptr);
    if (!(proofr1 && proofr2))
    {
        std::cout << "proof failed" << std::endl;
    }

    const char *message = msg.c_str();
    unsigned char buffer_digest[SHA256_DIGEST_LENGTH];
    uint8_t *digest;
    digest = SHA256((const unsigned char *)message, strlen(message), buffer_digest);

    BN_bin2bn(digest, SHA256_DIGEST_LENGTH, z_bn);
    BN_set_negative(z_bn, 1);

    BN_mod_mul(u1_bn, z_bn, r_inversed_bn, nmod_bn, bn_ctx);
    BN_mod_mul(u2_bn, s_bn, r_inversed_bn, nmod_bn, bn_ctx);

    EC_POINT *p_PUB1 = EC_POINT_new(secp256k1_group);
    EC_POINT *p_PUB2 = EC_POINT_new(secp256k1_group);


    int err_2 = EC_POINT_mul(secp256k1_group, p_PUB1, u1_bn, R1_point, u2_bn, NULL);

    int err_4 = EC_POINT_mul(secp256k1_group, p_PUB2, u1_bn, R2_point, u2_bn, NULL);

    std::string pub_key_1(EC_POINT_point2hex(secp256k1_group, p_PUB1, POINT_CONVERSION_COMPRESSED, NULL));
    std::string pub_key_2(EC_POINT_point2hex(secp256k1_group, p_PUB2, POINT_CONVERSION_COMPRESSED, NULL));

    Addresses adr;
    std::string address_1 = adr.pub2addr(pub_key_1);
    std::string address_2 = adr.pub2addr(pub_key_2);
    
    BN_free(nmod_bn);
    BN_free(r_bn);
    BN_free(s_bn);
    BN_free(r_inversed_bn);
    BN_free(s_inversed_bn);
    BN_free(z_bn);
    BN_free(u1_bn);
    BN_free(u2_bn);
    BN_CTX_free(bn_ctx);
    EC_GROUP_free(secp256k1_group);
    EC_POINT_free(R1_point);
    EC_POINT_free(R2_point);

    return (address == address_1 || address == address_2);
}

