#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
    use super::transaction;
    #[test]
    fn new_transaction() {
        let tx = transaction::new_forgerie_transaction(222u128);
        assert_eq!(format!("{}", tx), "hash = some haaash\ndata = 222\nfrom = From your mom!\nto = To your mom!\namount = 100000000\ntx_type = 2\nsign = BIG SIIIIIGN\nprevious = some sign, meh\nblockID = 4\nepoch = 5\n")
    }

    #[test]
    fn serializable_transaction() {
        let tx = transaction::new_forgerie_transaction(232u128);
        assert_eq!(
            format!("{}", tx.serialize()),
            r#"{"hash":"some haaash","data":232,"from":"From your mom!","to":"To your mom!","amount":100000000,"tx_type":2,"typevalue":{"name":"Name","value":6},"sign":"BIG SIIIIIGN","previous":"some sign, meh","blockid":4,"epoch":5}"#
        );
    }

    #[test]
    fn deserialize_transaction() {
        let tx = transaction::new_forgerie_transaction(232u128);
        assert_eq!(
            tx,
            transaction::deserialize_transaction(String::from(
                r#"{
            "hash": "some haaash",
            "data": 232,
            "from": "From your mom!",
            "to": "To your mom!",
            "amount": 100000000,
            "tx_type": 2,
            "typevalue": {
              "name": "Name",
              "value": 6
            },
            "sign": "BIG SIIIIIGN",
            "previous": "some sign, meh",
            "blockid": 4,
            "epoch": 5
          }
          "#
            ))
        );
    }

    #[test]
    fn hash_transaction() {
        let tx = transaction::new_forgerie_transaction(232u128);
        use super::*;
        println!("{}", tx.serialize());
        assert_eq!(tx.hash(), "65e4037cba825c093499b6df3f26cf8bd8255e594836aec1f3355a1a656b014e25accdf0579d4682eed6108eac08ecdc")
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

extern crate hex;
pub mod transaction {
    pub use serde::{Deserialize, Serialize}; //To serialize/deserialize a transaction
    use sha3::{Digest, Sha3_384}; //To hash transaction struct
    use std::fmt; // To impl Display trait for the transaction

    #[derive(Serialize, Deserialize, PartialEq, Debug)]
    pub struct NameValue {
        pub name: String,
        pub value: u64,
    }

    impl Clone for NameValue {
        fn clone(&self) -> Self {
            NameValue {
                name: self.name.clone(),
                value: self.value,
            }
        }
    }

    #[derive(Serialize, Deserialize, PartialEq, Debug)]
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
    impl fmt::Display for Transaction {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            write!(f, "hash = {}\ndata = {}\nfrom = {}\nto = {}\namount = {}\ntx_type = {}\nsign = {}\nprevious = {}\nblockID = {}\nepoch = {}\n", self.hash,
            self.data,
            self.from,
            self.to,
            self.amount,
            self.tx_type,
            self.sign,
            self.previous,
            self.blockid,
            self.epoch,)
        }
    }
    pub fn new_forgerie_transaction(data: u128) -> Transaction {
        Transaction {
            hash: String::from("some haaash"),
            data,
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
            epoch: 5u64,
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
    impl Transaction {
        pub fn serialize(&self) -> String {
            serde_json::to_string(&self).unwrap()
        }
        pub fn hash(&self) -> String {
            let mut hasher = Sha3_384::new();
            // write input message
            hasher.update(format!("{}", self.serialize()));
            // read hash digest
            let one_hash = hasher.finalize();

            let mut hasher = Sha3_384::new();
            hasher.update(format!("{}", hex::encode(one_hash)));
            format!("{}", hex::encode(hasher.finalize()))
        }
        pub fn len(&self) -> usize {
            let mut count: usize = 0;
            for _ in format!("{}", self).chars() {
                count += 1;
            }
            count
        }
        // fn sign(&self, )
    }
    pub fn deserialize_transaction(transaction: String) -> Transaction {
        let transaction: Transaction = serde_json::from_str(&transaction).unwrap();
        transaction
    }

    impl Clone for Transaction {
        fn clone(&self) -> Self {
            Transaction {
                hash: self.hash.clone(),
                data: self.data,
                from: self.from.clone(),
                to: self.to.clone(),
                amount: self.amount,
                tx_type: self.tx_type,
                typevalue: self.typevalue.clone(),
                sign: self.sign.clone(),
                previous: self.previous.clone(),
                blockid: self.blockid,
                epoch: self.epoch,
            }
        }
    }
}
