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

struct WalletUtils {
    keyPair: KeyPair
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
}