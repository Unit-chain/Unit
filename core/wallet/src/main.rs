extern crate ed25519_dalek;
extern crate rand;

use std::str::FromStr;
use sha3::Sha3_512;
use hex_literal::hex;
use rand::rngs::OsRng;
use ed25519_dalek::*;
use bip0039::{Count, Language, Mnemonic};

fn main() {
    println!("Hello, world!");
    let mut csprng = OsRng{};
    let keypair: Keypair = Keypair::generate(&mut csprng);
    let public_key_bytes: [u8; PUBLIC_KEY_LENGTH] = keypair.public.to_bytes();
    println!("{:?}", public_key_bytes);

    /// Generates an English mnemonic with 12 words randomly
    let mnemonic = Mnemonic::generate(Count::Words24);

    /// Gets the phrase
    let phrase = mnemonic.phrase();
    println!("phrase: {}", phrase);

    /// Generates the HD wallet seed from the mnemonic and the passphrase.
    let seed = mnemonic.to_seed("123");
    println!("seed: {}", hex::encode(&seed[..]));

    /// Generates a Simplified Chinese mnemonic with 12 words randomly
    let mnemonic = Mnemonic::generate_in(Language::English, Count::Words12);
    println!("phrase: {}", mnemonic.phrase());

    let mnem = Mnemonic::from_str(&*"123".to_string());

    println!("from string: {}", mnemonic.phrase());
}
