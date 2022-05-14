extern crate ed25519_dalek;
extern crate rand;
use sha3::Sha3_512;
use hex_literal::hex;
use bip0039::{Count, Language, Mnemonic};

fn getPrivateKeyAsString() {}

fn generateMnemonicPhrase(length: i8, lang: &Language) -> String {
    if length == 12 {
        let mnemonic: Mnemonic = Mnemonic::generate_in(lang, Count::Words12);
        mnemonic.phrase()
    } else if length == 24 {
        let mnemonic: Mnemonic = Mnemonic::generate_in(lang, Count::Words24);
        mnemonic.phrase()
    } else {
        panic!("Phrase should be only 12 or 24 words length");
    }
}

fn generateSeedFromMnemonic(mnemonic: &Mnemonic, passphrase: &str) -> [u8; 64] {
    mnemonic.to_seed(passphrase)
}