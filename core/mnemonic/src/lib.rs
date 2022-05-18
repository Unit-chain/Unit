#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}

extern crate rand;
extern crate hex;

mod MnemonicUnitGenerator {
    use std::io::Cursor;
    use std::collections::HashMap;
    use bip0039::{Count, Language, Mnemonic};
    use rand::rngs::OsRng;
    use hmac::{Hmac, Mac};
    use sha3::{Digest, Sha3_384, Sha3_512};
    use byteorder::{ReadBytesExt, WriteBytesExt, BigEndian, LittleEndian};
    type HmacSha256 = Hmac<Sha3_512>;

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

    pub fn mnemonic_hmac_512(seed: &mut String) -> String {
        let mut mac = HmacSha256::new_from_slice(hex::encode(seed.as_str()).as_bytes()).expect("HMAC can take key of any size");
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
    }

    // pub fn get_base_58() -> String {
    //     let mut scores = HashMap::new();

    // }
}
