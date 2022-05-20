#[cfg(test)]
mod tests {
    use super::MnemonicUnitGenerator::*;
    // #[test]
    // fn it_works() {
    //     let result = 2 + 2;
    //     assert_eq!(result, 4);
    // }

    #[test]
    fn generate_key_base58() {
        let mut m_24: String = generate_mnemonic_24();
        let mut seed: String = String::from("3aacab2287f1ff822a61c332e76fc6d27eb114e5466ff1f64b3c6c934e2af86702dff0f12420463034b4b9b58d43389da6aedafadba6b3a883d6f86f4f7803c0");
        // println!("{:?}", seed.as_bytes());
        println!("{:?}", std::str::from_utf8(seed.as_bytes()));
        let mut hmac512_str = mnemonic_hmac_512(&mut seed);
        let mut left = get_left(&mut hmac512_str);
        let mut right = get_right(&mut hmac512_str);
        let base58: String = get_base_58(&mut left, &mut right);
        println!("{:?}", base58);
    }
}

extern crate rand;
extern crate hex;
extern crate base58;

pub mod MnemonicUnitGenerator {
    use std::io::Cursor;
    use std::str::FromStr;
    use bip0039::{Count, Language, Mnemonic};
    use hmac::{Hmac, Mac};
    use sha3::{Digest, Sha3_384, Sha3_512};
    use byteorder::{ReadBytesExt, WriteBytesExt, BigEndian, LittleEndian};
    use base58::ToBase58;
    type HmacSha3_512 = Hmac<Sha3_512>;

    pub fn generate_mnemonic_12() -> String {
        let mnemonic = Mnemonic::generate(Count::Words12);
        String::from(mnemonic.phrase())
    }

    pub fn generate_mnemonic_24() -> String {
        let mnemonic = Mnemonic::generate(Count::Words24);
        String::from(mnemonic.phrase())
    }

    pub fn seed_from_mnemonic(phrase: &mut String, password: &mut String) -> String { 
        let mnemonic = Mnemonic::from_phrase_in(Language::English, phrase.as_str()).unwrap();
        hex::encode(mnemonic.to_seed(password))
    }

    pub fn mnemonic_hmac_512(seed: &mut String) -> String { // not work seed
        let mut mac = HmacSha3_512::new_from_slice(hex::encode(seed.as_str()).as_bytes()).expect("HMAC can take key of any size");
        mac.update(b"Bitcoin seed");
        let result = mac.finalize();
        let code_bytes = result.into_bytes();
        hex::encode(code_bytes)
    }

    pub fn get_left(mnemonic_hmac_512: &mut String) -> String {
        let phrase: String = String::from(mnemonic_hmac_512.as_str());
        String::from(&phrase[0..32])
    }

    pub fn get_right(mnemonic_hmac_512: &mut String) -> String { // master_chain_code
        let phrase: String = String::from(mnemonic_hmac_512.as_str());
        String::from(&phrase[32..64])
    }

    pub fn get_master_private_key(left_mnemonic: &mut String) -> u32 {
        let mut rdr = Cursor::new(left_mnemonic.as_bytes());
        rdr.read_u32::<BigEndian>().unwrap()
        // to_be_bytes() - maybe should use later
    }

    pub fn get_base_58(left_mnemonic: &String, master_chain_code: &String) -> String {
        const NET_VERSION: &str = "0488b21b";
        let depth_byte: &[u8] = b"\x00";
        let parent_fingerprint: &[u8; 4] = b"\x00\x00\x00\x00";
        let child_number_bytes: &[u8; 4]  = b"\x00\x00\x00\x00";
        let mut phrase: Vec<u8> = left_mnemonic.clone().as_bytes().to_vec();
        phrase.insert(0, b"\x00"[0]);
        let mut concat_all: Vec<u8> = Vec::new();
        concat_all.extend(hex::decode(NET_VERSION).ok().unwrap());
        concat_all.extend_from_slice(depth_byte);
        concat_all.extend_from_slice(parent_fingerprint);
        concat_all.extend_from_slice(child_number_bytes);
        concat_all.extend_from_slice(String::from(master_chain_code).as_bytes());
        concat_all.extend(phrase);
        concat_all.as_slice().to_base58()
    }
}
