#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}
pub trait Serializable {
    fn serialize(&self) -> String {
        String::from("(Read more...)")
    }
}

impl Serializable for transaction::Transaction {
    fn serialize(&self) -> String {
        String::from(serde_json::to_string(&self).unwrap())
    }
}

// pub fn stacked_transactions<'a>(
//     transaction: transaction::Transaction,
//     stack: &'a transaction::Stack<transaction::Transaction>,
// ) {

// }

#[path = "../../block/src/lib.rs"]
mod blockimplementation;

extern crate hex;
pub mod transaction {
    use super::blockimplementation;
    pub use serde::{Deserialize, Serialize}; //To serialize/deserialize a transaction
    use sha3::{Digest, Sha3_384}; //To hash transaction struct
    pub struct StackUnit<T> {
        pub stack: Vec<T>,
    }

    impl<tr: transaction::Transaction> StackUnit<tr: transaction::Transaction> {
        pub fn new() -> Self {
            StackUnit { stack: Vec::new() }
        }
        pub fn length(&self) -> usize {
            self.stack.len()
        }
        pub fn pop(&mut self) -> Option<tr: transaction::Transaction> {
            self.stack.pop()
        }
        pub fn push(&mut self, item: transaction::Transaction) {
            self.stack.push(item)
        }
        pub fn is_empty(&self) -> bool {
            self.stack.is_empty()
        }
        pub fn peek(&self) -> Option<&transaction::Transaction> {
            self.stack.last()
        }
        pub fn pop_all(&self) -> &mut Vec<String> {}
        // pub fn to_block(&self, index: u64, previous: String) -> Block {
        //     blockimplementation::new(index, )
        // }
    }

    #[derive(Serialize, Deserialize)]
    pub struct NameValue {
        pub name: String,
        pub value: u64,
    }
    #[derive(Serialize, Deserialize)]
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
        pub blockid: u128,
        pub epoch: u64,
    }
    impl Transaction {
        fn new_forgerie_transaction(
            hash: String,
            data: u128,
            from: String,
            to: String,
            amount: u128,
            tx_type: u64,
            typevalue: NameValue,
            sign: String,
            previous: String,
            blockid: u128,
            epoch: u64,
        ) -> Transaction {
            Transaction {
                hash: String::from("some haaash"),
                data: 123456u128,
                from: String::from("From your mom!"),
                to: String::from("To your mom!"),
                amount: 100000000u128,
                tx_type: 2u64,
                typevalue: NameValue {
                    name: String::from("Name"),
                    value: 6u64,
                },
                sign: String::from("BIG SIIIIIGN"),
                previous: String::from("some sign, meh"),
                blockid: 4u128,
                blockid: 5u64,
            }
        }
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
            blockid: u128,
            epoch: u64,
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
                blockid,
                epoch,
            }
        }
        pub fn hash(&self) -> String {
            let mut hasher = Sha3_384::new();

            // write input message
            hasher.update(format!(
                "{}{}{}{}{}{}{}{}{}{}{}{}",
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
                self.epoch,
                self.blockid
            ));
            // read hash digest
            format!("hash {:?}", hex::encode(hasher.finalize()))
        }

        // fn sign(&self, )
    }
    pub fn deserialize_transaction(transaction: String) -> Transaction {
        let transaction: Transaction = serde_json::from_str(&transaction).unwrap();
        transaction
    }
}
