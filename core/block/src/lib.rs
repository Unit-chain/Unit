extern crate hex;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}

pub use serde::{Deserialize, Serialize};
pub use transaction;
pub use serializer;

pub mod blockimplementation {
    use sha3::{Digest, Sha3_384};
    use std::time::SystemTime;
    pub use crate::transaction::transaction::Transaction;
    pub use super::{Deserialize, Serialize};

    #[derive(Serialize, Deserialize)]
    pub struct Block{
        pub net_version: u64,
        pub date: u128,
        pub index: u64,
        pub hash: String,
        pub tx: Vec<Transaction>,
        pub previous: String
    }

    impl Block{ 
        pub fn new(net_version: u64, index: u64, previous: String) -> Block { 
            Block {
                net_version: net_version,
                date: SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).ok().unwrap().as_millis(),
                index: index,
                hash: String::from(""),
                tx: Vec::new(),
                previous: previous
            }
        }

        pub fn generate_hash(&mut self, txs: &mut Vec<Transaction>) -> () {
            let mut hash: String = String::from("");
            for tx in txs { 
                hash = format!("{}{}", &hash, tx.hash);
            }
            let mut hasher = Sha3_384::new();
            hasher.update(hash.as_bytes());
            self.hash = hex::encode(hasher.finalize());
        }

        pub fn push_transaction(&mut self, transaction: &mut Transaction) -> () {
            self.tx.push(transaction.clone());
        }

        fn serialize(&self) -> String {
            String::from(serde_json::to_string(&self).unwrap())
        }
    }
    pub fn deserialize_block(block: String) -> Block {
        let block: Block = serde_json::from_str(&block).unwrap();
        block
    }
}