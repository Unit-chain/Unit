#include "BIP39.hpp"
#include "BIP32.hpp"
#include "addresses.hpp"

#define PHRASE_12 12
#define PHRASE_24 24
#define UNIT_COINTYPE 992
#define INTERNAL_CHANGE 1
#define EXTERNAL_CHANGE 0

struct BIP44Result
{
    std::string path;
    std::string extended_prv;
    std::string extended_pub;
    std::string prv;
    std::string pub;
    std::string address;
    BIP39Result mnemonic;
    std::string error;
    BIP44Result()
    {
        error = "unknown";
    }
    BIP44Result(std::string err)
    {
        error = err;
    }
    BIP44Result(std::string _path, std::string _ext_prv, std::string _ext_pub, std::string _prv, std::string _pub, std::string _address, BIP39Result _bip39)
    {
        path = _path;
        extended_prv = _ext_prv;
        extended_pub = _ext_pub;
        prv = _prv;
        pub = _pub;
        address = _address;
        mnemonic = _bip39;
    }
};

class BIP44
{
private:
public:
    /// @brief Generates BIP44 wallet WITHOUT additional user password.
    /// @param len Passphrase length expected. Please, use macroses PHRASE_12 and PHRASE_24 -  it will be more readable.
    /// @param account Account number. Max value is 65535.
    /// @param change Account type. Please, use macros EXTERNAL_CHANGE for external wallet and INTERNAL_CHANGE for internal wallet - it will be more readable.
    /// @return
    BIP44Result generateWallet(int len, uint16_t account, unsigned char change);

    /// @brief Generates BIP44 wallet WITH additional user password.
    /// @param len Passphrase length expected. Please, use macroses PHRASE_12 and PHRASE_24 -  it will be more readable.
    /// @param account Account number. Max value is 65535.
    /// @param change Account type. Please, use macros EXTERNAL_CHANGE for external wallet and INTERNAL_CHANGE for internal wallet - it will be more readable.
    /// @param password User password for safety.
    /// @return
    BIP44Result generateWallet(int len, uint16_t account, unsigned char change, std::string password);

    /// @brief Generates new address for provided account number.
    /// @param mnemonic Mnemonic for generation. It can be obtained from wallet generation 'mnemonic' field.
    /// @param account Account number. For this account will be generated new address.
    /// @param change Account type. Please, use macros EXTERNAL_CHANGE for external wallet and INTERNAL_CHANGE for internal wallet - it will be more readable.
    /// @param address_index Index of the new address.
    /// @return
    BIP44Result generateAddress(BIP39Result mnemonic, uint16_t account, unsigned char change, uint16_t address_index);
};

BIP44Result BIP44::generateWallet(int len, uint16_t account, unsigned char change)
{
    if (len != PHRASE_12 && len != PHRASE_24)
        return BIP44Result("BAD PHRASE");

    if (change != INTERNAL_CHANGE && change != EXTERNAL_CHANGE)
        return BIP44Result("BAD CHANGE");

    BIP39 mnemonic = BIP39();
    BIP39Result mnemonic_code;

    if (len == PHRASE_12)
        mnemonic_code = mnemonic.generateMnemonic_12("");
    else
        mnemonic_code = mnemonic.generateMnemonic("");

    BIP32 bip32 = BIP32();

    BIP32::BIP32NodeResult m_node = bip32.gen_master(mnemonic_code.seed);
    if (m_node.xprv_base58 == "")
        return BIP44Result("BAD MASTER");

    BIP32::BIP32NodeResult purpose_node = bip32.gen_child_from_xprv(m_node, HARDENED_VALUE + 44, "01");
    if (purpose_node.xprv_base58 == "")
        return BIP44Result("BAD PUPROSE");

    BIP32::BIP32NodeResult coin_node = bip32.gen_child_from_xprv(purpose_node, HARDENED_VALUE + UNIT_COINTYPE, "02");
    if (coin_node.xprv_base58 == "")
        return BIP44Result("BAD COINTYPE");

    BIP32::BIP32NodeResult account_node = bip32.gen_child_from_xprv(coin_node, HARDENED_VALUE + account, "03");
    if (account_node.xprv_base58 == "")
        return BIP44Result("BAD ACCOUNT");

    BIP32::BIP32NodeResult change_node = bip32.gen_child_from_xprv(account_node, change, "04");
    if (change_node.xprv_base58 == "")
        return BIP44Result("BAD CHANGE NODE");

    BIP32::BIP32NodeResult address_node = bip32.gen_child_from_xprv(change_node, 0, "05");
    if (address_node.xprv_base58 == "")
        return BIP44Result("BAD ADDRESS");

    Addresses adr;
    std::string wallet_address = adr.pub2addr(address_node.ecdsa_pub);

    return BIP44Result("m/44'/992'/" + std::to_string(account) + "'/" + std::to_string(change) + "/0",
                       address_node.xprv_base58,
                       address_node.xpub_base58,
                       address_node.ecdsa_prv,
                       address_node.ecdsa_pub,
                       wallet_address,
                       mnemonic_code);
}

BIP44Result BIP44::generateWallet(int len, uint16_t account, unsigned char change, std::string password)
{
    if (len != PHRASE_12 && len != PHRASE_24)
        return BIP44Result("BAD PHRASE");

    if (change != INTERNAL_CHANGE && change != EXTERNAL_CHANGE)
        return BIP44Result("BAD CHANGE");

    BIP39 mnemonic = BIP39();
    BIP39Result mnemonic_code;

    if (len == PHRASE_12)
        mnemonic_code = mnemonic.generateMnemonic_12(password);
    else
        mnemonic_code = mnemonic.generateMnemonic(password);

    BIP32 bip32 = BIP32();

    BIP32::BIP32NodeResult m_node = bip32.gen_master(mnemonic_code.seed);
    if (m_node.xprv_base58 == "")
        return BIP44Result("BAD MASTER");

    BIP32::BIP32NodeResult purpose_node = bip32.gen_child_from_xprv(m_node, HARDENED_VALUE + 44, "01");
    if (purpose_node.xprv_base58 == "")
        return BIP44Result("BAD PUPROSE");

    BIP32::BIP32NodeResult coin_node = bip32.gen_child_from_xprv(purpose_node, HARDENED_VALUE + UNIT_COINTYPE, "02");
    if (coin_node.xprv_base58 == "")
        return BIP44Result("BAD COINTYPE");

    BIP32::BIP32NodeResult account_node = bip32.gen_child_from_xprv(coin_node, HARDENED_VALUE + account, "03");
    if (account_node.xprv_base58 == "")
        return BIP44Result("BAD ACCOUNT");

    BIP32::BIP32NodeResult change_node = bip32.gen_child_from_xprv(account_node, change, "04");
    if (change_node.xprv_base58 == "")
        return BIP44Result("BAD CHANGE NODE");

    BIP32::BIP32NodeResult address_node = bip32.gen_child_from_xprv(change_node, 0, "05");
    if (address_node.xprv_base58 == "")
        return BIP44Result("BAD ADDRESS");

    Addresses adr;
    std::string wallet_address = adr.pub2addr(address_node.ecdsa_pub);

    return BIP44Result("m/44'/992'/" + std::to_string(account) + "'/" + std::to_string(change) + "/0",
                       address_node.xprv_base58,
                       address_node.xpub_base58,
                       address_node.ecdsa_prv,
                       address_node.ecdsa_pub,
                       wallet_address,
                       mnemonic_code);
}

BIP44Result BIP44::generateAddress(BIP39Result mnemonic, uint16_t account, unsigned char change, uint16_t address_index) 
{
    BIP32 bip32 = BIP32();
    BIP32::BIP32NodeResult m_node = bip32.gen_master(mnemonic.seed);
    if (m_node.xprv_base58 == "")
        return BIP44Result("BAD MASTER");

    BIP32::BIP32NodeResult purpose_node = bip32.gen_child_from_xprv(m_node, HARDENED_VALUE + 44, "01");
    if (purpose_node.xprv_base58 == "")
        return BIP44Result("BAD PUPROSE");

    BIP32::BIP32NodeResult coin_node = bip32.gen_child_from_xprv(purpose_node, HARDENED_VALUE + UNIT_COINTYPE, "02");
    if (coin_node.xprv_base58 == "")
        return BIP44Result("BAD COINTYPE");

    BIP32::BIP32NodeResult account_node = bip32.gen_child_from_xprv(coin_node, HARDENED_VALUE + account, "03");
    if (account_node.xprv_base58 == "")
        return BIP44Result("BAD ACCOUNT");

    BIP32::BIP32NodeResult change_node = bip32.gen_child_from_xprv(account_node, change, "04");
    if (change_node.xprv_base58 == "")
        return BIP44Result("BAD CHANGE NODE");

    BIP32::BIP32NodeResult address_node = bip32.gen_child_from_xprv(change_node, address_index, "05");
    if (address_node.xprv_base58 == "")
        return BIP44Result("BAD ADDRESS");

    Addresses adr;
    std::string wallet_address = adr.pub2addr(address_node.ecdsa_pub);

    return BIP44Result("m/44'/992'/" + std::to_string(account) + "'/" + std::to_string(change) + "/"+std::to_string(address_index),
                       address_node.xprv_base58,
                       address_node.xpub_base58,
                       address_node.ecdsa_prv,
                       address_node.ecdsa_pub,
                       wallet_address,
                       mnemonic);
}