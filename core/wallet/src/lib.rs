#[path = "../../serializer/src/lib.rs"]
pub mod Serializer;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}


extern crate ed25519_dalek;
extern crate rand;
use sha3::Sha3_512;
use hex_literal::hex;


struct Wallet{
    keyPair: KeyPair,
    publicKey: PublicKey,
    privateKey: PrivateKey,
    address: String
}


impl Wallet {
    fn new(keyPair: PrivateKey, publicKey: PublicKey, privateKey: PrivateKey, address: String) -> Wallet {
        Wallet{
            keyPair,
            publicKey,
            privateKey,
            address
        }
    }

    fn generate() -> Wallet{
        let mut csprng = OsRng{};
        let keypair: Keypair = Keypair::generate(&mut csprng);
        Wallet {
            keyPair: keypair,
            publicKey: keypair.public,
            privateKey: keypair.secret,
            address: keypair.public.
        }
    }

    fn getPrivateKeyAsString(KeyPair) {
        
    }

    fn generateMnemonicPhrase(length: i8, lang: &Language) -> &str {
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
}