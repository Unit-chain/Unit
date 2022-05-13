extern crate ed25519_dalek;
extern crate rand;
use sha3::Sha3_512;
use hex_literal::hex;

use rand::rngs::OsRng;
use ed25519_dalek::*;

fn main() {
    println!("Hello, world!");
    let mut csprng = OsRng{};
    let keypair: Keypair = Keypair::generate(&mut csprng);
    let public_key_bytes: [u8; PUBLIC_KEY_LENGTH] = keypair.public.to_bytes();
    println!("{:?}", public_key_bytes);
}
