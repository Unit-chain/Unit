extern crate ed25519_dalek;
extern crate rand;
extern crate ed25519;
extern crate hex;
extern crate hmac;

use hex_literal::hex;
use sha3::{Digest, Sha3_384, Sha3_512};
use ed25519_dalek::*;
use std::time::{SystemTime, UNIX_EPOCH};
use hmac::{Hmac, Mac};
use binascii::b32decode;
use bip32::{Mnemonic, Prefix, XPrv};
use rand_core::OsRng;
type HmacSha256 = Hmac<Sha3_512>;


fn main() {
    // println!("Hello, world!");
    // let mut csprng = OsRng{};
    // let keypair: Keypair = Keypair::generate(&mut csprng);
    // let public_key_bytes: [u8; PUBLIC_KEY_LENGTH] = keypair.public.to_bytes();
    // println!("{:?}", public_key_bytes);

    // /// Generates an English mnemonic with 12 words randomly
    // let mnemonic = Mnemonic::generate(Count::Words12);

    // /// Gets the phrase
    // let phrase = mnemonic.phrase();
    // println!("phrase: {}", phrase);

    // println!("seed: {:?}", hex::encode(mnemonic.to_seed("1111")));

    // let mut mac = HmacSha256::new_from_slice(hex::encode(mnemonic.to_seed("1111")).as_bytes())
    // .expect("HMAC can take key of any size");
    // mac.update(b"Bitcoin seed");

    // // `result` has type `CtOutput` which is a thin wrapper around array of
    // // bytes for providing constant time equality check
    // let result = mac.finalize();
    // let code_bytes = result.into_bytes();
    // println!("hex: {:?}", hex::encode(code_bytes));
    // println!("bytes: {:?}", code_bytes);


    // let mut output_buffer = [0u8; 200];
    // let message = "MJUW4YLTMNUWSLLSOMQGS4ZAORUGKIDCMVZXIII=";

    // let result = b32decode(&message.as_bytes(), &mut output_buffer).ok().unwrap();
    // println!("result {:?}", result);
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

// Generate random Mnemonic using the default language (English)
let mnemonic = Mnemonic::random(&mut OsRng, Default::default());

// Derive a BIP39 seed value using the given password
let seed = mnemonic.to_seed("password");

// Derive the root `XPrv` from the `seed` value
let root_xprv = XPrv::new(&seed);

// Derive a child `XPrv` using the provided BIP32 derivation path
let child_path = "m/0/2147483647'/1/2147483646'";
let child_xprv = XPrv::derive_from_path(&seed, &child_path.parse().unwrap()).unwrap();

// Get the `XPub` associated with `child_xprv`.
let child_xpub = child_xprv.public_key();

// Serialize `child_xprv` as a string with the `xprv` prefix.
let child_xprv_str = child_xprv.to_string(Prefix::XPRV);
assert!(child_xprv_str.starts_with("xprv"));

// Serialize `child_xpub` as a string with the `xpub` prefix.
let child_xpub_str = child_xpub.to_string(Prefix::XPUB);
assert!(child_xpub_str.starts_with("xpub"));

// Get the ECDSA/secp256k1 signing and verification keys for the xprv and xpub
let signing_key = child_xprv.private_key();
let verification_key = child_xpub.public_key();

println!("public key{:?}", hex::encode(signing_key.to_bytes()));
// Sign and verify an example message using the derived keys.
use bip32::secp256k1::ecdsa::{
    signature::{Signer, Verifier},
    Signature
};

let example_msg = b"Hello, world!";
let signature: Signature = signing_key.sign(example_msg);
assert!(verification_key.verify(example_msg, &signature).is_ok());

}
