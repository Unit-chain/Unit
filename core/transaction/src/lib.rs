#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}
extern crate hex;
pub mod transaction {
    use sha3::{Digest, Sha3_384};

    pub struct NameValue {
        pub name: String,
        pub value: u64,
    }
    pub struct Transaction {
        pub hash: String,
        pub data: u128,
        pub from: String,
        pub to: String,
        pub amount: u128,
        pub tx_type: u64,
        pub typevalue: NameValue,
        pub sign: String,
        pub previous: String,
    }
    impl Transaction {
        pub fn new(
            hash: String,
            data: u128,
            from: String,
            to: String,
            amount: u128,
            tx_type: u64,
            typevalue: NameValue,
            sign: String,
            previous: String,
        ) -> Transaction {
            Transaction {
                hash,
                data,
                from,
                to,
                amount,
                tx_type,
                typevalue,
                sign,
                previous,
            }
        }
        pub fn hash(&self) -> String {
            let mut hasher = Sha3_384::new();

            // write input message
            hasher.update(format!(
                "{}{}{}{}{}{}{}{}{}{}",
                self.amount,
                self.data,
                self.from,
                self.hash,
                self.previous,
                self.sign,
                self.to,
                self.tx_type,
                self.typevalue.name,
                self.typevalue.value,
            ));
            // read hash digest
            format!("hash {:?}", hex::encode(hasher.finalize()))
        }
    }
}
