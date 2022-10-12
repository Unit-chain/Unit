#include "BIP44.hpp"

using namespace std;

int main()
{
    // BIP39 bip = BIP39();
    // BIP39Result mnemonic24 = bip.generateMnemonic("");
    // cout << "-----24 mnemonic-----" << endl;
    // cout << "Input passphrase: " << endl;
    // cout << "Phrase: " << mnemonic24.mnemonic << endl;
    // cout << "Seed: " << "2977f2a6a3baea099f2e1531f3d8f2fa4a5f25ffd5d2cd9de478e4c6cfa856d5859a0960d7c019450447fb9f8ee4003654533d96b60d1210c5baefdddd53097a" << endl;
    // BIP39Result mnemonic12 = bip.generateMnemonic_12("");
    // cout << endl << "-----12 mnemonic-----" << endl;
    // cout << "Input passphrase: " << endl;
    // cout << "Phrase: " << mnemonic12.mnemonic << endl;
    // cout << "Seed: " << mnemonic12.seed << endl;

    // BIP39Result mnemonic24_pass = bip.generateMnemonic("aboba");
    // cout << endl << "-----24 mnemonic-----" << endl;
    // cout << "Input passphrase: aboba" << endl;
    // cout << "Phrase: " << mnemonic24_pass.mnemonic << endl;
    // cout << "Seed: " << mnemonic24_pass.seed << endl;
    // BIP39Result mnemonic12_pass = bip.generateMnemonic_12("aboba");
    // cout << endl << "-----12 mnemonic-----" << endl;
    // cout << "Input passphrase: aboba" << endl;
    // cout << "Phrase: " << mnemonic12_pass.mnemonic << endl;
    // cout << "Seed: " << mnemonic12_pass.seed << endl;

    // BIP32 bip32 = BIP32();
    // BIP32::BIP32NodeResult res = bip32.gen_master("2977f2a6a3baea099f2e1531f3d8f2fa4a5f25ffd5d2cd9de478e4c6cfa856d5859a0960d7c019450447fb9f8ee4003654533d96b60d1210c5baefdddd53097a");
    // cout << endl << "xprv hex: " << res.hex_prv_key << endl;
    // cout << endl << "xprv base58: " << res.xprv_base58 << endl;
    // cout << endl << "xpub hex: " << res.hex_pub_key << endl;
    // cout << endl << "xpub base58: " << res.xpub_base58 << endl;
    // BIP32::BIP32NodeResult res2 = bip32.gen_child_from_xprv(res, 0, "01");
    // BIP32::BIP32NodeResult res3 = bip32.gen_child_from_xprv(res2, 0, "02");
    // BIP32::BIP32NodeResult res4 = bip32.gen_child_from_xprv(res3, 0, "03");

    BIP44 bip44;
    BIP44Result r = bip44.generateWallet(PHRASE_24, 0, EXTERNAL_CHANGE);
    cout << "WALLET PATH: " << r.path << endl;
    cout << "WALLET SEED: " << r.mnemonic.seed << endl;
    cout << "WALLET E_XPRV: " << r.extended_prv << endl;
    cout << "WALLET E_XPUB: " << r.extended_pub << endl;
    cout << "WALLET ADDRESS: " << r.address << endl;

    BIP44Result r2 = bip44.generateAddress(r.mnemonic, 0, EXTERNAL_CHANGE, 1);
    cout << "WALLET PATH: " << r2.path << endl;
    cout << "WALLET SEED: " << r2.mnemonic.seed << endl;
    cout << "WALLET E_XPRV: " << r2.extended_prv << endl;
    cout << "WALLET E_XPUB: " << r2.extended_pub << endl;
    cout << "WALLET ADDRESS: " << r2.address << endl;

    return 0;
}