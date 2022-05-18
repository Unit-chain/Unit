extern crate ed25519_dalek;
extern crate rand;
extern crate ed25519;
extern crate hex;
extern crate hmac;

// use hex_literal::hex;
use sha3::{Digest, Sha3_384, Sha3_512};
use rand::rngs::OsRng;
use ed25519_dalek::*;
use bip0039::{Count, Language, Mnemonic};
use std::time::{SystemTime, UNIX_EPOCH};
use hmac::{Hmac, Mac};
type HmacSha256 = Hmac<Sha3_512>;

fn main() {
    println!("Hello, world!");
    let mut csprng = OsRng{};
    let keypair: Keypair = Keypair::generate(&mut csprng);
    let public_key_bytes: [u8; PUBLIC_KEY_LENGTH] = keypair.public.to_bytes();
    println!("{:?}", public_key_bytes);

    /// Generates an English mnemonic with 12 words randomly
    let mnemonic = Mnemonic::generate(Count::Words12);

    /// Gets the phrase
    let phrase = mnemonic.phrase();
    println!("phrase: {}", phrase);

    println!("seed: {:?}", hex::encode(mnemonic.to_seed("1111")));

    let mut mac = HmacSha256::new_from_slice(hex::encode(mnemonic.to_seed("1111")).as_bytes())
    .expect("HMAC can take key of any size");
    mac.update(b"Bitcoin seed");

    // `result` has type `CtOutput` which is a thin wrapper around array of
    // bytes for providing constant time equality check
    let result = mac.finalize();
    let code_bytes = result.into_bytes();
    println!("hex: {:?}", hex::encode(code_bytes));
    println!("bytes: {:?}", code_bytes);
    // let string: String = hex::encode(keypair.secret.to_bytes());

    // println!("{:?}", string);
    // println!("private key bytes before encoding {:?}", keypair.secret.to_bytes());
    // println!("private key bytes after decoding {:?}", hex::decode(string).ok().unwrap());

    // let strintest: String = String::from("112");

    // let mut hasher = Sha3_384::new();
    // hasher.update(strintest.as_bytes());
    // println!("hash {:?}", hex::encode(hasher.finalize()));
    // /// Generates the HD wallet seed from the mnemonic and the passphrase.
    // let seed = mnemonic.to_seed("123");
    // println!("seed: {}", hex::encode(&seed[..]));
    // /// Generates a Simplified Chinese mnemonic with 12 words randomly
    // let mnemonic = Mnemonic::generate_in(Language::English, Count::Words12);
    // println!("phrase: {}", mnemonic.phrase());

    // let valid_keypair = pkcs8::KeypairBytes {
    //     secret_key: keypair.secret.to_bytes(),
    //     public_key: Some(keypair.public.to_bytes()),
    // };

    // println!("key bytes {:?}", valid_keypair.to_bytes().unwrap());

    // println!("{:?}", std::str::from_utf8(&keypair.secret.to_bytes()));
}
