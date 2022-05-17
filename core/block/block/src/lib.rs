use std::time::{SystemTime, UNIX_EPOCH};
use sha3::Sha3_384;
#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}


pub mod blockimplementation {
    struct Block{
        pub date: u128,
        pub index: u64,
        pub hash: String,
        pub tx: Vec<Transaction>,
        pub previous: String
    }

    pub impl Block{ 
        fn new(index: &mut i64, tx: &mut Vec<Transaction>) -> Block { 
            Block {
                date: SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).ok().unwrap().as_millis(),
                index: index,
                hash: 
                tx: tx,
            }
        }

        fn generate_hash(txs: &mut Vec<Transaction>) -> String {
            let bytes_vector: Vec<u8> = Vec::new();
            for tx in txs { 
                bytes_vector.push();
            }
            let hash: String = String::from("");
            for byte in bytes_vector {
                hash = format!("{}{}", &hash, String::from_utf8(byte));
            }
            let mut hasher = Sha3_384::new();
            
        }
    }
}